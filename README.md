# Donkey Kong 64: Recompiled — Android

Android version of [Donkey Kong 64: Recompiled](https://github.com/Rainchus/Donkey-Kong-64-Recompiled), kept as a
direct fork of it so upstream changes merge straight in. It started from
[deivid22srk/dk64-recomp-android](https://github.com/deivid22srk/dk64-recomp-android), the original Android port.
Made for Android handhelds with a built-in controller and phones alike. Tested on the **AYN Odin 2 Portal**
(1920×1080, Android 13).

What this version adds (details in [CHANGELOG.md](CHANGELOG.md)):

- Upstream Donkey Kong 64: Recompiled (game version 1.0.3) — branch `main` follows upstream, `android` is this port
- App ID `io.github.almostdrownedrat.dk64` ("DK64 Recompiled") — installs next to other DK64 apps
- **Touch buttons** (N64 layout: stick, A, B, Z, L, R, START, C buttons; D-pad hidden by default). The
  **controller icon** at the top centre opens their settings: **Touch buttons Automatic / Always / Off**
  (Automatic hides them while a controller is used and a touch brings them back — on a handheld you normally
  never see them), move and resize, button size, show / hide single buttons, opacity, stick sensitivity, reset.
  Touches off the buttons still work as a mouse in the menus
- Options menu on Android's **Back** (swipe from the edge to show the system bar and tap back, or the back
  gesture), in game; Back closes it again
- Real full screen, no black strip from the navigation bar
- Fixed audio crackling and pitch jumps
- Fixed a frozen picture after overlays and the missing restart after loading a ROM

### AI assistance

The changes in this Android version were made with AI assistance ([Claude Code](https://claude.com/claude-code)). Planning,
testing on the device and the decision what gets released are by the repository owner. Tested on the
**AYN Odin 2 Portal** — use at your own risk. This applies only to this version's own changes, not to the code of the
original projects.

> ⚠️ **No game assets.** The APK on the [Releases](../../releases) page contains only this project's code (like the
> upstream and original port releases): you need your own legally obtained ROM of **Donkey Kong 64 (USA, NTSC-U 1.0)**,
> sha1 `cf806ff2603640a748fca5026ded28802f1f4a50`, which the app asks for on the first start ("Load ROM").
> Building it yourself needs that ROM too (the recompiler reads it): the steps are in
> [.github/workflows/build.yml](.github/workflows/build.yml).

Branch `android` is this port; `main` mirrors upstream. For installing, the GPU driver
menu (Turnip) and technical notes see the [original port's README](https://github.com/deivid22srk/dk64-recomp-android#readme); the Turnip
integration is described in [docs/DRIVER-VULKAN.md](docs/DRIVER-VULKAN.md), the build architecture in
[DESIGN.md](DESIGN.md).

## Credits and license

This Android version:
- [deivid22srk/dk64-recomp-android](https://github.com/deivid22srk/dk64-recomp-android) — the Android port
  this version is based on (NDK build, SDLActivity integration, SAF file bridge, libadrenotools/Turnip support,
  lifecycle handling)
- [thorskin6657-sudo/dk64-recomp-android](https://github.com/thorskin6657-sudo/dk64-recomp-android) — reference
  for the separate app identity
- [GreenNaugahyde/ZeldaWWHDRecompAndroid](https://github.com/GreenNaugahyde/ZeldaWWHDRecompAndroid) —
  full-screen approach (`hideSystemBars`, MPL-2.0), via the Wind Waker Odin fork

Game and runtime:
- [Rainchus/Donkey-Kong-64-Recompiled](https://github.com/Rainchus/Donkey-Kong-64-Recompiled) — DK64: Recompiled
  by Rainchus, Ballaam, Killklli, Green Bean and UmedMuzl (see [README-UPSTREAM.md](README-UPSTREAM.md))
- [N64Recomp](https://github.com/N64Recomp/N64Recomp) by Mr-Wiseguy — static recompiler
- [N64ModernRuntime](https://github.com/N64Recomp/N64ModernRuntime) (fork used: [Killklli](https://github.com/Killklli/N64ModernRuntime))
- [RecompFrontend](https://github.com/Killklli/RecompFrontend) — launcher and config menus (UI design by thecozies)
- [RT64](https://github.com/rt64/rt64) by Dario (DarioSamo) — renderer, with [plume](https://github.com/renderbag/plume)
- [Donkey Kong 64 decompilation](https://gitlab.com/dk64_decomp/dk64) — headers and ROM tools
- [libadrenotools](https://github.com/bylaws/libadrenotools) by bylaws — custom Vulkan drivers
- [SDL2](https://www.libsdl.org/), [RmlUi](https://github.com/mikke89/RmlUi), [lunasvg](https://github.com/sammycage/lunasvg),
  [zstd](https://github.com/facebook/zstd) and the other libraries listed in the submodules

License: GPL-3.0, see [LICENSE](LICENSE) / [COPYING](COPYING). Source code of all changes is in this repository.

*Fan project, not affiliated with Nintendo or Rare. Donkey Kong 64 is © Nintendo / Rare.*
