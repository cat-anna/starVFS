#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "config.h"

namespace StarVFS {

class iFileReader;
class iFileWritter;

class FileTable;
struct File;
class StarVFS;
class Register;
class FileHandle;
class HandleTable;
using HandleEnumerateFunc = std::function<bool(FileID)>; //shall return false to break enumeration

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

namespace Containers {
	class iContainer;
	class FileTableInterface;
}

using CreateContainerResult = std::pair < VFSErrorCode, Containers::iContainer* > ;

using UniqueFileTableInterface = std::unique_ptr<Containers::FileTableInterface>;
using Container = std::unique_ptr<Containers::iContainer>;

//-------------------------------------------------------------------------------------------------

union FileFlags {
	uint8_t intval;
	struct {
		uint8_t Valid : 1;				//Entry is valid
		uint8_t Directory : 1;
		uint8_t SymLink : 1;		//not yet used
		uint8_t Deleted : 1;			//valid but deleted
										//uint8_t Used : 1;
										//uint8_t unused7 : 1;
										//shadowed ?
	};

	bool ValidDirectory() const { return Valid && Directory; }
	bool ValidFile() const { return Valid && !Directory; }
	bool ValidSymlink() const { return Valid && SymLink; }
};
static_assert(sizeof(FileFlags) == 1, "File flags may have only 1 byte");

struct BaseFileInfo {
	//in:
	CString m_NamePointer;
	FileID m_ContainerFileID;
	FileID m_ParentIndex;	    ///in local domain, 0 shall be invalid 
	FileID m_SymLinkIndex;	    ///in local domain, 0 shall be invalid 
	FileFlags m_Flags;
	//out:
	FileID m_GlobalIndex;		///filled during registration
};

struct File final {
	FileID m_ContainerFileID;
	ContainerID m_ContainerID;
	StringID m_NameStringID;
	FileFlags m_Flags;
	FileSize m_Size;
	FileID m_GlobalFileID;
	FileID m_ParentFileID;
	FileID m_NextSibling;		//ignored when not a directory
	FileID m_FirstChild;
	FileID m_SymLinkIndex;		//valid if SymLink flag
	FilePathHash m_Hash;
};
static_assert(std::is_pod<File>::value, "File structure must be a POD type");

//-------------------------------------------------------------------------------------------------

template<class T>
struct unique_table {
	
	using item_t = T;

	unique_table(const unique_table&) = delete;
	unique_table& operator=(const unique_table&) = delete;
	unique_table(): m_Table(), m_Size(0) { }

	void New(size_t len) {
		m_Table.reset(new T[len]);
		m_Size = len;
	}

	T* get() { return m_Table.get(); }
	const T* get() const { return m_Table.get(); }

	size_t size() const { return m_Size; }
	size_t byte_size() const { return m_Size * sizeof(T); }

	T& operator[](size_t idx) { return m_Table[idx]; }
	const T& operator[](size_t idx) const { return m_Table[idx]; }

	void swap(unique_table &other) {
		if (this == &other)
			return;
		other.m_Table.swap(m_Table);
		std::swap(other.m_Size, m_Size);
	}

	void reset() {
		m_Size = 0;
		m_Table.reset();
	}

	operator bool() const { return static_cast<bool>(m_Table); }
private:
	std::unique_ptr<T[]> m_Table;
	size_t m_Size = 0;
};

} //namespace StarVFS 
