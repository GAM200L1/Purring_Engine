--[[
    +-----------------------------------------------------------------------------+
    |                              Purring Engine                                 |
    |                             premake5.lua File                               |
    +-----------------------------------------------------------------------------+
    | Filename:     premake5.lua                                                  |
    | Created On:   4 September 2023                                              |
    +-----------------------------------------------------------------------------+
    | Last Updated: 17 March 2024                                                 |
    | Updated By:   Hans ONG You Yang                                             |
    | Email:        youyang.o\@digipen.edu                                        |
    +-----------------------------------------------------------------------------+
    | Author:       Brandon HO Jun Jie                                            |
    | Email:        brandonjunjie.ho\@digipen.edu                                 |
    +-----------------------------------------------------------------------------+
    | Brief:  Workspace and project configuration for the Purring Engine project  |
    |         using Premake. This file defines the build configurations,          |
    |         include directories, dependencies, and project setups to ensure a   |
    |         streamlined build process across different environments.            |
    +-----------------------------------------------------------------------------+
    | All content (c) 2024 DigiPen Institute of Technology Singapore.             |
    | All rights reserved.                                                        |
    +-----------------------------------------------------------------------------+
]]

workspace "Purring_Engine"
    architecture "x64"              -- Target architecture for the project.
    startproject "Application"      -- Sets the Application project as the startup project.

    -- Build configurations
    configurations
    {
        "Debug",                    -- Debug build configuration.
        "Release",                  -- Release build configuration.
        "GameRelease"               -- Game release build configuration.
    }

-- Global variables to define output directories for binaries and intermediates based on the configuration, system, and architecture.
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"         -- Output directory for the project.
gameoutputdir = "Release-%{cfg.system}-%{cfg.architecture}"             -- Output directory for the game release build configuration.

-- Global table to keep track of include directories for external libraries used in the project.
IncludeDir = {}
IncludeDir["GLFW"]          = "vendor/GLFW/include"
IncludeDir["ImGui"]         = "vendor/imgui"
IncludeDir["glm"]           = "vendor/glm"
IncludeDir["stb_image"]     = "vendor/stb_image"
IncludeDir["GLEW"]          = "vendor/GLEW/include"
IncludeDir["FMOD"]          = "vendor/FMOD/core/inc"        -- FMOD CORE API
IncludeDir["FMODStudio"]    = "vendor/FMOD/studio/inc"      -- FMOD STUDIO
IncludeDir["RTTR"]          = "vendor/RTTR/include"
--IncludeDir["mono"]          = "vendor/mono/include"

-- Group for projects that are external libraries. These projects are treated as dependencies for the main engine and application projects.
group "Library"
    -- GLFW library project setup. GLFW is used for window management and input handling.
    project "GLFW"
        location "vendor/GLFW"
        kind "StaticLib"   
        warnings "Off"
        language "C"

        -- Set the output directory for the compiled library and intermediate files.
        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")
        flags { "MultiProcessorCompile" }                           -- Enable multi-processor compilation.

        -- Specify the GLFW source and header files.
        files
        {
            "vendor/GLFW/include/GLFW/*.h",
            "vendor/GLFW/src/*.h",
            "vendor/GLFW/src/*.c"
        }
        defines { "_GLFW_WIN32" }                                   -- Define the GLFW_WIN32 macro to enable Windows-specific code.

        -- Platform-specific configurations
        filter "system:windows"
            systemversion "latest"                                  -- Use the latest Windows SDK.
        filter "configurations:Debug"
            staticruntime "off"
            runtime "Debug"
            symbols "on"                                            -- Enable debug symbols.
        filter "configurations:Release"
            staticruntime "on"
            runtime "Release"
            optimize "on"                                           -- Enable optimizations.        
        filter "configurations:GameRelease"
			runtime "Release"
            staticruntime "on"
			optimize "on"
            targetdir ("bin/" .. gameoutputdir .. "/vendor/%{prj.name}")
            objdir ("bin-int/" .. gameoutputdir .. "/vendor/%{prj.name}")


    -- ImGui library project setup. ImGui is used for graphical user interfaces within the engine and tools.
    project "ImGui"
        location "vendor/imgui"
        kind "StaticLib"
        warnings "Off"                                              -- Disable warnings for ImGui.
        language "C++"
        cppdialect "C++17"

        -- Set the output directory for the compiled library and intermediate files.
        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")
        flags { "MultiProcessorCompile" }

        -- Specify the ImGui source and header files, including backends for GLFW and OpenGL3.
        files
        {
            "vendor/imgui/*.h",
            "vendor/imgui/*.cpp",
            "vendor/imgui/backends/imgui_impl_glfw.*",
            "vendor/imgui/backends/imgui_impl_opengl3.*",
            "vendor/imgui/misc/**.h",
            "vendor/imgui/misc/**.cpp"
        }
        removefiles { "vendor/imgui/misc/freetype/*.*" }            -- Exclude freetype backend files.
        includedirs
        {
            "vendor/imgui",
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.FMOD}"
        }

        -- Platform-specific configurations
        filter "system:windows"
            systemversion "latest"
        filter "configurations:Debug"
            runtime "Debug"
            staticruntime "off"
            symbols "on"
        filter "configurations:Release"
            runtime "Release"
            staticruntime "on"
            optimize "on"
        filter "configurations:GameRelease"
            kind "None"                                             -- Set to "None" as to not compile ImGui for GameRelease configuration.
   
