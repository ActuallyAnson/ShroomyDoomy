/******************************************************************************/
/*!
\file    main.cpp
\project FruitLoops
\author  Zhang TongYan
\date    Dec 03, 2024
\brief   main application entry point (WinMain). This includes setting up engine
         systems, memory checks, and error handling for the FruitLoops engine.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "main.h"
#include "Core.h"
#include "Graphics.h"
#include "ECSHub.h"
#include "Factory.h"
#include "AssetsManager.h"
#include "LevelManager.h"
#include "Input.h"
#include "Log.h"

using namespace FruitLoops;

WindowProps windowProperties;

/*!
 * \brief Entry point for the Windows application.
 * Initializes the game engine, runs the main game loop, and handles cleanup.
 * \param hInstance Handle to the current instance of the application.
 * \param hPrevInstance Handle to the previous instance of the application (unused).
 * \param lpCmdLine Command-line arguments as a null-terminated string (unused).
 * \param nShowCmd Specifies how the window is to be shown (unused).
 * \return Returns 0 to indicate successful execution.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    // Console
    #ifdef _DEBUG
    AllocConsole(); // Create a console
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout); // Redirect stdout to the console
    freopen_s(&stream, "CONOUT$", "w", stderr); // Redirect stderr to the console
    freopen_s(&stream, "CONIN$", "r", stdin);  // Redirect stdin to the console
    std::cout.clear();  // Clear the stream flags
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return 0;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return 0;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif

    UNUSED_PARAM(hInstance);
    UNUSED_PARAM(hPrevInstance);
    UNUSED_PARAM(lpCmdLine);
    UNUSED_PARAM(nShowCmd);

    std::set_terminate(HandleTerminate);
    SetupLogging();
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


    try {
        ecsHub = new ECSHub();

        std::unique_ptr<GameObjectFactory> factoryInstance = std::make_unique<GameObjectFactory>();
        std::unique_ptr<AssetsManager> assetsManagerInstance = std::make_unique<AssetsManager>();
        std::unique_ptr<LevelManager> levelManagerInstance = std::make_unique<LevelManager>();
        std::unique_ptr<FruitloopEngine> engineInstance = std::make_unique<FruitloopEngine>();
        std::unique_ptr<InputSystem> inputSystemInstance = std::make_unique<InputSystem>();
        
        // Initialize the engine
        engineInstance->Initialize();

        // Run the game loop
        engineInstance->GameLoop();

        // Shutdown and cleanup
        engineInstance->ShutDown();

        // Cleanup dynamically allocated objects
        delete ecsHub;
    }
    catch (const std::exception& e) {
        spdlog::get("file_logger")->error("Caught std::exception in WinMain: {}", e.what());
        spdlog::get("file_logger")->flush();
    }
    catch (...) {
        spdlog::get("file_logger")->error("Caught an unknown exception in WinMain.");
        spdlog::get("file_logger")->flush();
    }

#ifdef _DEBUG
    // Wait for input before closing console (optional)
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
#endif

    return 0; // Return 0 indicates successful execution
}
