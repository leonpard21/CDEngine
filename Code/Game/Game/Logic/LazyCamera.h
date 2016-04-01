#ifndef LAZY_CAMERA_H
#define LAZY_CAMERA_H

#include "Engine/Controller/Controller.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/EulerAngle.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/ColMatrix.h"
#include "Engine/Math/MathTool.h"
#include "Engine/Time/Time.h"
#include "Engine/UserInput/UserInput.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Graphics/Common/Camera.h"
#include "Engine/DebugShape/DebugShape.h"
#include <cassert>

class RelativeScreenInput : public EAE_Engine::Controller::Controller
{
public:
  RelativeScreenInput(EAE_Engine::Common::ITransform* pTransform) :
    EAE_Engine::Controller::Controller(pTransform)
  {
  }

  void Update() 
  {
    EAE_Engine::Common::ICamera* pCam = EAE_Engine::Graphics::CameraManager::GetInstance().GetCam();
    if (!pCam)
      return;
    EAE_Engine::Math::Vector3 movement = GetInput(pCam);
    _pTransform->SetForward(movement.GetNormalize());
    EAE_Engine::Physics::RigidBody* pRB = (EAE_Engine::Physics::RigidBody*)_pTransform->GetComponent(getTypeID<EAE_Engine::Physics::RigidBody>());
    {
      pRB->AddForce(movement, EAE_Engine::Common::ForceMode::Velocity);
    }
    EAE_Engine::Math::Vector3 velocity = pRB->GetVelocity();
    if (movement.Magnitude() < 0.01f)
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
  EAE_Engine::Math::Vector3 GetInput(EAE_Engine::Common::ICamera* pCam)
  {
    EAE_Engine::Math::Vector3 offset = EAE_Engine::Math::Vector3::Zero;
    // Get the direction
    {
      EAE_Engine::Math::Vector3 forward = pCam->GetTransform()->GetForward();
      forward._y = 0.0f;
      forward.Normalize();
      EAE_Engine::Math::Vector3 right = pCam->GetTransform()->GetRight();
      right._y = 0;
      right.Normalize();
      if (EAE_Engine::UserInput::IsKeyPressed('W'))
      {
        offset = forward;
      }
      if (EAE_Engine::UserInput::IsKeyPressed('S'))
      {
        offset = forward * -1.0f;
      }
      if (EAE_Engine::UserInput::IsKeyPressed('A'))
      {
        offset = right * -1.0f;
      }
      if (EAE_Engine::UserInput::IsKeyPressed('D'))
      {
        offset = right * 1.0f;
      }
    }
    // Get the speed
    const float unitsPerSecond = 10.0f;	// This is arbitrary
                                        // This makes the speed frame-rate-independent
    const float unitsToMove = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();	// Normalize the offset																	
    offset *= unitsToMove;
    return offset;
  }


};

class LazyCamera : public EAE_Engine::Controller::Controller
{
public:
  LazyCamera(EAE_Engine::Common::ICamera* pCamera) :
    EAE_Engine::Controller::Controller(pCamera->GetTransform()),
    _phi(20.0f), _theta(20.0f)
  {
    _inner = 6.0f;
    _outter = 10.0f;
  }

  void ResetCamera(EAE_Engine::Common::ITransform* pTarget)
  {
    _pTarget = pTarget;
    Reset();
  }

  void Update()
  {
    if (!_pTarget)
      return;
    UpdateInputRotation();
    UpdatePosition();
    UpdateOrientation();

    return;
  }

private:
  void UpdateInputRotation()
  {
    EAE_Engine::Math::Quaternion rotation = EAE_Engine::Math::Quaternion::Identity;
    float rotationOffset = RotateAroundY();
    if(fabsf(rotationOffset) > 0.0f)
      _pTransform->RotateAround(GetAimingPoint(), EAE_Engine::Math::Vector3::Up, rotationOffset);
    float rotationOffset2 = RotateAroundX();
    {
      EAE_Engine::Math::Vector3 movement = EAE_Engine::Math::Vector3(0.0f, rotationOffset2, 0.0f);
      _pTransform->Move(movement);
    }

  }

