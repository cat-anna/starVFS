	
group ""
	project "StarVFS"
		location(bindir .. "StarVFS")
		kind "StaticLib"
		files {
			"**",
		}
		
include "generator.lua"
		GenerateModules(bindir .. "SVFSRegister.h", path.getrelative(os.getcwd(), dir.base) .. "/")
	