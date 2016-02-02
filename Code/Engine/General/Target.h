#ifndef TARGET_H
#define TARGET_H


#if defined(_WIN32) || defined(WIN32)
#include "Target.Win32.h"
#else
#error "Must include platform target file."
#endif // WIN32

#ifndef CACHE_LINE_ALIGNMENT_BYTES
#error "Must define CACHE_LINE_ALIGNMENT_BYTES."
#endif CACHE_LINE_ALIGNMENT_BYTES


#endif // TARGET_H