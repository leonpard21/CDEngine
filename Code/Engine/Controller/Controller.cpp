#include "Controller.h"
#include "Engine/UserInput/UserInput.h"
#include "Engine/Time/Time.h"
#include "Engine/Math/Vector.h"
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Controller
	{

		Controller::Controller(Common::ITransform* pTrans) :
			_pTransform(pTrans), _active(true)
		{
			
		}

		void Controller::Update() 
		{

		}

		////////////////////////////////ControllerManager//////////////////////////////
		void ControllerManager::Clean()
		{
			for (std::vector<Controller*>::iterator iter = _controllers.begin(); iter != _controllers.end();)
			{
				Common::IController* pController = *iter++;
				SAFE_DELETE(pController);
			}
			_controllers.clear();
		}
		
		void ControllerManager::Update()
		{
			// Because we will use the Controller to add Actor, in which case we will add Controller,
			// so here I just get the size at first, then I will not access the _controllers directly,
			// this is not a good solution, but it works right now.
			// Later I really really should fix it.
			size_t size = _controllers.size();
			for (size_t i = 0; i < size; ++i)
			{
				Common::IController* pController = _controllers[i];
				if (pController && pController->IsActive())
					pController->Update();
			}
		}

		void ControllerManager::FixedUpdate()
		{
			size_t size = _controllers.size();
			for (size_t i = 0; i < size; ++i)
			{
				Common::IController* pController = _controllers[i];
				if (pController && pController->IsActive())
					pController->FixedUpdate();
			}
		}

		void ControllerManager::Remove(Common::ITransform* pTrans)
		{
			// Remember that an Actor may have multi controllers,
			// so we need to iterator the whole list until the end,
			// and delete all of the Controller contains ITramsform
			std::vector<Controller*>::iterator iter = _controllers.end();
			for (iter = _controllers.begin(); iter != _controllers.end();)
			{
				Common::IController* pController = *iter;
				//const char* pName = pController->GetTransform()->GetGameObj()->GetName();
				if (pController && pController->GetTransform() == pTrans)
				{
					iter = _controllers.erase(iter++);
					SAFE_DELETE(pController);
				}
				else 
				{
					++iter;
				}
			}
		}

		////////////////////////////////static_members/////////////////////////////////
		ControllerManager* ControllerManager::s_pInternalInstance = nullptr;

		ControllerManager& ControllerManager::GetInstance()
		{
			if (!s_pInternalInstance)
				s_pInternalInstance = new ControllerManager();
			return *s_pInternalInstance;
		}

		void ControllerManager::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}
	}
}