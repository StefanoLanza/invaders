-- Options

-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2022, build/gmake
local rootDir = _MAIN_SCRIPT_DIR
local rootBinDir = path.join(_MAIN_SCRIPT_DIR, "bin")

-- Filters
local filter_vs = "action:vs*"
local filter_make = "action:gmake"
local filter_xcode = "action:xcode*"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("Invaders")
	configurations { "Debug", "Release" }
	platforms { "x64" }
	language "C++"
	location (workspacePath)
	characterset "MBCS"
	flags   { "MultiProcessorCompile", } --"ConformanceMode", }
	cppdialect "C++latest" -- for designated initialized under MSVC
	exceptionhandling "Off"
	rtti "Off"
	startproject "Invaders"

filter { filter_vs }
	defines { "WINDOWS", "_HAS_EXCEPTIONS=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",}
	buildoptions { 
		"/permissive-", 
	}
	disablewarnings { "4100" } -- unreferenced formal parameter
	system "Windows"
	defines { "_ENABLE_EXTENDED_ALIGNED_STORAGE", }

filter { filter_vs, filter_x86, }
	defines { "WIN32", "_WIN32", }

filter { filter_vs, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_vs, filter_debug, }
	defines {   }

filter { filter_vs, filter_release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", }

filter { filter_xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET
	
filter "platforms:x64"
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

project("Engine")
	kind "StaticLib"
	files { "src/engine/*.*", }
	includedirs { "./", "src", }
	filter "system:linux"
		files {"src/engine/linux/**.*"}
		links {"ncursesw", "dl", }
		-- relocation R_X86_64_PC32 against symbol `_ZNSt6vectorI5ImageSaIS0_EED1Ev' can not be used when making a shared object; recompile with -fPIC
		-- TODO investigate
		buildoptions { "-fPIC", }
	filter "system:Windows"
		files {"src/engine/windows/**.*"}
	filter {}

project("Game")
	kind "StaticLib"
	files { "src/game/**.*", }
	includedirs { "./", "src", }
	links {"Engine", }

project("Scripts")
	kind "SharedLib"
	files { "src/scripts/**.*", }
	includedirs { "./", "src", }
	links { "Game", "Engine", }

project("Invaders")
	kind "ConsoleApp"
	files { "src/main/*.*", }
	includedirs { ".", "external", "src", }
	filter { filter_make }
		-- ! Order matters with make
		links { "inih", "ncursesw", "dl", "Scripts", "Game", "Engine", }
	filter { filter_vs }
		links { "inih", "Game", "Engine", "Scripts", }
	filter {}

	-- Copy assets to target dir
	local assetDir = path.join(rootDir, "assets")
	local targetDir = "%{cfg.buildtarget.directory}"
	local binDir = path.join(rootDir, "bin")
	postbuildcommands {
		"{ECHO}, Copying assets to target folder "..targetDir,
		--"{MKDIR} "..binDir,
		--"{COPY} %{cfg.buildtarget.directory}/*.exe "..binDir,
		--"{COPY} %{cfg.buildtarget.directory}/*.dll "..binDir,
		"{COPY} "..assetDir.."/* "..targetDir,
	}
	debugdir(path.join(workspacePath, targetDir))
