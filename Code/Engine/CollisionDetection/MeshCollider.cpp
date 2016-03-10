#include "MeshCollider.h"
#include "RigidBody.h"
#include "CollisionDetectionFunctions.h"
#include "Math/ColMatrix.h"
#include "SpatialPartition/Octree.h"
#include "DebugShape/DebugShape.h"

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
			Math::ColMatrix44& transformMat = _pTransform->GetLocalToWorldMatrix();
			Math::Vector3 targetStartPoint = transformMat * pTargetRB->GetPos();
			Math::Vector3 targetEndPoint = transformMat * pTargetRB->PredictPosAfter(i_follisionTimeStep);
			Core::CompleteOctree* pOctree = Core::OctreeManager::GetInstance()->GetOctree();
			if (pOctree == nullptr)
				return false;
			std::vector<Core::TriangleIndex> triangles = pOctree->GetTrianlgesCollideWithSegment(targetStartPoint, targetEndPoint);
			std::vector<uint32_t> triangleIndices;
			for (uint32_t index = 0; index < triangles.size(); ++index)
			{
				triangleIndices.push_back(triangles[index]._index0);
				triangleIndices.push_back(triangles[index]._index1);
				triangleIndices.push_back(triangles[index]._index2);
			}
			Mesh::AOSMeshData* pData = EAE_Engine::Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData("collisionData");
			std::vector<Math::Vector3> vertices = pData->GetVertexPoses(triangleIndices);
			if (vertices.size() < 3)
				return false;
			bool collided = false;
			Math::Vector3& vertex0 = vertices[0];
			Math::Vector3& vertex1 = vertices[1];
			Math::Vector3& vertex2 = vertices[2];
			float u = 0, v = 0, w = 0, t = 0;
			int result = Collision::IntersectSegmentTriangle(targetStartPoint, targetEndPoint, vertex0, vertex1, vertex2, 
				u, v, w, t);
			if (result == 0)
				collided = false;
			if (t > 1.0f)
				collided = false;
			if (t < o_tmin)
			{
				collided = true;
				o_tmin = t;
				o_collisionPoint = vertex0 * u + vertex1 * v + vertex2 * w;
				o_collisionNormal = Math::Vector3::Cross((vertex1 - vertex0), (vertex2 - vertex1));
			}
			return collided;
		}

	}
}
