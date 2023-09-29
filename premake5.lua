-- premake5.lua
workspace "Purring_Engine"
    architecture "x64"
    startproject "Application"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


-- Include directories 
IncludeDir = {}
IncludeDir["GLFW"]          = "vendor/GLFW/include"
IncludeDir["ImGui"]         = "vendor/imgui"
IncludeDir["glm"]           = "vendor/glm"
IncludeDir["stb_image"]     = "vendor/stb_image"
IncludeDir["GLEW"]          = "vendor/GLEW/include"
IncludeDir["FMOD"]          = "vendor/FMOD/core/inc" -- CORE

-- external libraries
group "Library"
    -- GLFW
    project "GLFW"
        location "vendor/GLFW"

        kind "StaticLib"        
        staticruntime "on"

        language "C"

        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")

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
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"
    -- imgui
    project "ImGui"
        location "vendor/imgui"
        kind "StaticLib"
        staticruntime "on"
        
        language "C++"
        cppdialect "C++17"

        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")

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
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"
   
group ""

-- Purring_Engine
project "Purring_Engine"
    location "Purring_Engine"

    kind "StaticLib"
    staticruntime "on"

    language "C++"
    cppdialect "C++17"
    
    warnings "Extra"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prpch.h"
    pchsource "Purring_Engine/src/prpch/prpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl"
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
        "vendor/freetype/include"
    }

    libdirs
    {
        "vendor/GLEW/lib/Release/x64",
        "vendor/FMOD/core/lib/x64",
        "vendor/freetype/libs"
    }

    links
    {
        "GLFW",
        "glew32s",
        "ImGui",
        "opengl32",  -- not sure if needed
        "fmod_vc"
    }

    linkoptions { "/ignore:4006" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
			runtime "Debug"
			symbols "on"

    filter "configurations:Release"
			runtime "Release"
			optimize "on"

-- Application
project "Application"
    location "Application"

    kind "ConsoleApp"
    staticruntime "on"

    language "C++"
    cppdialect "C++17"

    warnings "Extra"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

    includedirs
    {
        "%{prj.name}/src",
        "Purring_Engine/src",
        "vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLEW}",
        "%{IncludeDir.FMOD}"
    }

    libdirs
    {
        "vendor/freetype/libs"
    }

    links
    {
        "Purring_Engine",
        "freetype"
    }
    
    linkoptions { "/ignore:4006", "/ignore:4098", "/ignore:4099"}

    postbuildcommands
    {
        ("{COPYDIR} ../Assets ../bin/" .. outputdir .. "/Assets"),
        ("{COPYDIR} ../Shaders ../bin/" .. outputdir .. "/Shaders"),
        ("{COPYFILE} ../vendor/FMOD/core/lib/x64/fmod.dll ../bin/" .. outputdir .. "/Application"),
        ("{COPYFILE} config.json ../bin/" .. outputdir .. "/Application")        
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
			runtime "Debug"
			symbols "on"

    filter "configurations:Release"
			runtime "Release"
			optimize "on"
