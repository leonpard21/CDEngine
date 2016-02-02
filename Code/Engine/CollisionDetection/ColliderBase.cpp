#include "ColliderBase.h"
#include <cmath>
#include <cassert>
#include "OBBCollider.h"


namespace EAE_Engine
{
	namespace Collider
	{
		Collider::Collider(): _pTransform(nullptr), _isTrigger(false)
		{
			_hashtype = HashedString("Collider");
		}

		Collider::~Collider()
		{
			_OnCollidecallbackList.Clear();
 		}

		void Collider::IterateCallbackLsit(CollisionInfo collisionInfo)
		{
			Container::LinkedElement<OnCollideCallback*>* pCurrent = _OnCollidecallbackList._pHead;
			for (; pCurrent != NULL;)
			{
				Container::LinkedElement<OnCollideCallback*>* pTemp = pCurrent;
				pCurrent = pCurrent->_pNext;
				OnCollideCallback* pCallbackFunc = pTemp->GetValue();
				if (pCallbackFunc)
				{
					pCallbackFunc(this, collisionInfo);
				}
			}
		}

		//////////////////////////////////
		/*The Manager of Colliders*/
		ColliderManager::ColliderManager():
			_numOfColliders(100)
		{}

		ColliderManager::~ColliderManager()
		{}


		Collider* ColliderManager::AddToColliderList(Collider* pCollider)
		{
			if (!pCollider)
			{
				return nullptr;
			}
			_colliderList.push_back(pCollider);
			return pCollider;
		}


		float ColliderManager::Update(float fElpasedTime)
		{
			//Iterate advance all of the Colliders.
			float remainTime = IterateAdvanceColliders(&_colliderList, fElpasedTime);
			return remainTime;
		}

		float ColliderManager::IterateAdvanceColliders(std::vector<Collider*>* pTempColliderList, float fElpasedTime)
		{
			assert(fElpasedTime > 0.0f);
			size_t iterate_time = 100;
			while (fElpasedTime > 0.0f && --iterate_time != 0)
			{
				//Get the first Collision time.
				CollisionInfo firstCollisionInfo;
				bool collided = GetFirstCollisionInfo(pTempColliderList, fElpasedTime, firstCollisionInfo);
				if (!collided)
				{
					// if there is no collision anymore, we should move to the next part after collision detection.
					break;
				}
				// if there will/were some collision: 
				float firstCollisionTime = firstCollisionInfo._firstCollisionTime;
				// if the collision time happens in the past, then the 
				if (firstCollisionTime < 0.0f)
				{
					// Should use minimumTranslationVector to remove them.
					// Now I don't care the penetration at all.
					break;
				}
				else
				if (firstCollisionTime >= 0.0f && firstCollisionTime <= 1.0f)// if the first collision happens in this frame.
				{
					float advanceTime = firstCollisionTime * fElpasedTime;
					AdvanceToFirstCollisionTime(pTempColliderList, advanceTime);
					DealWithAWhenCollideB(firstCollisionInfo);
					float newfElpasedTime = fElpasedTime - advanceTime;
					fElpasedTime = newfElpasedTime;
				}
				else if (firstCollisionTime > 1.0f)//if the first collision happens in the future.
				{
					// Since the collision happens in the future, we can move the the process after collision detection.
					break;
				}
			}
			if (fElpasedTime > 0.0f)
				AdvanceToFirstCollisionTime(pTempColliderList, fElpasedTime);
			return fElpasedTime;
		}

		bool ColliderManager::GetFirstCollisionInfo(std::vector<Collider*>* pColliderList, float fElpasedTime, CollisionInfo& o_collisInfo)
		{
			if (!pColliderList)
			{
				return false;
			}
			o_collisInfo = { nullptr, nullptr, FLT_MAX, Math::Vector3::Zero };
			bool collision_result = false;
			for (std::vector<Collider*>::iterator itOut = pColliderList->begin(); itOut != pColliderList->end(); ++itOut)
			{
				for (std::vector<Collider*>::iterator it = itOut; it != pColliderList->end(); ++it)
				{
					if (*it == *itOut)
					{
						continue;
					}
					float o_collisionTime = FLT_MAX;
					Math::Vector3 o_collisionAxis = Math::Vector3::Zero;
					bool collided = (*itOut)->DetectCollision(*it, fElpasedTime, o_collisionTime, o_collisionAxis);
					if (collided)
					{
						assert(o_collisionTime >= 0.0f);//When Colliding, the collision should be larger than 0.0f.
						float firstCollisionTime = fminf(o_collisInfo._firstCollisionTime, o_collisionTime);
						o_collisInfo = { *itOut, *it, firstCollisionTime, o_collisionAxis };
						collision_result = true;
					}
					
				}
			}
			//	PROFILE_PRINT_RESULTS();
			return collision_result;
		}

