/*
  * Generated by cppsrc.sh
  * On 2015-03-15  7:53:40.63
  * by Immethis
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef ContainerFile_H
#define ContainerFile_H

namespace MoonGlare {
namespace FileSystem {
namespace MoonGlareContainer {

struct CFile {
	CFile();
	~CFile();
	bool open(const char* name);
	void close();

	bool write(const void *buf, size_t len, size_t count = 1) {
		m_file->write((char*)buf, len * count);
		return true;
	}

	void seek_read_back(size_t offset);
	void seek_read(size_t offset) { m_file->seekg(offset, std::ios::beg); }
	bool read(const void *buf, size_t len, size_t count = 1) {
		m_file->read((char*)buf, len * count);
		return true;
	}

	bool WriteBlock(UniqueByteTable &data, Headers::u32 dataSize, CurrentVersion::DataBlock &block);
	bool WriteBlock(const void *data, Headers::u32 blockSize, CurrentVersion::DataBlock &block);

	template <class T>
	bool WriteStructBlock(T* data, CurrentVersion::DataBlock &block, Headers::u32 Count = 1) {
		return WriteBlock(data, sizeof(T) * Count, block);
	}

	bool ReadBlock(UniqueByteTable &data, const CurrentVersion::DataBlock &block);

	Headers::CurrentVersion::FilePointer WriteLocation() const { return static_cast<Headers::CurrentVersion::FilePointer>(m_file->tellp()); }

	operator bool() const { return m_file && static_cast<bool>(*m_file) && m_file->good(); }
protected:
	std::unique_ptr<std::fstream> m_file;
};
#endif

} //namespace MoonGlareContainer 
} //namespace FileSystem 
} //namespace MoonGlare 
