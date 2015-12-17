#pragma once

namespace StarVFS {

struct Settings {
	struct Initial {
		enum {
			FileTableSize	= 1 * 1024,
			StringTableSize	= 4 * 1024,
			HandleTableSize = 1 * 1024,
		};
	};
};

using FileSize = uint32_t;
using FileID = uint16_t; //value 0 means invalid id
using ContainerID = uint8_t;
using StringID = uint32_t;

using Char = char;
using String = std::string;// std::basic_string<Char>;
using CString = Char*;

extern void (*StarVFSLogSink)(const char *file, const char *function, unsigned line, const char *log, const char *type);

} //namespace StarVFS 

#ifdef STARVFS_LOG_TO_SINK

#define STARVFS_DEFINE_DEFAULT_LOG_API

#define STARVFSLOG(What, fmt, ...)\
	do { \
		char buf[4096]; \
		sprintf(buf, fmt, __VA_ARGS__); \
		::StarVFS::StarVFSLogSink(__FILE__, __FUNCTION__, __LINE__, buf, #What);\
	} while(0)

#else

#define STARVFS_DEFINE_DEFAULT_LOG_API

#define STARVFSLOG(What, fmt, ...)\
	do { \
		char buf[1024]; \
		sprintf(buf, "[%s][%s:%d] StarVFS: " fmt "\n", #What, strrchr(__FILE__, '\\')+1, __LINE__, __VA_ARGS__); \
		std::cout << buf << std::flush;\
	} while(0)

#endif

#define STARVFSErrorLog(fmt, ...) STARVFSLOG(ERROR, fmt, __VA_ARGS__)

#ifdef DEBUG 
#define STARVFSDebugLog(fmt, ...) STARVFSLOG(DEBUG, fmt, __VA_ARGS__)
#else
#define STARVFSDebugLog(fmt, ...) do { } while(0)
#endif

#define STARVFSDebugInfoLog STARVFSDebugLog