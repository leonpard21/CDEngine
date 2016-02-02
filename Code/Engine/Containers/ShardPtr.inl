
namespace EAE_Engine
{
	//should never use this
	template<typename T>
	inline ShardPtr<T>::ShardPtr() :
		_ptr(nullptr),
		_pRefCount(nullptr)
	{
	}


	template<typename T>
	inline ShardPtr<T>::ShardPtr(const T* i_ptr) :
		_ptr(const_cast<T*>(i_ptr)),
		_pRefCount(new uint32_t(1))// alloc the memory for the ref count at the beginning.
	{
		assert(_pRefCount != NULL);
		*_pRefCount = 1;// at the beginning, the reference should be 1.
	}

	template<typename T>
	inline ShardPtr<T>::~ShardPtr()
	{
		//check the referce count
		assert(_pRefCount != NULL);
		if (!_pRefCount)
		{
			SAFE_DELETE(_ptr);
			return;
		}
		if (*_pRefCount == 0)
		{
			SAFE_DELETE(_ptr);
			SAFE_DELETE(_pRefCount);
			return; 
		}

		assert(*_pRefCount>0);
		--(*_pRefCount);
		//at first, clear the own ptr
		if ((*_pRefCount) == 0)
		{
			SAFE_DELETE(_ptr);
			SAFE_DELETE(_pRefCount);
		}
	}

	template<typename T>
	inline ShardPtr<T>::ShardPtr(const ShardPtr<T>& i_other)
	{
		if (!i_other._ptr)
		{
			_ptr = nullptr;
			_pRefCount = new uint32_t(1);
			return;
		}

		_ptr = i_other._ptr;
		_pRefCount = i_other._pRefCount;
		//we should inc reference count
		assert(_pRefCount != NULL);
		(*_pRefCount)++;
	}

	template<typename T>
	inline ShardPtr<T>& ShardPtr<T>::operator=(const ShardPtr<T>& i_other)
	{
		//release the ownership of the data of shared_ptr1
		assert(_pRefCount != NULL);

		--(*_pRefCount);
		//at first, clear the own ptr
		if ((*_pRefCount) == 0)
		{
			SAFE_DELETE(_ptr);
			SAFE_DELETE(_pRefCount);
		}
		//second, set the data to other's data
		if (!i_other._ptr)
		{
			_ptr = nullptr;
			_pRefCount = new uint32_t(1);
			return *this;
		}

		_ptr = i_other._ptr;
		_pRefCount = i_other._pRefCount;
		//we should inc reference count
		assert(_pRefCount != NULL);
		(*_pRefCount)++;
		return *this;
	}

	template<typename T>
	inline bool ShardPtr<T>::operator==(const T* i_pOther)
	{
		if (_ptr == i_pOther)
		{
			return true;
		}
		return false;
	}

	template<typename T>
	inline bool ShardPtr<T>::ContainsSamePtr(const ShardPtr& i_other)
	{
		if (_ptr == i_other._ptr)
		{
			return true;
		}
		return false;
	}


	template<typename T>
	inline T* ShardPtr<T>::operator->()
	{
		return _ptr;
	}

	template<typename T>
	inline T& ShardPtr<T>::operator*()
	{
		assert(_ptr);
		return *_ptr;
	}

	template<typename T>
	inline bool ShardPtr<T>::IsNull()
	{
		if (_ptr == nullptr)
		{
			return true;
		}
		return false;
	}


} // namespace Engine
