
local function processContainers(basedir, outdata)
	local files = { }
	for i,v in ipairs(os.matchfiles(basedir .. "core/Container/*Container.h")) do
		local n = string.match (v, "/([^i]%w+)%.h$")
		if n then
			local f = string.gsub(v, basedir, "", 1)
			files[#files + 1] = {
				file = f, 
				name = n,
			}
			outdata.headers[#outdata.headers + 1] = f
			
		end		
	end
	
	local fbeg = [[
namespace Container {
	template<class T>
	void RegisterContainers(T t) {
]]
	local fm = ""
	for i,v in ipairs(files) do
		fm = fm .. string.format("\t\tt.RegisterContainer<%s>(\"%s\");\n", v.name, v.name)
	end
	local fend = [[
	}
}]]
	outdata.functions[#outdata.functions + 1] = fbeg .. fm .. fend
end

local function processModules(basedir, outdata)
	local files = { }
	for i,v in ipairs(os.matchfiles(basedir .. "core/Module/*Module.h")) do
		local n = string.match (v, "/([^i]%w+)%.h$")
		if n then
			local f = string.gsub(v, basedir, "", 1)
			files[#files + 1] = {
				file = f, 
				name = n,
			}
			outdata.headers[#outdata.headers + 1] = f
			
		end		
	end
	
	local fbeg = [[
namespace Modules {
	template<class T>
	void RegisterModules(T t) {
]]
	local fm = ""
	for i,v in ipairs(files) do
		fm = fm .. string.format("\t\tt.RegisterModule<%s>(\"%s\");\n", v.name, v.name)
	end
	local fend = [[
	}
}]]
	outdata.functions[#outdata.functions + 1] = fbeg .. fm .. fend
end

local function processExporters(basedir, outdata)
	local files = { }
	for i,v in ipairs(os.matchfiles(basedir .. "core/Exporter/*Exporter.h")) do
		local n = string.match (v, "/([^i]%w+)%.h$")
		if n then
			local f = string.gsub(v, basedir, "", 1)
			files[#files + 1] = {
				file = f, 
				name = n,
			}
			outdata.headers[#outdata.headers + 1] = f
			
		end		
	end
	
	local fbeg = [[
namespace Exporters {
	template<class T>
	void RegisterExporter(T t) {
]]
	local fm = ""
	for i,v in ipairs(files) do
		fm = fm .. string.format("\t\tt.RegisterExporter<%s>(\"%s\");\n", v.name, v.name)
	end
	local fend = [[
	}
}]]
	outdata.functions[#outdata.functions + 1] = fbeg .. fm .. fend
end

function GenerateModules(outfilename, basedir)
	basedir = basedir or ""
	
	if not outfilename then
		print "Out file name is not provided!"
		os.exit(1)
	end
	
	local outdata = {
		headers = { },
		functions = { }
	}
	
	processContainers(basedir, outdata)
	processModules(basedir, outdata)
	processExporters(basedir, outdata)
	
	local genout = { }

	local function addout(line, ...)
		if not line then
			return
		end
		genout[#genout + 1] = line
		addout(...)
	end
	
	addout "/*"
	addout " * Automatically generated file"
	addout " * DO NOT EDIT!"
	addout " */"
	addout ""
	for i,v in ipairs(outdata.headers) do
		addout (string.format("#include <%s>", v))
	end
	addout ""
	addout "namespace StarVFS {"
	addout ""
	for i,v in ipairs(outdata.functions) do
		addout (v)
		addout ""
	end
	
addout [[
template<class T>
void RegisteerAll(T t) {
	Container::RegisterContainers(t);
	Module::RegisterModules(t);
	Exporters::RegisterExporters(t);
}
]]	

	addout "}"
	
	f = io.open(outfilename, "w")
	for i,v in ipairs(genout) do
		f:write(v, "\n")
	end	
	f:close()
end