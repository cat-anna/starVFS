	
group "Libraries"
	project "StarVFS"
		location(dir.bin .. "StarVFS")
		kind "StaticLib"
		files {
			"**",
		}
		
include "generator.lua"
		local outfile = dir.bin .. "SVFSRegister.h"
		print("cwd", os.getcwd())
		GenerateModules(outfile, os.getcwd() .. "/")
		files { outfile }
		-- path.getrelative(os.getcwd(), dir.base)