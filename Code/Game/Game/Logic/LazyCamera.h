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
    _pTransform->Move(movement);
  }

private:
  // This is the same example using the cVector class from the provided Math library instead:
  EAE_Engine::Math::Vector3 GetInput(EAE_Engine::Common::ICamera* pCam)
  {
    EAE_Engine::Math::Vector3 offset = EAE_Engine::Math::Vector3::Zero;
    // Get the direction
    {
      EAE_Engine::Math::Vector3 forward = pCam->GetTransform()->GetForward();
      EAE_Engine::Math::Vector3 right = pCam->GetTransform()->GetRight();
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
        offset = right;
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
    _phi(20.0f)
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
    UpdateOrientation();
    UpdatePosition();
    return;
  }

private:
  void UpdateOrientation() 
  {
    if (!_pTarget)
      return;
    EAE_Engine::Math::Vector3 relativePos = _pTarget->GetPos() - _pTransform->GetPos();
    relativePos._y = 0.0f;
    EAE_Engine::Math::Vector3 relativeForward = relativePos.GetNormalize();
    EAE_Engine::Math::Vector3 targetForward = relativeForward;// _pTarget->GetForward();
    EAE_Engine::Math::Vector3 camForward = _pTransform->GetForward();
    float angle = EAE_Engine::Math::Degree(camForward, targetForward);
    EAE_Engine::Math::Vector3 normal = EAE_Engine::Math::Vector3::Cross(camForward, targetForward).Normalize();
    // get the direction of the angle.
    float direct = EAE_Engine::Math::Vector3::Dot(normal, EAE_Engine::Math::Vector3::Up);
    if (direct < 0)
      angle = -angle;
    // only when the camera is too far away from the Camera's Forward, we rotate the camera.
    if (angle > _phi || angle < -_phi)
    {
      // first, set the camera to look at the target
      _pTransform->SetForward(targetForward);
      // second, rotate back so that the target will keep staying at the edge
      EAE_Engine::Math::Quaternion rotation(_phi * EAE_Engine::Math::DegreeToRadian, normal);
      _pTransform->Rotate(rotation);
    }
  }

  void UpdatePosition() 
  {
    if (!_pTarget)
      return;
    EAE_Engine::Math::Vector3 relativePos = _pTarget->GetPos() - _pTransform->GetPos();
    relativePos._y = 0.0f;
    if (relativePos.Magnitude() > _outter)
    {
      EAE_Engine::Math::Vector3 newPos = _pTarget->GetPos() + relativePos.Normalize() * -_outter;
      newPos = newPos + EAE_Engine::Math::Vector3(0.0f, 2.0f, 0.0f);
      _pTransform->SetPos(newPos);
    }
    else if (relativePos.Magnitude() < _inner)
    {
      EAE_Engine::Math::Vector3 newPos = _pTarget->GetPos() + relativePos.Normalize() * -_inner;
      newPos = newPos + EAE_Engine::Math::Vector3(0.0f, 2.0f, 0.0f);
     // _pTransform->SetPos(newPos);
    }
  }

  void Reset() 
  {
    if (!_pTarget)
      return;
    _pTransform->SetLocalPos(_pTarget->GetPos() + _pTarget->GetForward() * -10.0f + EAE_Engine::Math::Vector3::Up * 1.0f);
  }

private:
  EAE_Engine::Common::ITransform* _pTarget;
  float _phi;
  float _theta;

  float _inner;
  float _outter;
};

#endif// LAZY_CAMERA_H