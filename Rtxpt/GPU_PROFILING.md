# GPU stage timing

Build `Rtxpt` in Release, then run from `bin`:

```powershell
.\Rtxpt.exe --d3d12 --nonInteractive --scene convergence-test.scene.json --width 1920 --height 1080 --overrideToRealtimeMode --realtimeAA 3 --stopAnimations --useReSTIRDI 0 --useReSTIRGI 0 --gpuProfileCSV C:/Temp/upstream.csv
```

Create the output directory first. `--gpuProfileCSV` enables an eight-frame ring of
NVRHI timer queries. On D3D12 these are timestamp pairs, including around Streamline
RR. Results are polled before reading; a busy ring drops a sample instead of waiting
for the GPU. CSV values are milliseconds, and an unexecuted stage is `-1`.
The file is flushed every 120 collected frames. Profiling keeps rendering while
the window is unfocused; normal execution keeps its original focus behavior.
With v1.9.0, CSV profiling disables the async compute prefix so timer queries
remain on the graphics queue. The upstream Tracy profiler remains available.

Stage boundaries match the Diligent port's path-trace/post-process interval:

| Column | Work |
| --- | --- |
| Total | Before `SampleRenderCode` through final framebuffer blit |
| BUILD | Denoising-layer ray dispatch and its surrounding resource-state commands |
| LightsBaker | `LightsBaker::UpdateEnd` |
| FILL | Entire path-trace sample loop, including dispatch state and barriers |
| Guides | Specular hit-distance denoise and optional guide debug visualization |
| RRPrepare | `PrepareDLSSRRGuides` |
| RR | RR resource tagging/transitions, evaluation and state cleanup |
| PostProcess | Pre-tone-map effects through tone mapping and final blit |
| VBufferExport | Visibility-buffer export, executed only for depth/motion debug views |

`Total` excludes scene/AS preparation, `UpdateLighting`, UI and Present. It is not
the window-title frame time. It includes gaps between the named stages. The port
does not have a separate VBufferExport dispatch. PostProcess includes upstream
debug/zoom work if enabled, so disable visualization when comparing renderers.
Resource-recreation frames and the first 16 application frames are not sampled.
The v1.9.0 integration was built in Release and smoke-tested on D3D12 with
DLSS-RR at 640x360 using `convergence-test.scene.json`: automatic capture exited
successfully and produced 47 timing rows. Vulkan timing remains unverified.

At application frame 120, `<csv>.settings.txt` records the actual camera, resolution,
bounce/sample counts, NEE settings, denoising-layer count, ReSTIR state and RR preset.
Use this to verify the configuration instead of relying only on command-line text.
`--neeCandidateSamples N` overrides the candidate count after UI presets, before
shader compilation; values are clamped to the supported range. Omit it to retain
the ordinary defaults. `--NEEType 0/1/2` selects Uniform/Power/NEE-AT, and
`--useNEE 0` provides a diagnostic no-NEE comparison.

Warm up for at least 30 seconds after shader compilation, then collect at least
1000 more frames, one renderer at a time. Check that frame IDs advance and no
unexpected `-1` values occur. Compare GPU intervals rather than a stale FPS title.

The 2026-09-15 comparison and controlled experiments are documented in the port's
`DiligentSamples/Samples/RTXPT/tests/GPU_PERFORMANCE_COMPARISON.md`.
