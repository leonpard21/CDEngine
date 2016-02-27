#ifndef EAE_ENGINE_COLLISION_MESHCOLLIDER_H
#define EAE_ENGINE_COLLISION_MESHCOLLIDER_H
#include "ColliderBase.h"
#include "Engine/Mesh/AOSMeshData.h"


namespace EAE_Engine
{
	namespace Collider 
	{
		class MeshCollider : public Collider 
		{
		public:
			MeshCollider(Common::ITransform* pTransform);
			void Init(Common::ITransform* pTarget, const char* pMeshKey);
			bool TestCollisionDiscrete(Common::ITransform* pTarget, float& io_follisionTimeStep, Math::Vector3& o_collisionPoint);
			bool DetectCollision(Collider* i_pOther, float fElpasedTime, float& o_collisionTime, Math::Vector3& o_collisionAxis) 
			{ return false; }
		private:
			Mesh::AOSMeshData* _pAOSMeshData;
			Common::ITransform* _pTargetTransform;
		};
	}
}

#endif//EAE_ENGINE_COLLISION_MESHCOLLIDER_H