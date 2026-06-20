# Changelog

All notable client-side patches applied while bringing the upstream v487 source up against a private DSO-style server. Earliest items at the bottom of each section.

> Server-side patches (DSO emulator) are tracked in that repo. This file only covers what was changed in the client tree.

---

## 2026-05-13 — client text-crash hardening + callback safety

- Hardened text rendering and glyph decode paths in `DProject/_Interface/Base/Text.cpp` to prevent null/invalid-buffer crashes during chat/server-select/tooltips.
- Added safer text-entry handling and normalization in UI flows (including `ToolTip`, `ToolTip_Item`, `SystemMsg`, and server/account/game packet text paths) to avoid malformed/unsafe string propagation.
- Reworked event callback dispatch lifetime handling in `DProject/EventHandler/GameEventMng.cpp`, `DProject/EventHandler/GameEventMng.h`, `DProject/EventHandler/EventHandler.h`, and `DProject/ContentsSystem/ContentsSystem.h` so re-entrant/remove-during-iterate flows stay stable.
- Synced VM source updates to Linux client source tree and published updated runtime EXE/PDB.

## 2026-05-13 — item/equip stability pass + temporary debug cleanup

- Removed temporary debug-only instrumentation used during recent item/equip/storage investigation from:
  - `LibProj/CsFileTable/ItemMng.cpp`
  - `DProject/_Interface/00.Contents/EnchantOptionContents.cpp`
  - `DProject/_Interface/Game/Warehouse.cpp`
- Kept defensive null-guard behavior in warehouse rendering paths while removing noisy crash-log spam.
- Updated client-side equip/stat display flow to avoid recent crash paths hit during equip/unequip testing (including Digimon-window-open cases).
- Synced the VM-edited client source into the Linux git source tree for commit/push.

## Crash-log follow-up — server-select glyph AV + inventory lock/unlock AV

Two more defensive fixes from triaging `logs/crash.log` after the memory-skill pass landed. Both are pre-existing latent crashes unrelated to memory skills, surfaced once the rest of the stability work removed earlier-blocking faults.

- **`DProject/_Interface/Base/Text.cpp:1296`** — `_ReadFTBmp` now bounds-checks the **source** index `bitmap->buffer[p*BitWidth + q]`, not just the destination. The earlier patch only guarded `bitmap->buffer != NULL` + the destination index range; if `sx`/`sy` exceed the FT_Bitmap's actual `width`/`rows` (degenerate glyph, fallback sizing), the source read indexed past the FreeType buffer and AV'd. Hit on the **CSelectServer::ResetServerList** path when rendering a server-name string — every server-select screen entry could crash the client. Fixed by computing `srcTotal = BitWidth * bitmap->rows` once and skipping any `srcIdx >= srcTotal`.
- **`DProject/Data_Inven.cpp`** — `ItemLock_ItemType` and `ItemUnlock_ItemType` both still did `g_pItemMng->GetItem(m_pInven[i].m_nType)->GetInfo()` blind. The earlier round of Data_Inven null-checks claimed to cover these but only guarded the inner `pFTItem` via `SAFE_POINTER_CON` — by which point the outer `GetItem()->GetInfo()` chain had already AV'd if `GetItem()` returned NULL. Same outer-`GetItem() != NULL` guard pattern as `GetCount_Item_TypeL` applied to both.

## Memory-skill viability — buff bar / inventory / glyph / chip-attach crashes

Defensive fixes around the memory-skill chip pipeline. Each one masked or hard-crashed the cast / heal / register flow until patched.

