/*
  * Generated by cppsrc.sh
  * On 2015-12-10 19:39:52,96
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/
#include "iContainer.h"
#include "FolderContainer.h"
#include <boost/filesystem.hpp>

#ifndef STARVFS_DISABLE_FOLDERCONTAINER
namespace StarVFS {
namespace Containers {

FolderContainer::FolderContainer(FileTableInterface *fti, String Path) :
		iContainer(fti),
		m_Path(std::move(Path)) {

	if (m_Path.back() != '/')
		m_Path += '/';
}

FolderContainer::~FolderContainer() {
}

//-------------------------------------------------------------------------------------------------

const String& FolderContainer::GetFileName() const { return m_Path; }
RWMode FolderContainer::GetRWMode() const { return RWMode::RW; }

//-------------------------------------------------------------------------------------------------

bool FolderContainer::ScanPath() {
	m_FileEntry.clear();

	std::function<void(const String&, FileType)> handler;

	handler = [this, &handler](const String& path, FileType type) {
		auto subpath = path.substr(m_Path.length() - 1, path.length() - m_Path.length() + 1);

		for (auto &it : subpath)
			if (it == '\\')
				it = '/';

		uint64_t fsize = 0;
		if (type == FileType::File)
			try {
				fsize = (FileSize)boost::filesystem::file_size(path);
			} 
			catch (...) {
			}
		m_FileEntry.emplace_back(Entry{ type, path, subpath, fsize, });
	};

	if (!EnumerateFolder(m_Path, handler)) {
		STARVFSErrorLog("Failed to enumerate folder %s", m_Path.c_str());
		return false;
	}

	m_FileEntry.shrink_to_fit();
	return true;
}

bool FolderContainer::ReloadContainer() {
	return ScanPath();
}

bool FolderContainer::RegisterContent() const {
	auto fti = GetFileTableInterface();
	StarVFSAssert(fti);

	auto fcount = GetFileCount();

	if (!fti->EnsureReserve(fcount)) {
		STARVFSErrorLog("Failed to reload folder container");
		return false;
	}

	for (FileID cfid = 0, j = fcount; cfid < j; ++cfid) {
		auto &f = m_FileEntry[cfid];

		FileID fid = fti->AllocFileID((CString)f.m_SubPath.c_str());
		if (!fid){
			STARVFSErrorLog("Failed to alloc fileid for %s", f.m_SubPath.c_str());
			continue;
		}

		bool success;
		switch (f.m_Type) {
		case FileType::Directory:
			success = fti->CreateDirectory(fid, cfid);
			break;
		case FileType::File:
			success = fti->CreateFile(fid, cfid, static_cast<FileSize>(f.m_FileSize));
			break;
		default:
			STARVFSErrorLog("Invalid FileType value!");
			StarVFSAssert(false);
			continue;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

FileID FolderContainer::GetFileCount() const {
	return static_cast<FileID>(m_FileEntry.size());
}

bool FolderContainer::GetFileData(FileID ContainerFID, ByteTable &out) const {
	out.reset();
	if (ContainerFID >= m_FileEntry.size())
		return false;

	auto &f = m_FileEntry[ContainerFID];

	std::ifstream inp(f.m_FullPath.c_str(), std::ios::in | std::ios::binary);
	if (!inp) 
		return false;

	auto size = (size_t)boost::filesystem::file_size(f.m_FullPath);

	out.make_new(size);
	inp.read(out.get(), size);
	inp.close();
	return true;
}

//-------------------------------------------------------------------------------------------------

template <class T>
bool FolderContainer::EnumerateFolder(const String &Path, T func) {
	using boost::filesystem::directory_iterator;
	using boost::filesystem::recursive_directory_iterator;
	boost::filesystem::path p(Path);
	if (!boost::filesystem::is_directory(Path))
		return false;
	
	try {
		recursive_directory_iterator it(Path);

		for (; it != recursive_directory_iterator();) {
			auto item = it->path();
			FileType type;

			String fullPath = item.string();

			if (boost::filesystem::is_regular_file(item))
				type = FileType::File;
			else
				type = FileType::Directory;

			func(fullPath, type);

			while(it != recursive_directory_iterator())
				try {
					++it;
					break;
				} 
				catch (...) {
					it.no_push();
				}
			}
	}
	catch (const std::exception &e) {
		STARVFSErrorLog("Exception: %s", e.what());
		return true;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

bool FolderContainer::CanOpen(const String& Location) {
	return boost::filesystem::is_directory(Location);
}

CreateContainerResult FolderContainer::CreateFor(StarVFS *svfs, const String& MountPoint, const String& Location) {
	StarVFSAssert(svfs);
	return svfs->CreateContainer<FolderContainer>(MountPoint, Location);
}

#if 0

FileWritter FolderContainer::GetFileWritter(const string& file)
{
	string s = FullPath(file);
	boost::filesystem::path p(s);
	boost::filesystem::create_directories(p.parent_path());
	auto ptr = std::make_shared<FolderFileWritter>(const_cast<FolderContainer*>(this), s);
	return ptr;
}

FileReader FolderContainer::GetFileReader(const RawFilePointer *file) const
{
	const FolderContainerPointer *ptr = dynamic_cast<const FolderContainerPointer*>(file);
	if (!ptr) {
		AddLog(Error, "Invalid file pointer!");
		return FileReader();
	}
	return GetFileReader(ptr->SubPath);
}

FileWritter FolderContainer::GetFileWritter(const RawFilePointer *file)
{
	const FolderContainerPointer *ptr = dynamic_cast<const FolderContainerPointer*>(file);
	if (!ptr) {
		AddLog(Error, "Invalid file pointer!");
		return FileWritter();
	}
	return GetFileWritter(ptr->SubPath);
}

//-------------------------------------------------------------------------------------------------

bool FolderContainer::FileExists(const string& file) const
{
	string s = FullPath(file);
	return boost::filesystem::exists(s);
}

bool FolderContainer::EnumerateFolder(const RawFilePointer *root, FolderEnumerateFunc func) const
{
	if (!func) return false;
	const FolderContainerPointer *ptr = dynamic_cast<const FolderContainerPointer*>(root);
	if (!ptr && root) {
		AddLog(Error, "Invalid file pointer!");
		return false;
	}

	string s;
	if (ptr)
		s = FullPath(ptr->SubPath);
	else
		s = m_Path;

	using boost::filesystem::directory_iterator;
	boost::filesystem::path p(s);
	if (!boost::filesystem::exists(p))
		return false;
	directory_iterator it(p);
	for (; it != directory_iterator(); ++it) {
		auto item = it->path();
		FileType type;
		if (boost::filesystem::is_regular_file(item))
			type = FileType::File;
		else
			type = FileType::Directory;

		string fn = item.filename().string();
		string hash = item.string();
		std::transform(hash.begin(), hash.end(), hash.begin(), [](int c)->int { if (c == '\\') return '/'; return c; });

		FolderContainerPointer *fptr;
		FolderContainerPointer **fptrptr = &m_PtrMap[hash];
		if (!*fptrptr) {
			m_PtrList.emplace_back(std::make_unique<FolderContainerPointer>());
			fptr = m_PtrList.back().get();
			*fptrptr = fptr;
			fptr->SubPath = string(hash.c_str() + m_Path.length());
			if (type == FileType::File)
				fptr->Size = (FileSize)boost::filesystem::file_size(hash);
		} else
			fptr = *fptrptr;

		func(fn, type, fptr);
	}
	return true;
}

const string& FolderContainer::GetFileName() const
{
	return m_Path;
}

#endif

} //namespace Containers 
} //namespace StarVFS 

#endif
