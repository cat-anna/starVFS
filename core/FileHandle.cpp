/*
  * Generated by cppsrc.sh
  * On 2015-12-11 21:28:28,57
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/
#include "StarVFSInternal.h"

namespace StarVFS {

//-------------------------------------------------------------------------------------------------

FileSize FileHandle::GetSize() const {
	if (!m_Owner)
		return 0;
	return m_Owner->HandleGetSize(*this);
}

String FileHandle::GetFullPath() const {
	if (!m_Owner)
		return String();
	return m_Owner->HandleGetFullPath(*this);
}

//-------------------------------------------------------------------------------------------------

bool FileHandle::GetFileData(ByteTable &data) const {
	if (!m_Owner)
		return false;
	return m_Owner->HandleGetFileData(*this, data);
}

//-------------------------------------------------------------------------------------------------

FileID FileHandle::GetFID() const {
	if (!m_Owner)
		return FileID(0);
	return m_Owner->HandleGetFID(*this);
}

RWMode FileHandle::GetRWMode() const {
	if (!m_Owner)
		return RWMode::None;
	return m_Owner->HandleGetRWMode(*this);
}

bool FileHandle::IsDirectory() const {
	if (!m_Owner)
		return false;
	return m_Owner->HandleIsDirectory(*this);
}

bool FileHandle::IsSymlink() const {
	if (!m_Owner)
		return false;
	return m_Owner->HandleIsSymlink(*this);
}

//-------------------------------------------------------------------------------------------------

bool FileHandle::IsHandleValid() const {
	if (!m_Owner)
		return false;
	return m_Owner->HandleIsHandleValid(*this);
}

//-------------------------------------------------------------------------------------------------

bool FileHandle::EnumerateChildren(HandleEnumerateFunc func) const {
	if (!m_Owner)
		return false;
	return m_Owner->HandleEnumerateChildren(*this, func);
}

//-------------------------------------------------------------------------------------------------

void FileHandle::Close() {
	if (!m_Owner)
		return;
	m_Owner->HandleClose(*this);
	m_Owner = nullptr;
	m_Generation = 0;
	m_HandleID = 0;
}

void FileHandle::CloneTo(FileHandle &h) const {
	if (!m_Owner)
		return;
	return m_Owner->HandleCloneTo(*this, h);
}

} //namespace StarVFS 