- **`DProject/_Interface/Game/BuffInfo.cpp:631`** — `m_nMAX_Col` render-cap now scoped to `IF_BUFF_TARGET` only (was: applied to every buff window, including `IF_BUFF_MY_T` / `IF_BUFF_MY_D`). When the player has a target window narrowed for layout, the cap collapses to 1–2 columns and silently drops the player's own buff icons mid-render — Valor of Marcus / passive tamer-base buff icons would vanish after casting any memory skill, returning only after relog. Cap now only applies to the third-party-target buff strip where the original cap was needed.
- **`DProject/Data_Inven.cpp`** — Null-check defensive patches in `GetCount_Item_TypeL`, `Find_Item_TypeLS`, `RemoveOrReduceItems` (decrement loop), and `ItemLock_ItemType`. Each previously did `g_pItemMng->GetItem(m_pInven[i].m_nType)->GetInfo()` blind; when an inventory slot's `m_nType` doesn't resolve (stale slot post-consume, corrupted itemId from a server-side bug, etc.) the dereference crashed instantly. Now skips with `continue` if `GetItem(...) == NULL` and a `SAFE_POINTER_CON` on `pInfo`. Fixed an AV when the chip-consume side-effect of casting a memory skill cleared the source item slot before the inventory recompute tick.
- **`DProject/network/cCliGameSkill.cpp:RecvDigimonAddSkillSuccess`** — Added `SAFE_POINTER_RET(pFTItem)` + `SAFE_POINTER_RET(pFTInfo)` after the `GetItem(nItemIDX)` / `GetInfo()` calls. If the server registers a memory skill whose chip item isn't in the client's ItemList.bin (a content mismatch — common while iterating bin edits), the `pFTInfo` deref would AV. Now bails cleanly so the registration still applies; just the attach VFX is skipped.
- **`DProject/_Interface/Base/Text.cpp:1271`** — Defensive `m_pFTData == NULL` check + bounds check inside `_ReadFTBmp`. After a memory-skill cast, `cDigimonTalk::Print` would fire a Korean-localized message; if the glyph for a particular character wasn't pre-cached, `_ReadFTBmp` indexed into a NULL `m_pFTData` and crashed. The wider Text.cpp diff (+65/-51) reorganizes the glyph-load path to NULL-guard each FT data pointer before use.
- **`DProject/Skill.cpp`** — `nsCSDEBUG::CrashLogger::LogMessage` diagnostics in `__UseSkill` + `_Target_My_Partner_Digimon` along the memory-skill dispatch (`memSkill=…` / `dispatcher=…`). Output lands in `logs/crash.log` and was load-bearing for tracing why pSkill::UseSkillChip was firing with the wrong evoStep / target UID during the initial wiring. Leave the logging in — it's gated by `CrashLogger` init and costs nothing once stable.

Whitespace-only normalization in `CharacterCreateContents.cpp`, `CharacterSelectContents.cpp`, `ItemRewardContents.cpp`, `Popup.cpp`, `PrintMsg.cpp`, `cCliGateReceive.cpp` — trailing-space cleanup from editing in the VM, no functional change.

---

## Build / toolchain

