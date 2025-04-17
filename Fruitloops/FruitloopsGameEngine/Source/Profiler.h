/******************************************************************************/
/*!
\file    Profiler.h
\Proj    Shroomy Doomy
\author  Foo Ming Sheng
\date    Nov 08, 2024
\brief   Declares the Profiler class for tracking system performance in the
         game. Provides methods to start and stop profiling individual systems,
         log performance metrics, and reset frame timings.

         The Profiler tracks each system's execution time per frame and logs
         results as percentages and average times.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#ifndef PROFILER_H
#define PROFILER_H

class Profiler {
public:
    std::chrono::high_resolution_clock::duration frameDuration;

    // Start profiling for a specific system
    void Start(const std::string& systemName);

    // Stop profiling for a specific system
    void Stop(const std::string& systemName);

    // Calculate and log performance results as percentages
    void LogResults();

    // Call this at the beginning of each frame to reset timings
    void StartFrame();

private:
    struct SystemData {
        std::chrono::high_resolution_clock::duration totalDuration;
        int frameCount = 0;
    };

    std::unordered_map<std::string, SystemData> systems;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;
};

#endif