  void UpdateOrientation() 
  {
    if (!_pTarget)
      return;
    EAE_Engine::Math::Vector3 relativePos = GetAimingPoint() - _pTransform->GetPos();
    {
      EAE_Engine::Math::Vector3 relativeForward = relativePos;
      relativeForward._y = 0;
      relativeForward.Normalize();
      EAE_Engine::Math::Vector3 camForward = _pTransform->GetForward();
      float angle = EAE_Engine::Math::Degree(camForward, relativeForward);
      EAE_Engine::Math::Vector3 normal = EAE_Engine::Math::Vector3::Cross(camForward, relativeForward).Normalize();
      // get the direction of the angle.
      float direct = EAE_Engine::Math::Vector3::Dot(normal, EAE_Engine::Math::Vector3::Up);
      // if (direct < 0)
      //   angle = -angle;
      // only when the camera is too far away from the Camera's Forward, we rotate the camera.
      if (angle > _phi)
      {
        float rotateAngle = angle - _phi;
        if(normal.Magnitude() > 0.0001f)
        {
          EAE_Engine::Math::Quaternion rotation(rotateAngle * EAE_Engine::Math::DegreeToRadian, normal);
          _pTransform->Rotate(rotation);
        }
        //_pTransform->SetForward(relativeForward);
        //_pTransform->LookAt(GetAimingPoint());
      }
      //_pTransform->LookAt(GetAimingPoint());
    }
  }

  void UpdatePosition()
  {
    if (!_pTarget)
      return;
    EAE_Engine::Math::Vector3 movement = EAE_Engine::Math::Vector3::Zero;
    EAE_Engine::Math::Vector3 relativePos = GetAimingPoint() - _pTransform->GetPos();
    if (fabsf(relativePos._y) > 1.0f)
    {
      movement._y = (fabsf(relativePos._y) - 1.0f) * EAE_Engine::Math::GetSign<float>(relativePos._y);
    }

    if (relativePos.Magnitude() > _outter)
    {
      movement = movement + relativePos.Normalize() * (relativePos.Magnitude() - _outter);
    }
    else if (relativePos.Magnitude() < _inner)
    {
      movement = movement + relativePos.Normalize() * (relativePos.Magnitude() - _inner);
    }
    _pTransform->Move(movement);
  }

  void Reset() 
  {
    if (!_pTarget)
      return;
    _pTransform->SetLocalPos(GetAimingPoint() + _pTarget->GetForward() * -10.0f + EAE_Engine::Math::Vector3::Up * 2.0f);
  }

  float RotateAroundY()
  {
    float rotatAngle = 0.0f;
    if (EAE_Engine::UserInput::IsKeyPressed(VK_LEFT))
    {
      rotatAngle -= 1.0f;
    }
    if (EAE_Engine::UserInput::IsKeyPressed(VK_RIGHT))
    {
      rotatAngle += 1.0f;
    }
    const float unitsPerSecond = 1.0f;
    const float unitsToRotate = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();
    return rotatAngle * unitsToRotate;
    // EAE_Engine::Math::Quaternion quat(rotatAngle * unitsToRotate, EAE_Engine::Math::Vector3::Up);
    // return quat;
  }

  float RotateAroundX()
  {
    float rotatAngle = 0.0f;
    if (EAE_Engine::UserInput::IsKeyPressed(VK_UP))
    {
      rotatAngle += 1.0f;
    }
    if (EAE_Engine::UserInput::IsKeyPressed(VK_DOWN))
    {
      rotatAngle -= 1.0f;
    }
    const float unitsPerSecond = 1.0f;
    const float unitsToRotate = unitsPerSecond * EAE_Engine::Time::GetSecondsElapsedThisFrame();
    return rotatAngle * unitsToRotate;
  }

  EAE_Engine::Math::Vector3 GetAimingPoint()
  {
    return _pTarget->GetPos() + EAE_Engine::Math::Vector3(0.0f, 2.0f, 0.0f);
  }

private:
  EAE_Engine::Common::ITransform* _pTarget;
  float _phi;
  float _theta;

  float _inner;
  float _outter;
};

#endif// LAZY_CAMERA_H