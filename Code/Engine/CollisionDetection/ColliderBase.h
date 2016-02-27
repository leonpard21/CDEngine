#ifndef EAEENGINE_COLLIDER_H
#define EAEENGINE_COLLIDER_H
#include "Engine/Common/Interfaces.h"
#include "Engine/General/HashString/HashedString.h"
#include "Engine/Containers/LinkedList.h"
#include "Engine/Math/Vector.h"
#include "Engine/Time/Time.h"
#include <vector>

namespace EAE_Engine
{
	namespace Physics
	{
		class RigidBody;
	}

	namespace Collider
	{

		class Collider;
		class ICollisionCallback;

		struct CollisionInfo
		{
			Collider* _pColliderA;   // self
			Collider* _pColliderB;   // other
			float _firstCollisionTime;
			Math::Vector3 _collisionAxle;
			CollisionInfo() = default;
			inline Collider* GetOther(Collider* pSelf) 
			{
				return pSelf == _pColliderA ? _pColliderA : _pColliderB;
			}
		};
		typedef bool OnCollideCallback(Collider* pSelf, CollisionInfo& collisionInfo);

		class Collider : public Common::ICompo
		{
			Collider(const Collider& i_Collider) = delete;
			Collider& operator= (const Collider& i_Collider) = delete;

		public:
			Collider();
			virtual ~Collider();
			virtual Common::ICompo* GetComponent(typeid_t type);
			Common::ITransform* GetTransform() { return _pTransform; }
			void SetType(const HashedString& i_type) { _hashtype = i_type; }

			virtual bool TestCollision(Common::IRigidBody* pTargetRB, float i_follisionTimeStep, float& o_firstCollisionTime,
				Math::Vector3& o_collisionPoint, Math::Vector3& o_collisionNormal) = 0;
			//only when the return is true, the collision happens, the o_collisionTime and o_collisionAxis have their real means.
			virtual bool DetectCollision(Collider* i_pOther, float fElpasedTime, float& o_collisionTime, Math::Vector3& o_collisionAxis) = 0;
			inline bool IsSameType(const HashedString& i_type);
			inline void AdvanceCollider(float fElpasedTime);
			inline void RegistOnCollideCallback(bool OnCollideCallback(Collider* pSelf, CollisionInfo& collisionInfo));
			virtual void IterateCallbackLsit(CollisionInfo collisionInfo);
			inline bool IsTrigger(){ return _isTrigger; }
		protected:
			HashedString _hashtype;
			Container::LinkedList< OnCollideCallback*> _OnCollidecallbackList;
			Common::ITransform* _pTransform;
			//When the Collider is a Trigger, that means we don't care the collision time or some other info.
			//So sometimes we can use faster algorithms on the Collider.
			bool _isTrigger;
		};

		/*The Manager of Colliders*/
		class ColliderManager
		{
			void Init()
			{
				_colliderList.reserve(_numOfColliders);
			}
			ColliderManager();
		public:
			~ColliderManager();
			void Clean();
			Collider* AddToColliderList(Collider* pCollider);
			//Collider* CreateCollider(Common::ITransform* pTrans, const Math::Vector3& size, const Math::Vector3& offset);
			//void AdvanceAllObjs(float fTargetTime);
			void Update();
			void Remove(Collider* pCollider);
			void Remove(Common::ITransform* pTrans);
			void InstallCollsionFeedbackByType(HashedString type, bool OnCollideCallback(Collider* pSelf, CollisionInfo& collisionInfo));
			std::vector<Collider*>& GetColliderList() { return _colliderList; }

		private:
			bool GetFirstCollisionInfo(std::vector<Collider*>* pColliderList, float fElpasedTime, CollisionInfo& o_collisInfo);
			float AdvanceToFirstCollisionTime(std::vector<Collider*>* pColliderList, float fAdvanceTime);
			void DealWithAWhenCollideB(CollisionInfo collisionInfo);
			float IterateAdvanceColliders(std::vector<Collider*>* pColliderList, float fElpasedTime);

		private:
			std::vector<Collider*> _colliderList;
			size_t _numOfColliders;
			static ColliderManager* s_pInternalInstance;
		public:
			static ColliderManager* GetInstance();
			static void CleanInstance();
		};

		Collider* CreateOBBCollider(Common::ITransform* pTrans, const Math::Vector3& size, const Math::Vector3& offset = Math::Vector3::Zero);
			

		inline void RemoveCollider(Common::ITransform* pTrans)
		{
			ColliderManager::GetInstance()->Remove(pTrans);
		}
	}
}

#include "ColliderBase.inl"

#endif//EAEENGINE_COLLIDER_H