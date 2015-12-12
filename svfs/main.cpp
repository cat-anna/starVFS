#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <iostream>

#include <core/nStarVFS.h>
#include <core/Modules/Remote.h>

int main(int argc, char **argv) {
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

	return 0;
}
