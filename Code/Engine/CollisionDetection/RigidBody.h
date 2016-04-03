#ifndef EAE_ENGINE_PHYSICS_RIGIDBODY
#define EAE_ENGINE_PHYSICS_RIGIDBODY

#include "Engine/Common/Interfaces.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/General/Singleton.hpp"
#include "Engine/General/EngineObj.h"
#include "Engine/SpatialPartition/Octree.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Collider
	{
		class Collider;
	}

	namespace Physics 
	{
		class RigidBody;
		class RigidBodyManager;
		class Physics : public Singleton<Physics>
		{
		public:
			Physics();
			~Physics();
			void Init();
			void FixedUpdateBegin();
			void FixedUpdate();
			void FixedUpdateEnd();
      bool RayCast(Math::Vector3 origin, Math::Vector3 end, std::vector<EAE_Engine::Core::TriangleIndex>& o_triangles);
			RigidBody* AddRigidBody(Common::ITransform* pTransform);
			Math::Vector3 GetGravity() { return _gravity; }

		private:
			RigidBodyManager* _pRigidBodyManager;
			Math::Vector3 _gravity;
			float _accumulatTime;
      Core::CompleteOctree* _pCompleteOctreeTree;
		};


		class RigidBody : public Reflection<RigidBody>, public Common::IRigidBody
		{
		public:
			RigidBody(Common::ITransform* pTransform);
			~RigidBody();
			Common::ICompo* GetComponent(typeid_t type) { return _pTransform->GetComponent(type); }
			Common::ITransform* GetTransform() { return _pTransform; }
			Math::Vector3 GetVelocity() const { return _currentVelocity; }
			void SetVelocity(const Math::Vector3& velocity) 
			{
				_currentVelocity = velocity; 
				_lastVelocity = velocity;
			}
			Math::Vector3 GetPos() const { return _currentPos; }
			void SetPos(const Math::Vector3& pos) 
			{ 
				_currentPos = pos; 
				_lastPos = pos;
			}
			//Math::Quaternion GetRotation();
			//void SetRotation(Math::Quaternion& rotation);
			float GetMass() const { return _mass; }
			void SetMass(float mass) { _mass = mass; }
			bool useGravity() { return _useGravity; }
			void SetCollisionDetectionMode(Common::CollisionDetectionMode mode) { _mode = mode; }
			Common::CollisionDetectionMode GetCollisionDetectionMode() const { return _mode; }
			void AddForce(Math::Vector3& force, Common::ForceMode mode = Common::ForceMode::Force);
			bool Advance(std::vector<Collider::Collider*>& i_colliderList, float i_timeStep, int& io_testDepth);
			void BlendForTimeGap(float blendAlpha);
			Math::Vector3 PredictPosAfter(float timeStep);

		private:
			struct CollisionInfo
			{
				CollisionInfo() : _mint(FLT_MAX), _collisionPoint(Math::Vector3::Zero), _collisionNormal(Math::Vector3::Zero) {}
				float _mint;
				Math::Vector3 _collisionPoint;
				Math::Vector3 _collisionNormal;
			};

			bool DetectionCollision(std::vector<Collider::Collider*>& colliderList, float timeStep, CollisionInfo& o_collisionInfo);
			void Response(Math::Vector3 force, float timeStep);
			friend class RigidBodyManager;
		private:
			Common::ITransform* _pTransform;
			// for RigidBody's Pos and Rotation, they only have world value.
			Math::Quaternion _spin;
			Math::Vector3 _angularVelocity;
			Math::Vector3 _currentPos;
			Math::Vector3 _currentVelocity;

			Math::Vector3 _lastPos;
			Math::Vector3 _lastVelocity;

			float _mass;
			Common::CollisionDetectionMode _mode;
			bool _useGravity;
			

			Math::Vector3 _outForceWorkingOn;
			Math::Vector3 _totalForceWorkingOn;
		};


		class RigidBodyManager
		{
		public:
			~RigidBodyManager();
			RigidBody* AddRigidBody(Common::ITransform* pTransform);
			RigidBody* GetRigidBody(Common::ITransform* pTransform);
			void FixedUpdateBegin();
			void FixedUpdate();
			void FixedUpdateEnd();


		private:
			std::vector<RigidBody*> _rigidBodys;
		};

	}
}

RTTI_DECLARE_META_TYPE(EAE_Engine::Physics::RigidBody)
#endif//EAE_ENGINE_PHYSICS_RIGIDBODY