		void ColliderManager::DealWithAWhenCollideB(CollisionInfo collisionInfo)
		{
			//if both A and B are not trigger.
			if (!collisionInfo._pColliderA->IsTrigger() && !collisionInfo._pColliderB->IsTrigger())
			{
				collisionInfo._pColliderA->IterateCallbackLsit(collisionInfo);
				collisionInfo._pColliderB->IterateCallbackLsit(collisionInfo);
			}
			//if both A and B are triggers.
			else if (collisionInfo._pColliderA->IsTrigger() && collisionInfo._pColliderB->IsTrigger())
			{
				//...logic for trigger collision.
			}
		}

		float ColliderManager::AdvanceToFirstCollisionTime(std::vector<Collider*>* pColliderList, float fAdvanceTime)
		{
			if (!pColliderList)
			{
				return 0.0f;
			}
			for (std::vector<Collider*>::iterator it = pColliderList->begin(); it != pColliderList->end(); ++it)
			{
				if (*it)
					(*it)->AdvanceCollider(fAdvanceTime);
			}
			return fAdvanceTime;
		}

		void ColliderManager::Remove(Collider* pCollider)
		{
			for (std::vector<Collider*>::iterator it = _colliderList.begin(); it != _colliderList.end(); ++it)
			{
				if (*it && (*it) == pCollider)
				{
					Collider* pCollider = (*it);
					it = _colliderList.erase(it);
					SAFE_DELETE(pCollider);
					break;
				}
			}
		}

		void ColliderManager::Remove(Common::ITransform* pTrans)
		{
			// Since one Actor may contain multi collider,
			// we need to remove all of the colliders related to this ITransform.
			for (std::vector<Collider*>::iterator it = _colliderList.begin(); it != _colliderList.end(); )
			{
				Collider* pCollider = *it;
				if (pCollider && pCollider->GetTransform() == pTrans)
				{
					it = _colliderList.erase(it++);
					SAFE_DELETE(pCollider);
				}
				else 
				{
					++it;
				}
			}
		}

		void ColliderManager::InstallCollsionFeedbackByType(HashedString type, bool OnCollideCallback(Collider* pSelf, CollisionInfo& collisionInfo))
		{
			 for (std::vector<Collider*>::iterator it = _colliderList.begin(); it != _colliderList.end(); ++it)
			{
				if (*it && (*it)->IsSameType(type))
				{
					(*it)->RegistOnCollideCallback(OnCollideCallback);
				}
			}
		}

		////////////////////////////////static_members/////////////////////////////////
		ColliderManager* ColliderManager::s_pInternalInstance = nullptr;

		ColliderManager* ColliderManager::GetInstance()
		{
			if (!s_pInternalInstance)
			{
				s_pInternalInstance = new ColliderManager();
				s_pInternalInstance->Init();
			}
			return s_pInternalInstance;
		}

		void ColliderManager::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}
		
		Collider* CreateOBBCollider(Common::ITransform* pTrans, const Math::Vector3& size, const Math::Vector3& offset)
		{
			OBBCollider* pObbCollider = new OBBCollider();
			pObbCollider->InitOBBCollider(pTrans, size, offset);
			ColliderManager::GetInstance()->AddToColliderList(pObbCollider);
			return pObbCollider;
		}