- **`DProject/DProject.vcxproj`** — Added `COMPAT_487;COMPAT_487_2;PATCH_QUESTS_SPINZ` to `Release_English|Win32` preprocessor definitions (previously only on `Give_English`). Without them, world entry hits NULL-deref crashes inside achievement-check / digimon-init paths.
- **`DProject/DProject.vcxproj`** — All 19 `<AdditionalDependencies>` entries now reference `freetype213.lib` (was `freetype28.lib`, originally `Freetype239.lib`). FT 2.13.x upgrade; see the FreeType section below.
- **`LibProj/Freetype/include/`** — Replaced 2.8 headers with the 2.13.3 set (8 deprecated headers removed, 18 new ones added — DLG subdirectory, ftcolor, ftdriver, etc.). Header + lib version match remains critical (the prior 2.10.1-headers-vs-2.3.9-lib AV taught us). Build the lib from upstream `freetype-2.13.3/builds/windows/vc2010/freetype.vcxproj` with the **Release Static | Win32** config edited to use `<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>` (matches DProject's `/MD`); the resulting `objs/Win32/Release Static/freetype.lib` is the vendored `freetype213.lib`. Lib is gitignored (`*.lib`) — drop it into `Gamebryo2.3/GB_Lib/ThirdPartyLib/freetype213.lib` after building.
- **`Gamebryo2.3/GB_Lib/ThirdPartyLib/freetype213.lib`** *(loose, gitignored)* — replaces `freetype28.lib`. Win32, v143 toolset, static, /MD Release.
- 23 `.vcxproj` files migrated from `<PlatformToolset>v140` to `v143` (mostly libpng/zlib/upstream); MFC for v143 must be installed separately in VS 2022.
- 14 `ToolLibs/*.vcxproj` files: added `..\..\..\..\GB_Include;..\..\..\..\CoreLibs\NiSystem;..\..\..\..\CoreLibs\NiSystem\Win32;..\..\..\..\CoreLibs\NiMain;..\..\..\..\CoreLibs\NiMain\Win32` to `<AdditionalIncludeDirectories>` so `NiBuildConfiguration.h` resolves.
- 7 `ToolLibs/*.vcxproj` files: added `..\..\..\..\..\DirectX_June2010\Include` for D3DX9 headers.
- `NiStripify.vcxproj` excluded `xbstrip\xbstrip.cpp` (Xbox D3D8 code) from build.
- 3 `CoreLibsPhysX/*.vcxproj` (NiPhysX, NiPhysXFluid, NiPhysXParticle) — added engine include paths.
- 5 shader projects (`NiBinaryShaderLib`, `NSBShaderLib`, `NSFParserLib`, `NiD3DXEffectShaderLib`, `NiD3D10BinaryShaderLib`) — removed `<TargetName>NameDX9</TargetName>` and `<TargetName>NameD3D10</TargetName>` overrides so output `.lib` filenames match what `DProject.vcxproj` expects to link.
- `Gamebryo2.3/CoreLibs/NiAudio/Miles/Win32/VC80/NiMilesAudio.vcxproj` — fixed include path `..\..\..\..\..\..\MilesSound\include` → `..\..\..\..\..\..\LibProj\MilesSound\include`.
- `Gamebryo2.3/ThirdPartyCode/OpenEXR/openexr_compat.h` (new) — single-line `#include <functional>` shim, force-included into `IlmImf.vcxproj` so the C++17 build still resolves `std::binary_function` references.
- `IlmImf.vcxproj` — preprocessor defines `_HAS_DEPRECATED_ADAPTOR_TYPEDEFS=1;_HAS_AUTO_PTR_ETC=1;_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS=1` and `<ForcedIncludeFiles>..\..\..\openexr_compat.h</ForcedIncludeFiles>` (no inheritance).
- `LibProj/CsFileTable/Excel/cExcel.h` — `#import` path normalized: was hardcoded to a Windows-specific developer path, now `#import "..\..\..\msado15.dll" no_namespace rename("EOF", "adoEOF")` so the build works from anywhere `msado15.dll` is at the repo root.

---

## UI — `cIcon::RenderCount` zero-count assert removed (no popup)

`Icon.cpp:235` had `assert_cs( nCount != 0 )` immediately followed by `if (nCount <= 0 || nCount == 1) return;` — the function ALREADY handles `nCount <= 0` gracefully, so the assert was pure dev-warning noise. In `Release_English`, `assert_cs._log()` calls `MessageBoxA`, popping a "CsAssert" dialog every frame the gift / reward / consigned-shop UI rendered an item with stack count zero. Server-side serialization filters help on the regular item-list paths but the `cServer_Reward::RenderGift` UI path reads from `cItemInfo` where empty/expired slots can legitimately have count zero — the renderer is supposed to skip them silently, which is exactly what the next `if/return` does.

- **`DProject/_Interface/Base/Icon.cpp:235`** — removed the bogus `assert_cs( nCount != 0 )`. Behavior unchanged (next-line graceful early-return handles the same condition), popup gone.

Same pattern as the Text.cpp FT skip-on-error fix below: `assert_cs` immediately followed by an early-return guard is dev-only warning noise that should never have shipped to Release_English. Other instances in client code likely warrant the same treatment as they surface; sweep candidates can be found via `grep` for `assert_cs(.*); if (.*) return` patterns.

---

## UI — FreeType glyph-load failures: silent skip (no popup)

The previous patch left `assert_cs( error == 0 )` in front of the new `if (error != 0) continue;` skip — log-only in Release_English, but `assert_cs._log()` calls both `LogAssert` and `MessageBoxA`, popping up a "CsAssert" dialog at every missing-glyph-in-font condition. Server-select and any text-render path with a non-Latin codepoint hit the popup.

- **`DProject/_Interface/Base/Text.cpp`** — All 13 sites of the form `assert_cs( error == 0 ); if( error != 0 ) continue;` collapsed to just the `if/continue` plus a header comment explaining why the assert was removed. FT_Load_Glyph / FT_Load_Char failure on a missing-from-font glyph is an expected condition (not a bug); silent skip is the right behavior, and the next char in the loop renders normally. Net effect: no more "CsAssert" popup at server-select or anywhere a font is missing a requested codepoint.

---

## UI — `cStringAnalysis` per-call state moved off singleton

`cStringAnalysis` was a process-wide singleton (`g_pStringAnalysis`) that held its **per-call working state** — `m_pString`, `m_listParcing`, `m_szToken`, `m_nTokenIndex`, `m_nTotalWidth`, `m_pTextInfo`, `m_pStrInfo`, `m_OrgColor`, `m_nOrgBoldLevel`, `m_nLineLimit`, `m_pStringList`, `m_bNewLine`, `m_strTempText` — as **member variables**. Every `Cut*` / `_MakeToken*` / `_AddText` / `_Parcing*` call mutated those, then a final transfer-to-stringList step reset them. With multiple call sites — UI rendering on the main thread, plus packet-driven chat / system-message / notice events on the io_ctx network thread (`cCliGame::OnExecute → SyncInBuff → ChatContents::Chat_Process → ChatMainWindow::Notify → g_pStringAnalysis->Cut`) — two threads regularly entered `Cut` simultaneously. The defensive `SAFE_NIDELETE(m_pString)` block at the start of each `Cut*` was a band-aid for the same shared-state bug: it tried to recover from "previous call left state" but couldn't close the cross-thread reentrancy window. The losing thread's `m_pString = NiNew cString` got freed by the winning thread's defensive cleanup; the freed slot was reused by the NiAllocator for an unrelated object (the symbolizer typically named it `cData_QuickSlot`); the next `m_pString->GetListSize()` AV'd on the garbage-vtable-or-member layout. The crash was random because it required two callers to interleave in a specific order; the most recent observed instance was a `cString::GetListSize+0x12 → cData_QuickSlot::GetQuickSlot+0xa` SEH AV from the chat path at `2026-05-10 10:13:33.554`.

Fix: introduce a stack-allocated `sCutContext` struct that holds all per-call working state. Every public `Cut*` method instantiates one locally and passes `sCutContext& ctx` through to all helpers. The singleton now has *no* shared per-call state to race over.

- **`DProject/_Interface/Base/StringAnalysis.h`** — new `sCutContext` nested struct with the 13 per-call fields and a destructor that frees a leftover `m_pString` / drains `m_listParcing` if any caller exits early. Helper signatures changed: `_Parcing`, `_ParcingValue`, `_ParcingTypeEnd`, `_RemoveParcing`, `_MakeToken_Parcing_ForCardMaster`, `_MakeToken_Parcing`, `_MakeToken`, `_MakeToken_OnlyNewLine`, `_AddText` all take `sCutContext& ctx` as their first argument. Pure helpers (`_ParcingTypeStart`, `_ParcingValueEnableCheck`) unchanged. `m_vecStringTotalText`, `m_ParcingColor`, `m_bChangeColor` stay on the singleton (legitimate cross-call config / result-vector that QuestHelper.cpp / QuestDiscript.cpp read between calls — same-thread API contract, not race-prone).
- **`DProject/_Interface/Base/StringAnalysis.cpp`** — 13 methods refactored to use the local context: `Cut`, `Cut_NoMultyLine`, `Cut_LinkParcing`, `Cut_Parcing`, `Cut_Parcing_ForCardMaster`, `AddCut`, `Cut_MailParcing`, `Cut_LimitWigth`, plus the helpers above. All 6 copies of the "Defensive cleanup — a previous Cut_Parcing variant may have aborted mid-flight…" block deleted (the bug they tried to compensate for is gone by construction). `Init()` / `Delete()` no longer manage per-call state. `Cut_Parcing_ForCardMaster` is the one variant where `cString*` is caller-owned; it nulls `ctx.m_pString` before scope-exit so the dtor doesn't double-free.

Why it eliminates the entire bug class: every `Cut*` call now owns its own state on the stack. Two threads / reentrant calls / SEH unwinds during a FreeType glyph load — none of them can affect any other call's working state, because there *is* no shared working state. No mutex, no defer-to-main, no perf hit — just no shared mutable state to race over.

---

## Networking — logout → server-select empty list

When the user clicked the logout button in-game and the client returned to the server-select screen, the server list rendered empty — every reconnect to the auth server completed at the TCP level but the client never sent a Login packet (server log shows handshake response, then nothing, then a 25-second-later silent disconnect with `Account: 0`). Two cooperating bugs caused it:

- **`DProject/network/cNetwork.cpp`** — `net::stop()` (called from `InGameFlow::Terminate`) `xdelete`'d the `game` / `gate` / `account` / `*_tmp` globals but `xdelete` is just `delete` — it does **not** null the pointer. Subsequent calls into `ServerSelectContents::CheckConnectAccountSever` evaluated `if (net::account && net::account->IsActive())` against a dangling pointer; the `IsActive()` virtual call read freed memory and either misrouted the self-heal branch or silently corrupted state. Fix: explicitly null each global after delete (`xdelete(account); account = null;` etc.).
- **`nlib/client.cpp`** — added `static auto io_work = asio::make_work_guard(io_ctx);` next to the global `io_context`. `cClient::Run` starts a per-instance worker thread that calls `io_ctx.run()`. Without a work guard, when the game-session `cClient` is destroyed and its socket cancel()'d (logout), all pending operations complete with `operation_aborted`, the io_context goes idle, `run()` returns, and the worker thread exits. The post-logout self-heal then creates a fresh `cCliAccount` whose `Run()` spawns a **new** worker, but `io_ctx.run()` on a stopped/exhausted context returns immediately — so the post-logout `async_connect` completion handler never fires, `OnConnected` → `SendLogin` is never called, and the server's handshake bytes pile up unprocessed in the IOCP queue. Confusingly, the OS-level connect still succeeds and Windows IOCP completes overlapped writes at the kernel level (so any later bytes the client manages to enqueue *do* reach the wire), which made it look like the client was talking to the server even though the receive pipeline was completely dead. The work guard holds an outstanding work item so `run()` blocks across cClient lifetimes; subsequent self-heals reuse the still-pumping io_context and the connect handler fires normally.

Combined with the destructor / `operation_aborted` bail fixes already shipped in `757eb69`, this finishes the logout-and-back-to-server-select round trip without a relaunch.

---

## Networking — mob/digimon sync class dispatch

- **`DProject/network/cCliGameSync.cpp`** — Four sites (`SyncOutObject`, `SyncDeleteObject`, `SyncMoveObject`, `SyncWalkObject`) switched from `switch (GetClass(nUID))` to `switch (type.m_nClass)`. v487's UID-class accessor is `GetClass(u2)` — a 2-byte read that truncates the upper bits of `nClass` for any class enum value ≥ 4 (Monster, Npc, Party, CommissionShop). The full `cType` already carries `m_nClass` as a 5-bit packed field; reading it directly avoids the truncation entirely. Without this patch, server-spawned mobs (`nClass=Monster=4`) arrived as the wrong class and the sync handlers either no-op'd or routed them as a different object kind — no on-screen mob.
- **`nlib/client.cpp`** — `cClient::DoSend` rewritten to copy the wire bytes into a heap-owned `std::vector<uint8_t>` (held via `shared_ptr` capture) and consume the caller's `cPacket::m_buf` synchronously, instead of capturing `&p` by reference inside the `asio::async_write` completion handler. Callers (`SendXxx`) build `cPacket` on their stack and return immediately after `DoSend`; by the time the `io_context` thread fired the handler, `p` had been destroyed and `p.m_buf.consume(bt)` blew up at `+0x13/+0x15` inside `asio::basic_streambuf::consume`. This was the recurring `cClient::DoSend → asio::write_op → consume` teardown crash on every disconnect.

---

## Networking — IP / connection settings

- **`DProject/EngDefine.h:3`** — `#define ACCOUNT_IP "26.60.202.44"` → `"127.0.0.1"` (loopback).
- **`DProject/GlobalData.cpp`** — three hardcoded literal `"26.60.202.44"` → `"127.0.0.1"` (in `_SetAccountIP_Port` and `ToggleLiveServer`). One existing reference still uses the `ACCOUNT_IP` macro, so the define is the single source of truth.

---

## Diagnostics — global error / crash logger

New client-side crash + assert logger so we can post-mortem failures under Wine.

- **`LibProj/CsFunc/CrashLogger.{h,cpp}`** (new) — installs SEH (`SetUnhandledExceptionFilter`), `set_terminate`, `_set_purecall_handler`, `_set_invalid_parameter_handler`. Symbolizes via DbgHelp `SymInitialize` + `EnumProcessModules` + `SymLoadModuleEx`, falls back to module+offset when symbols can't load. Detects Wine via `GetProcAddress(ntdll, "wine_get_version")` and verifies whether the loaded `dbghelp.dll` is the Wine builtin (system32) or a native Microsoft copy in the app dir; only enables `Sym*` calls in the latter case (Wine's builtin asserts inside `dlls/dbghelp/symbol.c`).
- **`LibProj/CsFunc/assert_cs.cpp`** — every `_log()` overload calls `nsCSDEBUG::CrashLogger::LogAssert(file, line, expr, details)` before the original `MessageBoxA`. Korean labels (`소 스`, `라 인`, `조 건`) replaced with English (`Source:`, `Line:`, `Condition:`) so the popup is readable when Korean fonts aren't available.
- **`DProject/main.cpp`** — `nsCSDEBUG::CrashLogger::Init("logs\\crash.log")` at the top of `_tWinMain` (before any object init), so a crash during boot still gets logged.
- **`LibProj/CsFunc/CsFunc.vcxproj`** + **`.filters`** — added `CrashLogger.cpp` / `CrashLogger.h` to ClCompile/ClInclude.
- `dbghelp.lib` is `#pragma comment(lib, ...)` from `CrashLogger.cpp`, so DProject doesn't need to add it to `<AdditionalDependencies>`.

