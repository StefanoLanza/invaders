-- Options

-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2019

-- Filters
local filter_vs = "action:vs*"
local filter_xcode = "action:xcode*"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x86_64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("IKA-Invaders")
	configurations { "Debug", "Release" }
	platforms { "x86", "x86_64" }
	language "C++"
	location (workspacePath)
	characterset "MBCS"
	flags   { "MultiProcessorCompile", } --"ConformanceMode", }
	startproject "UnitTest"
	cppdialect "c++17"
	exceptionhandling "Off"
	rtti "Off"

filter { filter_vs }
	buildoptions { "/permissive-", }
	system "Windows"
	defines { "_ENABLE_EXTENDED_ALIGNED_STORAGE", }

filter { filter_vs, filter_x86, }
	defines { "WIN32", "_WIN32", }

filter { filter_vs, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_vs, filter_debug, }
	defines { "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { filter_vs, filter_release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { filter_xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter "system:windows"
	defines { "WINDOWS", "_HAS_EXCEPTIONS=0", }

filter "platforms:x86"
	architecture "x86"

filter "platforms:x86_64"
	architecture "x86_64"

filter "configurations:Debug*"
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter "configurations:Release*"
	defines { "NDEBUG", }
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

filter { "toolset:gcc" }
    -- https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
    buildoptions { "-ffat-lto-objects", "-Wno-unused-parameter", }

filter { "toolset:clang", "configurations:Debug*" }
	buildoptions { "/fsanitize=address" }

project("inih")
	kind "StaticLib"
	files { "external/inih-master/ini.c", "external/inih-master/ini.h", }
	includedirs { "./", "external/", }

project("Invaders")
	kind "ConsoleApp"
	files { "src/*.*", }
	includedirs { ".", "external", }
	links { "inih", }
	filter "system:linux"
		files {"src/linux/**.*"}
	filter "system:Windows"
		files {"src/windows/**.*"}
	filter {}
