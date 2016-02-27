#ifndef EAE_GAME_CONTROLLER_H
#define EAE_GAME_CONTROLLER_H
#include "Engine/Controller/Controller.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/ColMatrix.h"
#include "Engine/Time/Time.h"
#include "Engine/UserInput/UserInput.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Graphics/Common/Camera.h"
#include "Engine/Core/Entirety/World.h"
#include "Engine/CollisionDetection/RigidBody.h"
#include <cmath>
#include <cassert>

namespace 
{
	enum ActorType 
	{
		Player,
		Enemy,
		Pbullet,
		Ebullet,
	};

	bool _resetLevelController = false;
}


class CameraController : public EAE_Engine::Controller::Controller
{
public:
	CameraController(EAE_Engine::Common::ICamera* pCamera) :
		EAE_Engine::Controller::Controller(pCamera->GetTransform()), _pTargetTrans(nullptr), _pCamera(pCamera)
	{
		_distanceFromTarget = 5.0f;
	}
	virtual ~CameraController() 
	{
		_pTargetTrans = nullptr;
	}
	void SetTarget(EAE_Engine::Common::ITransform* pTargetTrans) 
	{
		_pTargetTrans = pTargetTrans;
		_lastPos = _pTargetTrans->GetPos();
		// Set the position of the Camera
		EAE_Engine::Math::ColMatrix44 viewMat = _pCamera->GetWroldToViewMatrix();
		// Remember that we are working on the ColMatrix and Right hand coordinate.
		// So the 3rd axis of the viewMatrix is pointing to the backward
		EAE_Engine::Math::Vector3 negalookat = viewMat.GetCol(2);
		EAE_Engine::Math::Vector3 pos = _lastPos + negalookat* _distanceFromTarget;
		_pTransform->SetPos(pos);
	}
	void Update()
	{
		if (_pTargetTrans == nullptr || _pTransform == nullptr)
		{
			return;
		}
		if (!EAE_Engine::Graphics::CameraManager::Valid())
			return;
		EAE_Engine::Math::Vector3 newPos = _pTargetTrans->GetPos();
		EAE_Engine::Math::Vector3 offset = newPos - _lastPos;
		_pTransform->SetPos(_pTransform->GetPos() + offset);
		_lastPos = newPos;
		{
			EAE_Engine::Math::Vector3 viewportPos = _pCamera->ConvertWorldToViewport(newPos);
			EAE_Engine::Math::Vector3 worldPos = _pCamera->ConvertViewportToWorld(viewportPos);
			assert((worldPos - newPos).Magnitude() < 0.1f);
		}
		// Set rotation
		//EAE_Engine::Math::Quaternion rotationOffset = RotateAroundY();
		//_pTransform->Rotate(rotationOffset);
	}
/*
	EAE_Engine::Math::Quaternion RotateAroundY()
	{
		float rotatAngle = 0.0f;
		if (EAE_Engine::UserInput::IsKeyPressed('A'))
		{
			rotatAngle -= 1.0f;
		}
		if (EAE_Engine::UserInput::IsKeyPressed('D'))
		{
			rotatAngle += 1.0f;
		}
		const float unitsPerSecond = 1.0f;
		const float unitsToRotate = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();
		EAE_Engine::Math::Quaternion quat(rotatAngle * unitsToRotate, EAE_Engine::Math::Vector3::Up);
		return quat;
	}
*/
private:
	EAE_Engine::Common::ICamera* _pCamera;
	EAE_Engine::Common::ITransform* _pTargetTrans;
	EAE_Engine::Math::Vector3 _lastPos;
	float _distanceFromTarget;
};

class PlayerController : public EAE_Engine::Controller::Controller
{
public:
	PlayerController(EAE_Engine::Common::ITransform* pTrans) :
		EAE_Engine::Controller::Controller(pTrans), 
		_cdRemain(0.0f), c_ShootingCD(0.3f)
	{}

	void Update()
	{
		// Set rotation
		EAE_Engine::Math::Quaternion rotationOffset = RotateAroundY();
		_pTransform->Rotate(rotationOffset);
		// set position
//		EAE_Engine::Math::Vector3 offset = GetInput();
//		_pTransform->Move(offset);


		float elpasedTime = EAE_Engine::Time::GetSecondsElapsedThisFrame();
		_cdRemain -= elpasedTime;
		if (_cdRemain > 0.0001f)
			return;
		if (EAE_Engine::UserInput::IsKeyPressed(VK_F1))
		{
			_resetLevelController = true;
			_cdRemain = 0.5f;
		}

	}

	void FixedUpdate()
	{
		EAE_Engine::Physics::RigidBody* pRB = (EAE_Engine::Physics::RigidBody*)_pTransform->GetComponent(getTypeID<EAE_Engine::Physics::RigidBody>());
		EAE_Engine::Math::Vector3 offset = GetInput();
		{
			pRB->AddForce(offset * 0.1f, EAE_Engine::Common::ForceMode::Velocity);
		}
		if (offset.Magnitude() < 0.01f) 
		{
			EAE_Engine::Math::Vector3 velocity = pRB->GetVelocity();
			velocity._x = 0.0f;
			velocity._z = 0.0f;
			pRB->SetVelocity(velocity);
		}
	}

private:
	// This is the same example using the cVector class from the provided Math library instead:
	EAE_Engine::Math::Vector3 GetInput()
	{
		EAE_Engine::Math::Vector3 offset = EAE_Engine::Math::Vector3::Zero;
		// Get the direction
		{
			EAE_Engine::Math::Vector3 forward = _pTransform->GetForward();
			if (EAE_Engine::UserInput::IsKeyPressed('W'))
			{
				offset = forward;
			}
			if (EAE_Engine::UserInput::IsKeyPressed('S'))
			{
				offset = forward * -1.0f;
			}
			if (EAE_Engine::UserInput::IsKeyPressed('Q'))
			{
				offset._y -= 1.0f;
			}
			if (EAE_Engine::UserInput::IsKeyPressed('E'))
			{
				offset._y += 1.0f;
			}
		}
		// Get the speed
		const float unitsPerSecond = 50.0f;	// This is arbitrary
											// This makes the speed frame-rate-independent
		const float unitsToMove = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();	// Normalize the offset																	
		offset *= unitsToMove;
		return offset;
	}

	EAE_Engine::Math::Quaternion RotateAroundY()
	{
		float rotatAngle = 0.0f;
		if (EAE_Engine::UserInput::IsKeyPressed('A'))
		{
			rotatAngle -= 1.0f;
		}
		if (EAE_Engine::UserInput::IsKeyPressed('D'))
		{
			rotatAngle += 1.0f;
		}
		const float unitsPerSecond = 1.0f;
		const float unitsToRotate = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();
		EAE_Engine::Math::Quaternion quat(rotatAngle * unitsToRotate, EAE_Engine::Math::Vector3::Up);
		return quat;
	}

private:
	float _cdRemain;
	const float c_ShootingCD;
};


#endif

