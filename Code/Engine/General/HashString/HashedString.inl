#include <assert.h>
#include <string.h>

namespace EAE_Engine
{
	inline HashedString::HashedString() :
		_hashCode(Hash(""))
	{
	}

	inline HashedString::HashedString(const char * i_string) :
		_hashCode(Hash(i_string))
#ifdef DEBUG_KEEP_STRING
		, _pString(_strdup( i_string ))
#endif
	{
	}

	inline HashedString::HashedString(const HashedString & i_other) :
		_hashCode(i_other._hashCode)
#ifdef DEBUG_KEEP_STRING
		, _pString( _strdup( i_other._pString ) )
#endif
	{
	}

	inline HashedString::~HashedString()
	{
#ifdef DEBUG_KEEP_STRING
		if (_pString)
			free(const_cast<char*>(_pString));
#endif

	}

	inline HashedString & HashedString::operator=(const HashedString & i_other)
	{
		_hashCode = i_other._hashCode;
#ifdef DEBUG_KEEP_STRING
		if (_pString)
			free(const_cast<char*>(_pString));

		_pString = i_other._pString;
#endif
		return *this;
	}

	inline HashCode HashedString::Get(void) const
	{
		return _hashCode;
	}

	inline bool HashedString::operator==(const HashedString & i_other) const
	{
		return _hashCode == i_other._hashCode;
	}

	inline bool HashedString::operator != (const HashedString & i_other) const
	{
		return _hashCode != i_other._hashCode;
	}

	inline bool HashedString::operator<(const HashedString & i_other) const
	{
		return _hashCode < i_other._hashCode;
	}
} // namespace Engine