> When running under Wine, drop a 32-bit Microsoft `dbghelp.dll` next to the exe and launch with `WINEDLLOVERRIDES="dbghelp=n,b"` to get symbolized stack traces.

---

## File-table managers — duplicate-row tolerance

Production pack data has duplicate rows in several tables (intentionally, for multi-categorized items). Upstream code asserts on dup-detect and crashes; we demote to log-and-skip (keeping the first mapping):

- **`LibProj/CsFileTable/ItemMng.cpp`** — `m_MapDisp` dup-detect (line ~1340) and `m_MapTypeName` dup-detect (line ~1363) now log via `CrashLogger::LogMessage` and `continue`. Added diagnostic logs of `_LoadFilePack` section offsets / nCount / sizeof for items, categories, cool times, disp map.
- **`LibProj/CsFileTable/SkillMng.cpp`** — `m_mapSkill` dup (line ~796) and `m_mapAreaCheck` dup (lines ~648 and ~817) — same demote pattern.
- **`LibProj/CsFileTable/BaseMng.cpp`** — `m_mapTamer` dup (line ~1675) — same.
- **`LibProj/CsFileTable/QuestMng.cpp:84`** — `assert_csm1( IsQuest( dwQuestID ) == true ... )` commented out. Callers (especially `AchieveContents::CheckType`) handle NULL return correctly when `PATCH_QUESTS_SPINZ` is on.

