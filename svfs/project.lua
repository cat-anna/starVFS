	
group "Utils"
	project "svfs"
		location(dir.bin .. "svfs")
		kind "ConsoleApp"
		links {
			"StarVFS",
			"lua",
			"StaticZLib",
			"boost_program_options",
		}
		files {
			"**",
		}	
		includedirs {
			"%{cfg.objdir}",
		}
		
		filter 'files:scripts/**.lua'
			buildmessage 'Compiling %{file.relpath} with bin2c'
			buildcommands 'bin2c -o "%{cfg.objdir}/%{file.basename}.lua.h" -n %{file.basename}_lua "%{file.relpath}" '
			buildoutputs '%{cfg.objdir}/%{file.basename}.lua.h'
			
