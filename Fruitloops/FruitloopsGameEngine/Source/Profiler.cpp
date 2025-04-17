/******************************************************************************/
/*!
\file    Profiler.cpp
\Proj    Shroomy Doomy
\author  Foo Ming Sheng
\date    Nov 08, 2024
\brief   Implements the Profiler class to measure performance of various
         systems in the game. Tracks and logs the duration each system takes
         per frame, providing average times and percentages of total frame time.

         Start and Stop functions mark time for individual systems, while
         StartFrame resets timings. LogResults outputs performance data for
         each system to the game loop log.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "Profiler.h"
#include "Log.h"  // Assuming LogGameLoop is in this or similar

/**
 * @brief Starts timing for a specific system.
 *
 * Records the current time as the start time for the specified system. This
 * method should be called when the system begins its processing.
 *
 * @param systemName The name of the system for which to start the timer.
 */
void Profiler::Start(const std::string& systemName) {
    startTimes[systemName] = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Stops timing for a specific system and calculates the duration.
 *
 * Captures the current time as the end time for the specified system and calculates
 * the duration since the last Start call for that system. Adds the duration to the
 * total time spent on that system and increments its frame count.
 *
 * @param systemName The name of the system for which to stop the timer.
 */
void Profiler::Stop(const std::string& systemName) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = endTime - startTimes[systemName];

    systems[systemName].totalDuration += duration;
    systems[systemName].frameCount++;
}

/**
 * @brief Resets the profiler to start timing for a new frame.
 *
 * Sets the duration of the frame to zero and resets the accumulated duration for each
 * system to zero. This method should be called at the start of each frame to ensure
 * accurate per-frame timing and analysis.
 */
void Profiler::StartFrame() {
    frameDuration = std::chrono::high_resolution_clock::duration::zero();
    // Reset each system's duration for the current frame
    for (auto& [systemName, data] : systems) {
        data.totalDuration = std::chrono::high_resolution_clock::duration::zero();
    }
}

/**
 * @brief Logs the results of the profiling for each system.
 *
 * Calculates and logs the average duration and the percentage of total frame time
 * for each system. This method should be called after all systems have been processed
 * to provide insight into the performance characteristics of each component of the game.
 */
void Profiler::LogResults() {
    for (const auto& [systemName, data] : systems) {
        // Calculate average duration in microseconds for the system
        double avgDuration = std::chrono::duration_cast<std::chrono::microseconds>(data.totalDuration).count() / static_cast<double>(data.frameCount);

        // Calculate percentage of total frame time
        double percentage = (data.totalDuration.count() / static_cast<double>(frameDuration.count())) * 100.0;

        // Log the results in milliseconds and as a percentage of total frame time

        FruitLoops::LogGameLoop("[{}] took {:.6f}% of total frame time ({:.6f} ms avg)",
            systemName, percentage, avgDuration / 1000.0);
    }
}