		///////////////////////////////////////////////////Global Functions////////////////////////////////////////////////////////////////
		/*
		bool ChangeReflection(CollisionInfo collisionInfo)
		{
			Collider* pSelfCollider = collisionInfo._pColliderA;
			Collider* pOtherCollider = collisionInfo._pColliderB;
			if (!pSelfCollider || !pOtherCollider)
			{
				return false;
			}
			if (!pSelfCollider->GetTransform() || !pOtherCollider->GetTransform())
			{
				return false;
			}
			//Read the http://chrishecker.com/images/e/e7/Gdmphys3.pdf.
			Math::Vector3 velocityA = pSelfCollider->GetTransform()->GetVelocity();
			Math::Vector3 velocityB = pOtherCollider->GetTransform()->GetVelocity();
			Math::Vector3 velocityAB = velocityA - velocityB;
			float mA = 1.0f;
			float mB = 1.0f;;
			float OneOvermA = 1.0f / mA;//should deal with the case when mA = 0.0f or Infinty.
			float OneOvermB = 1.0f / mB;
			float e = 1.0f;

			Math::Vector3 normalOfCollisionAxle = collisionInfo._collisionAxle.NormalXY();
			normalOfCollisionAxle = normalOfCollisionAxle.Normalize();
			bool selfIsBall = pSelfCollider->GetTransform()->CheckTag(HashedString("Ball"));
			if (selfIsBall)
			{
				Math::Vector3 reflectA = velocityA * -1.0f - normalOfCollisionAxle * 2.0f * (velocityA * -1.0f).Dot(normalOfCollisionAxle);
				pSelfCollider->GetGameObject()->SetVelocity(reflectA);
			}
			bool otherIsBall = pOtherCollider->GetGameObject()->CheckTag(StrSystem::HashedString("Ball"));
			if (otherIsBall)
			{
				Math::Vector3 reflectB = velocityB * -1.0f - normalOfCollisionAxle * 2.0f * (velocityB * -1.0f).Dot(normalOfCollisionAxle);
				pOtherCollider->GetGameObject()->SetVelocity(reflectB);
			}

		//	pSelfCollider->Contacting(true);
		//	pOtherCollider->Contacting(true);
			return true;
		}

		bool OnCollisionMonsterDie(CollisionInfo collisionInfo)
		{
			Collider* pSelfCollider = collisionInfo._pColliderA;
			Collider* pOtherCollider = collisionInfo._pColliderB;
			if (!pSelfCollider || !pOtherCollider)
			{
				return false;
			}
			if (pSelfCollider->GetGameObject().IsNull() || pOtherCollider->GetGameObject().IsNull())
			{
				return false;
			}
			bool selfIsMonster = pSelfCollider->GetGameObject()->CheckTag(StrSystem::HashedString("Monster"));
			if (selfIsMonster)
			{
				pSelfCollider->GetGameObject()->AddTagToMask(StrSystem::HashedString("Died"));
			}
			bool otherIsMonster = pOtherCollider->GetGameObject()->CheckTag(StrSystem::HashedString("Monster"));
			if (otherIsMonster)
			{
				pOtherCollider->GetGameObject()->AddTagToMask(StrSystem::HashedString("Died"));
			}
			return true;
		}
		*/
		/*
		Collider* ColliderManager::CreateCollider(Common::ITransform* pTrans, const Math::Vector3& size, const Math::Vector3& offset)
		{
			OBBCollider* pOBBCollider = new OBBCollider;
			pOBBCollider->InitOBBCollider(pTrans, size, offset);
			if (pOBBCollider->IsSameType(HashedString("Wall")))
			{
			//	pOBBCollider->RegistOnCollideCallback(ChangeReflection);
			}
			if (pOBBCollider->IsSameType(HashedString("Ball")))
			{
			//	pOBBCollider->RegistOnCollideCallback(ChangeReflection);
			//	pOBBCollider->RegistOnCollideCallback(OnCollisionMonsterDie);
			}
			else if (pOBBCollider->IsSameType(HashedString("Monster")))
			{
			//	pOBBCollider->RegistOnCollideCallback(ChangeReflection);
  			//	pOBBCollider->RegistOnCollideCallback(OnCollisionMonsterDie); 
			}
			else if (pOBBCollider->IsSameType(HashedString("Player")))
			{
			//	pOBBCollider->RegistOnCollideCallback(ChangeReflection);
			}
			return pOBBCollider;
		}

		*/
	}
}