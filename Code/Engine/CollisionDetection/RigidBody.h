#ifndef EAE_ENGINE_PHYSICS_RIGIDBODY
#define EAE_ENGINE_PHYSICS_RIGIDBODY

#include "Engine/Common/Interfaces.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/General/Singleton.hpp"
#include <vector>

namespace EAE_Engine 
{
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
			void FixedUpdate();
			void Synchronize();
			RigidBody* AddRigidBody(Common::ITransform* pTransform);
			Math::Vector3 GetGravity() { return _gravity; }

		private:
			RigidBodyManager* _pRigidBodyManager;
			Math::Vector3 _gravity;
			float _accumulatTime;
		};


		class RigidBody : public Common::IRigidBody
		{
		public:
			RigidBody(Common::ITransform* pTransform);
			~RigidBody();
			Common::ITransform* GetTransform() { return _pTransform; }
			Math::Vector3 GetVelocity() const { return _velocity; }
			void SetVelocity(Math::Vector3& velocity) { _velocity = velocity; }
			Math::Vector3 GetPos() const { return _baryCenter; }
			void SetPos(Math::Vector3& pos) { _baryCenter = pos; }
			//Math::Quaternion GetRotation();
			//void SetRotation(Math::Quaternion& rotation);
			float GetMass() const { return _mass; }
			void SetMass(float mass) { _mass = mass; }
			void SetCollisionDetectionMode(Common::CollisionDetectionMode mode) { _mode = mode; }
			Common::CollisionDetectionMode GetCollisionDetectionMode() const { return _mode; }
			void AddForce(Math::Vector3& force, Common::ForceMode mode = Common::ForceMode::Force);
			void Advance(float timeStep);
			void BlendForTimeGap(float blendAlpha);

		private:

		private:
			Common::ITransform* _pTransform;
			// for RigidBody's Pos and Rotation, they only have world value.
			Math::Quaternion _spin;
			Math::Vector3 _angularVelocity;
			Math::Vector3 _baryCenter;
			Math::Vector3 _velocity;
			Math::Vector3 _lastBaryCenter;

			float _mass;
			Common::CollisionDetectionMode _mode;
			bool _useGravity;
			

			Math::Vector3 _forceWorkingOn;
		};

		class RigidBodyManager
		{
		public:
			~RigidBodyManager();
			RigidBody* AddRigidBody(Common::ITransform* pTransform);
			RigidBody* GetRigidBody(Common::ITransform* pTransform);
			void FixedUpdate();
			void UpdateTransform(float blendAlpha);


		private:
			std::vector<RigidBody*> _rigidBodys;
		};

	}
}

#endif//EAE_ENGINE_PHYSICS_RIGIDBODY
