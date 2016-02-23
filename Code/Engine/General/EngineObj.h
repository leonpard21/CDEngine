#ifndef EAE_ENGINE_OBJECT
#define EAE_ENGINE_OBJECT

#include "RTTI.h"

namespace EAE_Engine
{
	/*
	 * First solution for RTTI,
	 * we can add the ADD_TYPEID() macro for all of the classes need to know their type.
	 * Remember we need the RTTI_DECLARE_META_TYPE to declear the type in advance.
	 */
	// Example class:
	class EngineObj
	{			
		ADD_TYPEID()
	};

	/* 
	 * Second solution for RTTI,
	 * we can use the getTypeID<T>
	 */
	template<typename T>
	class Reflection
	{
	public:
		//const char* _pName;
		// _hideFlags
		virtual typeid_t GetTypeID() const
		{                                                             
			return getTypeID<T>();
		}
	};


}
RTTI_DECLARE_META_TYPE(EAE_Engine::EngineObj)

#endif//EAE_ENGINE_OBJECT