group ""    -- End of external libraries group.


-- Main "Purring_Engine" project setup. This is the core of the engine.
project "Purring_Engine"
    location "Purring_Engine"
    kind "StaticLib"                -- Engine compiled as a static library.
    staticruntime "on"
    language "C++"
    cppdialect "C++17"
    warnings "Extra"                -- Enable extra warnings for the engine project.

    -- Set the target name, output directory, and intermediate directory for the engine.
    targetname "Purring_Engine_Editor"                              -- Name of the output binary when used in editor mode.
    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prpch.h"                                             -- Precompiled header file.
    pchsource "Purring_Engine/src/prpch/prpch.cpp"                  -- Source file for the precompiled header.

    buildoptions { "/bigobj" }                                      -- Increase object file size limit, because this project is quite large.
    flags { "MultiProcessorCompile" }                               -- Enable multi-processor compilation.

    -- Specify all the source and header files for the engine.
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl"
    }

    -- Remove the game-specific entry point for editor builds.
    removefiles
    {
        -- remove WinMain entrypoint
        "%{prj.name}/src/EntryPointGame.h"
    }

    -- Include directories for the engine and its dependencies.
    includedirs
    {
        "%{prj.name}/src",                                          -- Engine source directory.
        "%{prj.name}/src/prpch",                                    -- Precompiled header directory.
        "%{IncludeDir.GLFW}",                                       -- GLFW for window management and input handling.
        "%{IncludeDir.ImGui}",                                      -- ImGui for GUI.
        "%{IncludeDir.glm}",                                        -- GLM for math operations.
        "%{IncludeDir.stb_image}",                                  -- stb_image for image loading.
        "%{IncludeDir.GLEW}",                                       -- GLEW for OpenGL extensions.
        "%{IncludeDir.FMOD}",                                       -- FMOD Core API lib
        "%{IncludeDir.FMODStudio}",                                 -- FMOD Studio lib
        "vendor/freetype/include",                                  -- Freetype for font rendering.     
        "%{IncludeDir.RTTR}",                                       -- RTTR for reflection.                
        --"%{IncludeDir.mono}"
    }

    -- Library directories for linking against external dependencies.
    libdirs
    {
        "vendor/",                                                  -- Root directory for external libraries. 
        "vendor/GLEW/lib/Release/x64",                              -- GLEW lib       
        "vendor/FMOD/core/lib/x64",                                 -- FMOD Core API lib
        "vendor/FMOD/studio/lib/x64",                               -- FMOD Studio lib
        "vendor/freetype/libs",                                     -- Freetype lib
        "vendor/RTTR/lib"                                           -- RTTR lib
    }

    -- Libraries to link against.
    links
    {
        "GLFW",                                                     -- GLFW for window management and input handling.                                     
        "glew32s",                                                  -- GLEW for OpenGL extension loading.                              
        "ImGui",                                                    -- ImGui for GUI.       
        "opengl32",                                                 -- OpenGL for rendering.                         
        "fmod_vc",                                                  -- FMOD Core API lib      
        "fmodstudio_vc",                                            -- FMOD Studio lib
        "freetype"                                                  -- Freetype for font rendering.
        --"libmono-static-sgen",

    }

    linkoptions { "/ignore:4006" }                                  -- Ignore specific linker warnings.

    -- Windows-specific settings.
    filter "system:windows"
        systemversion "latest"                                      -- Use the latest Windows SDK.

    -- Configuration-specific settings for Debug, Release, and GameRelease.
    ----- Debug -----
    filter "configurations:Debug"
		runtime "Debug"
        staticruntime "off"
		symbols "on"                                                -- Enable debug symbols.

        libdirs
        {
            --"vendor/mono/lib/Debug"
        }

    ----- Release -----
    filter "configurations:Release"
		runtime "Release"
        staticruntime "on"
		optimize "on"
            
        libdirs
        { 
            --"vendor/mono/lib/Release"
        }
    
    ----- Game Release -----
    filter "configurations:GameRelease"
		runtime "Release"
        staticruntime "on"
		optimize "on"
        targetname "Purring_Engine"
        targetdir("bin/" .. gameoutputdir .. "/%{prj.name}")
        objdir("bin-int/" .. gameoutputdir .. "/%{prj.name}")

        defines { "GAMERELEASE" }

        libdirs
        {
            --"vendor/mono/lib/Release"
        }

        removelinks { "ImGui" }                                     -- Remove ImGui for game release as we do not need the editor developer tools in game release mode.

        -- removes editor files and change entrypoint to WinMain
        files
        {
            --"%{prj.name}/src/EntryPointGame.cpp"
        }

        -- Remove editor-specific files for the game release.
        removefiles
        {                
            --"%{prj.name}/src/EntryPoint.cpp",
            "%{prj.name}/src/Editor/*.h",
            "%{prj.name}/src/Editor/*.cpp"
        }

 
