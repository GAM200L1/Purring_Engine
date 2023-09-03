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
IncludeDir["Glad"]          = "vendor/Glad/include"
IncludeDir["ImGui"]         = "vendor/imgui"
IncludeDir["glm"]           = "vendor/glm"
IncludeDir["stb_image"]     = "vendor/stb_image"

-- external libraries
group "Library"
    -- GLFW
    project "GLFW"
        location "vendor/GLFW"

        kind "StaticLib"

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
            staticruntime "on"

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
            "vendor/imgui/*.cpp"
        }

        filter "system:windows"
            systemversion "latest"

        filter "system:linux"
            pic "On"
            systemversion "latest"
            cppdialect "C++17"
            staticruntime "On"

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"
    -- Glad
    project "Glad"
        location "vendor/Glad"
        kind "StaticLib"
        staticruntime "on"

        language "C"

        targetdir ("bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/vendor/%{prj.name}")

        files
        {
            "vendor/Glad/include/**.h",
            "vendor/Glad/src/glad.c"
        }

        includedirs
        {
            "%{IncludeDir.Glad}"
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

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prpch.h"
    pchsource "Purring_Engine/src/prpch.cpp"

    files
    {
        "%{prj.name}/include/**.h",
        "%{prj.name}/src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl"
    }

    includedirs
    {
        "%{prj.name}/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib" -- not sure if needed
    }

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

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/include/**.h",
		"%{prj.name}/src/**.cpp"
	}

    includedirs
    {
        "%{prj.name}/include",
        "Purring_Engine/include",
        "vendor",
        "%{IncludeDir.glm}"
    }

    links
    {
        "Purring_Engine"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
			runtime "Debug"
			symbols "on"

    filter "configurations:Release"
			runtime "Release"
			optimize "on"

