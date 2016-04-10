#include "Engine.h"
#include "Core/Entirety/World.h"
#include "Controller/Controller.h"
#include "Time/Time.h"
#include "Math/Quaternion.h"
#include "Engine/Graphics/Common/Camera.h"
#include "Engine/CollisionDetection/OBBCollider.h"
#include "Engine/CollisionDetection/ColliderBase.h"
#include "Engine/CollisionDetection/RigidBody.h"
#include "Engine/Mesh/AOSMeshData.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/DebugShape/DebugShape.h"
#include "Engine/UserInput/UserInput.h"
#include "Engine/SpatialPartition/Octree.h"

#include <vector>

namespace EAE_Engine
{
	namespace Engine
	{
		void RemoveGameObj(Common::ITransform* pTrans);
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
			EAE_Engine::Engine::RemoveGameObj(pTrans);
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
			UserInput::Input::GetInstance()->Init();
			Physics::Physics::GetInstance()->Init();
			bool result = Graphics::Initialize(i_renderingWindow);
			return result;
		}

		void FixedUpdate()
		{
			Physics::Physics::GetInstance()->FixedUpdateBegin();
			int fixedUpdateRunTime = Time::GetFixedUpdateRunTimesOnThisFrame();
			for (int i = 0; i < fixedUpdateRunTime; ++i)
			{
				Controller::ControllerManager::GetInstance().FixedUpdate();
				Physics::Physics::GetInstance()->FixedUpdate();
			}
			Physics::Physics::GetInstance()->FixedUpdateEnd();
		}

		void EngineUpdate() 
		{
			EAE_Engine::Time::OnNewFrame();
			UserInput::Input::GetInstance()->Update();
			Controller::ControllerManager::GetInstance().Update();
			Collider::ColliderManager::GetInstance()->Update();
			FixedUpdate();
			Graphics::Render();
			RemoveAllActorsInList();
		}

		void EngineRender()
		{
			
		}

		void EngineEnd()
		{
			UserInput::Input::Destroy();
			Core::OctreeManager::Destroy();
			Controller::ControllerManager::CleanInstance();
			Physics::Physics::Destroy();
			Collider::ColliderManager::CleanInstance();
			Graphics::ShutDown();
			Debug::DebugShapes::CleanInstance();
			Mesh::AOSMeshDataManager::Destroy();
			Core::World::CleanInstance();
			SAFE_DELETE(_pRemoveList);
		}

		void AddToRemoveList(Common::ITransform* pTrans)
		{
			if (!_pRemoveList || pTrans == nullptr)
				return;
			for (std::vector<EAE_Engine::Common::ITransform*>::iterator it = _pRemoveList->begin();it!= _pRemoveList->end();++it)
			{
        if (*it == pTrans)
          return;
			}
      // add all of the children of this transform
      for (uint32_t i = 0; i < pTrans->GetChildCount(); ++i)
      {
        EAE_Engine::Common::ITransform* pChild = pTrans->GetChild(i);
        if (pChild)
          _pRemoveList->push_back(pChild);
      }
      // add the transform itself
      _pRemoveList->push_back(pTrans);
		}

		void RemoveGameObj(Common::ITransform* pTrans)
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
			pCameraObj->GetTransform()->SetLocalRotation(rotation);
			Common::ICamera* pCamera = Graphics::AddCamrea(windowWidth, windowHeight, pCameraObj->GetTransform());
			return pCamera;
		}

	}
}