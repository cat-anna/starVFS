#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <mutex>
#include <iostream>
#include <boost/program_options.hpp>

#include "svfs.h"
#include "cli.h"

#include <core/Modules/Remote.h>

bool ParseArguments(int argc, char **argv, InitEnv &e);
bool RunCLI(SVFS& svfs, InitEnv &e);

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

	auto e = std::make_unique<InitEnv>();
	if (argc > 1) {
		if (!ParseArguments(argc, argv, *e)) {
			std::cout << "Unable to parse arguments!\n";
			return 1;
		}
	}

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

	if(!svfs->LoadConfig(*e)) {
		std::cout << "Unable to load initial configuration!\n";
		return 1;
	}

	if (e->m_RunCLI) {
		CLI cli(lua);
		if (!cli.Enter(*svfs, *e)) {
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

//-------------------------------------------------------------------------------------------------

bool SVFSInit(SVFS& svfs, InitEnv &e) {

	if(e.m_StartRemoteServer)
		svfs.LoadModule<StarVFS::Modules::Remote>(e.m_RemoteServerPort);

	return true;
}

//-------------------------------------------------------------------------------------------------

namespace po = boost::program_options;

struct Parser {
	po::options_description m_desc;
	InitEnv &m_Env;

	Parser(InitEnv &e): m_desc("Allowed options"), m_Env(e) {
		m_desc.add_options()
			("help", "produce help message")

			("no-cli", po::bool_switch(), "Do not enter CLI")

			("remote", po::bool_switch(), "Start remote server")
			("remote-port", po::value<int>()->default_value(0), "Set port for remote server")

			("script,S", po::value<std::vector<std::string>>(), "Execute scipt before entering CLI")

/*
			mount,m  mount container at mountpoint file[:mountpoint[:type]]
			export	TYPE:FILE:BASEPATH export content of BASEPATH

			inject FILE:LOCATION

*/

			;
	}

	bool Run(int argc, char **argv) {
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, m_desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << m_desc << "\n";
			return true;
		}

		m_Env.m_InitScripts = vm["script"].as<std::vector<std::string>>();

		m_Env.m_RunCLI = !vm["no-cli"].as<bool>();
		m_Env.m_StartRemoteServer = vm["remote"].as<bool>();
		m_Env.m_RemoteServerPort = vm["remote-port"].as<int>();

		return true;
	}

};

bool ParseArguments(int argc, char **argv, InitEnv &e) {
	Parser p(e);
	return p.Run(argc, argv);
}
