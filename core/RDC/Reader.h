/*
  * Generated by cppsrc.sh
  * On 2016-01-03 22:42:45,89
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef Reader_H
#define Reader_H

namespace StarVFS {
namespace RDC {

class Reader {
public:
 	Reader();
 	virtual ~Reader();

	static bool CanOpenFile(const String& FileName);

	static bool TestHeaderIntegrity(UniqueBlockFileDevice &device, FileHeader *header = nullptr);
protected:
	BlockFileDevice* GetDevice() { return m_FileDevice.get(); }
	const BlockFileDevice* GetDevice() const { return m_FileDevice.get(); }
	/* todo: private: */ std::unique_ptr<BlockFileDevice> m_FileDevice;
private:
};

} //namespace RDC 
} //namespace StarVFS 

#endif