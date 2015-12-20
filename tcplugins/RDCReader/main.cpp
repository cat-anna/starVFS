#include <pch.h>
#include <nfMoonGlare.h>
 
#include "Engine/FileSystem/nFileSystem.h"
#include "Engine/FileSystem/MoonGlareContainer/nMoonGlareContainer.h"
#include "Engine/FileSystem/MoonGlareContainer/ReaderFile.h"
#include "rdctcplugVersion.inc"

#include <Windows.h>
#include "wcxhead.h"

namespace MoonGlare {
namespace rdctcplug {

using namespace MoonGlare::FileSystem::MoonGlareContainer;
using namespace MoonGlare::FileSystem::MoonGlareContainer::Headers;
using namespace MoonGlare::FileSystem::MoonGlareContainer::Headers::CurrentVersion;

struct MetaFileBlock {
	UniqueCharTable Data;
	u32 DataLen;
};

struct RDCTCContainer : public Reader {
	GABI_DECLARE_STATIC_CLASS(RDCTCContainer, Reader)
public:
	RDCTCContainer(const string &File) :
			BaseClass() {

		m_FileName = File;
		if (OpenContainer(m_Meta)) {
			SetReadable(true);
			SetReady(true);
		} else {
			AddLog(Error, "Unable to open container '" << File << "'");
		}

		GetSectionMetaFile(m_Meta);
		GenFileTableMetaFile(m_Meta);
		GenContainerMetaFile(m_Meta);

		if (IsReady()) {
			std::stack<ReaderFile*> stack;
			stack.push(m_RootFile);
			while (!stack.empty()) {
				auto *file = stack.top();
				stack.pop();
				if (file != m_RootFile)
					m_FullFileList.push_back(file);
				for (auto *it: file->Children)
					stack.push(it);
			}
		}

	}

	~RDCTCContainer() { }

	using BaseClass::FindFile;
	using ReaderFile = BaseClass::ReaderFile;

	const MetaFileBlock* GetMetaFileBlock(ReaderFile *file) const {
		auto it = m_MetaFiles.find(file);
		if (it == m_MetaFiles.end()) return nullptr;
		return it->second.get();
	}

	using FileList = std::list < ReaderFile* > ;

	FileList::const_iterator begin() const { return m_FullFileList.begin(); }
	FileList::const_iterator end() const { return m_FullFileList.end(); }
protected:
	std::map<ReaderFile*, std::unique_ptr<MetaFileBlock>> m_MetaFiles;
	FileList m_FullFileList;
	ReaderLoadMeta m_Meta = nullptr;
	

	void GenContainerMetaFile(ReaderLoadMeta &meta) {
		auto *file = AllocFile();
		m_RootFile->Children.push_back(file);
		file->Name = ".Container";
		file->Flags.MetaFile = 1;
		auto block = std::make_unique<MetaFileBlock>();
		
		std::stringstream ss;
		ss << "Container metafile\n\n";
		char buf[4096], buf128[128];
		char buf1[5] = {}; memcpy(buf1, &meta->header.FileSignature, 4);
		char buf2[5] = {}; memcpy(buf2, &meta->header.UserSignature, 4);
		char buf3[5] = {}; memcpy(buf3, &meta->Footer.Signature, 4);
		sprintf(buf128, " (%d in container)", meta->Footer.SectionsBlock.ContainerSize);

		sprintf(buf, 
				"Version: %d\n"
				"Flags: 0x%04x\n"
				"\n"
				"Section count: %d\n"
				"Section table location: 0x%x\n"
				"Section table byte size: %d%s\n"
				"\n"
				"Signatures:\n"
				"\tHeader: %s\n"
				"\tUser: %s\n"
				"\tFooter: %s\n",
				meta->header.Version,
				meta->header.Flags,
				meta->Footer.SectionCount,
				meta->Footer.SectionsBlock.FilePointer,
				meta->Footer.SectionsBlock.RealSize,
				((meta->Footer.SectionsBlock.RealSize == meta->Footer.SectionsBlock.ContainerSize) ? "" : buf128 ),
				buf1, buf2, buf3);

		ss << buf;

		string data = ss.str();
		auto len = data.length();
		char *cdata = new char[len + 1];
		memcpy(cdata, data.c_str(), len + 1);
		block->Data.reset(cdata);
		block->DataLen = (u32)len;
		file->DataBlock.RealSize = (u32)len;
		m_MetaFiles[file].swap(block);
	}

