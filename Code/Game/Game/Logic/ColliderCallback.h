#ifndef EAE_GAME_COLLIDERCALLBACK_H
#define EAE_GAME_COLLIDERCALLBACK_H

#include "Engine/Math/Vector.h"
#include "Engine/Time/Time.h"
#include "Engine/CollisionDetection/ColliderBase.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/Engine/Engine.h"

namespace 
{
	bool _resetLevel = false;
	bool _createNewEnemy = false;
	bool _createNewTurret = false;
}

bool OnCollideStop(EAE_Engine::Collider::Collider* pSelf, EAE_Engine::Collider::CollisionInfo& i_collisionInfo)
{
	if (pSelf == nullptr)
		return false;
	EAE_Engine::Common::ITransform* pTrans = pSelf->GetTransform();
//	EAE_Engine::Math::Vector3 newVelocity = pTrans->GetVelocity() * 0.0f;//EAE_Engine::Math::Vector3::Zero;
//	pTrans->SetVelocity(newVelocity);
	return true;
}

bool OnCollideRemove(EAE_Engine::Collider::Collider* pSelf, EAE_Engine::Collider::CollisionInfo& i_collisionInfo)
{
	if (pSelf == nullptr)
		return false;
	EAE_Engine::Engine::AddToRemoveList(pSelf->GetTransform());
	return true;
}

bool OnCollideRestart(EAE_Engine::Collider::Collider* pSelf, EAE_Engine::Collider::CollisionInfo& i_collisionInfo)
{
	_resetLevel = true;
	return true;
}

bool OnCollideCreateNewEnemy(EAE_Engine::Collider::Collider* pSelf, EAE_Engine::Collider::CollisionInfo& i_collisionInfo)
{
	_createNewEnemy = true;
	return true;
}
bool OnCollideCreateNewTurret(EAE_Engine::Collider::Collider* pSelf, EAE_Engine::Collider::CollisionInfo& i_collisionInfo)
{
	_createNewTurret = true;
	return true;
}



#endif//EAE_GAME_COLLIDERCALLBACK_H