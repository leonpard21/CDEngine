#include "GameObj.h"
#include "../Components/Transform.h"
#include "General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Core
	{
		GameObj::GameObj(const char* pName) : _pTransform(nullptr),
			_pName(CopyStr(pName))
		{
		}

		GameObj::~GameObj()
		{
			DeleteStr(_pName);
		}

		Common::ICompo* GameObj::GetComponent(typeid_t type)
		{
			for (std::vector<Common::Compo>::iterator it = _components.begin(); it!= _components.end(); ++it)
			{
				Common::Compo compo = *it;
				if(compo._typeId == type)
					return compo._pCompo;
			}
			return nullptr;
		}

		void GameObj::AddComponent(Common::Compo compo)
		{
			_components.push_back(compo);
		}

		Common::ITransform* GameObj::GetTransform()
		{ 
			return _pTransform;
		}

	}
}