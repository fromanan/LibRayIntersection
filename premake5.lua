package.path = './?.lua;' .. package.path;
PremakeHelpers = require("PremakeHelpers");

outputdir = PremakeHelpers.OutputDirectory;

PROJECT_ROOT = "LibRayIntersection";

-- Include directories relative to root folder (solution directory)
IncludeDir = {
	GLEW = PROJECT_ROOT .. "/vendor/glew-1.9.0",
	GLM = PROJECT_ROOT .. "/vendor/glm",
	Other = PROJECT_ROOT .. "/vendor/other"
};

workspace (PROJECT_ROOT)
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	platforms
	{
		"Win32",
		"Win64"
	}

	startproject (PROJECT_ROOT)

--include (IncludeDir.GLEW)
--include (IncludeDir.GLM)

project (PROJECT_ROOT)
	location (PROJECT_ROOT)
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	entrypoint ("wWinMainCRTStartup")

	pchheader ("stdafx.h")
	pchsource (PROJECT_ROOT .. "/src/stdafx.cpp")

	files
	{
		-- Source Files
		PremakeHelpers.IncludeCHeaders,
		PremakeHelpers.IncludeCPPSources,

		-- Resource Files
		("%{prj.name}/res/" .. PROJECT_ROOT .. ".rc"),
		("%{prj.name}/res/" .. PROJECT_ROOT .. ".aps"),
		("%{prj.name}/res/" .. PROJECT_ROOT .. ".rc2"),

		-- Not Sure...
		"%{prj.name}/src/graphics/graphics.ffs_gui",
		"%{prj.name}/src/graphics/sync-graphics.fsy",

		-- Graphics Libraries
		"%{IncludeDir.GLEW}/**.h",
		"%{IncludeDir.GLEW}/**.cpp",
		"%{IncludeDir.GLM}/**.hpp",
		"%{IncludeDir.GLM}/**.inl",

		"%{IncludeDir.Other}/**"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/graphics",
		"%{IncludeDir.GLEW}",
		"%{IncludeDir.GLEW}/include/GL",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.GLM}/core",
		"%{IncludeDir.GLM}/gtc",
		"%{IncludeDir.Other}"
	}

	links
	{
		"ijgjpeg.dll",
		"libvrml.dll",

		"opengl32.lib",
		"glu32.lib",
		"winmm.lib",

		"dwmapi.lib",
		"advapi32.lib",

		"VERSION.dll",
		"MSVCP140D.dll",
		"Mfc140d.dll",
		"KERNEL32.dll",
		"USER32.dll",
		"GDI32.dll",
		"SHELL32.dll",
		"OLEAUT32.dll",
		"VCRUNTIME140D.dll",
		"Ucrtgbased.dll",
		"WS2_32.dll"
	}

	defines
	{
		"WINDOWS_EXPORT_ALL_SYMBOLS",
		"_UNICODE"
	}

	linkoptions
	{
		"/NODEFAULTLIB:library"
	}

	defines
	{
		"_AFXDLL"
	}

	-- Use of deprecated symbol : 'register'
	buildoptions { "-IGNORE:C5033" }

	filter "platforms:Win32"
		architecture "x86"

	filter "platforms:Win64"
    	architecture "x64"

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		links { "user32", "gdi32" }
		--defines { "WIN32" }

	filter "configurations:Debug"
		symbols "On"
		defines { "_DEBUG" }
		links { "msvcrtd.lib" }
		runtime "Debug"

	filter "configurations:Release"
		optimize "On"
		links { "msvcrt.lib" }
		runtime "Release"

	filter "configurations:Dist"
		optimize "On"
		links { "msvcrt.lib" }
		runtime "Release"