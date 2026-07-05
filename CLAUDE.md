# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

OJGL is a Windows/OpenGL C++ framework for producing demoscene "demos" (real-time
audiovisual productions synced to music). Each production (`Edison2021`, `Edison2022`,
`Edison2025`, `Eldur`, `QED`, `DodensTriumf`, `InnerSystemLab`) is a `Demo` subclass under
`ojgl/examples/demos/`. Most visual work happens in GLSL fragment shaders, not C++.

## Build & run

Built with **CMake**, targeting **Win32 (x86)**. Builds must run from a **Visual Studio
developer PowerShell** (x86). If your shell isn't already one, enter it first:

```
Import-Module "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -Arch x86 -VsInstallPath "C:\Program Files\Microsoft Visual Studio\2022\Community"
```

There are two generators, each with its own build directory (both are also defined as
CMake presets in `CMakePresets.json`):

- **Visual Studio** → `build/` — for Debug / OptimizedDebug / Release.
- **Ninja Multi-Config** → `build-ninja/` — required for `CrinklerRelease` (Crinkler only works
  with the Ninja/Makefile generators; see the note in the root `CMakeLists.txt`).

Configure once per generator, then build any config:

```
# Visual Studio build tree (build/)
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
cmake --build .\build --config=Debug          # or OptimizedDebug, or Release

# Ninja build tree (build-ninja/), needed for Crinkler
cmake -S . -B build-ninja -G "Ninja Multi-Config"
cmake --build .\build-ninja\ --config=CrinklerRelease
```

Or via presets: `cmake --preset windows-x86` / `windows-x86-ninja` to configure, then
`cmake --build --preset debug` / `crinklerrelease` etc.

This generates `build/OJGL.sln`, which you can also open in Visual Studio (`ojgl` is the
startup project). The build is defined by three `CMakeLists.txt` files: the root (config
types, CRT selection), `tlibc/`, and `ojgl/`. **To add or remove a source file, edit the
`OJGL_SOURCES` list in `ojgl/CMakeLists.txt`** (shaders/songs/headers are globbed for IDE
display only). There are no `.sln`/`.vcxproj` files under version control anymore.

Two targets:
- **`ojgl`** — the framework + demos (main executable).
- **`tlibc`** — a tiny custom C runtime (`libct.lib`). It replaces the standard CRT in the
  Release build so the final binary is small (a demoscene size constraint).

Configurations (the old `Unicode Debug`/`Unicode Release` configs were dropped):
- **Debug / OptimizedDebug** — Console subsystem, standard CRT, enables the `_DEBUG` code
  paths (free camera, timeline scrubbing, hot shader reload, RenderDoc capture). Does **not**
  link `tlibc`.
- **Release** — `/NODEFAULTLIB`, links `tlibc` + `libv2` + DirectX libs, `mainCRTStartup`
  entry, Windows subsystem, size optimization. This is the shippable minimal binary.
- **CrinklerRelease** — like Release, but linked and compressed with Crinkler (a drop-in
  `link.exe` replacement). **Ninja generator only** (`build-ninja/`); the VS generator lets
  MSBuild pick the linker so Crinkler can't be injected there.

**Working directory must be `ojgl/`** when running, because `ShaderReader` resolves shader
paths relative to `examples/shaders/`. The VS debugger working directory is set automatically
(`VS_DEBUGGER_WORKING_DIRECTORY`); from the CLI, run from `ojgl/`. The debug binary is at
`build/ojgl/Debug/ojgl.exe`.

Which demo runs is hardcoded in `ojgl/examples/Main.cpp` (`getDemo(DemoType::...)`). There is
no test suite.

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

**Music sync.** Songs are V2M-synth data (`libv2`, `src/music/`), embedded as byte arrays in
`ojgl/examples/songs/*_song.inc` and returned by `Demo::getSong()`. Music exposes
`syncChannels()`; visuals are beat-synced by feeding channel state into uniforms, by
convention named `C_<channel>_S` (seconds since last note, `getTimeSinceAnyNote()`) and
`C_<channel>_T` (total note hits, `getTotalHits()`). This naming recurs across every demo's
uniform callbacks.

**Shaders & resource embedding.** GLSL fragment shaders (`.fs`) and vertex shaders (`.vs`)
live in `ojgl/examples/shaders/`, with per-demo folders plus a shared `common/` folder.
`ShaderReader` serves shader source by path. In `_DEBUG` it reads from disk and detects
changes (`modified()`) for hot reload; in release nothing is on disk, so every shader is
**preloaded from `ojgl/examples/EmbeddedResources.h`**, which `#include`s each `.fs`/`.vs`
file as a string literal. **When you add or rename a shader, you must also add it to
`EmbeddedResources.h`** or it won't exist in release builds.

**Text.** `TextRenderer` (`examples/TextRenderer.hpp`) rasterizes strings to textures using
Windows GDI fonts (e.g. `getText("BORGILA", "Arial Black")`), passed to shaders as textures.

**Custom standard library.** Do not use `std::` containers/strings. Use `ojstd::` throughout
(`ojstd::string`, `ojstd::vector`, `ojstd::shared_ptr`, `ojstd::function`,
`ojstd::unordered_map`, `ojstd::Pair`, `ojstd::make_shared`) from `src/utility/OJstd.h`, and
`ojstd::sin/cos/clamp/smoothstep` etc. for math. This keeps the release binary independent of
the standard library (linked against `tlibc`).

## Adding a new demo

1. Create `ojgl/examples/demos/X.{h,cpp}` subclassing `Demo` (`src/demo/Demo.h`); implement
   `buildSceneGraph`, `getTitle`, `getSong`.
2. Add shaders under `ojgl/examples/shaders/x/` and register each in `EmbeddedResources.h`.
3. Add the song bytes as `ojgl/examples/songs/x_song.inc`.
4. Register the demo in `Main.cpp` (`DemoType` enum + `getDemo` switch) and select it in
   `main()`.

## Conventions

- Formatting: clang-format, `BasedOnStyle: WebKit`, `IndentWidth: 4` (`.clang-format`).
- `.fs` files are GLSL (associated as `glsl` in the editor).
- Use the `OJ_UNUSED(...)` macro (`src/utility/Macros.h`) for intentionally unused params.
