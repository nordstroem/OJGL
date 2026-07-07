# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

OJGL is a Windows/OpenGL C++ framework for producing demoscene "demos" (real-time
audiovisual productions synced to music). Each production (`Edison2021`, `Edison2022`,
`Edison2025`, `Edison2026`, `Eldur`, `QED`, `DodensTriumf`, `InnerSystemLab`, plus the
`Template` starting point) is a `Demo` subclass living in its own self-contained directory
under `productions/<name>/` — its C++, shaders, and music together. **Exactly one production is
compiled per build**, selected at configure time (see `OJGL_DEMO` below), so a release binary
contains only the files that production needs. Most visual work happens in GLSL fragment
shaders, not C++.

## Build & run

Built with **CMake**, targeting **Win32 (x86)**. Builds must run from a **Visual Studio
developer PowerShell** (x86). If your shell isn't already one, enter it first:

```
Import-Module "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -Arch x86 -VsInstallPath "C:\Program Files\Microsoft Visual Studio\2022\Community"
```

**Which production to build is chosen at configure time** with the `OJGL_DEMO` cache variable,
whose value is the directory name under `productions/` (e.g. `edison2026`, `edison2025`, `qed`,
`template`). It defaults to `edison2026`. The selected production declares its own music
backend (V2 or Clinkster), so there is no separate synth switch. Reconfigure to switch demos:
`cmake -S . -B build -DOJGL_DEMO=edison2025`.

There are two generators, each with its own build directory (both are also defined as
CMake presets in `CMakePresets.json`):

- **Visual Studio** → `build/` — for Debug / OptimizedDebug / Release.
- **Ninja Multi-Config** → `build-ninja/` — required for `CrinklerRelease` (Crinkler only works
  with the Ninja/Makefile generators; see the note in the root `CMakeLists.txt`).

Configure once per generator (pick the demo here), then build any config:

```
# Visual Studio build tree (build/); omit -DOJGL_DEMO to get the default (edison2026)
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DOJGL_DEMO=edison2025
cmake --build .\build --config=Debug          # or OptimizedDebug, or Release

# Ninja build tree (build-ninja/), needed for Crinkler
cmake -S . -B build-ninja -G "Ninja Multi-Config" -DOJGL_DEMO=edison2026
cmake --build .\build-ninja\ --config=CrinklerRelease
```

Or via presets: `cmake --preset windows-x86` / `windows-x86-ninja` to configure (add
`-DOJGL_DEMO=<name>` to pick a demo), then `cmake --build --preset debug` / `crinklerrelease`.

This generates `build/OJGL.sln`, which you can also open in Visual Studio (`ojgl` is the
startup project). The build is defined by: the root `CMakeLists.txt` (config types, CRT
selection, Crinkler wiring, and the `ojgl` executable), `cmake/OjglDemo.cmake` (the
`ojgl_add_demo` function + resource generation), `src/thirdparty/tlibc/CMakeLists.txt`, and one
`productions/<name>/CMakeLists.txt` per demo. **To add or remove a framework/app source file,
edit the `OJGL_SOURCES` list in the root `CMakeLists.txt`**; a demo's own sources are listed in
its `productions/<name>/CMakeLists.txt` via `ojgl_add_demo`. Shaders/songs/headers are globbed
for IDE display only. There are no `.sln`/`.vcxproj` files under version control.

Two targets:
- **`ojgl`** — the framework + selected demo (main executable).
- **`tlibc`** — a tiny custom C runtime (`libct.lib`) at `src/thirdparty/tlibc/`. It replaces
  the standard CRT in the Release build so the final binary is small (a demoscene size
  constraint).

Configurations (the old `Unicode Debug`/`Unicode Release` configs were dropped):
- **Debug / OptimizedDebug** — Console subsystem, standard CRT, enables the `_DEBUG` code
  paths (free camera, timeline scrubbing, hot shader reload, RenderDoc capture). Does **not**
  link `tlibc`.
- **Release** — `/NODEFAULTLIB`, links `tlibc` + the demo's synth (`libv2` or the assembled
  Clinkster object) + DirectX libs, `mainCRTStartup` entry, Windows subsystem, size
  optimization. This is the shippable minimal binary.
- **CrinklerRelease** — like Release, but linked and compressed with Crinkler (a drop-in
  `link.exe` replacement). **Ninja generator only** (`build-ninja/`); the VS generator lets
  MSBuild pick the linker so Crinkler can't be injected there.

The **working directory no longer matters** at runtime: in debug, `ShaderReader` reads shaders
via absolute paths generated at configure time; in release they are embedded. The debug binary
is at `build/Debug/ojgl.exe`. There is no test suite.

Debug-only runtime controls (in `Main.cpp`): Esc quit · ←/→ seek ±5s · ↑/↓ prev/next scene ·
Space pause · R restart · P capture frame (RenderDoc) · C log camera position. `_DEBUG` builds
use `FreeCameraController` for a fly-through camera; release builds use scripted camera paths
in the demo's `update()`.

## Architecture

**Scene graph of shader passes.** A `Demo::buildSceneGraph()` returns `vector<Scene>`, played
sequentially. Each `Scene` wraps a final `Buffer` plus a `Duration` and a name. A `Buffer`
(`src/render/Buffer.{h,cpp}`) is one fragment-shader pass rendering a fullscreen quad (or
meshes) into an FBO. Buffers are wired into a DAG:
- `setInputs(a, b, ...)` — output textures of other buffers, bound as samplers in this pass.
- `setFeedbackInputs(self)` — the buffer's own *previous frame* (for trails/feedback effects).
- `setRenderOnce(true)` — render a static buffer (e.g. noise) only once.

