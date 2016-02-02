
namespace EAE_Engine
{
	namespace Collider
	{

		inline bool Collider::IsSameType(const HashedString& i_type)
		{
			if (_hashtype == i_type)
				return true;
			return false;
		}

		inline void Collider::RegistOnCollideCallback(bool OnCollideCallback(Collider* pSelf, CollisionInfo& collisionInfo))
		{
			if (!_OnCollidecallbackList.Contains(OnCollideCallback) && OnCollideCallback)
			{
				_OnCollidecallbackList.Push(OnCollideCallback);
			}
		}

		inline void Collider::AdvanceCollider(float fElpasedTime)
		{
			if (_pTransform != nullptr)
			{
				//Now we don't consider the accerelation. Just for simple.
				Math::Vector3 newPos = _pTransform->GetPos() + _pTransform->GetVelocity() * fElpasedTime;
				_pTransform->SetPos(newPos);
			}
		}

	}
}
