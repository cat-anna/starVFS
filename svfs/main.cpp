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

#include "main.h"

static bool ParseArguments(int argc, char **argv, InitEnv &e);
static bool LoadInitConfig(SharedLua Lua, SVFS& svfs, InitEnv &e);

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
	if (argc > 1 && !ParseArguments(argc, argv, *e)) {
		std::cout << "Unable to parse arguments!\n";
		return 1;
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

	if (!LoadInitConfig(lua, *svfs, *e)) {
		printf("Unable to load initial config!\n");
		return 1;
	}

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

//-------------------------------------------------------------------------------------------------

static bool ExecuteCommandLinePipeline(SharedLua Lua, SVFS& svfs, InitEnv &e) {

//	std::cout << "[other settings] -> lua libs -> mounts -> injects -> scripts -> exports -> enter cli\n\n";

	for (auto &it : e.m_LuaLibs) {
		if (e.m_InitVerbose)
			printf("Loading library %s\n", it.c_str());
		if (!Lua->LoadLibrary(it.c_str()))
			return false;
	}

	for (auto &it : e.m_Mounts) {
		if (e.m_InitVerbose)
			printf("Mounting %s to %s\n", it.m_Path.c_str(), it.m_MountPoint.c_str());
		auto ret = svfs.OpenContainer(it.m_Path.c_str(), it.m_MountPoint.c_str(), 0);
		if (ret != ::StarVFS::VFSErrorCode::Success) {
			printf("Failed to mount container!\n");
			return false;
		}
	}

	//inject

	for (auto &it : e.m_Scripts) {
		bool isfile = false;// boost::filesystem::is_regular_file(it);
		if (e.m_InitVerbose) {
			if(isfile)
				std::cout << "Executing file: " << it << "\n" << std::flush;
			else
				std::cout << "Executing chunk: " << it << "\n" << std::flush;
		}
		bool ret;
		if (isfile) {
			ret = Lua->ExecuteScriptFile(it.c_str());
		} else {
			ret = Lua->ExecuteScriptChunk(it.c_str());
		}
		if (!ret)
			return false;
	}

	//export

	return true;
}

static bool LoadInitConfig(SharedLua Lua, SVFS& svfs, InitEnv &e) {
	if (!Lua)
		return false;

	if (e.m_StartRemoteServer)
		svfs.StartServer(e.m_RemoteServerPort);

	return ExecuteCommandLinePipeline(Lua, svfs, e);
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
			("version,v", "Print version information and exit")

			("remote", po::bool_switch(), "Start remote server")
			("remote-port", po::value<int>()->default_value(0), "Set port for remote server")

			("library,l", po::value<std::vector<std::string>>(), "Load lua library")
			("mount,m", po::value<std::vector<std::string>>(), "Mount container. format: FILE[:MOUNTPOINT]") //[:TYPE]
//			("inject,i", po::value<std::vector<std::string>>(), "Inject file into vfs. format: SYSPATH:VPATH. Virtual path will be created if not exits") //[:TYPE]
			("script,s", po::value<std::vector<std::string>>(), "Execute scipt before entering CLI. It may be file or chunk of code")
//			("export", po::value<std::vector<std::string>>(), "Export vfs content. format: TYPE:OUTFILE[:BASEPATH]")
			;
	}

	bool Run(int argc, char **argv) {
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, m_desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << m_desc << "\n";
			std::cout << "Arguments are processed in order:\n";
			std::cout << "[other settings] -> lua libs -> mounts -> injects -> scripts -> exports -> enter cli\n\n";
			exit(0);
			return true;
		}

		if (vm.count("version")) {
			std::cout << "version\n";
			exit(0);
			return true;
		}

		if(!vm["library"].empty())
			m_Env.m_LuaLibs = vm["library"].as<std::vector<std::string>>();
		if(!vm["mount"].empty())
			for (auto &it : vm["mount"].as<std::vector<std::string>>()) 
				m_Env.m_Mounts.push_back(InitEnv::MountInfo::FromString(it));
		if (!vm["inject"].empty())
			for (auto &it : vm["inject"].as<std::vector<std::string>>())
				m_Env.m_Injects.push_back(InitEnv::InjectInfo::FromString(it));
		if (!vm["script"].empty())
			m_Env.m_Scripts = vm["script"].as<std::vector<std::string>>();

		m_Env.m_RunCLI = !vm["no-cli"].as<bool>();
		m_Env.m_StartRemoteServer = vm["remote"].as<bool>();
		m_Env.m_RemoteServerPort = vm["remote-port"].as<int>();

		return true;
	}

};

static bool ParseArguments(int argc, char **argv, InitEnv &e) {
	Parser p(e);
	return p.Run(argc, argv);
}
