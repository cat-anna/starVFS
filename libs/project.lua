	
group "Libraries"
	project "StaticZlib"
		location(dir.bin .. "libz/StaticZlib")
		kind "StaticLib"
		files {
			"zlib-1.2.8/*",
		}
		filter "action:vs*"
			disablewarnings {
				"4131",
			}