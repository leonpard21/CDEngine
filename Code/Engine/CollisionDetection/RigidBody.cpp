#include "RigidBody.h"
#include "Time/Time.h"
#include "General/MemoryOp.h"
#include "ColliderBase.h"


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
			_mass(1.0f), _useGravity(true), _outForceWorkingOn(Math::Vector3::Zero), _totalForceWorkingOn(Math::Vector3::Zero),
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
				_outForceWorkingOn += force;
				break;
			case Common::ForceMode::Momentum:
				_currentVelocity += force * (1.0f / _mass);
				break;
			case Common::ForceMode::Velocity:
				_currentVelocity += force;
				break;
			}
			
		}

		bool RigidBody::DetectionCollision(std::vector<Collider::Collider*>& colliderList, float timeStep, CollisionInfo& o_collisionInfo)
		{
			bool collided = false;
			for (std::vector<Collider::Collider*>::iterator itCollider = colliderList.begin(); itCollider != colliderList.end(); ++itCollider)
			{
				Collider::Collider* pCollider = *itCollider;
				collided = pCollider->TestCollision(this, timeStep, o_collisionInfo._mint, o_collisionInfo._collisionPoint, o_collisionInfo._collisionNormal);
			}
			return collided;
		}

		bool RigidBody::Advance(std::vector<Collider::Collider*>& colliderList, float timeStep, int& io_testDepth)
		{
			if (io_testDepth == 0)
			{
				_totalForceWorkingOn = _outForceWorkingOn;
			}
			const float epsilon = 0.001f;
			bool hasSolutionForCollision = false;
			// Detect Collision with All of the Colliders.
			for (int depth = 0; depth < 3; ++depth)
			{
				CollisionInfo o_collisionInfo;
				if (timeStep < 0.00001f)
					break;
				bool collided = DetectionCollision(colliderList, timeStep, o_collisionInfo);
				if (!collided)
				{
					Response(_totalForceWorkingOn, timeStep);
					hasSolutionForCollision = true;
					break;
				}
				else //else if the collision happens
				{
					float mint = o_collisionInfo._mint;
					Math::Vector3 collisionPoint = o_collisionInfo._collisionPoint;
					Math::Vector3 collisionNormal = o_collisionInfo._collisionNormal;
					// calculate the removement 
					collisionNormal.Normalize();
					float passedTime = timeStep * mint - FLT_EPSILON;
					Response(_totalForceWorkingOn, passedTime);
					_currentPos += collisionNormal * 0.001f;
					timeStep = timeStep - passedTime;
					{
						// first, add an inverse force on the RigidBody
						// based on the surface.normal
						float dotForce = Math::Vector3::Dot(_totalForceWorkingOn, collisionNormal);
						_totalForceWorkingOn = _outForceWorkingOn - collisionNormal * dotForce;
						// second, deal with the velocity based on the collision surface.
						float currentSpeedY = Math::Vector3::Dot(_currentVelocity, collisionNormal);
						Math::Vector3 velocityY = collisionNormal * currentSpeedY;
						Math::Vector3 velocityX = _currentVelocity - velocityY;
						float newSpeed = velocityX.Magnitude();
						// if both of the 2 colliders has rigidBody, 
						// then we need to calculate their momentum.
						// Or the speed of the rigidBody will be zero along the direction of the collision normal.
						_currentVelocity = velocityX;
					}
				}
			}
			return hasSolutionForCollision;
		}

		/*
		 * Here I'm assuming the collision with face keep happening during this fixedUpdate.
		 * So whenever the collision happened, 
		 * I get rid the force has been absorbed by the collider it collided with.
		 */
		Math::Vector3 RigidBody::PredictPosAfter(float timeStep)
		{
			Math::Vector3 acceleration = _totalForceWorkingOn * (1.0f / _mass);
			return _currentPos + _currentVelocity * timeStep + acceleration * (0.5f * timeStep * timeStep);
		}

		/*
		 * We are using the Euler Integration at here:
		 * Be careful that Euler Integration is only 100% accurate
		 * if the rate of change is constant over the timestep.
		 * Since we get the _forceWorkingOn during each FixedUpdate, 
		 * we can assume the acceleration is not changing.
		 * If the Acceleration is changing during the fixed deltaTime, 
		 * we need to use other integration like "Runge Kutta order 4".
		 * For more: http://gafferongames.com/game-physics/integration-basics/
		 */
		void RigidBody::Response(Math::Vector3 force, float timeStep)
		{
			Math::Vector3 acceleration = force * (1.0f / _mass);
			Math::Vector3 movement = _currentVelocity * timeStep + acceleration * (0.5f * timeStep * timeStep);;
			_currentPos = _currentPos + movement;
			_currentVelocity = _currentVelocity + acceleration * timeStep;
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
	//		_currentVelocity = _lastVelocity * (1.0f - blendAlpha) + _currentVelocity * blendAlpha;
	//		_currentPos = _lastPos * (1.0f - blendAlpha) + _currentPos * blendAlpha;
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
			std::vector<Collider::Collider*>& colliderList = Collider::ColliderManager::GetInstance()->GetColliderList();
			for (std::vector<RigidBody*>::iterator itRB = _rigidBodys.begin(); itRB != _rigidBodys.end(); ++itRB)
			{
				RigidBody* pRB = *itRB;
				// Update the previous state
				pRB->_lastPos = pRB->_currentPos;
				pRB->_lastVelocity = pRB->_currentVelocity;
				if (pRB->_useGravity)
					pRB->_outForceWorkingOn = pRB->_outForceWorkingOn + Physics::GetInstance()->GetGravity() * pRB->_mass;
				// Detection the collision 
				int io_testDepth = 0;
				bool hasCollisionSolution = pRB->Advance(colliderList, fixedTimeStep, io_testDepth);
				if (!hasCollisionSolution) 
				{
					pRB->SetPos(pRB->_lastPos);
				}
				// reset the force working on this RigidBody
				pRB->_outForceWorkingOn = Math::Vector3::Zero;
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

