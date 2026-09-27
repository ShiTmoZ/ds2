# Dark Souls II: Scholar of the First Sin — Seamless Co-op (Enhanced Challenge Edition)

Welcome to the **Enhanced Challenge Edition** of Dark Souls II Seamless Co-op!

This repository provides multiplayer packages for **Dark Souls II: Scholar of the First Sin (v1.03 / Calibrations 2.02)** tailored for players who want a truly challenging, balanced co-op experience.

---

## ⚡ The Balance Problem & Our Solution

In vanilla co-op setups, playing with 2 or 3 players turns Dark Souls II into an 'easy mode':
- Bosses and enemies melted within seconds under focus fire.
- Default damage scaling per additional player was set to **0%**.
- Stagger/posture scaling was set to **0%**, allowing multiple players to permanently stunlock bosses to death.

### 🛡️ Core Balance Philosophy:
1. **Zero Nerf to Player Weapon Damage**: Your weapons hit with 100% genuine force. Floating damage numbers and weapon satisfaction are completely preserved.
2. **Dynamic Challenge Scaling for 2 and 3 Players**:
   - **Solo (1 Player)**: 100% standard vanilla difficulty.
   - **Duo (2 Players)**:
     - Normal Mobs: +60% HP, +25% DMG to players.
     - Bosses: +85% HP, +30% DMG to players.
     - Stagger Armor: +20% mobs, +30% bosses (prevents cheap stunlocking).
   - **Trio (3 Players - True Raid Boss Mode)**:
     - Normal Mobs: +120% HP, +50% DMG to players.
     - Bosses: +170% HP, +60% DMG to players.
     - Stagger Armor: +40% mobs, +60% bosses (demands coordinated combos and spatial awareness).

---

## 📦 Available Releases (Choose Your Flavor)

Head over to the **[Releases Section](https://github.com/ShiTmoZ/ds2/releases/tag/v0.3.0)** to download:

### 🌟 Edition 1: Gold-Team & Steam Spacewar Edition (RECOMMENDED)
**Asset:** `DS2-SeamlessCoop-GoldTeam-EnhancedChallenge-v0.3.0.zip`
- **Architecture**: LukeYui Seamless Co-op + OnlineFix (Spacewar AppID 480).
- **Zero Config**: No server hosting, no port-forwarding, no typing IP addresses.
- **In-Game Items**: Connect effortlessly using the in-game items (`Heliograph` to host, `Fragment of Brilliance` to join).
- **100% Antivirus Safe**: Built directly on the official Gold-Team release files with zero altered binaries.

#### Quick Setup:
1. Extract all contents of `DS2-SeamlessCoop-GoldTeam-EnhancedChallenge-v0.3.0.zip` directly into your game folder (alongside `DarkSoulsII.exe`).
2. Start the **Steam** desktop client in the background.
3. Open `SeamlessCoop/ds2sc_settings.ini` with Notepad and set your matching password in `cooppassword = ...`.
4. Launch the game using **`ds2sc_launcher.exe`**.
5. After finishing the tutorial area:
   - **Host**: Use the **Heliograph** item from your inventory to open your world.
   - **Joiner**: Use the **Fragment of Brilliance** item to enter your friend's session!

---

### 🌐 Edition 2: Standalone Dedicated Server Edition
**Asset:** `DS2-SeamlessCoop-Challenge-v0.3.0.zip`
- **Architecture**: Open-source C++ hook (`dinput8.dll`) + `SeamlessServer` (ds3os).
- **No Steam Required**: Connect over local LAN, Radmin VPN, ZeroTier, or direct IP.
- **In-Game Overlay**: Press **F1** or **Insert** anytime to inspect lobby stats and toggle settings.

#### Quick Setup:
- **Host**: Copy `Host` folder contents into game directory. Run `StartServer.bat`.
- **Joiner**: Copy `Joiner` folder contents into game directory. Set `server_ip = <host_ip>` in `ds2_seamless_coop.ini`.
