workspace "StortSpelProjekt"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Game"
    systemversion "latest"
    
project "Engine"
    location "Engine"
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
        ("{COPY} ../dll ../bin/%{cfg.buildcfg}/Game"),
        ("{COPY} ../dll ../bin/%{cfg.buildcfg}/Sandbox")
    }
    defines{"_CRT_SECURE_NO_DEPRECATE", "_CRT_NONSTDC_NO_DEPRECATE"}
        filter "configurations:Debug"
            defines { "_DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"

project "Game"
    location "Game"
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
        defines { "_DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "Sandbox"
    location "Sandbox"
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
        defines { "_DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "GTest"
    location "googletest"
    kind "StaticLib"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"
    files { "googletest/gtest/googletest/src/gtest-all.cc" }
    includedirs { "googletest/gtest/googletest/include", "googletest/gtest/googletest" }
    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "EngineTests"
    location "EngineTests"
    kind "ConsoleApp"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"
    files {"%{prj.location}/src/**.cpp", "src/**.h"}
    includedirs { "Engine/src/", "googletest/gtest/googletest/include/"}
    links {"Engine", "GTest" }
    filter "configurations:Debug"
        defines { "_DEBUG", "_CONSOLE" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "_CONSOLE" }
        optimize "On"