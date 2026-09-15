// Copyright (c) 2026, NVIDIA CORPORATION. All rights reserved.
#pragma once

#include <nvrhi/nvrhi.h>
#include <donut/core/log.h>
#include <array>
#include <fstream>
#include <iomanip>

// NVRHI D3D12 timer queries use timestamp pairs, including across Streamline.
// Poll completed frames without waiting; drop samples if the ring is full.
class GpuProfiler
{
public:
    enum class Stage { Total, Build, LightsBaker, Fill, Guides, RRPrepare, RR, PostProcess, VBufferExport, Count };

    void BeginFrame(nvrhi::IDevice* device, nvrhi::ICommandList* commandList, const std::string& csvPath)
    {
        if (csvPath.empty()) return;
        if (!m_initialized)
        {
            m_initialized = true;
            m_csv.open(csvPath);
            if (!m_csv) { donut::log::error("Cannot open GPU timing CSV: %s", csvPath.c_str()); return; }
            for (auto& frame : m_frames)
                for (auto& query : frame.queries)
                {
                    query = device->createTimerQuery();
                    if (!query) { donut::log::error("GPU timer query allocation failed"); return; }
                }
            m_csv << "Frame";
            for (auto name : c_Names) m_csv << ',' << name << "_ms";
            m_csv << '\n' << std::fixed << std::setprecision(6);
            m_available = true;
        }
        if (!m_available) return;
        Collect(device);
        ++m_frameNumber;
        for (size_t i = 0; i < m_frames.size(); ++i)
        {
            auto& frame = m_frames[i];
            if (frame.pending) continue;
            m_current = int(i);
            frame.number = m_frameNumber;
            frame.ended.fill(false);
            for (auto& query : frame.queries) device->resetTimerQuery(query);
            Begin(commandList, Stage::Total);
            return;
        }
        ++m_dropped;
    }

    void Begin(nvrhi::ICommandList* commandList, Stage stage)
    {
        if (m_current >= 0) commandList->beginTimerQuery(m_frames[m_current].queries[size_t(stage)]);
    }
    void End(nvrhi::ICommandList* commandList, Stage stage)
    {
        if (m_current < 0) return;
        auto& frame = m_frames[m_current];
        commandList->endTimerQuery(frame.queries[size_t(stage)]);
        frame.ended[size_t(stage)] = true;
        if (stage == Stage::Total) { frame.pending = true; m_current = -1; }
    }

private:
    static constexpr size_t c_StageCount = size_t(Stage::Count);
    static constexpr std::array<const char*, c_StageCount> c_Names = {
        "Total", "BUILD", "LightsBaker", "FILL", "Guides", "RRPrepare", "RR", "PostProcess", "VBufferExport" };
    struct Frame
    {
        std::array<nvrhi::TimerQueryHandle, c_StageCount> queries;
        std::array<bool, c_StageCount> ended{};
        uint64_t number = 0;
        bool pending = false;
    };
    void Collect(nvrhi::IDevice* device)
    {
        for (auto& frame : m_frames)
        {
            if (!frame.pending) continue;
            bool ready = true;
            for (size_t i = 0; i < c_StageCount; ++i)
                if (frame.ended[i]) ready &= device->pollTimerQuery(frame.queries[i]);
            if (!ready) continue;
            m_csv << frame.number;
            for (size_t i = 0; i < c_StageCount; ++i)
                m_csv << ',' << (frame.ended[i] ? 1000.0 * device->getTimerQueryTime(frame.queries[i]) : -1.0);
            m_csv << '\n';
            frame.pending = false;
            if (++m_collected % 120 == 0)
            {
                m_csv.flush();
                donut::log::info("GPU profiling: collected=%llu dropped=%llu", (unsigned long long)m_collected, (unsigned long long)m_dropped);
            }
        }
    }
    std::array<Frame, 8> m_frames;
    std::ofstream m_csv;
    uint64_t m_frameNumber = 0, m_collected = 0, m_dropped = 0;
    int m_current = -1;
    bool m_initialized = false, m_available = false;
};
