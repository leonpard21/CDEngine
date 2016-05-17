#ifndef EAE_ENGINE_CORE_GAME_OBJ
#define EAE_ENGINE_CORE_GAME_OBJ
#include "Common/Interfaces.h"
#include "Containers/LinkedList.h"
#include "Engine/General/EngineObj.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Core 
	{
		class GameObj : public Common::IGameObj, public Reflection<GameObj>
		{
			GameObj() = delete;
			GameObj(const GameObj& i_other) = delete;
			GameObj& operator=(const GameObj& i_other) = delete;
			//ADD_TYPEID()
		public:
			GameObj(const char* pName);
			virtual ~GameObj();
			Common::ICompo* GetComponent(typeid_t type);
			void AddComponent(Common::Compo compo);
			template<typename T>
			void AddComponent(T* pCompo);
      template<typename T>
      T* GetComponent();
			Common::ITransform* GetTransform(); 
			void SetTransform(Common::ITransform* pTrans) { _pTransform = pTrans; }
			const char* GetName() { return _pName; }
		private:
			Common::ITransform* _pTransform;
			std::vector<Common::Compo> _components;
			char* _pName;
			//     _tag
			//     _layer
			//     _active
			bool _isStatic;
		};

		template<typename T>
		void GameObj::AddComponent(T* pCompo)
		{
			Common::Compo compo = { pCompo, getTypeID<T>()};
			_components.push_back(compo);
		}

    template<typename T>
    T* GameObj::GetComponent()
    {
      for (std::vector<Common::Compo>::iterator it = _components.begin(); it != _components.end(); ++it)
      {
        Common::Compo compo = *it;
        if (compo._typeId == getTypeID<T>())
          return (T*)compo._pCompo;
      }
      return nullptr;
    }
	}
}
RTTI_DECLARE_META_TYPE(EAE_Engine::Core::GameObj)

#endif//EAE_ENGINE_CORE_GAME_OBJ