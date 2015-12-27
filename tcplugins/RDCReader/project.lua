
group "tcplugins"
	project "RDCReader"
		location(dir.bin .. "tc/RDCReader")
		kind "SharedLib"
		targetdir(dir.bin .. "tc/RDCReader")
		
		links  {
		}
		files {
			"**",
			"../../core/**",
		}
		defines {
			"STARVFS_LOG_TO_SINK",
		}
		
local outfile = dir.bin .. "/SVFSRemote"
		postbuildcommands {
			"echo Building RDCReader plugin installer...",
			"del /Q \"" .. dir.bin  .. "/RDCReader.zip\"",
			"zip RDCReader.zip pluginst.inf *.wcx*",
		}		
		
		filter "platforms:x32" 
			targetextension ".wcx"
		filter "platforms:x64" 
			targetextension ".wcx64"
			
		filter 'files:pluginst.inf'
			buildmessage 'Copying pluginst.inf'
			buildcommands 'cp "%{file.relpath}" "%{file.basename}.inf"'
			buildoutputs '%{cfg.objdir}%{file.basename}.inf'
			