#ifndef _NPPLAT_H_
#define _NPPLAT_H_

#include "npapi.h"

#include <stdio.h>

#ifndef HIBYTE
#define HIBYTE(i) (i >> 8)
#endif

#ifndef LOBYTE
#define LOBYTE(i) (i & 0xff)
#endif

#endif //_NPPLAT_H_
