#ifndef EAE_ENGINE_CONTROLLER_H
#define EAE_ENGINE_CONTROLLER_H
#include "Engine/Common/Interfaces.h"
#include "Engine/General/EngineObj.h"
#include <vector>

namespace EAE_Engine
{
	namespace Controller
	{
		class Controller : public Reflection<Controller>, public Common::IController
		{
		public:
			Controller(Common::ITransform* pTrans);
			virtual ~Controller() { _pTransform = nullptr; }
      virtual Common::ICompo* GetComponent(typeid_t type) { return _pTransform->GetComponent(type); }
			virtual void Update();
			virtual void FixedUpdate() {}
			virtual Common::ITransform* GetTransform() { return _pTransform; }
			bool IsActive() { return _active; }
			void SetActive(bool active) { _active = active; }

		protected:
			Common::ITransform* _pTransform;
			bool _active;
		};

		class ControllerManager 
		{
		public:
			void Clean();
			void AddController(Controller* pController) { _controllers.push_back(pController); }
			void Update();
			void FixedUpdate();
			void Remove(Common::ITransform* pTrans);

			std::vector<Controller*> _controllers;

		private:
			ControllerManager() {}
			static ControllerManager* s_pInternalInstance;
		public:
			static ControllerManager& GetInstance();
			static void CleanInstance();
		};
	}
}

#endif//EAE_ENGINE_CONTROLLER_H