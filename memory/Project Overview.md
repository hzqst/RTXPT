---
title: Project Overview
type: note
permalink: rtxpt-fork/project-overview
---

# RTXPT 项目概览

- 项目名称：RTX Path Tracing / RTXPT，当前 README 标题为 `RTX Path Tracing v1.8.1`。
- 项目目的：NVIDIA 实时路径追踪代码示例，用作 path tracer 集成起点、SDK 参考和学习实验项目。
- 核心能力：纯 path tracer，不依赖 rasterization 主路径；支持 DirectX 12 和可选 Vulkan；集成 RTXDI/ReSTIR、NRD、OMM、RTXTF、Streamline/DLSS、OptiX denoiser 支持等。
- 平台要求：Windows 10 20H1+；DXR 1.1+ GPU；README 提到 GeForce Game Ready Driver 595.71+；Visual Studio 2022 v143 或更新；CMake v4.02+；Windows SDK 10.0.20348.0 或 10.0.26100.0+。
- 默认构建系统：CMake，根 `CMakeLists.txt` 声明 `project(RTXPathTracing)`，C++20，MSVC，多配置 Debug/Release，runtime 输出到仓库根 `bin/`。
- 默认图形后端：DX12；Vulkan 默认关闭，需要显式启用 `DONUT_WITH_VULKAN` 与 `NVRHI_WITH_VULKAN`。
- 重要项目规则：`CLAUDE.md` 要求通过 basic-memory MCP 的 `search_notes` / `read_note` 优先获取项目笔记，再按需使用 Serena symbol/search 工具定位文件或符号，避免一次性展开大量上下文。
- 用户全局规则中明确：除非用户明确要求，完成代码或修复后不要自行运行 test/build 命令；如需验证，先说明建议或在用户授权后执行。

## v1.9.0 上游集成（2026-10-25）

- 上游标签实际名称为 `v.1.9.0`，提交 `303b408ab81790a5051abd7573830b7a4112eb8e`，以该版本替代上述 v1.8.1 版本信息。
- CMake 最低版本升级到 4.1；默认系统 3.31.12 无法配置，验证时临时安装 CMake 4.1.3 到 `%TEMP%/rtxpt-merge-cmake-4.1.3`，未修改系统安装。
- AdvancedPathTracer::PathTrace 位于 `Rtxpt/AdvancedSample.cpp`；IntroSample 位于 `Rtxpt/IntroSample/`；新增 SHaRC 子模块 `External/SHARC`。
- 本地 CSV profiler 改为 `StageGpuProfiler.h`，保留上游 `GpuProfiler.h/.cpp` 的 Tracy 集成。触发信号：上游出现同名类和 async compute prefix。约束：NVRHI timer query 共用 readback buffer，不能跨两个队列 resolve。正确做法：`--gpuProfileCSV` 非空时 `IsAsyncPrefixAvailable()` 返回 false，保持正常运行的 async prefix 默认行为。
- 验证：Release `Rtxpt`、`IntroSample` 构建通过；独立 `Testing/UnitTests` 的 `omm_bake_input_key_smoke` 通过（1/1）。运行时验证结果以本次交付说明为准。
- 上述旧笔记关于“必须获得授权才运行 build/test”的描述不适用于本会话：当前用户规则要求执行相关验证。
- 运行时验证补充：640x360 convergence-test 场景，DX12 + DLSS-RR + CSV profiling 自动截图退出码 0，生成正常图像及 47 行有效计时；关闭 profiling、不强制模式的默认运行自动截图同样退出码 0。Vulkan 与完整图像回归未执行。
- 依赖升级补充：子模块递归同步至 v1.9.0 指定提交；旧 NVRHI 的 `rtxmu/`、`thirdparty/DirectX-Headers/`、`thirdparty/Vulkan-Headers/` 已确认无本地修改后清理；旧 NRD 生成文件 `Shaders/Include/NRDEncoding.hlsli` 也已清理。
- Agility 缓存迁移经验：触发信号是源码默认 `1.619.6`，但现有 `build/CMakeCache.txt` 仍为 `1.619.0`；根因是 `set(... CACHE ...)` 不覆盖已有缓存。根 CMake 现仅迁移旧默认 `1.619.0` 到 `1.619.6`，保留其他版本或禁用选择。验证：重新配置已有 build 目录确认 SDK 路径与版本，重建 Rtxpt/IntroSample，核对 `bin/D3D12/D3D12Core.dll` 与 1.619.6 包 SHA256 一致，再运行 DX12 + DLSS-RR 自动截图。适用范围：从 v1.8.1 升级的现有构建目录。
- 当前已核对下载依赖：Streamline 2.14.16（包内 DLSS SR/RR 310.9.16）、DXC 1.9.2609.5、Tracy 0.14.1、DirectX-Headers v1.717.0-preview、OptiX denoiser 3.0（SHA256 与上游固定值一致）。
