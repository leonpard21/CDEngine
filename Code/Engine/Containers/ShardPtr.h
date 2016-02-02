#ifndef __SHARD_PTR_H
#define __SHARD_PTR_H

#include <assert.h>
//#include "Implements/Implements.h"

namespace EAE_Engine
{
	template<typename T>
	class ShardPtr
	{
		//should never use this
		inline ShardPtr();
	public:
		explicit ShardPtr(const T* i_ptr);
		inline ShardPtr(const ShardPtr& i_other);
		inline ~ShardPtr();
		inline ShardPtr& operator=(const ShardPtr& i_other);
		inline bool operator==(const T* i_pOther);
		inline bool ContainsSamePtr(const ShardPtr& i_other);


		inline T* operator->();
		inline T& operator*();
		//inline void operator delete(void* i_ptr);
		//inline void operator delete[]();

		inline bool IsNull();


	private:
		T* 	_ptr;
		uint32_t* _pRefCount;
	};

} // namespace Engine

#include "ShardPtr.inl"

#endif // __SHARD_PTR_H	