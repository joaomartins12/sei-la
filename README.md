# DMO Client (v487, English)

> Engine sources are included so the entire chain (Core00 → Core06 → game libs → exe) can be built from scratch. Pre-built `.lib` and `.pch` artifacts are gitignored; you produce them yourself.

---

## Toolchain

- **OS:** Windows 10/11 (64-bit host fine; client itself is Win32)
- **IDE:** Visual Studio 2022 Community (or higher)
- **Toolset:** **v143** for everything. The repo's `.vcxproj` files have been migrated from v140/v141 to v143 across all 7 Core solutions and game projects.
- **Required VS workloads/components:**
  - Desktop development with C++
  - **MFC for v143** (separate component — required by `common_vs2019/common.vcxproj` and a couple tool projects)
  - Windows 10/11 SDK (latest)
- **DirectX SDK June 2010:** included at `DirectX_June2010/`. No separate install needed; project paths reference it.
- **PhysX SDK 2.6.4:** included at `PhysX_2.6.4_SDK/`. Don't substitute 2.7.x or 2.8.x — Gamebryo 2.3 expects 2.6.x struct layouts.
- **FreeType 2.8:** headers vendored at `LibProj/Freetype/include/`, static lib at `Gamebryo2.3/GB_Lib/ThirdPartyLib/freetype28.lib`. Don't replace with 2.10.x headers — they'd ABI-mismatch the lib.
- **MilesSound:** at `LibProj/MilesSound/`.

> **Note on Linux/Wine:** The exe runs under Wine + DXVK on Linux. The build itself must happen on Windows (or a Win VM); MSBuild can be driven via SSH from a Linux host. See "Cross-build via SSH" below.

---

## Build order (top to bottom)

Each solution depends on the previous ones being built first. **Build all in `Release` configuration unless noted.** The output libs land in `Gamebryo2.3/GB_Lib/ReleaseLib/` (or `ThirdPartyLib/`) and are picked up by linker paths in later solutions.

Solutions live at `Gamebryo2.3/Build/Win32/VC80/`.

### 1. `Core00_ThirdPartyCode.sln`
Vendored third-party libs (zlib, libpng, etc.). These ship as `.vcxproj` only and produce `.lib` files used by everything else. Build first.

### 2. `Core01_CoreLibs.sln`
Gamebryo engine core: `NiSystem`, `NiMain`, `NiAnimation`, `NiParticle`, `NiInput`, `NiCollision`, `NiPNGReader`, `NiVisualTracker`, `NiMilesAudio`, etc. Long build (~50 sub-projects).

### 3. `Core02_CoreLibsPhysX.sln`
Gamebryo's PhysX 2.6.x bindings: `NiPhysX`, `NiPhysXFluid`, `NiPhysXParticle`. Requires `PhysX_2.6.4_SDK/` (vendored).

### 4. `Core03_ToolLibs.sln`
Asset/animation tool libs: `NiAnimationCompression`, `NiControllerExtractor`, `NiStripify`, `NiDevImageQuantizer`, `NiHDRReader`, `NiOpenEXRReader`, `NiViewMath`. The `NiOpenEXRReader` project pulls in `IlmImf` (OpenEXR) and uses a small compatibility shim — see `Gamebryo2.3/ThirdPartyCode/OpenEXR/openexr_compat.h`.

### 5. `Core04_ToolLibsPhysX.sln`
PhysX-flavoured tool libs (mostly stubs/wrappers).

### 6. `Core05_ShaderLibs.sln`
Shader compilation/runtime libs: `NiBinaryShaderLib`, `NSBShaderLib`, `NSFParserLib`, `NiD3DXEffectShaderLib`, `NiD3D10BinaryShaderLib`. Each is built **once** (no separate DX9 vs DX10 outputs — `<TargetName>` overrides have been removed so the lib name is consistent with what `DProject.vcxproj` links against).

### 7. `Core06_AppFrameworks.sln` *(optional)*
Sample app shells. Skip unless you're working on engine demos — the game client doesn't need this.

### 8. Game project: `DProject.sln`
Sit at the **repo root** (`DProject.sln`). When you build `DProject` in the `Release_English|Win32` configuration, MSBuild walks its `<ProjectReference>` graph and builds the dependent game-specific libraries automatically: `CsFunc`, `CsThread`, `CsDM`, `CsFilePack`, `CsFileTable`, `CsGamebryo2.3` (output `CsGB2.3.lib`), `CsGBChar`, plus `nlib`, `lib_json`, `common_vs2019/common`.

#### Configurations

| Configuration | Output | Use case |
| --- | --- | --- |
| `Debug` | `DProject_d.exe` | Debugging only. |
| `Release_English` | `DProject_R_ENG.exe` | **Recommended** for English-region builds. No Themida/Steam/anti-cheat code. |
| `Give_English` | `DProject_R_ENG.exe` (Give) | Shipping flavour with Themida wrappers, GameGuard hooks, USECONSOLE etc. Avoid unless you specifically need it. |
| Other regional configs | — | `Release_Thai`, `Release_Taiwan`, `Release_Hong_QA`, etc. Not maintained here. |

#### Output location

`Release_English` outputs to `..\..\..\Release\GSP\DProject_R_ENG.exe` relative to `DProject/DProject.vcxproj`. From the repo root that resolves to `<one-level-up>\Release\GSP\DProject_R_ENG.exe`. The PDB is alongside.

