#include "OBBCollider.h"
#include <cmath>
#include "Math/ColMatrix.h"
#include "Math/Quaternion.h"
#include "RigidBody.h"

namespace EAE_Engine
{
	namespace Collider
	{
		bool CheckContacting(OverlapAndSepTime collideTime)
		{
			bool Contacting = true;

			//if the _sepTime<=0.0f, then no matter the _overlapTime, they are seperated.
			if (collideTime._sepTime <= 0.0f)
			{
				Contacting = false;
			}
			//in this case, the overlaping happens before the frame and haven't been finished, 
			//because we know that the _sepTime is >0.0f;
			else if (collideTime._overlapTime < 0.0f)
			{
				Contacting = true;
			}
			//in this case, the overlaping happens in the future, but this test.
			else if (collideTime._overlapTime > 1.0f)
			{
				Contacting = false;
			}
			else if (collideTime._overlapTime > collideTime._sepTime)
			{
				Contacting = false;
			}
			return Contacting;
		}


		OBBCollider::OBBCollider() : Collider(),
			_center(Math::Vector3::Zero), _size(Math::Vector3(1.f, 1.f, 1.f))
		{
			//const char* str = EngineInfo::s_pGlobalNew->GetStringPool()->find("OBBCollider");
			_hashtype = HashedString("OBBCollider");
		}


		OBBCollider::~OBBCollider()
		{}

		OBBCollider* OBBCollider::InitOBBCollider(Common::ITransform* pTrans, const Math::Vector3& size, const Math::Vector3& offset)
		{
			_pTransform = pTrans;
			_size = size;
			_center = offset;
			return this;
		}

		
		bool OBBCollider::DetectCollision(Collider* i_pOther, float fElpasedTime, float& o_collisionTime, Math::Vector3& o_collisionAxis)
		{
			//set for the case there is no collision.
			o_collisionTime = FLT_MAX;
			o_collisionAxis = Math::Vector3::Zero;

			//Do the collision detection based on the types on the Colliders.
			if (i_pOther->IsSameType(HashedString("OBBCollider")))
			{
				OBBCollider* pOBBCollider = reinterpret_cast<OBBCollider*>(i_pOther);
				OverlapAndSepTime result;
				bool collided = OBBCollider::DetectCollisionIn2OBBbySAT(*this, *pOBBCollider, fElpasedTime, result);
				//set the collisionInfo got from SAT algorithm.
				o_collisionTime = fmaxf(result._overlapTime, 0.0f);//wrap the collision time to 0.f
				o_collisionAxis = result._collisionAxle;
				return collided;
			}
			return false;
		}

		bool OBBCollider::CalculateOverlapSepTimeForSAT(OBBCollider& i_boxA, OBBCollider& i_boxB, Math::Vector3 relative_movementInASpace, const Math::Vector3& axle, OverlapAndSepTime& o_result)
		{
			float rangeOfBoxA = fabsf(i_boxA._size.x() * axle.x()) + fabsf(i_boxA._size.y()*axle.y()) + fabsf(i_boxA._size.z()*axle.z());
			float rangeOfBoxB = fabsf(i_boxB._size.x() * axle.x()) + fabsf(i_boxB._size.y()*axle.y()) + fabsf(i_boxB._size.z()*axle.z());
			rangeOfBoxA *= .5f;
			rangeOfBoxB *= .5f;
			float boxAProjOnAxle = (i_boxA._pTransform->GetPos() + i_boxA._center).Dot(axle);
			float boxBProjOnAxle = (i_boxB._pTransform->GetPos() + i_boxB._center).Dot(axle);
			float relativeSpeedInAOnAxle = relative_movementInASpace.Dot(axle);
			// static cases:
			if (fabsf(relativeSpeedInAOnAxle)<FLT_EPSILON)
			{
				if (fabsf(boxBProjOnAxle - boxAProjOnAxle)<=rangeOfBoxA + rangeOfBoxB)
				{
					o_result._overlapTime = -FLT_MAX;
					o_result._sepTime = FLT_MAX;
					o_result._collisionAxle = axle;
					return true;
				}
				else 
				{
					o_result._overlapTime = FLT_MAX;
					o_result._sepTime = FLT_MAX;
					o_result._collisionAxle = Math::Vector3::Zero;
					return false;
				}
			}
			//dynamic cases:
			if (relativeSpeedInAOnAxle<0)//if boxB moves to the left side of the axle, it pass the right edge of boxA at first, left edge at second. 
			{
				o_result._overlapTime = (boxAProjOnAxle + rangeOfBoxA + rangeOfBoxB - boxBProjOnAxle) / relativeSpeedInAOnAxle;
				o_result._sepTime = (boxAProjOnAxle - rangeOfBoxA - rangeOfBoxB - boxBProjOnAxle) / relativeSpeedInAOnAxle;
			}
			else if (relativeSpeedInAOnAxle>0)//if boxB moves to the right side of the axle, it pass the left edge of boxA at first, right edge at second. 
			{
				o_result._overlapTime = (boxAProjOnAxle - rangeOfBoxA - rangeOfBoxB - boxBProjOnAxle) / relativeSpeedInAOnAxle;
				o_result._sepTime = (boxAProjOnAxle + rangeOfBoxA + rangeOfBoxB - boxBProjOnAxle) / relativeSpeedInAOnAxle;
			}
			o_result._collisionAxle = axle;
			if (o_result._overlapTime>o_result._sepTime)
			{
				return false;
			}
			return true;
		}

