#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <unordered_set>

#include "../../include/challenge_scale.h"
#include "../../include/hooks.h"
#include "../../include/mod.h"
#include "../../include/session.h"
#include "../../include/sync.h"
#include "../../include/utils.h"

namespace DS2Coop::Sync {

namespace {

uintptr_t ExeBase() {
    static const uintptr_t Base = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    return Base;
}

constexpr uint32_t kGameManagerImp = 0x11613A8;
constexpr uint32_t kDamageWriteRva = 0x16A300;
constexpr ptrdiff_t kGeneratorInChr = 0x110;
constexpr int kGenAreas = 8;
constexpr ptrdiff_t kGenRecordSize = 0x90;

std::atomic<bool> g_enabled{ true };
ChallengeSettings g_settings{};

std::mutex g_scaleMutex;
std::unordered_set<uintptr_t> g_scaledChrs;
ULONGLONG g_lastScanTick = 0;
constexpr ULONGLONG kScanIntervalMs = 250;

// Safe SEH Memory Readers
bool ReadPtr(uintptr_t Addr, uintptr_t* Out) {
    __try {
        *Out = *reinterpret_cast<const uintptr_t*>(Addr);
        return *Out != 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        *Out = 0;
        return false;
    }
}

bool ReadI32(uintptr_t Addr, int32_t* Out) {
    __try {
        *Out = *reinterpret_cast<const int32_t*>(Addr);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool IsPlayerCharacterSafe(uintptr_t Chr, uintptr_t LocalChr) {
    if (!Chr) return false;
    if (LocalChr && Chr == LocalChr) return true;
    __try {
        const int32_t Gen = *reinterpret_cast<const int32_t*>(Chr + kGeneratorInChr);
        if (Gen == -1) return true; // Human player has -1
        if (LocalChr) {
            const uintptr_t LocalVtbl = *reinterpret_cast<const uintptr_t*>(LocalChr);
            const uintptr_t ChrVtbl = *reinterpret_cast<const uintptr_t*>(Chr);
            if (LocalVtbl && LocalVtbl == ChrVtbl && Gen == -1) return true;
        }
        return false;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool IsBossFightActiveSafe() {
    uintptr_t Gm = 0, Events = 0, Boss = 0;
    if (!ReadPtr(ExeBase() + kGameManagerImp, &Gm)) return false;
    if (!ReadPtr(Gm + 0x70, &Events)) return false;
    if (!ReadPtr(Events + 0x88, &Boss)) return false;
    __try {
        return *reinterpret_cast<const int32_t*>(Boss + 0x14) > 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

// Hook for Damage Write (exe+0x16A300)
// When an enemy strikes a human player, scale damage up.
// When a player strikes an enemy, DO NOT scale damage down: keep damage genuine & satisfying!
typedef void (__fastcall *DamageWriteFn)(uintptr_t Chr, int32_t Damage, uint32_t Arg3, uint32_t Arg4);
DamageWriteFn g_origDamageWrite = nullptr;

void __fastcall DamageWriteDetour(uintptr_t Chr, int32_t Damage, uint32_t Arg3, uint32_t Arg4) {
    if (g_enabled.load() && Damage > 0 && Chr) {
        const int count = GetChallengeActivePlayerCount();
        if (count >= 2) {
            uintptr_t Gm = 0, Local = 0;
            if (ReadPtr(ExeBase() + kGameManagerImp, &Gm)) {
                ReadPtr(Gm + 0xD0, &Local);
            }
            if (IsPlayerCharacterSafe(Chr, Local)) {
                const bool isBoss = IsBossFightActiveSafe();
                const float mult = (count == 2)
                    ? (isBoss ? g_settings.boss_dmg_2p : g_settings.mob_dmg_2p)
                    : (isBoss ? g_settings.boss_dmg_3p : g_settings.mob_dmg_3p);
                Damage = static_cast<int32_t>(Damage * mult);
            }
        }
    }
    if (g_origDamageWrite) {
        g_origDamageWrite(Chr, Damage, Arg3, Arg4);
    }
}

void ScanAndScaleEnemiesSafe() {
    const int count = GetChallengeActivePlayerCount();
    if (count < 2) return;

    uintptr_t Gm = 0, GenMgr = 0, Local = 0;
    if (!ReadPtr(ExeBase() + kGameManagerImp, &Gm)) return;
    if (!ReadPtr(Gm + 0x40, &GenMgr)) return;
    ReadPtr(Gm + 0xD0, &Local);

    const bool isBossFight = IsBossFightActiveSafe();
    const float mobHpMult = (count == 2) ? g_settings.mob_hp_2p : g_settings.mob_hp_3p;
    const float bossHpMult = (count == 2) ? g_settings.boss_hp_2p : g_settings.boss_hp_3p;

    std::lock_guard<std::mutex> lock(g_scaleMutex);

    for (int slot = 0; slot < kGenAreas; ++slot) {
        uintptr_t block = 0;
        if (!ReadPtr(GenMgr + 0x20 + slot * 8, &block)) continue;
        uintptr_t first = 0;
        uint32_t n = 0;
        if (!ReadPtr(block + 0x18, &first) || !ReadPtr(block + 0x20, &n)) continue;
        if (!first || n == 0 || n > 256) continue;

        for (uint32_t i = 0; i < n; ++i) {
            const uintptr_t rec = first + i * kGenRecordSize;
            uintptr_t chr = 0;
            if (!ReadPtr(rec, &chr) || !chr) continue;

            if (g_scaledChrs.contains(chr)) continue;
            if (IsPlayerCharacterSafe(chr, Local)) continue;

            __try {
                const int32_t curHp = *reinterpret_cast<const int32_t*>(chr + 0x168);
                const int32_t maxHp = *reinterpret_cast<const int32_t*>(chr + 0x170);
                const int32_t baseMax = *reinterpret_cast<const int32_t*>(chr + 0x174);

                if (curHp <= 0 || maxHp <= 0) continue;

                const bool isBoss = isBossFight || maxHp >= 2500;
                const float mult = isBoss ? bossHpMult : mobHpMult;

                const int32_t newMax = static_cast<int32_t>(maxHp * mult);
                const int32_t newBase = static_cast<int32_t>(baseMax * mult);
                const int32_t newCur = static_cast<int32_t>(curHp * mult);

                *reinterpret_cast<int32_t*>(chr + 0x170) = newMax;
                *reinterpret_cast<int32_t*>(chr + 0x174) = newBase;
                *reinterpret_cast<int32_t*>(chr + 0x168) = newCur;

                g_scaledChrs.insert(chr);
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                // Safely ignore unmapped/transient records
            }
        }
    }
}

} // namespace

bool InstallChallengeScale(bool Enabled) {
    static bool Installed = false;
    g_enabled.store(Enabled);

    // Sync settings from ModConfig
    const auto& cfg = DS2Coop::SeamlessCoopMod::GetInstance().GetConfig();
    g_settings.enabled = cfg.challenge_scale_enabled;
    g_settings.preset = cfg.challenge_preset;
    g_settings.mob_hp_2p = cfg.mob_hp_2p;
    g_settings.mob_dmg_2p = cfg.mob_dmg_2p;
    g_settings.boss_hp_2p = cfg.boss_hp_2p;
    g_settings.boss_dmg_2p = cfg.boss_dmg_2p;
    g_settings.mob_hp_3p = cfg.mob_hp_3p;
    g_settings.mob_dmg_3p = cfg.mob_dmg_3p;
    g_settings.boss_hp_3p = cfg.boss_hp_3p;
    g_settings.boss_dmg_3p = cfg.boss_dmg_3p;

    if (!Installed) {
        Installed = true;
        if (Hooks::HookManager::GetInstance().InstallHook(
                reinterpret_cast<void*>(ExeBase() + kDamageWriteRva),
                reinterpret_cast<void*>(&DamageWriteDetour),
                reinterpret_cast<void**>(&g_origDamageWrite))) {
            LOG_INFO("[CHALLENGE] Dynamic scaling damage hook installed at exe+0x%X", kDamageWriteRva);
        } else {
            LOG_WARNING("[CHALLENGE] Failed to install damage hook at exe+0x%X", kDamageWriteRva);
        }
    }
    return g_origDamageWrite != nullptr;
}

void ChallengeScaleGameTick() {
    if (!g_enabled.load()) return;
    const ULONGLONG now = GetTickCount64();
    if (now - g_lastScanTick >= kScanIntervalMs) {
        g_lastScanTick = now;
        ScanAndScaleEnemiesSafe();
    }
}

void ChallengeScaleOnReset() {
    std::lock_guard<std::mutex> lock(g_scaleMutex);
    g_scaledChrs.clear();
}

void ChallengeScaleOnMapChange() {
    std::lock_guard<std::mutex> lock(g_scaleMutex);
    g_scaledChrs.clear();
}

int GetChallengeActivePlayerCount() {
    auto& session = Session::SessionManager::GetInstance();
    if (!session.IsActive()) return 1;
    const size_t players = session.GetPlayers().size();
    return players > 0 ? static_cast<int>(players) : 1;
}

bool IsChallengeScaleActive() {
    return g_enabled.load() && GetChallengeActivePlayerCount() >= 2;
}

float GetCurrentMobHpScale() {
    const int count = GetChallengeActivePlayerCount();
    if (count == 2) return g_settings.mob_hp_2p;
    if (count >= 3) return g_settings.mob_hp_3p;
    return 1.0f;
}

float GetCurrentMobDmgScale() {
    const int count = GetChallengeActivePlayerCount();
    if (count == 2) return g_settings.mob_dmg_2p;
    if (count >= 3) return g_settings.mob_dmg_3p;
    return 1.0f;
}

float GetCurrentBossHpScale() {
    const int count = GetChallengeActivePlayerCount();
    if (count == 2) return g_settings.boss_hp_2p;
    if (count >= 3) return g_settings.boss_hp_3p;
    return 1.0f;
}

float GetCurrentBossDmgScale() {
    const int count = GetChallengeActivePlayerCount();
    if (count == 2) return g_settings.boss_dmg_2p;
    if (count >= 3) return g_settings.boss_dmg_3p;
    return 1.0f;
}

} // namespace DS2Coop::Sync
