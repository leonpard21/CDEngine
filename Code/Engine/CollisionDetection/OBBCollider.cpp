#include "OBBCollider.h"
#include <cmath>
#include "Math/ColMatrix.h"
#include "Math/Quaternion.h"

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

		
		/*
		OverlapAndSepTime OBBCollider::CalculateOverlapSepTimeForSAT(
			Engine::Math::Vector4 boxASize, const Math::Vector4& i_movementPerFrameA, const Math::Matrix44& i_ObjAtoWorld,
			Engine::Math::Vector4 boxBSize, const Math::Vector4& i_movementPerFrameB, const Math::Matrix44& i_ObjBtoWorld,
			Engine::Math::Vector3& axle)
		{
			//here, we should get the matrix of convert BoxA's local coordinate to BoxB's local coordinate.
			Engine::Math::Matrix44 boxBTransInvert;
			bool inverTest = i_ObjBtoWorld.GetInvert(boxBTransInvert);
			assert(inverTest);
			assert(i_ObjBtoWorld * boxBTransInvert == Engine::Math::Matrix44::Identity);
			Engine::Math::Matrix44 boxAtoBMatrix = i_ObjAtoWorld * boxBTransInvert;

			//In boxA's local coordinate, it's location is (0.0f, 0.0f, 0.0f, 1.0f)
			//We will move the center of boxA into boxB's local coordinate.
			Engine::Math::Vector4 boxACenter = Engine::Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
			Engine::Math::Vector4 boxBCenter = Engine::Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);

			//calculate the projection of boxA's size project on the axle in boxB's local coordinate system.
			Engine::Math::Vector4 boxAWidthVec = Math::Vector4(boxASize.x() / 2.0f, 0, 0.0f, 0.0f);
			Engine::Math::Vector4 boxAHeightVec = Math::Vector4(0.0f, boxASize.y() / 2.0f, 0.0f, 0.0f);
			Engine::Math::Vector4 boxADepthVec = Math::Vector4(0.0f, 0.0f, boxASize.z() / 2.0f, 0.0f);
			float boxASizeProjectOnAxleInB = fabsf((boxAWidthVec * boxAtoBMatrix).Dot(axle)) + fabsf((boxAHeightVec * boxAtoBMatrix).Dot(axle)) + fabsf((boxADepthVec * boxAtoBMatrix).Dot(axle));
			//calculate the projection of boxB's size project on the axle in boxB's local coordinate system.
			Engine::Math::Vector4 boxBWidthVec = Math::Vector4(boxBSize.x() / 2.0f, 0, 0.0f, 0.0f);
			Engine::Math::Vector4 boxBHeightVec = Math::Vector4(0.0f, boxBSize.y() / 2.0f, 0.0f, 0.0f);
			Engine::Math::Vector4 boxBDepthVec = Math::Vector4(0.0f, 0.0f, boxBSize.z() / 2.0f, 0.0f);
			float boxBSizeProjectOnAxleInB = fabsf((boxBWidthVec).Dot(axle)) + fabsf((boxBHeightVec).Dot(axle)) + fabsf((boxBDepthVec).Dot(axle));
			//project the center of boxA into the axle in boxB's local coordinate system.
			float boxACenterProjInB = (boxACenter * boxAtoBMatrix).Dot(axle);
			
			//so the range of boxB's proejction on the axle should be:
			//remember that, we expand the boxB's projection
			//by boxA's size projected on the axle in boxB's coordinate system
			//So we can just treat boxA as a single point projected on the axle in B's coordinate system, too.
			float boxBLeftProjOnAxle = (boxBCenter).Dot(axle) - boxBSizeProjectOnAxleInB - boxASizeProjectOnAxleInB; //center of B in B's local coordinate is (0, 0, 0, 1)
			float boxBrightProjOnAxle = (boxBCenter).Dot(axle) + boxBSizeProjectOnAxleInB + boxASizeProjectOnAxleInB; //center of B in B's local coordinate is (0, 0, 0, 1)

			//Because we suppose there is no previous collision,
			//so boxA's center can only be on the left or right side of the boxB's range
			//What's more, we should also calculate the distance based on the right and left side.
			float distanceOverlapWithBoxB = 0.0f;//the distance boxA will collide with boxB
			float distanceSepWithBoxB = 0.0f;//the distance boxA will seperate with boxB
			//calculate the speed on the axle
			Engine::Math::Vector3 relative_movement_InB = (i_movementPerFrameA - i_movementPerFrameB) * boxBTransInvert;
			float relativeMoveOnAxle = relative_movement_InB.Dot(axle);
			//if A's movement is larger than B's movement, 
			//and the relatedmovement's projection on the axle is positive, 
			//then A to B's related_speed is positive, to the >0 direction of the axle.
			if (relativeMoveOnAxle > 0) //boxA is moving to right related to boxB
			{
				distanceOverlapWithBoxB = (boxBLeftProjOnAxle - boxACenterProjInB);
				distanceSepWithBoxB = (boxBrightProjOnAxle - boxACenterProjInB);
			}
			else //boxA is moving to left related to boxB
			{
				distanceOverlapWithBoxB = (boxBrightProjOnAxle - boxACenterProjInB);
				distanceSepWithBoxB = (boxBLeftProjOnAxle - boxACenterProjInB);
			}

			OverlapAndSepTime result;

			//we should deal with the static cases:
			if (relativeMoveOnAxle == 0.0f)
			{
				if (boxACenterProjInB > boxBLeftProjOnAxle && boxACenterProjInB > boxBrightProjOnAxle)
				{
					result._overlapTime = FLT_MAX;
					result._sepTime = FLT_MAX;
					result._collisionAxle = Math::Vector3::Zero;
				}
				else if (boxACenterProjInB < boxBLeftProjOnAxle && boxACenterProjInB < boxBrightProjOnAxle)
				{
					result._overlapTime = FLT_MAX;
					result._sepTime = FLT_MAX;
					result._collisionAxle = Math::Vector3::Zero;
				}
				else
				{
					result._overlapTime = -FLT_MAX;
					result._sepTime = FLT_MAX;
					result._collisionAxle = Math::Vector3::Zero;
				}
				return result;
			}
			assert(relativeMoveOnAxle != 0.0f);
			result._overlapTime = distanceOverlapWithBoxB / relativeMoveOnAxle;
			result._sepTime = distanceSepWithBoxB / relativeMoveOnAxle;
			//because the axle is in the BoxB's local coordinate system,
			//so we move it to the world.
			result._collisionAxle = axle * i_ObjBtoWorld;
			assert(result._overlapTime <= result._sepTime);
			return result;
		}

		//Check two bounding boxes, if they has collided, return true.
		//else return false;
		bool OBBCollider::DetectCollisionIn2OBBbySAT(OBBCollider& i_boxA, OBBCollider& i_boxB, float fElpasedTime, OverlapAndSepTime& collisionInfo)
		{
			Engine::Math::Vector3 xAxle = Engine::Math::Vector3(1.0f, 0.0f, 0.0f);
			Engine::Math::Vector3 yAxle = Engine::Math::Vector3(0.0f, 1.0f, 0.0f);
			Engine::Math::Vector3 zAxle = Engine::Math::Vector3(0.0f, 0.0f, 1.0f);

			//get the matrix of the boxA
			Engine::Math::Matrix44 boxAMoveMatrix = Math::Matrix44::GetTranformLH(i_boxA._pGameObject->GetPosition());
			float angleA = (i_boxA._pGameObject->GetAngle() * 3.14159265359f) / 180.0f;
			Engine::Math::Matrix44 boxARotateMatrix = Math::Matrix44::GetRotateAroundZLH(angleA);
			Engine::Math::Matrix44 boxATransMatrix = boxARotateMatrix * boxAMoveMatrix;
			//get the matrix of the boxB
			Engine::Math::Matrix44 boxBMoveMatrix = Math::Matrix44::GetTranformLH(i_boxB._pGameObject->GetPosition());
			float angleB = (i_boxB._pGameObject->GetAngle() * 3.14159265359f) / 180.0f;
			Engine::Math::Matrix44 boxBRotateMatrix = Math::Matrix44::GetRotateAroundZLH(angleB);
			Engine::Math::Matrix44 boxBTransMatrix = boxBRotateMatrix * boxBMoveMatrix;
			//Calculate the movement in this frame.
			Engine::Math::Vector4 movementPerFrameA = i_boxA._pGameObject->GetVelocity() * fElpasedTime;// * deltaTime
			Engine::Math::Vector4 movementPerFrameB = i_boxB._pGameObject->GetVelocity() * fElpasedTime;// * deltaTime

			OverlapAndSepTime resultOnAxles[15];
			//We should check boxA collide boxB on both X axle, Y axle, and Z axle.
			resultOnAxles[0] = CalculateOverlapSepTimeForSAT(i_boxA._size, movementPerFrameA, boxATransMatrix,
				i_boxB._size, movementPerFrameB, boxBTransMatrix, xAxle);//resultOnX 
			resultOnAxles[1] = CalculateOverlapSepTimeForSAT(i_boxA._size, movementPerFrameA, boxATransMatrix,
				i_boxB._size, movementPerFrameB, boxBTransMatrix, yAxle);//resultOnY
			resultOnAxles[2] = CalculateOverlapSepTimeForSAT(i_boxA._size, movementPerFrameA, boxATransMatrix,
				i_boxB._size, movementPerFrameB, boxBTransMatrix, zAxle);//resultOnZ
			//Now let's should check boxB collide boxA on both X axle, Y axle, and Z axle.
			resultOnAxles[3] = CalculateOverlapSepTimeForSAT(i_boxB._size, movementPerFrameB, boxBTransMatrix,
				i_boxA._size, movementPerFrameA, boxATransMatrix, xAxle);//resultOnX 
			resultOnAxles[4] = CalculateOverlapSepTimeForSAT(i_boxB._size, movementPerFrameB, boxBTransMatrix,
				i_boxA._size, movementPerFrameA, boxATransMatrix, yAxle);//resultOnY
			resultOnAxles[5] = CalculateOverlapSepTimeForSAT(i_boxB._size, movementPerFrameB, boxBTransMatrix,
				i_boxA._size, movementPerFrameA, boxATransMatrix, zAxle);//resultOnZ

			collisionInfo._overlapTime = -FLT_MAX;
			collisionInfo._sepTime = FLT_MAX;
			collisionInfo._collisionAxle = Math::Vector3::Zero;
			for (size_t index = 0; index < 6; ++index)
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
		*/
		
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
			Math::Vector4 movementPerFrameA = i_boxA._pTransform->GetVelocity() * fElpasedTime;
			Math::Vector4 movementPerFrameB = i_boxB._pTransform->GetVelocity() * fElpasedTime;
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