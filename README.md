# Dark Souls II: Scholar of the First Sin — Seamless Co-op (Enhanced Challenge Edition)

An advanced cooperative modification and dynamic balancing framework for **Dark Souls II: Scholar of the First Sin** (64-bit).

This fork extends the groundbreaking work of the original DS2 Seamless Co-op project by introducing a **Dynamic Non-Linear Challenge Scaling System**, ensuring cooperative gameplay preserves the tension, precision, and punishing difficulty that define the Soulsborne genre.

---

## Key Features

* **True Seamless Progression:** Play continuously from character creation in *Things Betwixt* all the way to the *Throne of Want* without disconnections, soapstones, or phantom timers.
* **Soul Memory & Restriction Bypass:** Summoning ignores Soul Memory tiers, level differences, and Human Effigy status.
* **Persistent World Synchronization:** Bonfire resting, levers, shortcut gates, elevator states, and key progression events synchronize seamlessly between players.
* **Isolated Fair Loot:** Chests and world drops are independently claimable by each player; picking up loot in a host's world permanently registers to your character.
* **No Disconnect on Death or Boss Defeat:** Defeating an area boss keeps your session alive; fallen companions automatically resurrect at the nearest synchronized bonfire.
* **Private Dedicated Server (ds3os):** Operates on an isolated, local server architecture with zero connection to Bandai Namco / FromSoftware infrastructure (0% risk of softbans).
* **In-Game ImGui HUD (F1 / Insert):** Real-time overlay featuring session status, net-diagnostics, language toggles, and live difficulty controls.

---

## Dynamic Non-Linear Challenge Scaler

### The Problem with Vanilla Co-op
In standard cooperative play, FromSoftware games become drastically simplified. Splitting enemy and boss aggro across multiple players trivializes attack patterns, while synchronized weapon swings induce permanent stunlocks on regular mobs. Vanilla boss scaling (a flat +50% HP buff) fails to compensate for double the stamina bars, Estus pools, and damage uptime.

### Our Solution: Peak-Skill Dynamic Scaling
Instead of artificially reducing player damage (which makes weapons feel weak and unrewarding), our system **keeps player damage output 100% un-nerfed and genuine**. Simultaneously, it hooks the game engine in real time to scale health pools and enemy threat levels dynamically based on live lobby population:

| Metric | 1 Player (Solo) | 2 Players (Co-op Duo) | 3+ Players (Trio Squad) |
| :--- | :---: | :---: | :---: |
| **Player Weapon Damage** | 1.0x (100%) | **1.0x (100% Un-nerfed)** | **1.0x (100% Un-nerfed)** |
| **Regular Mob Health Pool** | 1.0x | **+45% (1.45x)** | **+80% (1.80x)** |
| **Enemy Damage to Players** | 1.0x | **+20% (1.20x)** | **+35% (1.35x)** |
| **Boss Health Pool** | 1.0x | **+85% (1.85x)** | **+150% (2.50x)** |
| **Boss Attack Damage** | 1.0x | **+30% (1.30x)** | **+45% (1.45x)** |

* **Zero Damage Nerfs:** When your ultra greatsword lands for 550 damage, the game registers and renders the full 550 damage.
* **Anti-Melt Encounters:** Scaled health pools require proper combat rotations and tactical positioning rather than mindless button mashing.
* **High-Stakes Combat:** Boosted incoming damage ensures defensive lapses and reckless trades are punished immediately.
* **Covenant of Champions Compatible:** Joining the Company of Champions covenant further stacks on top of these multipliers for ultimate hardcore runs.

---

## Installation & Setup

### Requirements
* Dark Souls II: Scholar of the First Sin (64-bit, version 1.02 / 1.03)
* Microsoft Visual C++ 2015-2022 Redistributable (x64)
* A LAN emulator (e.g., Radmin VPN or ZeroTier) for cross-network play

### Host Installation
1. Download the latest build from the **[Releases](https://github.com/ShiTmoZ/ds2/releases)** tab or GitHub Actions artifacts.
2. Extract the contents of the `Host` folder directly into your game's executable directory (where `DarkSoulsII.exe` is located, typically `Steam/steamapps/common/Dark Souls II Scholar of the First Sin/Game/`).
3. Run `StartServer.bat` inside the game directory to launch the dedicated matchmaking server.
4. Launch `DarkSoulsII.exe`. Press **F1** or **Insert** in-game to configure your lobby and set a password.

### Joiner Installation
1. Extract the contents of the `Joiner` folder into your game's `Game/` directory.
2. Open `ds2_seamless_coop.ini` and set `server_ip` to the Host's virtual IP (e.g., from Radmin VPN).
3. Launch `DarkSoulsII.exe`, press **F1** or **Insert**, enter the Host's password, and select **Join**.

---

## Configuration Reference (`ds2_seamless_coop.ini`)

All dynamic challenge parameters are fully customizable without recompiling:

```ini
# ============================================================================
# Dynamic Non-Linear Challenge Scaling System
# ============================================================================
challenge_scale_enabled = true
challenge_preset = dynamic

# 2-Player Multipliers (Balanced for Peak Player Skill)
mob_hp_2p = 1.45
mob_dmg_2p = 1.20
boss_hp_2p = 1.85
boss_dmg_2p = 1.30

# 3-Player Multipliers (Hardened for Squads)
mob_hp_3p = 1.80
mob_dmg_3p = 1.35
boss_hp_3p = 2.50
boss_dmg_3p = 1.45
```

---

## Building from Source

### Automated GitHub Actions CI/CD
Every commit pushed to this repository triggers an automated Windows x64 build via GitHub Actions. Pre-compiled binaries and complete distribution packages can be downloaded immediately from the **Actions** tab.

### Local Compilation (Visual Studio 2022)
```powershell
# Clone the repository
git clone https://github.com/ShiTmoZ/ds2.git
cd ds2

# Configure and build Release x64 binary
cmake -S mod -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
```
The compiled DLL will be located at `build/bin/Release/dinput8.dll`.

---

## Credits & Acknowledgments
* **Original DS2 Seamless Co-op:** Created by *Restezzz / scheissgeist* & inspired by *LukeYui*.
* **Dedicated Server Backbone:** Built on *ds3os*.
* **Hooking & UI:** Powered by *MinHook* and *Dear ImGui*.
