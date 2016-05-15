/*
  * Generated by cppsrc.sh
  * On 2015-12-17 22:12:37,21
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/
#include "../StarVFSInternal.h"
#include "../Container/InternalContainerHeaders.h"
#include "InternalExporter.h"
#include <fstream>

namespace StarVFS {
namespace Exporters {

using namespace Containers::InternalContainerHeaders;

static const char InternalContainerCFileHeader[] =
"/*\n"
"  Automatically generated file. Do not edit.\n"
"  Generated by StarVFS InternalContainer exporter\n"
"  at $(GENDATE).\n"
"*/\n";
static const char InternalContainerCFileFooter[] = "\n";

struct InternalExporter::ExporterImpl {
	ExporterImpl(const InternalExporter *Owner): m_Owner(Owner), m_Meta() {
		m_Meta.m_VersionMagic = VersionMagic::Version0;

	}

	static String Replace(const char *input, const char *what, const String& newval) {
		String r(input);
		auto pos = r.find(what);
		if (pos == String::npos)
			return r;
		r.replace(pos, strlen(what), newval);
		return r;
	}

	template<size_t SIZE, class ...ARGS>
	static const char *xsprintf(char (&buf)[SIZE], const char *fmt, ARGS ...args) {
		snprintf(buf, SIZE, fmt, std::forward<ARGS>(args)...);
		return buf;
	}

	struct FileData {
		InternalFile m_InternalFile;
		FileID m_VFSFID;
	};

	FileData& NewFile() {
		m_Files.emplace_back();
		auto &f = m_Files.back();
		memset(&f.m_InternalFile, 0, sizeof(f.m_InternalFile));
		f.m_InternalFile.m_FileID = (FileID)(m_Files.size() - 1);
		f.m_InternalFile.m_Flags = InternalFileFlags::Valid;
		return f;
	}

	bool ScanFID(FileID inputfid, FileData &parent) {
		auto svfs = m_Owner->GetSVFS();

		auto handler = [this, &parent, svfs](FileID fid) {
			auto &f = NewFile();
	//		std::cout << "marked file: " << svfs->GetFullFilePath(fid) << " " << m_Files.size() << " " << &f << "\n";
			f.m_VFSFID = fid;
			f.m_InternalFile.m_ParentID = parent.m_InternalFile.m_FileID;
			f.m_InternalFile.m_NextSiblingID = parent.m_InternalFile.m_FirstChildID;
			auto fptr = svfs->GetFileTable()->GetFile(fid);
			f.m_InternalFile.m_Hash = fptr->m_Hash;
			parent.m_InternalFile.m_FirstChildID = f.m_InternalFile.m_FileID;
			if (svfs->IsFileDirectory(fid)) {
				f.m_InternalFile.m_Flags |= InternalFileFlags::Directory;
				ScanFID(fid, f);
			}

			return true;
		};

		auto h = svfs->OpenFile(inputfid);
		if (!h.EnumerateChildren(handler))
			return false;
		h.Close();
		return true;
	}

	bool ScanFiles(const String &vfsbase) {
		auto svfs = m_Owner->GetSVFS();

		auto h = svfs->OpenFile(vfsbase);
		if (!h) {
			STARVFSErrorLog("%s is not a valid. Cannot export!", vfsbase.c_str());
			return false;
		}
		if (!h.IsDirectory()) {
			STARVFSErrorLog("%s is not a directory. Cannot export!", vfsbase.c_str());
			return false;
		}

		m_Files.clear();

		NewFile().m_InternalFile.m_Flags = 0;
		auto &root = NewFile();
		root.m_InternalFile.m_Flags |= InternalFileFlags::Directory;
		return ScanFID(h.GetFID(), root);
	}
	
	bool Prepare(const String &vfsbase) {
		m_FileTableName = m_Owner->m_Name + "_FileTable";
		m_FileDataTableName = m_Owner->m_Name + "_FileDataTable";
		m_ContainerMetaName = m_Owner->m_Name + "_Container";

		m_FileTableTypeName = "InternalFile";
		m_FileDataTableTypeName = "unsigned char";
		m_ContainerMetaTypeName = "ContainerMeta";

		if (!ScanFiles(vfsbase))
			return false;

		return true;
	}

