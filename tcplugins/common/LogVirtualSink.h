/*
  * Generated by cppsrc.sh
  * On 2016-01-08 22:38:03,59
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef LogVirtualSink_H
#define LogVirtualSink_H

#include <memory>
#include <core/nStarVFS.h>

class LogVirtualSink : public StarVFS::Modules::iModule {
public:
 	LogVirtualSink(StarVFS::StarVFS *svfs);
 	virtual ~LogVirtualSink();

	virtual bool Enable() override;
	virtual bool Disable() override;
protected:
	//TODO: add stuff there
private: 
	struct Impl;
	struct LogFile;
	std::unique_ptr<Impl> m_Impl;

	static LogVirtualSink *s_Instance;
};

#endif
