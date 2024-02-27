-- premake5.lua
workspace "Purring_Engine"
    architecture "x64"
    startproject "Application"

    configurations
    {
        "Debug",
        "Release",
        "GameRelease"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
gameoutputdir = "Release-%{cfg.system}-%{cfg.architecture}"


-- Include directories 
IncludeDir = {}
IncludeDir["GLFW"]          = "vendor/GLFW/include"
IncludeDir["ImGui"]         = "vendor/imgui"
IncludeDir["glm"]           = "vendor/glm"
IncludeDir["stb_image"]     = "vendor/stb_image"
IncludeDir["GLEW"]          = "vendor/GLEW/include"
IncludeDir["FMOD"]          = "vendor/FMOD/core/inc" -- CORE
IncludeDir["RTTR"]          = "vendor/RTTR/include"
--IncludeDir["mono"]          = "vendor/mono/include"

-- external libraries
group "Library"
    -- GLFW
    project "GLFW"
        location "vendor/GLFW"

        kind "StaticLib"   
        
        warnings "Off"

        language "C"

        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")

        flags
        {
            "MultiProcessorCompile"
        } 

        files
        {
            "vendor/GLFW/include/GLFW/*.h",
            "vendor/GLFW/src/*.h",
            "vendor/GLFW/src/*.c"
        }

        defines
        {
            "_GLFW_WIN32",
        }

        filter "system:windows"
            systemversion "latest"

        filter "configurations:Debug"
            staticruntime "off"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            staticruntime "on"
            runtime "Release"
            optimize "on"

        filter "configurations:GameRelease"
			runtime "Release"
            staticruntime "on"
			optimize "on"

            targetdir ("bin/" .. gameoutputdir .. "/vendor/%{prj.name}")
            objdir ("bin-int/" .. gameoutputdir .. "/vendor/%{prj.name}")

    -- imgui
    project "ImGui"
        location "vendor/imgui"
        kind "StaticLib"

        warnings "Off"
        
        language "C++"
        cppdialect "C++17"

        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")

        flags
        {
            "MultiProcessorCompile"
        } 

        files
        {
            "vendor/imgui/*.h",
            "vendor/imgui/*.cpp",
            "vendor/imgui/backends/imgui_impl_glfw.*",
            "vendor/imgui/backends/imgui_impl_opengl3.*",
            "vendor/imgui/misc/**.h",
            "vendor/imgui/misc/**.cpp"
        }

        removefiles
        {
            "vendor/imgui/misc/freetype/*.*"
        }

        includedirs
        {
            "vendor/imgui",
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.FMOD}"
        }

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
            kind "None"
   
group ""