	void WriteCcode(std::ostream &outfile) {
		char buf[1024];
		auto svfs = m_Owner->GetSVFS();

		outfile << Replace(InternalContainerCFileHeader, "$(GENDATE)", "date");
		outfile << "\n";
		//print/include ic header
		outfile << "#include <cstdint>\n";
		outfile << "#include <StarVFS/core/config.h>\n";
		outfile << "#include <StarVFS/core/Container/InternalContainerHeaders.h>\n";
		outfile << "\n";
//enter namespace
		outfile << "using namespace StarVFS::Containers::InternalContainerHeaders;\n";
		outfile << "\n";
		outfile << "static const " << m_FileTableTypeName << " " << m_FileTableName << "[];\n";
		outfile << "static const " << m_FileDataTableTypeName << " " << m_FileDataTableName << "[];\n";
		outfile << "extern const " << m_ContainerMetaTypeName << " " << m_ContainerMetaName << ";\n";
		outfile << "\n";
//container meta
		outfile << "const " << m_ContainerMetaTypeName << " " << m_ContainerMetaName << " = {\n";
		outfile << xsprintf(buf, "\t(VersionMagic)0x%08x,\n", m_Meta.m_VersionMagic);
		outfile << xsprintf(buf, "\t0x%02x,\n", m_Meta.m_Flags);
		outfile << xsprintf(buf, "\t%d,\n", m_Meta.m_unused);
		outfile << xsprintf(buf, "\t(const ::StarVFS::CString)\"%s\",\n", m_Owner->m_Name.c_str());
		outfile << xsprintf(buf, "\t%d,\n", m_Files.size());
		outfile << xsprintf(buf, "\t%s,\n", m_FileTableName.c_str());
		outfile << xsprintf(buf, "\t%s,\n", m_FileDataTableName.c_str());
		outfile << "};\n";
		outfile << "\n";
//filedatatable
		outfile << "static const " << m_FileDataTableTypeName << " " << m_FileDataTableName << "[] = {\n";
		outfile << "\t0, /* Heading zero */\n";
		FileSize CurrentOffset = 1;
		for (auto &f : m_Files) {
			if (f.m_InternalFile.m_Flags & InternalFileFlags::Directory)
				continue;
			if (!(f.m_InternalFile.m_Flags & InternalFileFlags::Valid))
				continue;
			outfile << xsprintf(buf, "/* %s */\n", svfs->GetFullFilePath(f.m_VFSFID).c_str());
			f.m_InternalFile.m_DataOffset = CurrentOffset;
			ByteTable ct;
			auto h = svfs->OpenFile(f.m_VFSFID);
			if (!h || !h.GetFileData(ct)) {
				outfile << "\t0, /* Error reading file! */\n";
				++CurrentOffset;
				f.m_InternalFile.m_Size = 1;
				continue;
			}
			f.m_InternalFile.m_Size = ct.byte_size();
			outfile << "\t";
			for (FileSize i = 0; i < ct.byte_size(); ++i) {
				outfile << xsprintf(buf, "0x%02x,", ct[i] & 0xFF);
			}
			CurrentOffset += ct.byte_size();
			outfile << "\n";
		}
		outfile << "};\n";		
		outfile << "\n";
//filetable
		outfile << "static const " << m_FileTableTypeName << " " << m_FileTableName << "[] = {\n";
		for (auto &f : m_Files) {
			auto name = svfs->GetFileName(f.m_VFSFID);
			outfile << xsprintf(buf, "\t{ 0x%02x, %d, (const ::StarVFS::CString)\"%s\", %d, 0x%08xu, 0x%08xu, %d, %d, %d, %d, },\n", 
								f.m_InternalFile.m_Flags,			  //uint8_t m_Flags;
								0,									  //uint8_t m_unused;
								name ? name : "",					  //CString m_FileName;
								f.m_InternalFile.m_Size,			  //FileSize m_Size;
								f.m_InternalFile.m_DataOffset,		  //FileSize m_DataOffset;
								f.m_InternalFile.m_Hash,			  //FilePathHash m_Hash;
								f.m_InternalFile.m_FileID,			  //FileID m_FileID;
								f.m_InternalFile.m_ParentID,		  //FileID m_ParentID;
								f.m_InternalFile.m_NextSiblingID,	  //FileID m_NextSiblingID;
								f.m_InternalFile.m_FirstChildID		  //FileID m_FirstChildID;
								);
		}
		outfile << "};\n";
		outfile << "\n";
//leave namespace
		outfile << InternalContainerCFileFooter;
	}
private:
	const InternalExporter *m_Owner;

	std::list<FileData> m_Files;
	ContainerMeta m_Meta;

	String m_FileTableName, m_FileTableTypeName;
	String m_FileDataTableName, m_FileDataTableTypeName;
	String m_ContainerMetaName, m_ContainerMetaTypeName;
};

//-------------------------------------------------------------------------------------------------

InternalExporter::InternalExporter(StarVFS *svfs):
		iExporter(svfs) {
	m_Name = "InternalContainer";
}

InternalExporter::~InternalExporter() {
}

//-------------------------------------------------------------------------------------------------

ExportResult InternalExporter::WriteLocalFile(const String &LocalFileName) {
//	ExporterImpl exporter(this);

//	if (!exporter.Prepare(VFSBase)) {
//		STARVFSErrorLog("Failed to prepare InternalContainer exporter!");
//		return ExportResult::FatalError;
//	}

//	std::ofstream outfile(LocalFileName, std::ios::out);
//	exporter.WriteCcode(outfile);
//	outfile.close();
	return ExportResult::Sucess;
}

} //namespace Exporters 
} //namespace StarVFS 

