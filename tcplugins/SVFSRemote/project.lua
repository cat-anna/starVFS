
group "tcplugins"
	project "SVFSRemote"
		location(bindir .. "tc/SVFSRemote")
		kind "SharedLib"
		targetdir(bindir .. "tc/SVFSRemote")
		
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
		
local outfile = bindir .. "/SVFSRemote"
		postbuildcommands {
			"pwd",
			"echo Building SVFSRemote plugin installer...",
			"del /Q \"" .. bindir .. "/SVFSRemote.zip\"",
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