	void GenFileTableMetaFile(ReaderLoadMeta &meta) {
		unsigned idx = 0;
		for (SectionIndex i = 0; i < meta->Footer.SectionCount; ++i) {
			char buf[4096], buf128[128];
			auto block = std::make_unique<MetaFileBlock>();

			switch (meta->Sections[i].Type) {
				case SectionType::FileTable:
					break;
				default:
					continue;
			}
			auto *file = AllocFile();
			m_RootFile->Children.push_back(file);
			sprintf(buf, ".FileTable.%d", idx);
			file->Name = buf;
			file->Flags.MetaFile = 1;

			std::stringstream ss;
			ss << "File table " << idx << " metafile\n\n";

			const auto *ft = meta->FileTable;
			sprintf(buf, 
					"Files count: %d\n"
					"Raw data section: %d\n"
					"String table section: %d\n\n", ft->Count, ft->RawDataSection, ft->StringTableSection);
			ss << buf;

			for (u32 fileidx = 0; fileidx < ft->Count; ++fileidx) {
				auto &file = ft->Table[fileidx];
				sprintf(buf128, " (%d in container)", file.FileBlock.ContainerSize);
				const char *name = meta->StringTable->Table + file.NamePointer;
				sprintf(buf,
						"File index: %d\n"
						"Size %d bytes%s\n"
						"Name offset: 0x%x (%s)\n"
						"Raw section offset: 0x%x\n"
						"Parent index: %d\n"
						"Flags: 0x%04x",
						fileidx, 
						file.FileBlock.RealSize, 
						((file.FileBlock.RealSize == file.FileBlock.ContainerSize) ? "" : buf128 ),
						file.NamePointer,
						name,
						file.FileBlock.FilePointer,
						file.ParentIndex, file.Flags);
				ss << buf; 
				if (file.Flags) { 
					ss << " -> ";
					if (file.Flags & FileTableEntryFlags::Folder) ss << "Directory ";
					if (file.Flags & FileTableEntryFlags::SymLink) ss << "SymLink ";
					if (file.Flags & FileTableEntryFlags::HasSymLink) ss << "HasSymLink ";
					if (file.Flags & FileTableEntryFlags::InvalidEntry) ss << "Invalid ";
				}
				ss << "\n\n";
			}

			++idx;
			string data = ss.str();
			auto len = data.length();
			char *cdata = new char[len + 1];
			memcpy(cdata, data.c_str(), len + 1);
			block->Data.reset(cdata);
			block->DataLen = (u32)len;
			file->DataBlock.RealSize = (u32)len;
			m_MetaFiles[file].swap(block);
		}
	}

	void GetSectionMetaFile(ReaderLoadMeta &meta) {
		auto *file = AllocFile();
		m_RootFile->Children.push_back(file);
		file->Name = ".SectionTable";
		file->Flags.MetaFile = 1;
		auto block = std::make_unique<MetaFileBlock>();
		
		std::stringstream ss;
		ss << "Section table metafile\n\n";

		for (SectionIndex i = 0; i < meta->Footer.SectionCount; ++i) {
			char buf[4096], buf128[128];
			auto &section = meta->Sections[i];
			const char *stype = "Unknown";
			switch (meta->Sections[i].Type) {
			case SectionType::EmptyEntry: stype = "Empty section"; break;
			case SectionType::FileTable: stype = "File table"; break;
			case SectionType::RawData: stype = "Raw data"; break;
			case SectionType::StringTable: stype = "String table"; break;
			} 
			sprintf(buf128, " (%d in container)", section.SectionBlock.ContainerSize);
			sprintf(buf, 
					"Section %d\n"
					"Type: %s\n"
					"Location: 0x%x\n"
					"Size %d bytes%s\n", 
					i, stype,
					section.SectionBlock.FilePointer, section.SectionBlock.RealSize, 
					((section.SectionBlock.RealSize == section.SectionBlock.ContainerSize) ? "" : buf128 ) );
			ss << buf << "\n";
		}

		string data = ss.str();
		auto len = data.length();
		char *cdata = new char[len + 1];
		memcpy(cdata, data.c_str(), len + 1);
		block->Data.reset(cdata);
		block->DataLen = (u32)len;
		file->DataBlock.RealSize = (u32)len;
		m_MetaFiles[file].swap(block);
	}
};

GABI_IMPLEMENT_STATIC_CLASS(RDCTCContainer)

} // namespace rdctcplug
} // namespace MoonGlare

//--------------------------------------------------------------------------------------

using namespace MoonGlare::rdctcplug;

#ifdef X32
#	pragma comment(linker, "/EXPORT:OpenArchive=_OpenArchive@4")
#	pragma comment(linker, "/EXPORT:CloseArchive=_CloseArchive@4")
#	pragma comment(linker, "/EXPORT:ReadHeader=_ReadHeader@8")
#	pragma comment(linker, "/EXPORT:ProcessFile=_ProcessFile@16")
#	pragma comment(linker, "/EXPORT:SetChangeVolProc=_SetChangeVolProc@8")
#	pragma comment(linker, "/EXPORT:SetProcessDataProc=_SetProcessDataProc@8")
#else

