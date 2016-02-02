/*
 * The HashedString class comes from Joe. 
 */
#ifndef __HASHED_STRING_H
#define __HASHED_STRING_H

#include <string>
//#define DEBUG_KEEP_STRING //Use this to enable the saved string value for debugging.

namespace EAE_Engine
{
	typedef size_t HashCode;

	class HashedString
	{
	public:
		HashedString();
		HashedString(const char * i_string);
		HashedString(const HashedString & i_other);
		HashedString & operator=(const HashedString & i_other);
		~HashedString();

		HashCode Get() const;

		bool operator==(const HashedString & i_other) const;
		bool operator!=(const HashedString & i_other) const;
		bool operator<(const HashedString & i_other) const;

		static HashCode Hash(const char * i_string);
		static HashCode Hash(const void * i_bytes, size_t i_count);

	private:
		HashCode _hashCode;

		//this will copy a string generated the hashstring for debugging.
#ifdef DEBUG_KEEP_STRING
		const char * 	_pString;
#endif
	};

} // namespace Engine

#include "HashedString.inl"

#endif // __HASHED_STRING_H