#include "MeshCollider.h"
#include "RigidBody.h"
#include "CollisionDetectionFunctions.h"
#include "Math/ColMatrix.h"

namespace EAE_Engine
{
	namespace Collider
	{
		MeshCollider::MeshCollider(Common::ITransform* pTransform) : Collider(), 
			_pAOSMeshData(nullptr)
		{
			_hashtype = HashedString("MeshCollider");
			_pTransform = pTransform;
		}

		void MeshCollider::Init(Common::ITransform* pTarget, const char* pMeshKey)
		{
			_pTargetTransform = pTarget;
			_pAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(pMeshKey);
		}

		bool MeshCollider::TestCollisionDiscrete(Common::ITransform* pTarget)
		{
			pTarget = _pTargetTransform;
			Math::Vector3 o_collisionPoint = Math::Vector3::Zero;
			if (_pAOSMeshData == nullptr)
				return false;
			bool collided = false;
			float tmin = FLT_MAX;
			Math::ColMatrix44& transformMat = _pTransform->GetLocalToWorldMatrix();
			Physics::RigidBody* pTargetRB = (Physics::RigidBody*)pTarget->GetComponent(getTypeID<Physics::RigidBody>());
			Math::Vector3 velocityRB = pTargetRB->GetVelocity();
			Math::Vector3 targetStartPoint = pTargetRB->GetPos();
			Math::Vector3 targetEndPoint = targetStartPoint + velocityRB * (Time::GetFixedTimeStep() + 0.5f);
			for (int index = 0; index < _pAOSMeshData->_indices.size(); index += 3)
			{
				Math::Vector3& vertex0 = _pAOSMeshData->GetVertex(index + 0);
				Math::Vector3& vertex1 = _pAOSMeshData->GetVertex(index + 1);
				Math::Vector3& vertex2 = _pAOSMeshData->GetVertex(index + 2);
				float u = 0, v = 0, w = 0, t = 0;
				int result = Collision::IntersectSegmentTriangle(targetStartPoint, targetEndPoint, vertex0, vertex1, vertex2, 
					u, v, w, t);
				if (result == 0)
					continue;
				if (t > 1.0f)
					continue;
				if (t < tmin) 
				{
					collided = true;
					tmin = t;
					o_collisionPoint = vertex0 * u + vertex1 * v + vertex2 * w;
				}
			}
			Math::Vector3 velocity = Math::Vector3::Zero;
			if (collided)
				pTargetRB->AddForce(velocityRB * -1.0f, Common::ForceMode::Velocity);
			return collided;
		}

	}
}