---

## Boot path — file-pack vs disk-bin mode

- **`DProject/App/GameApp.cpp`** — `Release_English` (`#else` branch, neither `_DEBUG` nor `_GIVE`) used to hardcode `g_eFileType = FT_BIN`, ignoring `dmo.ini`'s `[DEBUG] FilePack=1`. Now matches the `_DEBUG` and `_GIVE` branches: respects `g_bUseFilePack`. Without this fix, file-table loaders try to `fopen` `Data\Bin\<lang>\*.bin` files that only exist inside Pack03 → "[path] 파일이 존재 하지 않습니다." popup.
- **`DProject/App/GameApp.cpp`** — Post-load `g_pQuestMng->Reload(cPath)` block (the `#ifndef _DEBUG / _GIVE / PC_BANG_SERVICE_TEST` block at the end of `Thread_LoadFileTable`) is now skipped when `g_eFileType == FT_FILEPACK`. Previously it always ran in Release_English and tried to disk-`fopen` bins, asserting at `QuestMng.cpp:1366 (fp != NULL)` and cascading into a bResult assert.
- **`DProject/_Lan_English.cpp:137`** — Diagnostic `CsMessageBoxA(MB_OK, "%s\nHashCode : %lu\n\n", str, nRes)` for missing translation lookup commented out; fallback `return L"Translation needed"` changed to `return str`. Pack-shipped strings are already English; the diagnostic was firing for every uncovered phrase.

