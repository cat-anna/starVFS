#pragma once

namespace StarVFS {
namespace RDC {
namespace Version_1 {

namespace Headers {}
using namespace Headers;

namespace Headers {

using SectionIndex = u8;
using SectionPointer = u32;
using FilePointer = u32;
using FileIndex = u16;
using StringPointer = u32;
using Size = u32;

struct CompressionInfo {
	CompressionMode Mode = CompressionMode::None;
	u8 unused_8_0 = 'c';//UNUSED8;
	u8 unused_8_1 = 'm';//UNUSED8;
	u8 unused_8_2 = 'r';//UNUSED8;
	u32 RawSize = 0;
};
static_assert(sizeof(CompressionInfo) == 8, "DataBlock has invalid size!");

struct EncryptionInfo {
	EncryptionMode Mode = EncryptionMode::None;
	u8 unused_8_0 = 'e';//UNUSED8;
	u8 unused_8_1 = 'n';//UNUSED8;
	u8 unused_8_2 = 'c';//UNUSED8;
};
static_assert(sizeof(EncryptionInfo) == 4, "DataBlock has invalid size!");

struct CheckSumInfo {
	CheckSumMode Mode = CheckSumMode::None;
	u8 unused_8_0 = 'c';//UNUSED8;
	u8 unused_8_1 = 'h';//UNUSED8;
	u8 unused_8_2 = 'k';//UNUSED8;
};
static_assert(sizeof(CheckSumInfo) == 4, "DataBlock has invalid size!");

struct DataBlock {
	Size ContainerSize = 0;
	FilePointer FilePointer = 0;
	CompressionInfo Compression;
	EncryptionInfo Encryption;
	CheckSumInfo CheckSum;

	void Zero() { memset(this, 0, sizeof(*this)); }

	Size GetRawSize()const { return Compression.Mode == CompressionMode::None ? ContainerSize : Compression.RawSize; }
};
static_assert(sizeof(DataBlock) % 8 == 0, "DataBlock has invalid size!");

struct OffsetDataBlock {
	Size ContainerSize = 0;
	FilePointer FileOffset = 0;
	CompressionInfo Compression;
	EncryptionInfo Encryption;
	CheckSumInfo CheckSum;

	void Zero() { memset(this, 0, sizeof(*this)); }

	Size GetRawSize()const { return Compression.Mode == CompressionMode::None ? ContainerSize : Compression.RawSize; }
};
static_assert(sizeof(OffsetDataBlock) % 8 == 0, "OffsetDataBlock has invalid size!");

struct FileFooter {
	DataBlock SectionTableBlock;
	SectionIndex SectionCount = 0; ///Count of sections
	u8 unused_8_0 = UNUSED8;
	u8 unused_8_1 = UNUSED8;
	u8 unused_8_2 = UNUSED8;
	Signature Signature = Signature::Footer; ///Footer signature

	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert(sizeof(FileFooter) % 8 == 0, "FileFooter has invalid size!");

struct SectionDescriptor {
	/** descriptor of occupied data in file */
	DataBlock SectionBlock;
	/** Section type */
	SectionType Type = SectionType::EmptyEntry;
	u8 unused_8_0 = UNUSED8;
	u8 unused_8_1 = UNUSED8;
	u8 unused_8_2 = UNUSED8;
	u32 unused_32_0 = UNUSED32;

	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert((sizeof(SectionDescriptor) % 8) == 0, "SectionDescriptor has invalid size!");

struct MountEntrySection {
	SectionIndex StringTable = 0;
	SectionIndex RawDataSection = 0;
	SectionIndex DataBlockTable = 0;
	SectionIndex StructureSection = 0;
	SectionIndex HashTable = 0;
	u8 unused_8_0 = UNUSED8;
	u16 unused_16_0 = UNUSED16;
	StringPointer MountEntryName = 0;
	u16 MountEntryId = 0;
	u16 unused_16_1 = UNUSED16;
	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert(sizeof(MountEntrySection) % 8 == 0, "MountEntrySection has invalid size!");

struct BaseFileInfo {
	StringPointer NamePointer = 0;
	FileIndex ParentIndex = 0;
	FileIndex SymLinkIndex = 0;
	u8 Flags;
	u8 unused_8_0 = UNUSED8;
	u16 unused_16_0 = UNUSED16;
	u32 unused_32_0 = UNUSED32;
	void Zero() { memset(this, 0, sizeof(*this)); }

	struct FlagBits {
		enum {
			Directory = 0x01,
			SymLink = 0x02,
		};
	};
};
static_assert(sizeof(BaseFileInfo) % 8 == 0, "BaseFileInfo has invalid size!");

using HashSectionItemType = FilePathHash;

#if 0

struct FileTableEntry {
	/** File data block */
	DataBlock FileBlock;
//	/** File name offset in string table */
//	/** parent file index */
//	FileIndex SymLinkIndex;
//	/** FileFlags */
//	u16 Flags;
//
//	u16 unused_16_0 = 0;
	u64 unused_64_0 = 0;
};
static_assert(sizeof(FileTableEntry) % 8 == 0, "FileTableEntry has invalid size!");

struct FileTableSection {
	/** Index of data section */
	SectionIndex RawDataSection;
	/** Index of String table section */
	SectionIndex StringTableSection;
	/** unused */
	u16 unused_16_0;
	/** Elements in table */
	u32 Count;
	/** The Table */
	FileTableEntry Table[0];
};
static_assert(sizeof(FileTableSection) % 8 == 0, "FileTableEntry has invalid size!");
#endif

#if 0

struct Version {
	enum {
		Value = 1,
	};
};

//------------ file table section ------------

struct FileTableEntryFlags {
	enum {
		Folder			= 0x0001,	/** Entry describes directory. No data, and ChildrenCount contain valid value */
		SymLink			= 0x0100,	/** File data is shared between multiple files */
		HasSymLink		= 0x0200,	/** file has symlinks pointing to it */
		InvalidEntry	= 0x1000,
	};
};

//------------ raw data section ------------

struct RawDataSection {
	/** The Table */
	u8 Table[0];
};

//------------ string table section ------------

struct StringTableSection {
	/** The Table */
	char Table[0];
};

#endif

} //namespace Headers 
} //namespace Version_1
} //namespace RDC 
} //namespace StarVFS 
