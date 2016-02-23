#ifndef EAE_ENGINE_WORLD
#define EAE_ENGINE_WORLD
#include "Engine/General/EngineObj.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/Math/Vector.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Core 
	{
		class GameObj;

		class World : public EngineObj
		{
		public:
			~World();
			Common::IGameObj* AddGameObj(const char* pName, Math::Vector3& localpos);
			Common::IGameObj* GetGameObj(const char* pName);
			void Remove(Common::ITransform* pTransform);
			void Clean();
			std::vector<GameObj*> _gameObjList;

		/////////////////////////////static_members////////////////////////////////
		private:
			World() {}
			static World* s_pInternalInstance;
		public:
			static World& GetInstance();
			static void CleanInstance();
		};
	}
}
RTTI_DECLARE_META_TYPE(EAE_Engine::Core::World)

#endif//EAE_ENGINE_WORLD