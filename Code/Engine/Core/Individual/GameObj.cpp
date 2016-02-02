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
			
			return nullptr;
		}

		Common::ITransform* GameObj::GetTransform()
		{ 
			return _pTransform;
		}

		/*
		void GameObj::AddComponent(Common::ICompo* pCompo)
		{
			_compoList.Push(pCompo);
		}
		*/
	}
}