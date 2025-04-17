/******************************************************************************/
/*!
\file    Log.h
\Proj    Shroomy Doomy
\author  Foo Ming Sheng
\date    Nov 08, 2024
\brief   Declares logging utilities for the FruitloopEngine using the `spdlog`
         library, enabling detailed log output and subsystem testing.

         This file provides templated log functions for various engine systems
         (e.g., game loop, physics, graphics) with color-coded outputs to
         enhance readability. Additionally, it declares test functions for
         validating the functionality of different engine components, such as
         rendering and entity management.

Key functionalities:
- Console and file logging setup.
- Subsystem-specific logging for physics, graphics, entities, etc.
- Color-coded messages for clarity in log outputs.
- Test functions for engine component validation.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/



#pragma once



namespace FruitLoops {
    template<typename... Args>
    void test(const std::string& message, Args&&... args);
    /**
  * @brief Initializes logging for the application.
  *
  * Sets up console and file logging, formatting the output for clarity and ensuring logs are flushed periodically.
  */
    void SetupLogging();  // Declaration of SetupLogging

    /**
     * @brief Handles unhandled exceptions during termination.
     *
     * Logs any exceptions that occur during application termination to ensure error reporting.
     */
    void HandleTerminate();

    /**
     * @brief Logs messages related to the game loop.
     *
     * Formats logs with yellow text to distinguish game loop events.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The message to log.
     * @param args Arguments for formatting the message.
     */
     // Declare and define the LogGameLoop function template here
    template<typename... Args>
    void LogGameLoop(const std::string& message, Args&&... args) {
        auto gameLoop_logger = spdlog::get("GameLoop");

        // If the game loop logger doesn't exist, initialize it
        if (!gameLoop_logger) {
            gameLoop_logger = spdlog::stdout_color_mt("GameLoop");
        }

        // Set the pattern and the color for this log
        // Adding code for yellow text (\033[33m)
        gameLoop_logger->set_pattern("\033[33m[%Y-%m-%d %H:%M:%S.%e] [GameLoop] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        gameLoop_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Change back to the original pattern without color
        gameLoop_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

    /**
     * @brief Logs input-related messages.
     *
     * Uses green text formatting to log input-related messages.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The message to log.
     * @param args Arguments for formatting the message.
     */
    template<typename... Args>
    void logInput(const std::string& message, Args&&... args) {
        auto input_logger = spdlog::get("input_logger");

        // If the input logger doesn't exist, initialize it
        if (!input_logger) {
            input_logger = spdlog::stdout_color_mt("input_logger");
            input_logger->set_pattern("\033[92m[input] %v\033[0m"); // Light green color
        }

#ifdef _DEBUG
        // Log the message with formatting
        input_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif
    }

    /**
     * @brief Logs error messages in red.
     *
     * Formats error logs with red text for better visibility.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The error message to log.
     * @param args Arguments for formatting the message.
     */
    template<typename... Args>
    void logError(const std::string& message, Args&&... args) {
        auto error_logger = spdlog::get("error_logger");

        // If the error logger doesn't exist, initialize it
        if (!error_logger) {
            error_logger = spdlog::stdout_color_mt("error_logger");
        }

        // Set the pattern and the color for this log (e.g., red for errors)
        error_logger->set_pattern("\033[31m[%Y-%m-%d %H:%M:%S.%e] [error] %v\033[0m");

#ifdef _DEBUG
        // Log the error message with formatting
        error_logger->error(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        error_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

    /**
     * @brief Logs matrix-related operations.
     *
     * Formats logs related to matrix operations with light magenta text.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The matrix-related message to log.
     * @param args Arguments for formatting the message.
     */
     // Function to log matrix operations
    template<typename... Args>
    void LogMatrix(const std::string& message, Args&&... args) {
        auto matrix_logger = spdlog::get("matrix_logger");

        // If the matrix logger doesn't exist, initialize it
        if (!matrix_logger) {
            matrix_logger = spdlog::stdout_color_mt("matrix_logger");
        }

        // Set the pattern and the color for this log
        // Light magenta text
        matrix_logger->set_pattern("\033[95m[Matrix] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        matrix_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        matrix_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }


    /**
     * @brief Logs vector-related operations.
     *
     * Formats logs for vector operations with light purple text for distinction.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The vector-related message to log.
     * @param args Arguments for formatting the message.
     */
    template<typename... Args>
    void LogVector(const std::string& message, Args&&... args) {
        auto vector_logger = spdlog::get("vector_logger");

        // If the vector logger doesn't exist, initialize it
        if (!vector_logger) {
            vector_logger = spdlog::stdout_color_mt("vector_logger");
        }

        // Set the pattern and the color for this log
        // Light purple text
        vector_logger->set_pattern("\033[38;5;213m[Vector] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        vector_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        vector_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }


    /**
     * @brief Logs physics-related operations.
     *
     * Uses grey text formatting to log messages related to physics events and updates.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The physics-related message to log.
     * @param args Arguments for formatting the message.
     */
    template<typename... Args>
    void LogPhysics(const std::string& message, Args&&... args) {
        auto physics_logger = spdlog::get("physics_logger");

        // If the physics logger doesn't exist, initialize it
        if (!physics_logger) {
            physics_logger = spdlog::stdout_color_mt("physics_logger");
        }

        // Set the pattern and the color for this log
        // Light grey text
        physics_logger->set_pattern("\033[38;5;250m[Physics] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        physics_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        physics_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

    /**
     * @brief Logs entity-related operations.
     *
     * Logs entity creation, management, and destruction events with green text formatting.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The entity-related message to log.
     * @param args Arguments for formatting the message.
     */
    template<typename... Args>
    void LogEntity(const std::string& message, Args&&... args) {
        auto entity_logger = spdlog::get("entity_logger");

        // If the entity logger doesn't exist, initialize it
        if (!entity_logger) {
            entity_logger = spdlog::stdout_color_mt("entity_logger");
        }

        // Set the pattern and the color for this log
        // Light grey text
        entity_logger->set_pattern("\033[38;5;46m[Entity] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        entity_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        entity_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }


    /**
     * @brief Logs collision-related events.
     *
     * Formats logs for collision detection with light orange text for better distinction.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The collision-related message to log.
     * @param args Arguments for formatting the message.
     */
    template<typename... Args>
    void LogCollision(const std::string& message, Args&&... args) {
        auto collision_logger = spdlog::get("collision_logger");

        // If the collision logger doesn't exist, initialize it
        if (!collision_logger) {
            collision_logger = spdlog::stdout_color_mt("collision_logger");
        }

        // Set the pattern and the color for this log
        // Light orange color
        collision_logger->set_pattern("\033[38;5;214m[Collision] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        collision_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        collision_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }


    /**
     * @brief Logs serialization operations.
     *
     * Formats logs for serialization events with light magenta text.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The serialization-related message to log.
     * @param args Arguments for formatting the message.
     */
     // Function to log matrix operations
    template<typename... Args>
    void LogSerialization(const std::string& message, Args&&... args) {
        auto serialization_logger = spdlog::get("serialization_logger");

        // If the matrix logger doesn't exist, initialize it
        if (!serialization_logger) {
            serialization_logger = spdlog::stdout_color_mt("serialization_logger");
        }

        // Set the pattern and the color for this log
        // Light magenta text
        serialization_logger->set_pattern("\033[95m[Serialization] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        serialization_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        serialization_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

    /**
     * @brief Logs graphics-related operations for constant updating.
     *
     * Uses light blue text to format logs for graphics updates.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The graphics-related message to log.
     * @param args Arguments for formatting the message.
     */
     // Function is used for constant updating ONLY
     // Declare and define the logGraphics function template here
    template<typename... Args>
    void logGraphics(const std::string& message, Args&&... args) {
        auto graphics_logger = spdlog::get("graphics_logger");

        // If the graphics logger doesn't exist, initialize it
        if (!graphics_logger) {
            graphics_logger = spdlog::stdout_color_mt("graphics_logger");
        }

        // Set the pattern and the color for this log
        // Adding code for light blue text (\033[96m)
        graphics_logger->set_pattern("\033[96m[%Y-%m-%d %H:%M:%S.%e] [graphics] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        graphics_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        graphics_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

    /**
     * @brief Logs graphics-related operations used in tests.
     *
     * Uses blue text formatting to log messages related to graphics testing.
     *
     * @tparam Args Variadic template for formatting arguments.
     * @param message The graphics test message to log.
     * @param args Arguments for formatting the message.
     */
     // Function is used for Graphics ONLY
     // Declare and define the logGraphics function template here
    template<typename... Args>
    void LogGraphicstest(const std::string& message, Args&&... args) {
        auto graphicstest_logger = spdlog::get("graphicstest_logger");

        // If the graphics logger doesn't exist, initialize it
        if (!graphicstest_logger) {
            graphicstest_logger = spdlog::stdout_color_mt("graphicstest_logger");
        }

        // Set the pattern and the color for this log
        // Adding code for light blue text (\033[96m)
        graphicstest_logger->set_pattern("\033[96m[graphics] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        graphicstest_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Revert back to the original pattern without color
        graphicstest_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }
/**
* @brief Logs messages related to ImGui operations with formatted output.
*
* This function checks if an ImGui logger exists, creates it if not, sets the logging pattern,
* and logs the message. It uses ANSI color codes to color the log output yellow. After logging,
* it resets the pattern to the default without color.
*
* @tparam Args Variadic template to handle multiple argument types.
* @param message The message format string.
* @param args Arguments that are substituted into the message format.
*/
    template<typename... Args>
    void logImgui(const std::string& message, Args&&... args) {
        auto imgui_logger = spdlog::get("imgui_logger");

        // If the game loop logger doesn't exist, initialize it
        if (!imgui_logger) {
            imgui_logger = spdlog::stdout_color_mt("imgui_logger");
        }

        // Set the pattern and the color for this log
        // Adding code for yellow text (\033[33m)
        imgui_logger->set_pattern("\033[33m[%Y-%m-%d %H:%M:%S.%e] [IMGUI] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        imgui_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif

        // Change back to the original pattern without color
        imgui_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

/**
* @brief Logs messages related to audio operations with formatted output.
*
* This function performs similarly to `logImgui`, but for audio-related logs. It checks for an existing audio logger,
* creates one if necessary, sets a colored logging pattern, logs the message, and then resets the pattern.
*
* @tparam Args Variadic template to handle multiple argument types.
* @param message The message format string.
* @param args Arguments that are substituted into the message format.
*/
    template<typename... Args>
    void logAudio(const std::string& message, Args&&... args) {
        auto audio_logger = spdlog::get("audio_logger");

        // If the game loop logger doesn't exist, initialize it
        if (!audio_logger) {
            audio_logger = spdlog::stdout_color_mt("audio_logger");
        }

        // Set the pattern and the color for this log
        // Adding code for yellow text (\033[33m)
        audio_logger->set_pattern("\036[33m[%Y-%m-%d %H:%M:%S.%e] [IMGUI] %v\033[0m");

#ifdef _DEBUG
        // Log the message with formatting
        audio_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif
        // Change back to the original pattern without color
        audio_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }

/**
* @brief Logs messages related to game pause and resume operations with formatted output.
*
* This function ensures logging of pause-related events. It initializes a pause logger if it doesn't exist,
* sets a colored pattern for the logs, logs the formatted message, and finally resets the logging pattern to default.
*
* @tparam Args Variadic template to handle multiple argument types.
* @param message The message format string.
* @param args Arguments that are substituted into the message format.
*/
    template<typename... Args>
    void logPause(const std::string& message, Args&&... args) {
        auto pause_logger = spdlog::get("pause_logger");

        // If the game loop logger doesn't exist, initialize it
        if (!pause_logger) {
            pause_logger = spdlog::stdout_color_mt("pause_logger");
        }

        // Set the pattern and the color for this log
        // Adding code for yellow text (\033[33m)
        pause_logger->set_pattern("\015[33m[%Y-%m-%d %H:%M:%S.%e] [IMGUI] %v\033[0m");
#ifdef _DEBUG
        // Log the message with formatting
        pause_logger->info(message, std::forward<Args>(args)...);
#else
        (void)message;
        (void)(..., (void)args);
#endif
        // Change back to the original pattern without color
        pause_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    }
}
 