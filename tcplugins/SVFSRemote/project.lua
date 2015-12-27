
group "tcplugins"
	project "SVFSRemote"
		location(dir.bin .. "tc/SVFSRemote")
		kind "SharedLib"
		targetdir(dir.bin .. "tc/SVFSRemote")
		
		links  {
			"StarVFS",
		}
		files {
			"**",
			"../../core/**",
		}
		defines {
			"STARVFS_LOG_TO_SINK",
		}
		
		excludes {
			"project.lua",
		}
		
local outfile = dir.bin .. "/SVFSRemote"
		postbuildcommands {
			"echo Building SVFSRemote plugin installer...",
			"del /Q \"" .. dir.bin .. "/SVFSRemote.zip\"",
			"zip SVFSRemote.zip pluginst.inf *.wfx*",
		}		
		
		filter "platforms:x32" 
			targetextension ".wfx"
		filter "platforms:x64" 
			targetextension ".wfx64"
			
		filter 'files:pluginst.inf'
			buildmessage 'Copying pluginst.inf'
			buildcommands 'cp "%{file.relpath}" "%{file.basename}.inf"'
			buildoutputs '%{cfg.objdir}%{file.basename}.inf'
			