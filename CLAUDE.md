# CLAUDE.md

This file guides Agent Coding in this repository using a "progressive disclosure" approach: prioritize retrieving high-level information from Basic Memory notes first, then locate and read specific files/symbols only when needed, instead of expanding a large amount of context at once.

## Basic Memory (keep context concise)
1. Project knowledge notes live in `memory/` (tracked in git) and are served by the `basic-memory` MCP server configured in `.mcp.json` (project `rtxpt-fork`).
2. Use `search_notes` to discover notes and `read_note` only for the specific notes required by the current task (do not read all notes by default).
3. If note content is missing, stale, or insufficient, switch to targeted repository reads or symbol/search-based lookup, then maintain note quality with `write_note`, `edit_note`, or `delete_note`.
4. Persist notes through the `basic-memory` MCP tools (or `uvx basic-memory tool ...`); do not hand-edit the `.md` files, since frontmatter (`title`/`type`/`permalink`) is tool-managed.

## High-level information in this repository (read corresponding notes first)
- `Project Overview`: project purpose, platform requirements, build system, default backend, and repository-level working rules.
- `Architecture Entrypoints`: top-level directory map, CMake/build entry points, runtime entry points, and shader entry points.
- `Style and Conventions`: C++/HLSL style, naming conventions, formatting expectations, and CPU/GPU contract cautions.
- `Suggested Commands`: common PowerShell commands for navigation, CMake configuration/build, runtime invocation, and image regression scripts.
- `Task Completion Checklist`: completion checklist, verification expectations, and cautions around build/test execution in this repository.
- Module-specific topics (for example `PathTrace`, `RTXDI`, `NEE`, `Denoise`, `DLSS RR`, `Rendering Pipeline`) as separate notes when needed.

## "Source entry points" when notes are insufficient (query and read on demand)
- Build configuration: start from `CMakeLists.txt`, then `Rtxpt/CMakeLists.txt` for the `RtxptCore` library, shader compilation, and `Rtxpt` executable target.
- Runtime startup: inspect `Rtxpt/AdvancedSample.cpp` for `WinMain` / `main`, `AdvancedSample`, and `AdvancedPathTracer`.
- Main application flow: use Serena symbol lookup on `Rtxpt/Sample.h` and `Rtxpt/Sample.cpp` for scene loading, acceleration structures, lighting, path tracing, denoising, post-processing, and render-loop hooks.
- Command-line behavior: inspect `Rtxpt/SampleCommon/CommandLine.cpp` and related `SampleBaseApp` code only when command parsing or app initialization details are needed.
- Shader pipeline: start from `Rtxpt/shaders.cfg`, `Rtxpt/Shaders/PathTracerSample.hlsl`, and `Rtxpt/Shaders/PathTracer/PathTracer.hlsli`; then follow includes with Serena/search only for the specific shader feature under investigation.
- Feature modules: use targeted lookup under `Rtxpt/RTXDI`, `Rtxpt/NRD`, `Rtxpt/OpacityMicroMap`, `Rtxpt/Lighting`, `Rtxpt/Materials`, `Rtxpt/ProcessingPasses`, `Rtxpt/ToneMapper`, and `Rtxpt/Misc` based on the feature being changed.
- Tests and support scripts: start from `Support/tests/run_tests.ps1`, `_run.ps1`, `_1_render.ps1`, `_2_compare.ps1`, and `tests.json` when image regression behavior is relevant.
- External dependencies: inspect `External/` only after checking notes and local integration points; prefer dependency documentation/tools for API details instead of reading vendored sources broadly.

## Progressive disclosure key points
- Read notes first, then locate a single file/symbol; do not read the whole repository at once.
- Prefer Serena for code exploration (symbol overview/references/search), and read file contents only when necessary.
- Prefer Context7 for external dependency/library usage (query on demand).

## Important rules
- Project memory is served by the `basic-memory` MCP server; query it with `search_notes`/`read_note` before broad repository scans.
- Run Serena's `activate_project` at session start when using Serena MCP tools for code navigation.
