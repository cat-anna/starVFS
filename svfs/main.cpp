#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <mutex>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "svfs.h"
#include "cli.h"
#include "arguments.h"

int main(int argc, char **argv) {
//	svfs lvfs;
//	if (!lvfs.HandleArguments(argc - 1, argv + 1)) {
//		std::cout << "Unable to handle command line arguments\n";
//		return 1;
//	}
//
//	if (!lvfs.RunCommandLine()) {
//		std::cout << "Unable to start command line\n";
//		return 1;
//	}

	auto lua = Lua::New();
	if (!lua->Initialize()) {
		printf("Unable to initialize lua vm!\n");
		return 1;
	}

	auto svfs = std::make_unique<SVFS>(lua);
	if (!svfs->Initialize()) {
		printf("Unable to initialize svfs!\n");
		return 1;
	}
	
	if (argc > 1){
		Parser p;
		if (!p.Run(argc, argv)) {
			std::cout << "Unable to parse arguments!\n";
			return 1;
		}
		if (!lua->ExecuteScriptChunk(p.GetInitScript().c_str(), "initscript"))
			return 1;
	}

	auto e = std::make_unique<InitEnv>();
	if (e->m_RunCLI) {
		CLI cli(lua);
		if (!cli.Enter(*svfs)) {
			std::cout << "Unable to run CLI!\n";
			return 1;
		}
	}


#if 0
	StarVFS::StarVFS vfs;

	printf("Result: %d\n\n\n", vfs.OpenContainer(argc > 1 ? argv[1] : "tcp://127.0.0.1:55555"));
//	printf("Result: %d\n\n\n", vfs.OpenContainer(argc > 2 ? argv[2] : "./core/"));

//	printf("FileTable:\n");
//	vfs.DumpFileTable(std::cout);
	printf("Structure:\n");
	vfs.DumpStructure(std::cout);

	std::cout << std::flush;

	if (argc > 2 && argv[2][0] == '1') {
		vfs.LoadModule<StarVFS::Modules::Remote>();
		while (true)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

//	auto fid = vfs.FindFile("/premake5.lua");
//	printf("fid: %d\n", fid);
//	
//	printf("\n\n");
//
	auto h = vfs.OpenFile(argc > 3 ? argv[3] : "/premake5.lua");

	printf("path: %s\n", h.GetFullPath().c_str());
	printf("size: %d\n", h.GetSize());
	printf("rwmode: %d\n", h.GetRWMode());

	StarVFS::CharTable ct;
	if (!h.GetFileData(ct)) {
		printf("\nFailed to read data!\n");
	} else {
		printf("%s\n", ct.get());
	}

#endif
	return 0;
}
