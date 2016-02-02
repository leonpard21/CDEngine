namespace EAE_Engine
{
	template<typename T>
	inline bool NamedBitSet<T>::FindBitIndex(const HashedString& i_Name, unsigned int& o_BitIndex)
	{
		// see if i_Name exists in set
		for (unsigned int i = 0; i < c_NumBits; i++)
			if (_bitNames[i] == i_Name)
			{
				o_BitIndex = i;

				return true;
			}

		return false;
	}

	template<typename T>
	inline unsigned int NamedBitSet<T>::GetBitIndex(const HashedString& i_Name)
	{
		// see if i_Name exists in set
		for (unsigned int i = 0; i < c_NumBits; i++)
			if (_bitNames[i] == i_Name)
				return i;

		// if not find an empty slot
		HashedString Empty;

		for (unsigned int i = 0; i < c_NumBits; i++)
			if (_bitNames[i] == Empty)
			{
				_bitNames[i] = i_Name;
				return i;
			}

		// out of bits == bad
		assert(false);

		return 0;
	}

	template<typename T>
	inline bool NamedBitSet<T>::FindBitMask(const HashedString& i_Name, T& o_BitMask)
	{
		unsigned int i_BitIndex = 0;

		if (FindBitIndex(i_Name, i_BitIndex) == true)
		{
			o_BitMask = 1 << GetBitIndex(i_Name);

			return true;
		}

		return false;
	}

	template<typename T>
	inline T NamedBitSet<T>::GetBitMask(const HashedString& i_Name)
	{
		return 1 << GetBitIndex(i_Name);
	}

} // namespace Engine