---

## Active preprocessor flags (Release_English)

The current vcxproj defines:

```
WIN32;NDEBUG;_WINDOWS;NIRELEASE;VERSION_USA;D3DXFX_LARGEADDRESS_HANDLE;
COMPAT_487;COMPAT_487_2;PATCH_QUESTS_SPINZ
```

`COMPAT_487` / `COMPAT_487_2` activate v487-protocol shims and NULL-guard fast-paths in the file-table managers. `PATCH_QUESTS_SPINZ` adds NULL checks around `g_pQuestMng->GetQuest()` callers (e.g., `AchieveContents::CheckType`). **Don't drop these unless you're targeting a different server protocol** — without them you'll hit assert popups during world entry.

The `EngDefine.h` header (included via `_TestDefine.h` for `VERSION_USA`) supplies the rest: `UI_INVENTORY_RENEWAL`, `NEW_SHARESTASH`, `DEBUFF_ITEM`, `MASTERS_MATCHING`, `MEGAPHONE_BAN`, `CHAT_BAN`, `MINIGAME`, plus a long list of date-stamped feature flags.

`HANDSHAKE_DISABLE`, `FIX_NAMING`, and the Steam/Themida flags are intentionally **not** defined in `Release_English`.

---

## Cross-build from Linux via SSH

If your dev box is Linux and only the build machine is Windows:

```bash
# On the Win VM: enable OpenSSH server, drop your dev-box pubkey into
#   C:\ProgramData\ssh\administrators_authorized_keys  (note: not the user's authorized_keys)
# Network: bridge the VM (not NAT) so it gets a LAN IP.

# From Linux, full rebuild:
ssh user@<vm-ip> 'cd C:\path\to\repo && \
  C:\PROGRA~1\MICROS~1\2022\Community\MSBuild\Current\Bin\MSBuild.exe \
  DProject.sln /p:Configuration=Release_English /p:Platform=Win32 /m /nologo /v:m /clp:Summary'
```

`MSBuild.exe` lives in 8.3 short-path form to avoid quoting issues with the path's spaces. Without `/m` it'll still build but single-threaded.

---

## Runtime requirements

The exe expects these alongside it (in the directory you launch from):

- `Data\Pack01.pf` — resource pack (~10 GB; textures, models, etc.)
- `Data\Pack03.pf` — English tables pack (~70 MB; bin files for items, NPCs, quests, etc.)
- `dmo.ini` containing:
  ```
  [DEBUG]
  FilePack=1
  ```
  to read game data from packs. With `FilePack=0` the client falls back to `Data\Bin\<lang>\*.bin` loose files.
- For Wine launches: a Microsoft 32-bit `dbghelp.dll` next to the exe (the engine's stack tracer / freetype hinting can confuse Wine's builtin dbghelp). Launch with `WINEDLLOVERRIDES="dbghelp=n,b"` so Wine prefers the native one.

The launcher binaries (`DMLauncher.exe`, `GDMO.exe`, `UpdateManager.exe`) are not produced by this repo — pull them from your client distribution.

---

## What's gitignored

- All `*.lib`, `*.dll`, `*.exe`, `*.pdb`, `*.obj`, `*.pch`, `*.iobj`, `*.idb`, `*.tlog`, `*.exp`, `*.ilk` — build artifacts.
- `Release/`, `Release_English/`, `Give_English/`, `Debug/`, `x64/`, `Win32/` build output dirs.
- `bin/`, `obj/`, `.vs/`, `.vscode/`, `*.user` — IDE/MSBuild cruft.

See `.gitignore` for the full list. PCH files (`*.pch`) are ~100MB+ each and would blow past GitHub's 100MB-per-file limit; they regenerate on first build.

---

## Repository layout

```
.
├── DProject.sln                     # main game-client solution
├── DProject/                        # game client source (~600 .cpp/.h)
├── LibProj/                         # game-specific libs (Cs*, FreeType wrap, MilesSound)
│   ├── CsFunc/                      # logging, asserts, string helpers
│   ├── CsThread/                    # thread/sync primitives
│   ├── CsDM/                        # protocol marshalling helpers
│   ├── CsFilePack/                  # Pack01/Pack03 reader (DJB2 hash + XOR)
│   ├── CsFileTable/                 # bin-table loaders (Item, Skill, Map, etc.)
│   ├── CsGamebryo2.3/               # game-side wrappers around Gamebryo
│   └── CsGBChar/                    # character-rendering wrapper
├── Gamebryo2.3/                     # full Gamebryo engine source
│   └── Build/Win32/VC80/            # Core00–Core06 solutions
├── common_vs2019/                   # shared protocol/data structs
├── nlib/                            # asio-based net layer + cClient/cPacket
├── DirectX_June2010/                # vendored DX9 SDK headers + libs
├── PhysX_2.6.4_SDK/                 # vendored PhysX 2.6.4
├── packages/                        # NuGet packages (FreeType 2.8 .nupkg unpacked)
├── Tool/                            # NPCEditor, Helper, FullPackingTool, Launchers — Windows-only utils
├── 3DMAX_PlugIn/                    # 3ds Max exporter plugin
├── ThemidaSDK/                      # protector SDK (only used by Give/Steam configs)
├── SteamSDK/                        # Steamworks SDK (only used by Steam configs)
└── ServerConfig.xml                 # gameplay tuning (hatch rates, starter map, etc.)
```