Post-processing (blur, chromatic aberration, radial blur) is done by chaining buffers, e.g.
`scene shader → blur1 → blur2 → chrom_ab`. The `Edison2025` demo is the fullest example of
this pattern.

**Per-frame data into shaders.** Attach callbacks to a buffer:
- `setUniformCallback` returns a `UniformVector` (e.g. `Uniform1f`, `Uniform2f`,
  `UniformMatrix4fv`) rebuilt every frame.
- `setTextureCallback` returns `Uniform1t` texture bindings.
- `iCameraMatrix` uniform comes from `FreeCameraController::instance().getCameraMatrix()`.

**Music sync (two backends).** Each production declares `SYNTH V2` or `SYNTH CLINKSTER` in its
`ojgl_add_demo` call. **V2**: V2M-synth data (`libv2`, `src/music/`) embedded as a byte array in
`productions/<name>/music/*_song.inc` and returned by `Demo::getSong()`. **Clinkster**: the song
is baked into build-time assembly (`productions/<name>/music/song.asm`, `%include`d by the
shared `src/thirdparty/clinkster/clinkster.asm` and assembled by yasm/nasm); the demo does not
override `getSong()`. The backend is preprocessor-exclusive (`OJGL_SYNTH_V2` /
`OJGL_SYNTH_CLINKSTER`), so a binary uses exactly one. Either way, Music exposes
`syncChannels()`; visuals are beat-synced by feeding channel state into uniforms, by convention
named `C_<channel>_S` (seconds since last note, `getTimeSinceAnyNote()`) and `C_<channel>_T`
(total note hits, `getTotalHits()`). This naming recurs across every demo's uniform callbacks.

**Shaders & resource embedding (generated).** GLSL fragment shaders (`.fs`) and vertex shaders
(`.vs`) live in `productions/<name>/shaders/` plus the shared `productions/common/shaders/`;
each `.fs`/`.vs` wraps its source in an `R""( … )""` raw-string literal. `ShaderReader` serves
shader source by a virtual path (`<prefix>/<file>`, e.g. `edison2026/cube.fs`, `common/quad.vs`)
and resolves `#include` directives lazily, so registration order does not matter. **Each demo
declares exactly which shaders it embeds** — there is no globbing, so a production ships only
the files it actually uses (this matters most for the shared `common/` folder). In
`ojgl_add_demo`, list the demo's own shaders in `SHADERS` (filenames under the demo's
`shaders/` folder) and the shared ones it needs in `COMMON_SHADERS` (filenames under
`productions/common/shaders/`). From those two lists `ojgl_add_demo` generates
`EmbeddedShaders.h` (in `build/generated/`) — the release embed list plus, for debug, a
virtual→absolute-path map used for hot reload (`modified()`). A listed file that doesn't exist
is a configure-time error. When you add a `.fs`/`.vs`, add its name to the relevant list and
reconfigure. The virtual prefix defaults to the lowercased demo name; override with
`SHADER_PREFIX` (e.g. QED uses `QED`).

**Text.** `TextRenderer` (`src/app/TextRenderer.hpp`) rasterizes strings to textures using
Windows GDI fonts (e.g. `getText("BORGILA", "Arial Black")`), passed to shaders as textures.

**Custom standard library.** Do not use `std::` containers/strings. Use `ojstd::` throughout
(`ojstd::string`, `ojstd::vector`, `ojstd::shared_ptr`, `ojstd::function`,
`ojstd::unordered_map`, `ojstd::Pair`, `ojstd::make_shared`) from `src/utility/OJstd.h`, and
`ojstd::sin/cos/clamp/smoothstep` etc. for math. This keeps the release binary independent of
the standard library (linked against `tlibc`).

## Adding a new demo

Create a self-contained `productions/<name>/` directory:

1. `productions/<name>/X.{h,cpp}` subclassing `Demo` (`src/demo/Demo.h`); implement
   `buildSceneGraph`, `getTitle`, and (for V2) `getSong`.
2. `productions/<name>/shaders/*.fs` / `*.vs` for demo-specific shaders (shared ones live in
   `productions/common/shaders/`). List each shader you use in the demo's `ojgl_add_demo` call
   (`SHADERS` for the demo's own, `COMMON_SHADERS` for shared ones) — nothing is embedded unless
   listed.
3. Music: **V2** → `productions/<name>/music/<name>_song.inc`, `#include`d by the demo `.cpp`
   into a byte array. **Clinkster** → `productions/<name>/music/song.asm`.
4. `productions/<name>/CMakeLists.txt` with a single `ojgl_add_demo(NAME X SYNTH V2|CLINKSTER
   SOURCES X.cpp SHADERS ... COMMON_SHADERS ...)` call (see `cmake/OjglDemo.cmake` for options:
   `SHADERS`, `COMMON_SHADERS`, `SHADER_DIR`, `SHADER_PREFIX`, `MUSIC`, `HEADER`). The class name
   is `NAME`; the header defaults to `<NAME>.h`.
5. Build it with `-DOJGL_DEMO=<name>`. No changes to `Main.cpp` or the root `CMakeLists.txt` are
   needed — selection is by directory name.

## Conventions

- Formatting: clang-format, `BasedOnStyle: WebKit`, `IndentWidth: 4` (`.clang-format`).
- `.fs` files are GLSL (associated as `glsl` in the editor).
- Use the `OJ_UNUSED(...)` macro (`src/utility/Macros.h`) for intentionally unused params.
