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

		void MeshCollider::Init(const char* pMeshKey)
		{
			_pAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(pMeshKey);
		}

		bool MeshCollider::TestCollision(Common::IRigidBody* pTargetRB, float i_follisionTimeStep, float& o_tmin,
			Math::Vector3& o_collisionPoint, Math::Vector3& o_collisionNormal)
		{
			//o_tmin = FLT_MAX;
			if (_pAOSMeshData == nullptr)
				return false;
			bool collided = false;
			Math::ColMatrix44& transformMat = _pTransform->GetLocalToWorldMatrix();
			Math::Vector3 targetStartPoint = transformMat * pTargetRB->GetPos();
			Math::Vector3 targetEndPoint = transformMat * pTargetRB->PredictPosAfter(i_follisionTimeStep);
			for (uint32_t index = 0; index < _pAOSMeshData->_indices.size(); index += 3)
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
				if (t < o_tmin)
				{
					collided = true;
					o_tmin = t;
					o_collisionPoint = vertex0 * u + vertex1 * v + vertex2 * w;
					o_collisionNormal = Math::Vector3::Cross((vertex1 - vertex0), (vertex2 - vertex1));
				}
			}
			return collided;
		}

	}
}
