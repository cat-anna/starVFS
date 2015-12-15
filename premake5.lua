
local bindir = "bin_" .. _ACTION .. "/"

workspace "StarVFS"
	configurations { "Debug", "Release", }
	platforms { "x32", "x64" }
	language "C++"
	
	startproject "svfs"	
	location "."
	basedir "."
	debugdir "."
	targetdir(bindir)
  
	defines {
		"STARVFS_FOLDER_CONTAINER",
	}
	includedirs {
		".",
	}	
	
	flags { 
		"NoMinimalRebuild", 
		"MultiProcessorCompile", 
	}
	
	filter "system:windows"
		defines{ "WINDOWS", "OS_NAME=\"Windows\"", }
	filter "system:linux"
		defines{ "LINUX", "OS_NAME=\"Linux\"", }

	filter "action:gmake"
		defines { "GCC", }
		buildoptions{ 
			"-std=c++1y", 
		}
		-- linkoptions{ "-Wl,--no-as-needed", }
		links { 
			"dl",	
			"pthread",
			"boost_system",
			"boost_filesystem",	
		}
		disablewarnings {
			"unused-parameter",
		}
	filter "action:vs*"
		defines { 
			"MSVC", 
		}
		disablewarnings {
			"4100", -- unreferenced formal parameter
			"4201", -- nameless struct/union
			"4127", -- conditional expression is constant		
			"4996",
			"4200",
		}	
	
	filter "platforms:x32" 
		architecture "x32"
		defines { "X32", "PLATFORM_NAME=\"X32\"", }
	filter "platforms:x64" 
		architecture "x64"		
		defines { "X64", "PLATFORM_NAME=\"X64\"", }		
		
	filter "configurations:Debug"
		flags { "Symbols", }
		optimize "Debug"
		warnings "Extra"
		runtime "Debug"
		defines {
			"ConfigurationName=\"Debug\"",
			"DEBUG",
		}
	filter "configurations:Release"
		flags { }		
		optimize "Speed"
		warnings "Default"	
		runtime "Release"		
		defines {
			"ConfigurationName=\"Release\"",
			"RELEASE",	
		}	
	
	filter 'files:**.lua'
		buildmessage 'Compiling %{file.relpath} with bin2c'
		buildcommands 'bin2c -o "%{cfg.objdir}/%{file.basename}.lua.h" -n %{file.basename}_lua "%{file.relpath}" '
		buildoutputs '%{cfg.objdir}/%{file.basename}.lua.h'

	project "StarVFS"
		location(bindir .. "StarVFS")
		kind "StaticLib"
		files {
			"core/**",
			"rdc/**",
		}
		
	project "rdcc"
		location(bindir .. "rdcc")
		kind "ConsoleApp"
		links "StarVFS"
		files {
			"rdcc/**",
		}
		
	project "svfs"
		location(bindir .. "svfs")
		kind "ConsoleApp"
		links {
			"StarVFS",
			"lua53",
			"readline",
		}
		files {
			"svfs/**",
		}	
		includedirs {
			"%{cfg.objdir}",
		}
		
	local i,v
	for i,v in ipairs(os.matchfiles(os.getcwd() .. "/**/project.lua")) do
		print("Found project: " .. v)
		include(v)
	end
