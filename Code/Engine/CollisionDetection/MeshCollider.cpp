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

		bool MeshCollider::TestCollisionDiscrete(Common::ITransform* pTarget, float& io_follisionTimeStep, Math::Vector3& o_collisionPoint)
		{
			pTarget = _pTargetTransform;
			if (_pAOSMeshData == nullptr)
				return false;
			bool collided = false;
			float tmin = FLT_MAX;
			Math::ColMatrix44& transformMat = _pTransform->GetLocalToWorldMatrix();
			Physics::RigidBody* pTargetRB = (Physics::RigidBody*)pTarget->GetComponent(getTypeID<Physics::RigidBody>());
			{
				if (pTargetRB->useGravity())
					pTargetRB->AddForce(Physics::Physics::GetInstance()->GetGravity() * pTargetRB->GetMass());
			}
			Math::Vector3 velocityRB = pTargetRB->GetVelocity();
			Math::Vector3 targetStartPoint = pTargetRB->GetPos();
			Math::Vector3 targetEndPoint = targetStartPoint + velocityRB * io_follisionTimeStep + Math::Vector3(0.0f, -2.5f, 0.0f);
			Math::Vector3 normalOfFace = Math::Vector3::Zero;
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
					normalOfFace = Math::Vector3::Cross((vertex1 - vertex0), (vertex2 - vertex1));
				}
			}
			float passedTime = io_follisionTimeStep;
			if (collided)
			{
				/*
				Math::Vector3 velocity = pTargetRB->GetVelocity();
				float dotValue = Math::Vector3::Dot(velocity, normalOfFace.GetNormalize());
				velocity = velocity + normalOfFace.GetNormalize() * -dotValue;
				*/
				
				{
					// first, add an inverse force on the RigidBody
					// based on the surface.
					Math::Vector3 force = pTargetRB->GetForce();
					float dotForce = Math::Vector3::Dot(force, normalOfFace.GetNormalize());
					force = force + normalOfFace.GetNormalize() * -dotForce;
					pTargetRB->SetForce(force);
					// second, deal with the velocity based on the collision surface.
					Math::Vector3 velocity = pTargetRB->GetVelocity();
					float dotVelocity = Math::Vector3::Dot(velocity, normalOfFace.GetNormalize());
					Math::Vector3 velocityY = normalOfFace.GetNormalize() * dotVelocity;
					Math::Vector3 velocityX = velocity - velocityY;
					// if both of the 2 colliders has rigidBody, 
					// then we need to calculate their momentum.
					// Or the speed of the rigidBody will be zero along the direction of the collision normal.
					pTargetRB->SetVelocity(velocityX);
				}
				passedTime = tmin * io_follisionTimeStep;
			}
			pTargetRB->Advance(passedTime);
			io_follisionTimeStep = io_follisionTimeStep - passedTime;
			return collided;
		}

	}
}
