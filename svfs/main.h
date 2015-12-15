
struct InitEnv {
	struct BaseStringSet {
		virtual ~BaseStringSet() {}
		virtual bool split(const std::string &value) = 0;
	protected:
		bool xsplit(const char *&input, std::string &first) {
			if (!input || !*input)
				return true;

			auto dd = strchr(input, ':');
			if (!dd) {
				first = input;
				input = nullptr;
			} else {
				first = std::string(input, dd - input);
				input = dd + 1;
			}

			return true;
		}

		template<class ...ARGS>
		bool xsplit(const char *input, std::string &first, ARGS& ... args) {
			xsplit(input, first);
			return xsplit(input, args...);
		}
	};

	struct MountInfo : public BaseStringSet {
		std::string m_Path, m_MountPoint;
		virtual bool split(const std::string &value) override {
			xsplit(value.c_str(), m_Path, m_MountPoint);
			if (m_MountPoint.empty())
				m_MountPoint = "/";
			if (m_MountPoint.front() != '/')
				m_MountPoint = std::string("/") + m_MountPoint;
			return true;
		}
		static MountInfo FromString(const std::string &value) {
			MountInfo r;
			r.split(value);
			return r;
		}
	};

	struct InjectInfo : public BaseStringSet {
		//std::string m_Path, m_MountPoint;
		virtual bool split(const std::string &value) override {
			return false;// xsplit(value.c_str(), m_Path, m_MountPoint);
		}
		static InjectInfo FromString(const std::string &value) {
			InjectInfo r;
			// r.split(value);
			return r;
		}
	};

	std::vector<std::string> m_LuaLibs;
	std::vector<MountInfo> m_Mounts;
	std::vector<InjectInfo> m_Injects;
	std::vector<std::string> m_Scripts;
	//	std::vector<std::string> m_Exports;

	bool m_RunCLI = true;

	bool m_StartRemoteServer = false;
	int m_RemoteServerPort = 0;

	bool m_InitVerbose = true;
};