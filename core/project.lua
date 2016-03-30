	
group "Libraries"
	project "StarVFS"
		location(dir.bin .. "StarVFS")
		kind "StaticLib"
		files {
			"**",
		}
		
include "generator.lua"
		local outfile = dir.bin .. "SVFSRegister.h"
		GenerateModules(outfile, os.getcwd() .. "/")
		files { outfile }