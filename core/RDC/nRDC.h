
#pragma once
#ifndef nRDC_H
#define nRDC_H

#include "../nfStarVFS.h"

namespace StarVFS {
namespace RDC {

} //namespace RDC 
} //namespace StarVFS 

#ifdef _MSC_VER
#pragma pack(push, RDC_headers, 1)
#else
#error not supported compiler!
#endif

#include "Headers.h"
#include "Headers_v1.h"

#ifdef _MSC_VER
#pragma pack(pop, RDC_headers)
#endif

#include "BlockFileDevice.h"
#include "Builder.h"
#include "Builder_v1.h"

namespace StarVFS {
namespace RDC {
namespace Headers {

} //namespace Headers 
} //namespace RDC 
} //namespace StarVFS 

#endif
