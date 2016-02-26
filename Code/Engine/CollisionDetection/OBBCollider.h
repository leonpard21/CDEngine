#ifndef EAEENGINE_OBBCOLLIDER_H
#define EAEENGINE_OBBCOLLIDER_H

#include "ColliderBase.h"

namespace EAE_Engine
{
	namespace Collider
	{
		struct OverlapAndSepTime
		{
			Math::Vector3 _collisionAxle;//the axle which the collision happens.
			float _overlapTime;
			float _sepTime;
			OverlapAndSepTime() = default;
		};


		class OBBCollider: public Collider
		{
			OBBCollider(const OBBCollider& i_other) = delete;
			OBBCollider& operator=(const OBBCollider& i_other) = delete;
		public:
			OBBCollider();
			virtual ~OBBCollider();
			OBBCollider* InitOBBCollider(Common::ITransform* pTrans, const Math::Vector3& size, const Math::Vector3& offset = Math::Vector3::Zero);
			
			virtual bool TestCollisionDiscrete(Common::ITransform* pTarget) { return false; }
			bool DetectCollision(Collider* i_pOther, float fElpasedTime, float& o_collisionTime, Math::Vector3& o_collisionAxis);
			static bool DetectCollisionIn2OBBbySAT(OBBCollider& i_boxA, OBBCollider& i_boxB, float fElpasedTime, OverlapAndSepTime& collisionInfo);
		
		private:
			//Detect the Collision and Seperation time on the axle.
			/*
			static OverlapAndSepTime CalculateOverlapSepTimeForSAT(Engine::Math::Vector4 boxASize, const Math::Vector4& i_movementPerFrameA, const Math::Matrix44& i_ObjAtoWorld,
															Engine::Math::Vector4 boxBSize, const Math::Vector4& i_movementPerFrameB, const Math::Matrix44& i_ObjBtoWorld,
															Engine::Math::Vector3& axle);*/
			static bool CalculateOverlapSepTimeForSAT(OBBCollider& i_boxA, OBBCollider& i_boxB, Math::Vector3 relative_movementInA, const Math::Vector3& axle, OverlapAndSepTime& o_result);

		private:
			Math::Vector3 _size;  //the extents of the bounding box
			Math::Vector3 _center;//center point, it is under the local coordinate
		};



	}
}

#endif//EAEENGINE_OBBCOLLIDER_H