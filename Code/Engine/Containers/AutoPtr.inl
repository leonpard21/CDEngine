namespace EAE_Engine
{

	template<typename T>
	inline AutoPtr<T>::AutoPtr(T* i_ptr) :
		_ptr(i_ptr)
	{
	}

	template<typename T>
	inline AutoPtr<T>::AutoPtr(AutoPtr<T>& i_other) :
		_ptr(i_other._ptr)
	{
		// create new AutoPtr by taking ownership of existing AutoPtr
		// remember: There can be only one!
		i_other._ptr = NULL;
	}

	template<typename T>
	inline AutoPtr<T>& AutoPtr<T>::operator=(AutoPtr<T>& i_other)
	{
		// replace our existing ptr by taking ownership of another AutoPtr
		if (_ptr)
			delete(_ptr);

		_ptr = i_other._ptr;
		i_other._ptr = NULL;

		return *this;
	}

	template<typename T>
	inline AutoPtr<T>::~AutoPtr()
	{
		if (_ptr)
			delete _ptr;
	}

	template<typename T>
	inline T * AutoPtr<T>::operator->()
	{
		return _ptr;
	}

	template<typename T>
	inline T& AutoPtr<T>::operator*()
	{
		assert(_ptr);
		return *_ptr;
	}

	template<typename T>
	inline bool AutoPtr<T>::IsNull()
	{
		if (_ptr == nullptr)
		{
			return true;
		}
		return false;
	}
} // namespace Engine
