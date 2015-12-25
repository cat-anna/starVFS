/*
  * Generated by cppsrc.sh
  * On 2015-12-10 18:00:09,91
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef FileTable_H
#define FileTable_H

namespace StarVFS {

union FileFlags {
	uint8_t intval;
	struct {
		uint8_t Valid : 1;				//Entry is valid
		uint8_t Directory : 1;
		uint8_t SymLink : 1;
		//uint8_t Shadowed : 1;
		//uint8_t Used : 1;
		//uint8_t unused4 : 1;
		//uint8_t unused5 : 1;
		//uint8_t unused6 : 1;
		//uint8_t unused7 : 1;
		//deleted ?
		//used ?
		//shadowed ?
	};
};

static_assert(sizeof(FileFlags) == 1, "File flags may have only 1 byte");

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

class FileTable final {
public:
 	FileTable(unsigned Flags = 0);
 	~FileTable();

	void DumpStructure(std::ostream &out) const;
	void DumpFileTable(std::ostream &out) const;
	void DumpHashTable(std::ostream &out) const;

	File* AllocFile(const String& InternalFullPath);
	File* AllocFile(FileID Parent, FilePathHash PathHash, const CString FileName);
	
	template<class ...ARGS> FileID Lookup(ARGS... args) { return m_HashFileTable.Lookup(std::forward<ARGS>(args)...); }

	bool GetFileData(FileID fid, CharTable &data, FileSize *fsize = nullptr);
	
	bool IsValid(FileID fid) const { return fid && fid < m_Allocated && m_FileTable[fid].m_Flags.Valid; }
	File* GetFile(FileID fid) const {
		if (!IsValid(fid)) return nullptr;
		return &m_FileTable[fid];
	}
	File* GetRawFile(FileID fid) const {
		if (!fid || fid >= m_Allocated) return nullptr;
		return &m_FileTable[fid];
	}
	bool IsFile(FileID fid) const {
		auto f = GetFile(fid);
		return f && !f->m_Flags.Directory;
	}
	bool IsDirectory(FileID fid) const {
		auto f = GetFile(fid);
		return f && f->m_Flags.Directory;
	}

	File* GetFileParent(const File *f) const { return GetFile(f->m_ParentFileID); }
	File* GetFileFirstChild(const File *f) const { return GetFile(f->m_FirstChild); }
	File* GetFileNextSibling(const File *f) const { return GetFile(f->m_NextSibling); }

	const CString GetFileName(FileID fid) const;
	String GetFileFullPath(FileID fid) const;

	const StringTable* GetStringTable() const { return m_StringTable.get(); }
	const File* GetTable() const { return m_FileTable.get(); }
	FileID GetAllocatedFileCount() { return m_Allocated; }

	Containers::FileTableInterface *AllocateInterface(const String& MountPoint);
	bool EnsureCapacity(FileID RequiredEmptySpace);
private:
	std::unique_ptr<StringTable> m_StringTable;
	std::vector<UniqueFileTableInterface> m_Interfaces;
	HashFileTable m_HashFileTable;
	std::unique_ptr<File[]> m_FileTable;

	FileID m_Capacity, m_Allocated;

//Internal functions, mutex shall be locked before calling them
	bool Realloc(FileID NewCapacity);

	File* AllocNewFile();
	File* AllocNewFile(File *Parent, FilePathHash PathHash, const CString FName);
	File* AllocNewFile(const CString fullpath);

	File* GetRoot() const { return m_FileTable.get() + 1; }
};

} //namespace StarVFS 

#endif
