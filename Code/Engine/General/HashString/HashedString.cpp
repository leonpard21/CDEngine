#include <assert.h>

#include "HashedString.h"

namespace EAE_Engine
{
	HashCode HashedString::Hash(const char* i_string)
	{
		assert(i_string);

		return Hash(reinterpret_cast<void*>(const_cast<char*>(i_string)), strlen(i_string));
	}

	HashCode HashedString::Hash(const void * i_bytes, size_t i_bytecount)
	{
		// FNV hash, http://isthe.com/chongo/tech/comp/fnv/

		register const unsigned char * p = static_cast<const unsigned char*>(i_bytes);
		HashCode hash = 2166136261;

		for (unsigned int i = 0; i < i_bytecount; ++i)
			hash = 16777619 * (hash ^ p[i]);

		return hash ^ (hash >> 16);
	}
} // namespace Engine