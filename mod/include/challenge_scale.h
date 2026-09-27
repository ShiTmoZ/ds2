#pragma once

#include <cstdint>
#include <string>

namespace DS2Coop::Sync {

// Dynamic Non-Linear Challenge Scaling System
// Dynamically scales enemy health pools and incoming damage to players based on active player count.
// Keeps player damage output un-nerfed and satisfying while ensuring multi-player sessions preserve
// true Soulsborne difficulty.

struct ChallengeSettings {
    bool enabled = true;
    std::string preset = "dynamic"; // "dynamic", "vanilla", "hardcore", "custom"

    // 2-Player Multipliers (balanced for peak player skill)
    float mob_hp_2p = 1.45f;
    float mob_dmg_2p = 1.20f;
    float boss_hp_2p = 1.85f;
    float boss_dmg_2p = 1.30f;

    // 3-Player Multipliers (hardened for trio squads)
    float mob_hp_3p = 1.80f;
    float mob_dmg_3p = 1.35f;
    float boss_hp_3p = 2.50f;
    float boss_dmg_3p = 1.45f;
};

bool InstallChallengeScale(bool Enabled);
void ChallengeScaleGameTick();
void ChallengeScaleOnReset();
void ChallengeScaleOnMapChange();

int GetChallengeActivePlayerCount();
bool IsChallengeScaleActive();

float GetCurrentMobHpScale();
float GetCurrentMobDmgScale();
float GetCurrentBossHpScale();
float GetCurrentBossDmgScale();

} // namespace DS2Coop::Sync
