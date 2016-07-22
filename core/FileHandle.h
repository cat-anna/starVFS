/*
  * Generated by cppsrc.sh
  * On 2015-12-11 21:28:28,57
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef FileHandle_H
#define FileHandle_H

namespace StarVFS {

class FileHandle final {
public:
	FileHandle() : m_Owner(nullptr), m_HandleID(0), m_Generation(0) {}
	FileHandle(HandleTable *Owner, uint16_t hid, uint16_t Generation):
		m_Owner(Owner), m_HandleID(hid), m_Generation(Generation) { }

	FileHandle(const FileHandle &h) : m_Owner(nullptr), m_HandleID(0), m_Generation(0) { if (this != &h) h.CloneTo(*this); }
	FileHandle(FileHandle &&o): 
			m_Owner(std::move(o.m_Owner)), m_HandleID(std::move(o.m_HandleID)), m_Generation(std::move(o.m_Generation)) { 
		o.m_Owner = nullptr; 
	}

	~FileHandle() { 
		Close(); 
	}

	FileHandle& operator=(const FileHandle &h) {
		if(this != &h)
			h.CloneTo(*this);
		return *this;
	}
	FileHandle& operator=(FileHandle&& o) {
		m_Owner = std::move(o.m_Owner);
		o.m_Owner = nullptr;
		m_HandleID = std::move(o.m_HandleID);
		m_Generation = std::move(o.m_Generation);
		return *this;
	}

	void swap(FileHandle &o) {
		std::swap(m_Owner, o.m_Owner);
		std::swap(m_HandleID, o.m_HandleID);
		std::swap(m_Generation, o.m_Generation);
	}

	void CloneTo(FileHandle &h) const;
	FileHandle Clone() const {
		FileHandle h;
		CloneTo(h);
		return h;
	}

	FileSize GetSize() const;
	String GetFullPath() const;
	CString GetName() const;

	FilePathHash GetHash() const;

	bool GetFileData(ByteTable &data) const;

	RWMode GetRWMode() const;
	FileID GetFID() const;
	FileID GetParrentID() const;

	bool IsDirectory() const;
	bool IsSymlink() const;

	bool IsHandleValid() const;
	operator bool() const { return IsHandleValid(); }
	
	void Close();

	bool EnumerateChildren(HandleEnumerateFunc func) const;

	uint16_t GetHandleID() const { return m_HandleID; }
	uint16_t GetGeneration() const { return m_Generation; }
private: 
	HandleTable *m_Owner;
	uint16_t m_HandleID;
	uint16_t m_Generation;
};

} //namespace StarVFS 

#endif
