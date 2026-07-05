# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

OJGL is a Windows/OpenGL C++ framework for producing demoscene "demos" (real-time
audiovisual productions synced to music). Each production (`Edison2021`, `Edison2022`,
`Edison2025`, `Eldur`, `QED`, `DodensTriumf`, `InnerSystemLab`) is a `Demo` subclass under
`ojgl/examples/demos/`. Most visual work happens in GLSL fragment shaders, not C++.

## Build & run

Built with Visual Studio via `OJGL.sln`. There is no CMake/makefile — open the solution or
build with `msbuild OJGL.sln`. All configurations target **Win32 (x86)** even though the
platform dropdown shows x64.

Two projects:
- **`ojgl`** — the framework + demos (main executable).
- **`tlibc`** — a tiny custom C runtime. It replaces the standard CRT in size-optimized
  builds so the final release binary is small (a demoscene size constraint).

Configurations:
- **Debug / OptimizedDebug** — Console subsystem, standard CRT, enables the `_DEBUG` code
  paths (free camera, timeline scrubbing, hot shader reload, RenderDoc capture).
- **Release / Unicode Release** — `IgnoreAllDefaultLibraries`, links `tlibc` + `libv2` +
  DirectX libs, `mainCRTStartup` entry, Windows subsystem, whole-program optimization. This
  is the shippable minimal binary.

**Working directory must be `ojgl/`** when running (see `.vscode/launch.json`), because
`ShaderReader` resolves shader paths relative to `examples/shaders/`. The debug binary is at
`bin/ojgl/Win32/Debug/ojgl.exe`.

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
