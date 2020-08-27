workspace "StortSpelProjekt"
    architecture "x64"
    configurations { "Debug", "Release" }
    
project "Engine"
    location "Engine"
    systemversion "latest"
    kind "StaticLib"
    language "C++"
    pchheader "stdafx.h"
    pchsource "%{prj.location}/src/Headers/stdafx.cpp"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"
    files { "%{prj.location}/src/**.cpp", "%{prj.location}/src/**.h", "%{prj.location}/src/**.hlsl" }
    forceincludes { "stdafx.h" }
    
    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
    
    filter "configurations:*"
    cppdialect "C++17"
    includedirs {"Vendor/Include/", "%{prj.location}/src/Headers/"}
    libdirs { "Vendor/Lib/**" }

    links {
        "d3d12",
        "dxgi",
        "d3dcompiler",
        "assimp-vc140-mt"
    }

    postbuildcommands
    {
        ("{COPY} ../dll ../bin/%{cfg.buildcfg}/Game")
    }
    
        filter "configurations:Debug"
            defines { "_DEBUG", "_CONSOLE" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG", "_CONSOLE" }
            optimize "On"

project "Game"
    location "Game"
    systemversion "latest"
    kind "WindowedApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"
    files { "%{prj.location}/src/**.cpp", "%{prj.location}/src/**.h", "%{prj.location}/src/**.hlsl" }
    
    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
    
    filter "configurations:*"
        cppdialect "C++17"
    
    includedirs {"Vendor/Include/", "Engine/src/", "Engine/src/Headers/"}
    libdirs { "Vendor/Lib/**" }
    links {
        "Engine"
    }
    
    filter "configurations:Debug"
        defines { "_DEBUG", "_CONSOLE" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG", "_CONSOLE" }
        optimize "On"