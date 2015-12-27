	
group "Libraries"
	project "StarVFS"
		location(bindir .. "StarVFS")
		kind "StaticLib"
		files {
			"**",
		}
		
include "generator.lua"
		local outfile = bindir .. "SVFSRegister.h"
		GenerateModules(outfile, path.getrelative(os.getcwd(), dir.base) .. "/")
		files { outfile }
		