#endif


struct rdc_handle {
	enum class MagicEnum : unsigned __int32 {
		Header = 0xC0FFE00,
	};
	MagicEnum MagicHeader = MagicEnum::Header;

	tProcessDataProc ProcessProc;
	tChangeVolProc ChangeVolProc;
	std::shared_ptr<RDCTCContainer> container;
	RDCTCContainer::FileList::const_iterator TraverseIterator;
	RDCTCContainer::ReaderFile *CurrentFile = nullptr;

	void CallProcessProc(char *FileName, int Size) {
		if (ProcessProc)
			ProcessProc(FileName, Size);
	}

	static rdc_handle* cast(HANDLE hh) {
		auto *h = static_cast<rdc_handle*>(hh);
		if (!h) return nullptr;
		if (h->MagicHeader != MagicEnum::Header) return nullptr;
		return h;
	}
};

#define TC_API extern "C" __declspec(dllexport)  

TC_API HANDLE __stdcall OpenArchive(tOpenArchiveData *ArchiveData) {
	rdc_handle *h = new rdc_handle();

	h->container = std::make_shared<RDCTCContainer>(ArchiveData->ArcName);

	if (!h->container->IsReady() || !h->container->IsReadable()) {
		delete h;
		h = nullptr;
		ArchiveData->OpenResult = E_UNKNOWN_FORMAT;
	}
	ArchiveData->OpenResult = 0;
	h->TraverseIterator = h->container->begin();
	return (HANDLE)h;
}

TC_API int __stdcall ReadHeader(HANDLE hArcData, tHeaderData *HeaderData) {
	auto *h = rdc_handle::cast(hArcData);
	if (!h) return E_BAD_ARCHIVE;

	auto end = h->container->end();
	if (h->TraverseIterator == end) {
		h->TraverseIterator = h->container->begin();
		h->CurrentFile = nullptr;
		return E_END_ARCHIVE;
	}

	auto *file = *h->TraverseIterator;
	h->CurrentFile = file;
	++h->TraverseIterator;

	string name = file->Path();
	char *c = (char*)name.c_str();
	while (*c) {
		if (!c[1] && c[0] == '/') *c = 0;
		if (*c == '/') *c = '\\';
		++c;
	}

	HeaderData->ArcName[0] = 0;
	strcpy_s(HeaderData->FileName, name.c_str());
	HeaderData->PackSize = file->DataBlock.ContainerSize;
	HeaderData->UnpSize = file->DataBlock.RealSize;
	HeaderData->FileCRC = 0;
	HeaderData->HostOS = 0;
	HeaderData->FileTime = 0;
	HeaderData->FileAttr = 0;

	if (file->Flags.Directory)
		HeaderData->FileAttr |= 0x10;
	else
		HeaderData->FileAttr |= 0x01;
	if (file->Flags.MetaFile)
		HeaderData->FileAttr |= 0x04;
	return 0;
}

TC_API int __stdcall ProcessFile(HANDLE hArcData, int Operation, char *DestPath, char *DestName) {
	auto *h = rdc_handle::cast(hArcData);
	if (!h) return E_BAD_ARCHIVE;

	switch (Operation) {
	case PK_SKIP: 
		return 0;

	case PK_EXTRACT: {
		auto *f = h->CurrentFile;
		if (!f) return E_EREAD;
		if (f->Flags.MetaFile) {
			auto *data = h->container->GetMetaFileBlock(f);
			if (!f)
				return E_EREAD;
			std::ofstream out(DestName, std::ios::out | std::ios::binary);
			out.write(data->Data.get(), data->DataLen);
			out.close();
			return 0;
		}

		auto reader = h->container->GetFileReader(f);
		if (!reader)
			return E_EREAD;
		std::ofstream out(DestName, std::ios::out | std::ios::binary);
		out.write(reader->GetFileData(), reader->Size());
		out.close();

		return 0;
	}

	case PK_TEST: 
	default:
		return E_NOT_SUPPORTED;
	}
}

TC_API int __stdcall CloseArchive(HANDLE hArcData) {
	auto *h = rdc_handle::cast(hArcData);
	if (!h) return 1;
	delete h;
	return 0;
}

TC_API void __stdcall SetChangeVolProc(HANDLE hArcData, tChangeVolProc pChangeVolProc1) {
	auto *h = rdc_handle::cast(hArcData);
	if (!h) return;
	h->ChangeVolProc = pChangeVolProc1;
}

TC_API void __stdcall SetProcessDataProc(HANDLE hArcData, tProcessDataProc pProcessDataProc) {
	auto *h = rdc_handle::cast(hArcData);
	if (!h) return;
	h->ProcessProc = pProcessDataProc;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved ) {
    switch ( fdwReason ) { 
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
