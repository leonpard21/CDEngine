#ifndef EAE_ENGINE_OBJECT
#define EAE_ENGINE_OBJECT

#include "RTTI.h"


namespace EAE_Engine
{
	class EngineObj
	{
	public:
		//const char* _pName;
		// _hideFlags
			
		ADD_TYPEID();
	};

}
RTTI_DECLARE_META_TYPE(EAE_Engine::EngineObj)

#endif//EAE_ENGINE_OBJECT
