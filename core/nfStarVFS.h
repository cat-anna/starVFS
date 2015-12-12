#pragma once

#include "config.h"

namespace StarVFS {

class iFileReader;
class iFileWritter;
class iContainer;
using Container = std::unique_ptr<iContainer>;

class FileTable;
struct File;

class FileHandle;
class HandleTable;

template<class T, class INDEX> class DynamicStringTable;
using StringTable = DynamicStringTable<Char, StringID>;

using CharTable = std::unique_ptr<char[]>;

enum class VFSErrorCode {
	Success,
	ContainerDoesNotExits,
	ContainerCriticalError,
	UnknownContainerFormat,
	InternalError,
};

enum class OpenMode {
	CreateNew,
//	OpenOrCreate,
	OpenExisting,
};

enum class RWMode {
	None, R, W, RW,
};

namespace Modules {
	class iModule;
}
} //namespace StarVFS 
