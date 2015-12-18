/*
  * Generated by cppsrc.sh
  * On 
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/
#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <mutex>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "arguments.h"

#if 0

static bool LoadInitConfig(SharedLua Lua, SVFS& svfs, InitEnv &e) {
	if (!Lua)
		return false;

	if (e.m_StartRemoteServer)
		svfs.StartServer(e.m_RemoteServerPort);

	return ExecuteCommandLinePipeline(Lua, svfs, e);
}

#endif

//-------------------------------------------------------------------------------------------------

namespace po = boost::program_options;
struct Parser::PrivData  {
	po::options_description m_desc;
	po::variables_map m_vm;
	std::list<std::string> m_Lines;

	std::string GetInitScript() {
		std::stringstream ss;
		for (auto &it : m_Lines)
			ss << it << "\n";
		return ss.str();
	}

	void AddLine(std::string line) { m_Lines.emplace_back(std::move(line)); }
	template<class ...ARGS>
	void AddLine(const char *fmt, ARGS ...args) {
		char buf[8192];
		sprintf(buf, fmt, std::forward<ARGS>(args)...);
		AddLine(std::string(buf));
	}

	void AddPrint(const std::string &line) {
		AddLine("print([[%s]])", line.c_str());
	}
	template<class ...ARGS>
	void AddPrint(const char *fmt, ARGS ...args) {
		char buf[8192];
		sprintf(buf, fmt, std::forward<ARGS>(args)...);
		AddLine("print([[%s]])", buf);
	}

	bool GenInitScriptEnv() {
		return true;
	}
	bool ProcessInitialSettings() {
		//	m_Env.m_StartRemoteServer = vm["remote"].as<bool>();
		//	m_Env.m_RemoteServerPort = vm["remote-port"].as<int>();
		return true;
	}

	bool ProcessMainPipeline() {
		if (!m_vm["library"].empty()) {
			AddPrint("Loading Lua libs...");
			for (auto &it : m_vm["library"].as<std::vector<std::string>>()) {
				AddPrint("Loading library: %s", it.c_str());
				AddLine("require('%s')", it.c_str());
			}
			AddLine("");
		}

		if (!m_vm["mount"].empty()) {
			AddPrint("Mounting containers...");
			for (auto &it : m_vm["mount"].as<std::vector<std::string>>()) {
				auto info = InitEnv::MountInfo::FromString(it);
				AddPrint("Mounting %s on %s", info.m_Path.c_str(), info.m_MountPoint.c_str());
				AddLine("mount('%s', '%s')", info.m_Path.c_str(), info.m_MountPoint.c_str());
			}
			AddLine("");
		}

		//for (auto &it : e.m_Mounts) {
		//	if (e.m_InitVerbose)
		//		printf("Mounting %s to %s\n", it.m_Path.c_str(), it.m_MountPoint.c_str());
		//	auto ret = svfs.OpenContainer(it.m_Path.c_str(), it.m_MountPoint.c_str(), 0);
		//	if (ret != ::StarVFS::VFSErrorCode::Success) {
		//		printf("Failed to mount container!\n");
		//		return false;
		//	}
		//}

		//inject
		//	if (!vm["inject"].empty())
		//		for (auto &it : vm["inject"].as<std::vector<std::string>>())
		//			m_Env.m_Injects.push_back(InitEnv::InjectInfo::FromString(it));

		if (!m_vm["script"].empty()) {
			AddPrint("Executing scripts...");
			for (auto &it : m_vm["script"].as<std::vector<std::string>>()) {
				bool isfile = false;// boost::filesystem::is_regular_file(it);
				if (isfile)
					AddPrint("Executing file: %s", it.c_str());
				else
					AddPrint("Executing chunk: %s", it.c_str());
				if (isfile) {
					AddLine("dofile([[s]])", it.c_str());
				} else {
					AddLine("%s", it.c_str());
				}
			}
			AddLine("");
		}

		//export
		return true;
	}

	bool ProcessFinalSettings() { 
		//	m_Env.m_RunCLI = !vm["no-cli"].as<bool>();
		return true;
	}

	PrivData() : m_desc("Allowed options") {
		m_desc.add_options()
			("help", "produce help message")

			("print-init-script", po::bool_switch(), "Print init script generated from command line and exit")

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
		po::store(po::parse_command_line(argc, argv, m_desc), m_vm);
		po::notify(m_vm);

		if (m_vm.count("help")) {
			std::cout << m_desc << "\n";
			std::cout << "Arguments are processed in order:\n";
			std::cout << "[other settings] -> lua libs -> mounts -> injects -> scripts -> exports -> enter cli\n\n";
			exit(0);
		}

		if (m_vm.count("version")) {
			std::cout << "version\n";
			exit(0);
		}

		if (!GenInitScriptEnv())
			return false;
		if (!ProcessInitialSettings())
			return false;
		if (!ProcessMainPipeline())
			return false;
		if (!ProcessFinalSettings())
			return false;

		if (m_vm["print-init-script"].as<bool>()) {
			std::cout << GetInitScript();
			exit(0);
		}

		return true;
	}
};

//-------------------------------------------------------------------------------------------------

Parser::Parser() {
}

bool Parser::Run(int argc, char **argv) {
	m_InitScriptLines.clear();
	auto data = std::make_unique<PrivData>();
	if (!data->Run(argc, argv))
		return false;
	m_InitScriptLines.swap(data->m_Lines);
	return true;
}

std::string Parser::GetInitScript() {
	std::stringstream ss;
	for (auto &it : m_InitScriptLines)
		ss << it << "\n";
	return ss.str();
}