---

## UI — fixes

- **`DProject/_Interface/Game/MessageBox.cpp`** — Race-handler in `cMessageBox::DelMsg` / `InsertList`. The `cMessageBox` constructor takes ~9ms (texture/font setup) and internally hits an alertable wait that lets the network APC for a server response run on the same thread. With a fast/local server, that response can fire `RecvCreateResult` → `DelMsg(10019)` *before* the original `PrintMsg(10019)`'s `push_back` completes — the box gets inserted *after* its dismissal and then sits forever as "Loading server information". Fix: a static `g_pendingDismissals` set. `DelMsg` records the type when no matching box is found; `InsertList` checks the set after `NiNew` and drops the just-built box if its dismissal was already queued. This was the smoking-gun bug for the post-character-creation stuck-loading-message symptom against a localhost server.
- **`DProject/_Interface/07.CharacterCreate/CharacterCreate.cpp:158`** — Scrollbar `CtrlSize.x` was `18`, but `cScrollBar::Init` asserts `CtrlSize.x == btnSize.x` and `GetDefaultBtnSize()` returns `(16, 15)`. Changed to `16` to match.
- **`DProject/_Interface/00.Contents/ChannelContents.cpp:124`** — `ReceiveChannelIndex(void* pData)` cast `pData` as `int*` (4 bytes), but with `COMPAT_487` defined the source `GS2C_RECV_CURRENT_CHANNEL::channel_idx` is `n2` (2 bytes). The handler was reading 2 bytes of valid data + 2 bytes of stack garbage, which surfaced as a giant garbage number in the dropdown's selected-text. Now cast `short*` under `COMPAT_487`, zero-extend to `unsigned int`.
- **`DProject/_Interface/Base/Text.cpp`** — All 18 `FT_Load_Glyph(face, …, FT_LOAD_DEFAULT)` call sites in the `VERSION_USA` branches now pass `FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT`. FreeType 2.8's autohinter (`af_shaper_get_cluster`) was crashing inside the TrueType bytecode interpreter (`TT_RunIns`) on certain glyphs in the pack-shipped font under Wine. `FT_LOAD_NO_AUTOHINT` keeps the bytecode hinter (so glyphs still snap to pixel grid — text doesn't get cut off) but disables the autohinter chain that triggered the AV.
- **`DProject/_Interface/Base/Text.cpp`** — All 13 `assert_cs( error == 0 )` sites that follow an `FT_Load_Glyph` / `FT_Load_Char` call now skip the rest of the glyph iteration on failure (`if( error != 0 ) continue;` after the assert). Each site is inside a per-character `for` loop; previously a single FT failure left `slot->metrics` / `slot->bitmap` carrying stale data from the previous glyph and the subsequent `FT_Render_Glyph` + `_ReadFTBmp` would render garbage pixels, sometimes cascading into the StringAnalysis dirty-state bug below. assert_cs is kept for logging — the soft assert still records the failure to `crash.log`.
- **`DProject/_Interface/Base/StringAnalysis.cpp`** — All 9 entry points to the parser (the function that builds `m_pString` + `m_listParcing` from a tokenized template) now run a defensive cleanup of `m_pString` and `m_listParcing` before re-allocating: if a previous parse aborted mid-flight, `m_pString` was left non-null and the next call's null-asserts (`m_pString == NULL`) would fire, spamming `crash.log` indefinitely. The defensive cleanup matches the parser's own teardown path (`SAFE_NIDELETE` + per-`sPARCING* DeleteInstance`) so re-entry is now safe regardless of whether the previous call completed cleanly.
- **`DProject/_Interface/Base/String.cpp`** — `cStringList::CalMaxSize` now skips NULL entries instead of dereferencing them. The talk-balloon pool occasionally produces a dangling/NULL `cString*` after a `Delete()`/`Init()` reuse race (cTip::Update random digimon-tip path), and `(*it)->GetMaxSize()+0xa` was crashing on `m_nMaxSize` access. Defensive-only — does not change behaviour for valid lists.
- **`DProject/_Interface/Game/Popup.cpp`** — `_Make_Mainbar_Store()` adds a second "Gift / Reward Box" entry alongside the existing Cash Warehouse button. The new entry uses text-only (no `AddSprite_Head`) because the menu's render code shares one sprite across all entries; adding a second sprite caused only one entry to render. The `_Update_ForMouse_Mainbar_Store()` switch was extended with hover highlight + click handler firing `REWARD_ITEM_TOGGLE` for `WT_REWARD`, mirroring the toggle flow already present in `WareHouseContents.cpp:228-231`.
- **`DProject/_Interface/Game/Popup.cpp`** (`WT_NEW_CASH_WAREHOUSE` click) — fires `EVENT_CODE::CASH_TOGGLE` instead of `CASHWAREHOUSE_TOGGLE`. v487-vintage trim: the original event code (`CASHWAREHOUSE_TOGGLE`) is declared in `GameEventCode.h:80` but has zero `AddEvent` registrations anywhere in the v487 tree, so clicks vanished. The actual listener — `NewCashshopContents::ToggleCashWarehouse` — is bound to `CASH_TOGGLE` in `NewCashshopContents::MakeWorldData:544`. With this fix the click now flows: popup → `CASH_TOGGLE` → `ToggleCashWarehouse` → `Request_CashWarehouseInfo` → `SendReqCashShop` (`pItem::CashShop = 3930`).
- **`DProject/_Interface/Base/StringAnalysis.cpp`** (`_MakeToken` + `_MakeToken_OnlyNewLine`) — disown-then-allocate ordering for `m_pString`. Original code was `m_pStringList->AddTail(m_pString); m_pString = NiNew cString;` — between `AddTail` and the reassign, `m_pString` is shared with the stringList. If `NiNew` throws or downstream FT crashes, `m_pString` keeps the dangling shared pointer; the next `Cut`'s defensive cleanup at line 961-973 then `SAFE_NIDELETE`s a cString already owned by the stringList → use-after-free → crashes later in `cString::AddText` `m_listText.push_back` (`std::_List_node_emplace_op2::_Transfer_before+0xc`) on the `cTip::Update` random digimon-tip path. New ordering: `cString* toAdd = m_pString; m_pString = NULL; m_pStringList->AddTail(toAdd); m_pString = NiNew cString;` — if `NiNew` throws, `m_pString` stays NULL; defensive cleanup then no-ops.