-- Application project setup. This is the user-facing part of the engine, like a game editor or standalone game.
project "Application"
    location "Application"
    kind "ConsoleApp"                                               -- Application compiled as a console application.
    language "C++"
    cppdialect "C++17"
    warnings "Extra"                                                -- Enable extra warnings for the application project.
    targetname "Editor"
    targetdir("bin/" .. outputdir .. "/Editor")
    objdir("bin-int/" .. outputdir .. "/Editor")

    flags { "MultiProcessorCompile" }                               -- Enable multi-processor compilation.

    -- Specify all the source and header files for the application.
	files
	{
		"%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
        "Assets/Animation/**.anim",
        "Assets/Scenes/**.scene",
        "Assets/Prefabs/**.prefab"
	}

    -- Include directories for the application and its dependencies.
    includedirs
    {
        "%{prj.name}/src",
        "Purring_Engine/src",
        "vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLEW}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.FMODStudio}",
        "%{IncludeDir.RTTR}"
    }

    -- Library directories for linking against external dependencies.
    libdirs
    {
        "vendor/freetype/libs",
        "vendor/RTTR/lib"
    }
    
    -- Link against the Purring_Engine static library.
    links { "Purring_Engine" }
    linkoptions { "/ignore:4006", "/ignore:4098", "/ignore:4099"}

    -- Post-build commands can be used for tasks like copying assets or binaries to the output directory.
    postbuildcommands
    {
        -- empty
    }

    -- Windows-specific settings.
    filter "system:windows"
        systemversion "latest"
    
    -- Configuration settings for Debug
    filter "configurations:Debug"
        runtime "Debug"
        staticruntime "off"                                         -- Enable static runtime linking.
        symbols "on"                                                -- Enable debug symbols.    

        -- Copy assets and dependencies to the output directory after a successful build.
        postbuildcommands
        {
            ("{COPYDIR} ../Assets ../bin/" .. outputdir .. "/Assets"),
            ("{COPYDIR} ../Shaders ../bin/" .. outputdir .. "/Shaders"),
            ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. outputdir .. "/Editor"),
            ("{COPYFILE} ../vendor/FMOD/studio/lib/x64/fmodstudio.dll ../bin/" .. outputdir .. "/Editor"),
            ("{COPYFILE} ../vendor/RTTR/bin/rttr_core_d.dll ../bin/" .. outputdir .. "/Editor"),
            ("{COPYFILE} ../vendor/RTTR/bin/rttr_core_d.pdb ../bin/" .. outputdir .. "/Editor")
        }

        libdirs
        {
            "Purring_Engine/"
        }

        links { "rttr_core_d" }

    -- Configuration settings for Release
    filter "configurations:Release"
		runtime "Release"
        staticruntime "on"                                           -- Enable static runtime linking.
		optimize "on"                                                -- Enable optimizations.

        -- Copy assets and dependencies to the output directory after a successful build.
        postbuildcommands
        {
            ("{COPYDIR} ../Assets ../bin/" .. outputdir .. "/Assets"),
            ("{COPYDIR} ../Shaders ../bin/" .. outputdir .. "/Shaders"),
            ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. outputdir .. "/Editor"),
            ("{COPYFILE} ../vendor/FMOD/studio/lib/x64/fmodstudio.dll ../bin/" .. outputdir .. "/Editor"),
            ("{COPYFILE} ../vendor/RTTR/bin/rttr_core.dll ../bin/" .. outputdir .. "/Editor"),
        }
            
        links { "rttr_core" }

    filter "configurations:GameRelease"
		runtime "Release"                                            -- Use the release runtime library.            
        staticruntime "on"                                           -- Enable static runtime linking.
		optimize "on"                                                -- Enable optimizations.
        kind "WindowedApp"                                           -- Change the application to a windowed application for the final game release.  
        defines { "GAMERELEASE" }
        targetname "MarchOfTheMeows"                                 -- Output binary name for the final game.
        targetdir("bin/" .. gameoutputdir .. "/MarchOfTheMeows")
        objdir("bin-int/" .. gameoutputdir .. "/MarchOfTheMeows")

        -- Copy assets and dependencies to the output directory after a successful build.
        postbuildcommands
        {
            ("{COPYDIR} ../Assets ../bin/" .. gameoutputdir .. "/Assets"),
            ("{COPYDIR} ../Shaders ../bin/" .. gameoutputdir .. "/Shaders"),
            ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. gameoutputdir .. "/MarchOfTheMeows"),
            ("{COPYFILE} ../vendor/FMOD/studio/lib/x64/fmodstudio.dll ../bin/" .. outputdir .. "/MarchOfTheMeows"),
            ("{COPYFILE} ../vendor/RTTR/bin/rttr_core.dll ../bin/" .. gameoutputdir .. "/MarchOfTheMeows"),
        }

        links { "rttr_core" }
