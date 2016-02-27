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
			void Init(const char* pMeshKey);
			bool TestCollision(Common::IRigidBody* pTargetRB, float i_follisionTimeStep, float& o_firstCollisionTime,
				Math::Vector3& o_collisionPoint, Math::Vector3& o_collisionNormal);
			bool DetectCollision(Collider* i_pOther, float fElpasedTime, float& o_collisionTime, Math::Vector3& o_collisionAxis) 
			{ return false; }
		private:
			Mesh::AOSMeshData* _pAOSMeshData;
		};
	}
}

#endif//EAE_ENGINE_COLLISION_MESHCOLLIDER_H