---

## FreeType 2.13 upgrade

- **`Gamebryo2.3/GB_Lib/ThirdPartyLib/freetype213.lib`** + **`LibProj/Freetype/include/`** — upgraded from 2.8 to 2.13.3. Build steps in the `Build / toolchain` section. The 2.8 → 2.13 swap was orthogonal to the font-data crash documented below — same `Ins_MDRP` / `TT_RunIns` AV reproduced on 2.13 against the original pack01 tahoma.ttf, confirming the bug is in the font's bytecode tables, not in FreeType. Upgrade was kept for the 5 years of unrelated FT hardening + better support for modern fonts.
- **`Gamebryo2.3/GB_Lib/ThirdPartyLib/freetype28.lib`** is no longer referenced and can be removed; `freetype239.lib.2.3.9.bak` from the original ship is still around in case anyone wants to compare layouts.
- **`DProject/_Interface/Base/Text.cpp`** (`cText::GetCharWidth`) — kept `FT_LOAD_NO_HINTING` (in addition to `FT_LOAD_NO_AUTOHINT`). Reason: the v487 client's pack01-shipped `Data\font\tahoma.ttf` has glyphs whose TrueType bytecode crashes inside `Ins_MDRP` of the bytecode interpreter on certain inputs. Reproduces on FT 2.8 AND 2.13 → font-data bug. The user's pack01 was repacked with a clean `C:\Windows\Fonts\tahoma.ttf` (via the new `tools/NewTools/PackTool/`); after that the bytecode hinter is safe and `FT_LOAD_NO_HINTING` could be dropped to restore glyph snap. Kept here as belt-and-braces for any future drop-in font that has bad bytecode.

