#pragma once

#include <chrono>
#include <array>
#include <string>
#include <iostream>

#include "spdlog/spdlog.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/stdout_color_sinks.h>




enum class GCTimerID {
    Pause,
    Mark,
    Sweep,
    Compact,
    Copy,
    Count // Always last!
};

class GCTimerManager {
public:
    GCTimerManager() {
        auto timer_logger = spdlog::stdout_color_mt("GC_TIMER");			// stdout_color_st() for single threaded
        timer_logger->set_pattern("[%^%n%$] %v");
    }

    void start(GCTimerID id) {
        starts[static_cast<size_t>(id)] = clock::now();
    }

    void stop(GCTimerID id) {
        auto end = clock::now();
        auto idx = static_cast<size_t>(id);
        durations[idx] += end - starts[idx];
    }

    void reset() {
        durations.fill(duration_t::zero());
    }

    double elapsed(GCTimerID id) const {
        return std::chrono::duration<double>(durations[static_cast<size_t>(id)]).count();
    }

    void report() const {
        auto logger = spdlog::get("GC_TIMER");
        const char* names[] = { "Pause", "Mark", "Sweep", "Compact", "Copy"};
        for (size_t i = 0; i < static_cast<size_t>(GCTimerID::Count); ++i) {
            auto dur = std::chrono::duration<double, std::milli>(durations[i]).count();
            if (dur > 0)
            {
                logger->info("{}: {}ms", names[i], dur );
            }
        }
    }

    std::array<std::chrono::steady_clock::duration, static_cast<size_t>(GCTimerID::Count)>* GetDurations() { return &durations; }

private:
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration_t = std::chrono::steady_clock::duration;

    std::array<time_point, static_cast<size_t>(GCTimerID::Count)> starts;
    std::array<duration_t, static_cast<size_t>(GCTimerID::Count)> durations{};
};

class ScopedGCTimer {
public:
    ScopedGCTimer(GCTimerManager& mgr, GCTimerID id)
        : manager(mgr), timerID(id) {
        manager.start(timerID);
    }
    ~ScopedGCTimer() {
        manager.stop(timerID);
    }
private:
    GCTimerManager& manager;
    GCTimerID timerID;
};