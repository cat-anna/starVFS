	
group "Libraries"
	project "StaticZlib"
		location(dir.bin .. "libz/StaticZlib")
		kind "StaticLib"
		
		--there is no need fo ary debug mode in zlib
		undefines {
			"DEBUG"
		}
		optimize "Speed"
		warnings "Off"	
		flags {
			"NoRuntimeChecks",
		}
		
		files {
			"zlib-1.2.8/*",
		}
		filter "action:vs*"
			disablewarnings {
				"4131",
			}