-- Purring_Engine
project "Purring_Engine"
    location "Purring_Engine"

    kind "StaticLib"
    staticruntime "on"

    language "C++"
    cppdialect "C++17"
    
    warnings "Extra"

    -- editor builds in debug and release
    targetname "Purring_Engine_Editor"
    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prpch.h"
    pchsource "Purring_Engine/src/prpch/prpch.cpp"

    buildoptions
    {
        "/bigobj"
    }

    flags
    {
        "MultiProcessorCompile"
    }    

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl"
    }

    removefiles
    {
        -- remove WinMain entrypoint
        "%{prj.name}/src/EntryPointGame.h"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/src/prpch",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.GLEW}",
        "%{IncludeDir.FMOD}",
        "vendor/freetype/include",
        "%{IncludeDir.RTTR}",
        --"%{IncludeDir.mono}"
    }

    libdirs
    {
        "vendor/",
        "vendor/GLEW/lib/Release/x64",
        "vendor/FMOD/core/lib/x64",
        "vendor/freetype/libs",
        "vendor/RTTR/lib"
    }

    links
    {
        "GLFW",
        "glew32s",
        "ImGui",
        "opengl32",
        "fmod_vc",
        --"libmono-static-sgen",
        "freetype"
    }

    linkoptions { "/ignore:4006" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
			runtime "Debug"
            staticruntime "off"
			symbols "on"

            libdirs
            {
                --"vendor/mono/lib/Debug"
            }

    filter "configurations:Release"
			runtime "Release"
            staticruntime "on"
			optimize "on"
            
            libdirs
            {
                --"vendor/mono/lib/Release"
            }
    
    -- Game Release builds in release folder of editor, shares files with editor
    filter "configurations:GameRelease"
			runtime "Release"
            staticruntime "on"
			optimize "on"
            
            targetname "Purring_Engine"
            targetdir("bin/" .. gameoutputdir .. "/%{prj.name}")
            objdir("bin-int/" .. gameoutputdir .. "/%{prj.name}")

            defines 
            { 
                "GAMERELEASE"
            }

            libdirs
            {
                --"vendor/mono/lib/Release"
            }

            -- removes imgui link
            removelinks
            {
                "ImGui"
            }

            -- removes editor files and change entrypoint to WinMain
            files
            {
                --"%{prj.name}/src/EntryPointGame.cpp"
            }
            
            removefiles
            {                
                --"%{prj.name}/src/EntryPoint.cpp",
                "%{prj.name}/src/Editor/*.h",
                "%{prj.name}/src/Editor/*.cpp"
            }

-- Application
project "Application"
    location "Application"

    kind "ConsoleApp"

    language "C++"
    cppdialect "C++17"

    warnings "Extra"

    targetname "Editor"
    targetdir("bin/" .. outputdir .. "/Editor")
    objdir("bin-int/" .. outputdir .. "/Editor")

    flags
    {
        "MultiProcessorCompile"
    } 

	files
	{
		"%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
        "Assets/Animation/**.anim",
        "Assets/Scenes/**.scene",
        "Assets/Prefabs/**.prefab"
	}

    includedirs
    {
        "%{prj.name}/src",
        "Purring_Engine/src",
        "vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLEW}",
        "%{IncludeDir.FMOD}",
        "%{IncludeDir.RTTR}"
    }

    libdirs
    {
        "vendor/freetype/libs",
        "vendor/RTTR/lib"
    }
    
    links
    {
        "Purring_Engine"
    }

    linkoptions { "/ignore:4006", "/ignore:4098", "/ignore:4099"}

    postbuildcommands
    {

    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
			runtime "Debug"
            staticruntime "off"
			symbols "on"

            postbuildcommands
            {
                ("{COPYDIR} ../Assets ../bin/" .. outputdir .. "/Assets"),
                ("{COPYDIR} ../Shaders ../bin/" .. outputdir .. "/Shaders"),
                ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. outputdir .. "/Editor"),
                ("{COPYFILE} ../vendor/RTTR/bin/rttr_core_d.dll ../bin/" .. outputdir .. "/Editor"),
                ("{COPYFILE} ../vendor/RTTR/bin/rttr_core_d.pdb ../bin/" .. outputdir .. "/Editor")
            }

            libdirs
            {
                "Purring_Engine/"
            }

            links
            {
                "rttr_core_d"
            }

    filter "configurations:Release"
			runtime "Release"
            staticruntime "on"
			optimize "on"

            postbuildcommands
            {
                ("{COPYDIR} ../Assets ../bin/" .. outputdir .. "/Assets"),
                ("{COPYDIR} ../Shaders ../bin/" .. outputdir .. "/Shaders"),
                ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. outputdir .. "/Editor"),
                ("{COPYFILE} ../vendor/RTTR/bin/rttr_core.dll ../bin/" .. outputdir .. "/Editor"),
            }
            
            links
            {
                "rttr_core"
            }

    filter "configurations:GameRelease"
			runtime "Release"
            staticruntime "on"
			optimize "on"

            kind "WindowedApp"

            defines 
            { 
                "GAMERELEASE"
            }
            
            targetname "MarchOfTheMeows"
            targetdir("bin/" .. gameoutputdir .. "/MarchOfTheMeows")
            objdir("bin-int/" .. gameoutputdir .. "/MarchOfTheMeows")

            postbuildcommands
            {
                ("{COPYDIR} ../Assets ../bin/" .. gameoutputdir .. "/Assets"),
                ("{COPYDIR} ../Shaders ../bin/" .. gameoutputdir .. "/Shaders"),
                ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. gameoutputdir .. "/MarchOfTheMeows"),
                ("{COPYFILE} ../vendor/RTTR/bin/rttr_core.dll ../bin/" .. gameoutputdir .. "/MarchOfTheMeows"),
            }

            links
            {
                "rttr_core"
            }
