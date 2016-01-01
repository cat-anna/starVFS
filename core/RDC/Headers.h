#pragma once
#ifndef idEA0ADC8C_0290_45E0_A08F3CF53EFCAAAE
#define idEA0ADC8C_0290_45E0_A08F3CF53EFCAAAE

#include <cstdint>

namespace StarVFS {
namespace RDC {
namespace Headers {

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

using VersionValue = u16;

enum class Signature : u32 {
	RDC = 'R' | 'D' << 8 | 'C' << 16 | '!' << 24,
	FDC = 'F' | 'D' << 8 | 'C' << 16 | '!' << 24,

	Header = RDC,
	Footer = RDC,
};

struct FileHeader {
	Signature FileSignature = Signature::Header;
	VersionValue Version = 0;
	union {
		struct {
		};
		u16 intval = 0;
	} Flags;

	u32 unused_32_0 = 0;
	u32 unused_32_1 = 0;

	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert(sizeof(FileHeader) % 4 == 0, "FileHeader has invalid size!");

// There allways must be an EmptyEntry section at the end of section table
enum class SectionType : u8 {
	EmptyEntry,/** Ignore all section of that type */

	FileTable,
	RawData,
	StringTable,
};

enum class CompressionMode : u8 {
	None,
};

enum class EncryptionMode : u8 {
	None,
};

enum class CheckSumMode : u8 {
	None,
};

#if 0

struct MD5CheckSum {
	union {
		u8 bytes[16];
		u32	dwords[4];
		u64 qwords[2];
	};

	bool isValid() const { return !(qwords[0] && qwords[1]); }
	bool operator ==(const MD5CheckSum& chk) const {
		return qwords[0] == chk.qwords[0] && qwords[1] == chk.qwords[1];
	}

	MD5CheckSum() {
		qwords[0] = qwords[1] = 0;
	}
};

#endif

} //namespace Headers 
} //namespace RDC 
} //namespace StarVFS 

#endif // header
