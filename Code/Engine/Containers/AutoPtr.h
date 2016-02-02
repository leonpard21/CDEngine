#ifndef __AUTO_PTR_H
#define __AUTO_PTR_H

#include <assert.h>

namespace EAE_Engine
{

	template<typename T>
	class AutoPtr
	{
	public:
		explicit AutoPtr(T* i_ptr);
		inline AutoPtr(){}
		inline AutoPtr(AutoPtr& i_other);
		inline AutoPtr & operator=(AutoPtr& i_other);

		inline ~AutoPtr();

		inline T* operator->();
		inline T& operator*();

		inline bool IsNull();
		//We don't need this two function.
		// release();
		// change( T * i_newptr );

	private:
		T* 	_ptr;
	};

} // namespace Engine
#include "AutoPtr.inl"

#endif // __AUTO_PTR_H	