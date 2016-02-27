#include "RigidBody.h"
#include "Time/Time.h"
#include "General/MemoryOp.h"

namespace EAE_Engine 
{
	namespace Physics 
	{
		Physics::Physics() : 
			_pRigidBodyManager(nullptr), 
			_gravity(Math::Vector3(0.f, -9.8f, 0.f)),
			_accumulatTime(0.0f)
		{
		}

		Physics::~Physics() 
		{ 
			SAFE_DELETE(_pRigidBodyManager);
		}

		void Physics::Init() 
		{
			_pRigidBodyManager = new RigidBodyManager();
		}

		void Physics::FixedUpdateBegin()
		{
			_pRigidBodyManager->FixedUpdateBegin();
		}

		void Physics::FixedUpdate()
		{
			_pRigidBodyManager->FixedUpdate();
		}

		void Physics::FixedUpdateEnd()
		{
			_pRigidBodyManager->FixedUpdateEnd();
		}

		RigidBody* Physics::AddRigidBody(Common::ITransform* pTransform)
		{ 
			if (!_pRigidBodyManager)
				return nullptr;
			return _pRigidBodyManager->AddRigidBody(pTransform);
		}

		/////////////////////////////////RigidBody/////////////////////////////////////
		RigidBody::RigidBody(Common::ITransform* pTransform) :
			_pTransform(pTransform), _mode(Common::CollisionDetectionMode::Discrete),
			_currentVelocity(Math::Vector3::Zero), _angularVelocity(Math::Vector3::Zero),
			_mass(1.0f), _useGravity(true), _forceWorkingOn(Math::Vector3::Zero),
			_lastVelocity(Math::Vector3::Zero)
		{
			_currentPos = pTransform->GetPos();
			_lastPos = _currentPos;
			_spin = _pTransform->GetRotation();
		}

		RigidBody::~RigidBody()
		{
		
		}

		void RigidBody::AddForce(Math::Vector3& force, Common::ForceMode mode)
		{
			switch (mode)
			{
			case Common::ForceMode::Force:
				_forceWorkingOn += force;
				break;
			case Common::ForceMode::Momentum:
				_currentVelocity += force * (1.0f / _mass);
				break;
			case Common::ForceMode::Velocity:
				_currentVelocity += force;
				break;
			}
			
		}

		/*
		 * We are using the Euler Integration at here:
		 * Be careful that Euler Integration is only 100% accurate
		 * if the rate of change is constant over the timestep.
		 * Since we get the _forceWorkingOn during each FixedUpdate, 
		 * we can assume the acceleration is not changing.
		 * If the Acceleration is changing, 
		 * we need to use other integration like "Runge Kutta order 4".
		 * For more: http://gafferongames.com/game-physics/integration-basics/
		 */
		void RigidBody::Advance(float timeStep)
		{
			// Update the previous state
			_lastPos = _currentPos;
			_lastVelocity = _currentVelocity;
			Math::Vector3 acceleration = _forceWorkingOn * (1.0f / _mass);
			_currentPos += _currentVelocity * timeStep + acceleration * (0.5f * timeStep * timeStep);
			_currentVelocity += acceleration * timeStep;
			// before exit, we need to clean the _forceWorkingOn because the force is an instant value.
			_forceWorkingOn = Math::Vector3::Zero;
		}

		/*
		 * For each Update, 
		 * beacuse there will be some little gap left for the _accumulatTime,
		 * and we won't deal with it until the next frame.
		 * So we need to Blend the left time gap for the Transform,
		 * in that case the 
		 */
		void RigidBody::BlendForTimeGap(float blendAlpha)
		{
			_currentVelocity = _lastVelocity * (1.0f - blendAlpha) + _currentVelocity * blendAlpha;
			_currentPos = _lastPos * (1.0f - blendAlpha) + _currentPos * blendAlpha;
			_pTransform->SetPos(_currentPos);
		}

		////////////////////////////////////////RigidBodyManager////////////////////////////////////////

		RigidBodyManager::~RigidBodyManager() 
		{
			for (std::vector<RigidBody*>::iterator it = _rigidBodys.begin(); it != _rigidBodys.end();)
			{
				RigidBody* pRB = *it++;
				SAFE_DELETE(pRB);
			}
			_rigidBodys.clear();
		}

		RigidBody* RigidBodyManager::AddRigidBody(Common::ITransform* pTransform)
		{
			RigidBody* pRigidBody = new RigidBody(pTransform);
			_rigidBodys.push_back(pRigidBody);
			return pRigidBody;
		}

		RigidBody* RigidBodyManager::GetRigidBody(Common::ITransform* pTransform)
		{
			for (std::vector<RigidBody*>::iterator it = _rigidBodys.begin(); it != _rigidBodys.end(); ++it)
			{
				if ((*it)->GetTransform() == pTransform)
				{
					return *it;
				}
			}
			return nullptr;
		}

		void RigidBodyManager::FixedUpdateBegin()
		{
			for (std::vector<RigidBody*>::iterator it = _rigidBodys.begin(); it != _rigidBodys.end(); ++it)
			{
				RigidBody* pRB = (*it);
				pRB->SetPos(pRB->GetTransform()->GetPos());
			}
		}

		void RigidBodyManager::FixedUpdate()
		{
			float fixedTimeStep = Time::GetFixedTimeStep();
			for (std::vector<RigidBody*>::iterator it = _rigidBodys.begin(); it != _rigidBodys.end(); ++it)
			{
				(*it)->Advance(fixedTimeStep);
			}
		}

		/*
		 * Update the Transofrm so that the Rendering system will get the new reference
		 */
		void RigidBodyManager::FixedUpdateEnd()
		{
			float timeBlendAlpha = Time::GetFixedUpdateBlendAlphaOnThisFrame();
			for (std::vector<RigidBody*>::iterator it = _rigidBodys.begin(); it != _rigidBodys.end(); ++it)
			{
				(*it)->BlendForTimeGap(timeBlendAlpha);
			}
		}

	}
}

