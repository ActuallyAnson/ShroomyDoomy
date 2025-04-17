workspace "FruitLoops"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

    project "Fruitloops-ScriptCore"
        kind "SharedLib"
        language "C#"
        dotnetframework "4.8"

        targetdir "../../FruitloopsGameEngine/Assets/Scripts"
        objdir "../../FruitloopsGameEngine/Assets/Scripts/Intermediates"

        files { "Source/**.cs" }

        filter "configurations:Debug"
            optimize "Off"
            symbols "Default"

        filter "configurations:Release"
            optimize "On"
            symbols "Default"

        filter "configurations:Dist"
            optimize "Full"
            symbols "Off"