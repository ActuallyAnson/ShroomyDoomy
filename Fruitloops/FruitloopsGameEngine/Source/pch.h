#pragma once

/************************************************************************************
* Precompiled Header for FruitLoops Engine
*
* This file serves as a centralised inclusion of frequently used standard library
* headers, external libraries, math utilities, engine components, ECS infrastructure,
* and ImGui tools. It is designed to accelerate compilation by reducing redundancy
* across translation units.
************************************************************************************/

/*-----------------------------------------------------------------------------
| System Headers
-----------------------------------------------------------------------------*/
#ifdef _WIN32
#include <Windows.h>
#undef APIENTRY // Prevent conflicts with OpenGL definitions
#endif

/*-----------------------------------------------------------------------------
| C++ Standard Library Headers
-----------------------------------------------------------------------------*/
#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>
#include <utility>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <map>
#include <array>
#include <fstream>
#include <random>
#include <cassert>
#include <bitset>
#include <set>
#include <queue>
#include <typeinfo>
#include <iomanip>
#include <exception>
#include <filesystem>
#include <stack>
#include <deque>
#include <chrono>
#include <thread>
#include <cmath>

#define M_PI 3.14159265358979323846 // For use in trigonometry

/*-----------------------------------------------------------------------------
| Third-Party Libraries
-----------------------------------------------------------------------------*/
// GLEW & GLFW (OpenGL Context & Extensions)
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

// GLM (OpenGL Mathematics)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// FMOD (Audio)
#include <fmod/api/core/inc/fmod.hpp>

// spdlog (Logging)
#include "spdlog-1.14.1/include/spdlog/spdlog.h"
#include "spdlog-1.14.1/include/spdlog/sinks/basic_file_sink.h"
#include "spdlog-1.14.1/include/spdlog/sinks/stdout_color_sinks.h"
#include "spdlog-1.14.1/include/spdlog/fmt/ostr.h"

// JSON (Data Serialization)
#include <nlohmann/json.hpp>

// sol2 (Lua binding)
#include <sol/sol.hpp>

// Mono (C# Scripting Interface)
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>
#include <mono/metadata/reflection.h>

/*-----------------------------------------------------------------------------
| ImGui (Immediate Mode GUI)
-----------------------------------------------------------------------------*/
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

/*-----------------------------------------------------------------------------
| Core Math Utilities
-----------------------------------------------------------------------------*/
#include "Matrix3x3.h"
#include "Vector2D.h"
#include "Vector3D.h"

/*-----------------------------------------------------------------------------
| Engine Components
-----------------------------------------------------------------------------*/
#include "Transform.h"
#include "RigidBody.h"
#include "Material.h"
#include "CameraComponent.h"
#include "ConfigData.h"
#include "TileComponent.h"
#include "UnitComponent.h"
#include "Animation.h"
#include "FontData.h"
#include "ScriptComponent.h"
#include "Button.h"

/*-----------------------------------------------------------------------------
| ECS (Entity-Component System)
-----------------------------------------------------------------------------*/
#include "ECSDefinitions.h"
#include "IComponentArray.h"
#include "System.h"
#include "SystemManager.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "ECSHub.h"

/*-----------------------------------------------------------------------------
| Messaging & Observer Pattern
-----------------------------------------------------------------------------*/
#include "Subject.h"
#include "Message.h"
#include "Observer.h"

/*-----------------------------------------------------------------------------
| Miscellaneous Engine Utilities
-----------------------------------------------------------------------------*/
#include "GameTypes.h"
#include "Profiler.h"

/*-----------------------------------------------------------------------------
| Layer System
-----------------------------------------------------------------------------*/
#include "Layer.h"
#include "LayerStack.h"
#include "BackgroundLayer.h"
#include "GameLayer.h"
#include "FontLayer.h"
#include "EndMenuLayer.h"
#include "MainMenuLayer.h"
#include "PauseMenuLayer.h"
#include "StoryLayer.h"
#include "FPSLayer.h"
#include "EnvironmentLayer.h"
#include "UtilsLayer.h"
#include "UILayer.h"
#include "ChestOverlayLayer.h"

/*-----------------------------------------------------------------------------
| Game Control Systems
-----------------------------------------------------------------------------*/
#include "PauseManager.h"
