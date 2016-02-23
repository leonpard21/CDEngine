#include "World.h"
#include "../Individual/GameObj.h"
#include "../Components/Transform.h"

namespace EAE_Engine
{
	namespace Core
	{
		World::~World()
		{
		}

		Common::IGameObj* World::AddGameObj(const char* pName, Math::Vector3& localpos)
		{
			GameObj* pObj = new GameObj(pName);
			Transform* pTrans = new Transform(pObj);
			pTrans->SetLocalPos(localpos);
			pObj->SetTransform(pTrans);
			_gameObjList.push_back(pObj);
			return pObj;
		}

		Common::IGameObj* World::GetGameObj(const char* pName)
		{
			for (std::vector<GameObj*>::iterator it = _gameObjList.begin(); it != _gameObjList.end(); ++it)
			{
				GameObj* pObj = *it;
				if (strcmp(pName, pObj->GetName()) == 0) 
				{
					return pObj;
				}
			}
			return nullptr;
		}

		void World::Remove(Common::ITransform* pTransform)
		{
			std::vector<GameObj*>::iterator it = _gameObjList.end();
			for (it = _gameObjList.begin(); it != _gameObjList.end(); ++it)
			{
				GameObj* pObj = *it;
				if (pObj->GetTransform() == pTransform)
				{
					break;
				}
			}
			if (it != _gameObjList.end())
			{
				GameObj* pObj = *it;
				_gameObjList.erase(it);
				SAFE_DELETE(pTransform);
				SAFE_DELETE(pObj);
			}
		}

		void World::Clean()
		{
			for (std::vector<GameObj*>::iterator it = _gameObjList.begin(); it != _gameObjList.end(); )
			{
				GameObj* pObj = *it++;
				Common::ITransform* pTrans = pObj->GetTransform();
				SAFE_DELETE(pTrans);
				SAFE_DELETE(pObj);
			}
			_gameObjList.clear();
		}

		///////////////////////////////////static_members//////////////////////
		World* World::s_pInternalInstance = nullptr;

		World& World::GetInstance()
		{
			if (!s_pInternalInstance)
				s_pInternalInstance = new World();
			return *s_pInternalInstance;
		}

		void World::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}
	}
}