		bool OBBCollider::DetectCollisionIn2OBBbySAT(OBBCollider& i_boxA, OBBCollider& i_boxB, float fElpasedTime, OverlapAndSepTime& collisionInfo)
		{
			//Get rotation Matrix, for the axis of the local coordinate
			Math::Quaternion rorationBoxA = i_boxA._pTransform->GetRotation();
			Math::Quaternion rorationBoxB = i_boxB._pTransform->GetRotation();
			Math::ColMatrix44 boxARotateMatrix = Math::Quaternion::CreateColMatrix(rorationBoxA);
			Math::ColMatrix44 boxBRotateMatrix = Math::Quaternion::CreateColMatrix(rorationBoxB);
			// Get the Rotate&Transform Matrix
			Math::ColMatrix44 boxATransMatrix = i_boxA._pTransform->GetRotateTransformMatrix();
			Math::ColMatrix44 boxBTransMatrix = i_boxB._pTransform->GetRotateTransformMatrix();
			//Calculate the movement in this frame.
			Physics::RigidBody* pRBA = reinterpret_cast<Physics::RigidBody*>( i_boxA._pTransform->GetComponent(getTypeID<Physics::RigidBody>()) );
			Math::Vector4 movementPerFrameA = pRBA->GetVelocity() * fElpasedTime;
			Physics::RigidBody* pRBB = reinterpret_cast<Physics::RigidBody*>(i_boxB._pTransform->GetComponent(getTypeID<Physics::RigidBody>()) );
			Math::Vector4 movementPerFrameB = pRBB->GetVelocity()  * fElpasedTime;
			Math::Vector3 relative_movementInA = (movementPerFrameB - movementPerFrameA);
			//Get the local coordinate axes of the two OBB boxes.
			Math::Vector3 uofA[3] = { boxARotateMatrix.GetCol(0), boxARotateMatrix.GetCol(1), boxARotateMatrix.GetCol(2) };
			Math::Vector3 uofB[3] = { boxBRotateMatrix.GetCol(0), boxBRotateMatrix.GetCol(1), boxBRotateMatrix.GetCol(2) };
			//At first, we suppose that the collision may quit after we find out there is a seperation on one axle.
			//So we set the collision time to FLT_MAX
			collisionInfo._overlapTime = FLT_MAX;
			collisionInfo._sepTime = FLT_MAX;
			collisionInfo._collisionAxle = Math::Vector3::Zero;
			OverlapAndSepTime resultOnAxles[15]; //for the SAT test in 3D, we need to test 15 axles.
			//check the local x-, y-, z- of boxA as Seperating Axle.
			for (size_t i = 0; i < 3; ++i)
			{
				bool collided = CalculateOverlapSepTimeForSAT(i_boxA, i_boxB, relative_movementInA, uofA[i], resultOnAxles[i]);
				if (!collided)
					return false;
			}
			//check the local x-, y-, z- of boxB as Seperating Axle.
			for (size_t i = 0; i < 3; ++i)
			{
				bool collided = CalculateOverlapSepTimeForSAT(i_boxA, i_boxB, relative_movementInA, uofB[i], resultOnAxles[i + 3]);
				if (!collided)
					return false;
			}
			//check the cross axles of boxA and boxB, which has the extra 9 axles.
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					Math::Vector3 axle = uofA[i].Cross(uofB[j]);
					bool collided = CalculateOverlapSepTimeForSAT(i_boxA, i_boxB, relative_movementInA, axle, resultOnAxles[i * 3 + j + 6]);
					if (!collided)
						return false;
				}
			}
			//Now we know that the collision should happened. So we need to get the correct range of the collisionInfo.
			collisionInfo._overlapTime = -FLT_MAX;
			collisionInfo._sepTime = FLT_MAX;
			collisionInfo._collisionAxle = Math::Vector3::Zero;
			for (size_t index = 0; index < 15; ++index)
			{
				if (resultOnAxles[index]._overlapTime > collisionInfo._overlapTime)
				{
					collisionInfo._overlapTime = resultOnAxles[index]._overlapTime;
					collisionInfo._collisionAxle = resultOnAxles[index]._collisionAxle;//should be changed to store the collision point, but the axle.
				}
				if (resultOnAxles[index]._sepTime < collisionInfo._sepTime)
				{
					collisionInfo._sepTime = resultOnAxles[index]._sepTime;
				}
			}
			return CheckContacting(collisionInfo);//check if there is collision
		}

	}
}