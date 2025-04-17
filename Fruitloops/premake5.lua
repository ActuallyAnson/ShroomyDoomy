workspace "FruitLoops"
    architecture "x64"

    configurations{
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Fruitloops"
    location "FruitloopsGameEngine"
    kind "WindowedApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    warnings "High"
    
    pchheader "pch.h"
    pchsource "FruitloopsGameEngine/Source/pch.cpp"
    
    files {
        "FruitloopsGameEngine/Source/**.h",
        "FruitloopsGameEngine/Source/**.cpp",
    }
    
    includedirs{
        "FruitloopsGameEngine/Assets",
        "FruitloopsGameEngine/Dependancies/glfw/include",
        "FruitloopsGameEngine/Dependancies",
        "FruitloopsGameEngine/Source",
        "FruitloopsGameEngine/Dependancies/include",
        "FruitloopsGameEngine/Dependancies/include/lua",
        "FruitloopsGameEngine/Dependancies/include/mono"
    }

    links {
        "glfw3_mt.lib",        -- Link against the precompiled GLFW static library
        "opengl32.lib",      -- OpenGL library on Windows
        "glew32.lib",
        "fmod_vc.lib",
        "freetype.lib",
        "lua54.lib",
        "imguilib.lib",
        "mono-2.0-sgen.lib"
    }

    --linkoptions { "/NODEFAULTLIB:LIBCMT" }

    libdirs {
        "FruitloopsGameEngine/Dependancies/lib",  -- Make sure this path points to the directory where the precompiled glfw3.lib is located
    }

    -- Set the working directory to the Source folder
    debugdir "FruitloopsGameEngine/Source"

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines {}

        postbuildcommands {
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/glew32.dll %{cfg.targetdir}",
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/fmod.dll %{cfg.targetdir}",
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/freetype.dll %{cfg.targetdir}",
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/lua54.dll %{cfg.targetdir}",
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/mono-2.0-sgen.dll %{cfg.targetdir}",
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/MonoPosixHelper.dll %{cfg.targetdir}",
            "{COPY} ../FruitloopsGameEngine/Dependancies/lib/mscorlib.dll %{cfg.targetdir}"
        }
    
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"
        linkoptions { "/NODEFAULTLIB:LIBCMT" }

    filter "configurations:Release"
        defines "RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "DIST"
        optimize "On"

-- Include Fruitloops-ScriptCore project
include "FruitloopsGameEngine/Fruitloops-ScriptCore"