---

## Asset loading — Gamebryo

- **`LibProj/CsGamebryo2.3/CsGBObjectResMng.cpp:406`** — `assert_cs(stream.GetObjectCount() == 1)` was over-strict; production NIF terrain object streams sometimes contain >1 top-level object. Demoted: take the first object as `NiNode`, NULL-check; only fail if the stream is empty or the first object isn't an `NiNode` kind.

---

## Conventions / preferences captured during the session

- v487-vintage flags (`COMPAT_487`, `COMPAT_487_2`, `PATCH_QUESTS_SPINZ`) are added to `Release_English` rather than to a new config — keeps the build matrix minimal.
- "Ignore Themida/Steam/anti-cheat" — `SDM_DEF_XIGNCODE3_20181107`, `DEF_CORE_NPROTECT`, the Steam SDK flags, and `_GIVE` are all kept off in `Release_English`. The `XIGNCODE/` folder in shipped clients can be renamed to keep Wine from loading its xem modules.
- Dup-tolerance in file-table loaders is the standard pattern across `ItemMng`/`SkillMng`/`BaseMng`. New loaders that read pack-shipped tables should follow the same `find ... continue` pattern instead of asserting on uniqueness.
- All 18 `FT_Load_Glyph` sites in `Text.cpp` need the same flags. If you add a 19th, add `FT_LOAD_NO_AUTOHINT` immediately or you'll re-introduce the Wine autohinter AV.
- Don't bulk-`sed -i` on files inside the working tree — go via the IDE / careful per-line edits. (Lost a `vcxproj` to a botched in-place sed during the session; recovered from a tar backup.)
