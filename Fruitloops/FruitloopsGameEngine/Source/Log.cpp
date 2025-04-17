/******************************************************************************/
/*!
\file    Log.cpp
\Proj    Shroomy Doomy
\author  Foo Ming Sheng
\date    Nov 08, 2024
\brief   Implements core logging and testing functionality for the
         FruitloopEngine, including setup, error handling, and subsystem
         testing across various engine components.

         Key Components:
         - Logging setup and exception handling.
         - Tests for matrix and vector operations, physics, collision detection,
           entity management, serialization, and rendering.
         - Provides batch rendering and performance stats logging.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/



#include "pch.h"
#include "Log.h"
#include "Physics.h"
#include "Input.h"
#include "EntityManager.h"
#include "Collision.h"
#include "Serialization.h" // Include your serialization header
#include "Renderer.h"
#include "ShaderManager.h"

#include "Debug.h"

namespace FruitLoops {
    template<typename... Args>
    void test(const std::string& message, Args&&... args) {
        // Get or create the error logger
        std::shared_ptr<spdlog::logger> error_logger = spdlog::get("error_logger");
        if (!error_logger) {
            // Create the logger if it does not exist
            error_logger = spdlog::stdout_color_mt("error_logger");
            if (!error_logger) {
                std::cerr << "Failed to create logger." << std::endl;
                return;
            }

            // Set the initial pattern for highlighting errors
            error_logger->set_pattern("\033[31m[%Y-%m-%d %H:%M:%S.%e] [error] %v\033[0m");
        }

        try {
            // Log the formatted error message
#ifdef _DEBUG
            error_logger->error(message, std::forward<Args>(args)...);
#else   
            (void)message;
            (void)(..., (void)args);
#endif

            // Display the graphical error popup using fmt to format the string
            std::string formatted_message = fmt::format(message, std::forward<Args>(args)...);
            Debug::DisplayErrorPopup(formatted_message);

            // Reset the logger pattern back to normal after logging the error
            error_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
        }
        catch (const std::exception& e) {
            // Catch and log any exceptions thrown during the logging process
            std::cerr << "Exception caught while logging: " << e.what() << std::endl;
        }
    }
    /**
  * @brief Handles application termination by logging any uncaught exceptions.
  *
  * This function logs any unhandled exceptions that occur during the application's termination process.
  */
    void HandleTerminate() {
        try {
            // Rethrow the current exception to log it
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception& ex) {

            // Log unhandled exceptions
            spdlog::get("fileLogger")->error("Unhandled exception: {}", ex.what());
            spdlog::get("fileLogger")->flush();
        }
        catch (...) {
            // Catch all unknown exceptions
            spdlog::get("fileLogger")->error("Caught an unknown exception.");
            spdlog::get("fileLogger")->flush();
        }
    }

    /**
     * @brief Initializes the logging system for both console and file outputs.
     *
     * Sets up loggers for the console and file output. Formats logs with timestamps and ensures logs are flushed periodically.
     */
    void SetupLogging() {
        try {
            // Set up a console logger
            auto console = spdlog::stdout_color_mt("console");

            console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [welcome] %v");
#ifdef _DEBUG
            spdlog::info("Logger initialized.");
            spdlog::get("console")->info("Follow your nose! It always knows!");
#endif
            console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");

            // Set up a file logger
            auto fileLogger = spdlog::basic_logger_mt("fileLogger", "Logs.txt");

            // Set global log level
            spdlog::set_level(spdlog::level::info);

            // Set the console as the default logger
            spdlog::set_default_logger(console);

            // Log initialization
#ifdef _DEBUG
            spdlog::get("fileLogger")->info("Writing to log file before error...");
#endif

            spdlog::flush_every(std::chrono::seconds(5));
        }
        catch (const spdlog::spdlog_ex& ex) {
#ifdef _DEBUG
            std::cerr << "Log initialization failed: " << ex.what() << std::endl;
#endif
            UNUSED_PARAM(ex);
        }
    }

    /**
     * @brief Logs game loop events with yellow formatting.
     *
     * Logs game loop-related messages using a yellow color format for easy distinction in the console.
     *
     * @param message The message to log.
     */
    void LogGameLoop(const std::string& message) {
        auto gameLoopLogger = spdlog::get("GameLoop");

        // If the "GameLoop" logger doesn't exist, create it
        if (!gameLoopLogger) {
            gameLoopLogger = spdlog::stdout_color_mt("GameLoop");
        }

        // Set the pattern and the color for this log
        // Adding ANSI escape code for yellow text (\033[33m)
        gameLoopLogger->set_pattern("\033[33m[%Y-%m-%d %H:%M:%S.%e] [%n] %v\033[0m");

#ifdef _DEBUG
        // Log the message using the "GameLoop" logger
        gameLoopLogger->info(message);
#else   
        (void)message;
#endif

        // Revert back to the original pattern without color
        gameLoopLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }



}