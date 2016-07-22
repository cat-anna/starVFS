/*
  * Generated by cppsrc.sh
  * On 2015-12-11 21:48:12,05
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef HandleTable_H
#define HandleTable_H

namespace StarVFS {

class HandleTable final {
public:
 	HandleTable(FileTable *FT);
 	~HandleTable();

	FileHandle CreateHandle(FileID fid, RWMode ReadMode);

	FileSize HandleGetSize(const FileHandle& h) const;
	String HandleGetFullPath(const FileHandle& h) const;
	CString HandleGetName(const FileHandle& h) const;
	bool HandleGetFileData(const FileHandle& h, ByteTable &data) const;
	RWMode HandleGetRWMode(const FileHandle& h) const;
	bool HandleIsDirectory(const FileHandle& h) const;
	bool HandleIsSymlink(const FileHandle& h) const;
	bool HandleIsHandleValid(const FileHandle& h) const;
	void HandleClose(const FileHandle& h);
	bool HandleEnumerateChildren(const FileHandle& h, HandleEnumerateFunc &func) const;
	void HandleCloneTo(const FileHandle& src, FileHandle& dst);
	FileID HandleGetFID(const FileHandle& h) const;
	FileID HandleGetParrentID(const FileHandle& h) const;
	FilePathHash HandleGetHash(const FileHandle& h) const;

	void InvalidateCID(ContainerID cid);
private: 
	struct HandleData;
	FileTable *m_FileTable;
	std::vector<HandleData> m_HandleTable;
	uint16_t m_LastAllocatedID;

	HandleData *AllocHandle();
	HandleData* GetDataFromHandle(const FileHandle& h) const;
};

} //namespace StarVFS 

#endif
