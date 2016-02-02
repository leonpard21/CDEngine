#ifndef __NEW_H
#define __NEW_H

#ifdef _DEBUG
#include <crtdbg.h>
#define TrackableNew new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define TrackableNew new
#endif



#endif  // __NEW_H