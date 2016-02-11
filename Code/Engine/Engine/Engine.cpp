#include "Engine.h"
#include "Core/Entirety/World.h"
#include "Controller/Controller.h"
#include "Time/Time.h"
#include "Math/Quaternion.h"
#include "Engine/Graphics/Common/Camera.h"
#include "Engine/CollisionDetection/OBBCollider.h"
#include "Engine/CollisionDetection/ColliderBase.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/DebugShape/DebugShape.h"
#include <vector>

namespace EAE_Engine
{
	namespace Engine
	{
		void RemoveActor(Common::ITransform* pTrans);
	}
}

namespace 
{
	std::vector<EAE_Engine::Common::ITransform*>* _pRemoveList = nullptr;
	void RemoveAllActorsInList() 
	{
		std::vector<EAE_Engine::Common::ITransform*>::iterator iter = _pRemoveList->begin();

		for (; iter != _pRemoveList->end(); )
		{ 
			EAE_Engine::Common::ITransform* pTrans = *iter++;
			EAE_Engine::Engine::RemoveActor(pTrans);
		}
		_pRemoveList->clear();
	}

}

namespace EAE_Engine
{
	namespace Engine
	{
		bool EngineBegin(const HWND i_renderingWindow)
		{
			Time::Initialize();
			_pRemoveList = new std::vector<EAE_Engine::Common::ITransform*>();
			bool result = Graphics::Initialize(i_renderingWindow);
			return result;
		}

		void EngineUpdate() 
		{
			EAE_Engine::Time::OnNewFrame();
			float elpasedTime = EAE_Engine::Time::GetSecondsElapsedThisFrame();
			Collider::Update(elpasedTime); 
			EAE_Engine::Controller::ControllerManager::GetInstance().UpdateAll();
			RemoveAllActorsInList();
		}

		void EngineRender()
		{
			Graphics::Render();
		}

		void EngineEnd()
		{
			Controller::ControllerManager::CleanInstance();
			Collider::ColliderManager::CleanInstance();
			Graphics::ShutDown();
			Debug::DebugShapes::CleanInstance();
			Core::World::CleanInstance();
			SAFE_DELETE(_pRemoveList);
		}

		void AddToRemoveList(Common::ITransform* pTrans)
		{
			if (!_pRemoveList)
				return;
			for (std::vector<EAE_Engine::Common::ITransform*>::iterator it = _pRemoveList->begin();it!= _pRemoveList->end();++it)
			{
				if (*it == pTrans) 
				{
					return;
				}
			}
			_pRemoveList->push_back(pTrans);
		}

		void RemoveActor(Common::ITransform* pTrans)
		{
			Controller::ControllerManager::GetInstance().Remove(pTrans);
			Collider::ColliderManager::GetInstance()->Remove(pTrans);
			Graphics::RemoveMeshRender(pTrans);
			Core::World::GetInstance().Remove(pTrans);
		}

		Common::ICamera* CreateCamera(const char* pCamName, Math::Vector3& pos, Math::Quaternion& rotation,
			float windowWidth, float windowHeight)
		{
			Common::IGameObj* pCameraObj = EAE_Engine::Core::World::GetInstance().AddGameObj(pCamName, pos);
			pCameraObj->GetTransform()->Rotate(rotation);
			Common::ICamera* pCamera = Graphics::AddCamrea(windowWidth, windowHeight, pCameraObj->GetTransform());
			return pCamera;
		}

	}
}