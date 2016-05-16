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
	bool _resetLevelController = false;
}


class FlyCameraController : public EAE_Engine::Controller::Controller
{
public:
	FlyCameraController(EAE_Engine::Common::ICamera* pCamera) :
		EAE_Engine::Controller::Controller(pCamera->GetTransform()),
		_cdRemain(0.0f), c_ShootingCD(0.3f)
	{
	}

	void ResetCamera(EAE_Engine::Common::ITransform* pTarget)
	{
		_pTransform->SetLocalPos(EAE_Engine::Math::Vector3::Forward * 10.0f + EAE_Engine::Math::Vector3::Up * 5.0f);
	}

	void Update()
	{
		// Set rotation
		EAE_Engine::Math::Quaternion rotationOffset = RotateAroundY();
		_pTransform->Rotate(rotationOffset);
		// set position
		EAE_Engine::Math::Vector3 offset = GetInput();
		_pTransform->Move(offset);
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

	EAE_Engine::Math::Quaternion RotateAroundY()
	{
		float rotatAngle = 0.0f;
		if (EAE_Engine::UserInput::IsKeyPressed('A'))
		{
			rotatAngle = 1.0f;
		}
		if (EAE_Engine::UserInput::IsKeyPressed('D'))
		{
			rotatAngle = -1.0f;
		}
		const float unitsPerSecond = 1.0f;
		const float unitsToRotate = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();
		EAE_Engine::Math::Quaternion quat(rotatAngle * unitsToRotate, EAE_Engine::Math::Vector3::Up);
		return quat;
	}

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
		const float unitsPerSecond = 10.0f;	// This is arbitrary
											// This makes the speed frame-rate-independent
		const float unitsToMove = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();	// Normalize the offset																	
		offset *= unitsToMove;
		return offset;
	}

private:
	float _cdRemain;
	const float c_ShootingCD;
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
			pRB->AddForce(offset * 2.0f, EAE_Engine::Common::ForceMode::Velocity);
		}
		EAE_Engine::Math::Vector3 velocity = pRB->GetVelocity();
		if (offset.Magnitude() < 0.01f) 
		{
			velocity._x = 0.0f;
			velocity._z = 0.0f;
			pRB->SetVelocity(velocity);
		}
		EAE_Engine::Math::Vector3 velocityXZ(velocity._x, 0.0f, velocity._z);
		if (velocityXZ.Magnitude() > 20.0f)
		{
			velocityXZ = velocityXZ.GetNormalize() * 20.0f;
			velocityXZ._y = velocity._y;
			pRB->SetVelocity(velocityXZ);
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
			forward._y = 0.0f;
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
		const float unitsToMove = unitsPerSecond * EAE_Engine::Time::GetFixedTimeStep();	// Normalize the offset																	
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

