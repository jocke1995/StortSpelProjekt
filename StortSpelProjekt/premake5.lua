workspace "StortSpelProjekt"
    architecture "x64"
    configurations { "Debug", "Release" }
    location "build"
    
project "StortSpelProjekt"
    systemversion "latest"
    kind "WindowedApp"
    language "C++"
    pchheader "stdafx.h"
    pchsource "src/Engine/stdafx.cpp"
    targetdir "bin/%{cfg.buildcfg}"
    files { "src/**.cpp", "src/**.h", "src/**.hlsl" }
    forceincludes { "stdafx.h" }
    
    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
    
    filter "configurations:*"
    cppdialect "C++17"
    includedirs {"$(SoloutionDir)../Include", "$(SoloutionDir)../src"}
    libdirs { "Lib/**" }
    links {
        "d3d12",
        "dxgi",
        "d3dcompiler",
        "assimp-vc140-mt"
    }

    postbuildcommands
    {
        ("{COPY} ../dll ../bin/%{cfg.buildcfg}")
    }
    
        filter "configurations:Debug"
            defines { "_DEBUG", "_CONSOLE" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG", "_CONSOLE" }
            optimize "On"