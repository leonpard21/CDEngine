#include "LazyCamera.h"

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
#include "Engine/Core/Entirety/World.h"
#include "Engine/CollisionDetection/RigidBody.h"
#include <cassert>

#include "FlagController.h"
#include "Networking.h"


void RelativeScreenInput::Update()
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

// This is the same example using the cVector class from the provided Math library instead:
EAE_Engine::Math::Vector3 RelativeScreenInput::GetInput(EAE_Engine::Common::ICamera* pCam)
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

int FlagScore::s_score = 0;
int FlagScore::s_otherScore = 0;

void FlagScore::Init(const char* targetFlag)
{
  EAE_Engine::Common::IGameObj* pFlag = pFlag = EAE_Engine::Core::World::GetInstance().GetGameObj(targetFlag);
  if (pFlag)
    _pTargetFlag = pFlag->GetTransform();
}

void FlagScore::SetTarget(FlagScore* pOther)
{
  _pOther = pOther;
}

void FlagScore::Update()
{
  // Reset flag if the 2 players are too close
  if (_pOther)
  {
    EAE_Engine::Math::Vector3 pos = _pTransform->GetPos() - _pOther->GetTransform()->GetPos();
    if (pos.SqMagnitude() < 4.0f)
    {
      EAE_Engine::Common::ICompo* pCompo = EAE_Engine::Core::World::GetInstance().GetGameObj("flag0")->GetComponent(getTypeID<FlagController>());
      FlagController* pController0 = (FlagController*)pCompo;
      pController0->Reset();
      pCompo = EAE_Engine::Core::World::GetInstance().GetGameObj("flag1")->GetComponent(getTypeID<FlagController>());
      FlagController* pController1 = (FlagController*)pCompo;
      pController1->Reset();
    }
  }
  // EAE_Engine::Common::IGameObj* pChild = EAE_Engine::Core::World::GetInstance().GetGameObj();
  for (uint32_t i = 0; i < _pTransform->GetChildCount(); ++i)
  {
    EAE_Engine::Common::ITransform* pChild = _pTransform->GetChild(i);
    const char* pName = pChild->GetGameObj()->GetName();
    if (strcmp(pName, "playerRenderObj") == 0)
      continue;
    EAE_Engine::Common::ICompo* pCompo = pChild->GetComponent(getTypeID<FlagController>());
    FlagController* pController = (FlagController*)pCompo;
    if (!pController)
      continue;
    // Score if the carrying flag is close enough to the scorePoint
    EAE_Engine::Math::Vector3 relativePos = _scorePoint - _pTransform->GetPos();
    if (relativePos.SqMagnitude() < 4.0f)
    {
      pController->Reset();
      if (strcmp(_pTargetFlag->GetGameObj()->GetName(), "flag1") == 0)
        ++s_score;
      else
        ++s_otherScore;
    }
  }   
}

void LazyCamera::ResetCamera(EAE_Engine::Common::ITransform* pTarget)
{
  _pTarget = pTarget;
  Reset();
}

void LazyCamera::Update()
{
  if (!_pTarget)
    return;
  bool collided = HandleCollision();
  UpdateInputRotation();
  if (!collided)
  {
    _lastVisiblePos = GetAimingPoint();
  }
  UpdatePosition();
  UpdateOrientation();

  return;
}

void LazyCamera::UpdateInputRotation()
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

void LazyCamera::UpdateOrientation()
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
    }
    //_pTransform->LookAt(GetAimingPoint());
  }
}

void LazyCamera::UpdatePosition()
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

void LazyCamera::Reset()
{
  if (!_pTarget)
    return;
  _lastVisiblePos = GetAimingPoint();
  _pTransform->SetLocalPos(GetAimingPoint() + _pTarget->GetForward() * -10.0f + EAE_Engine::Math::Vector3::Up * 2.0f);
}

float LazyCamera::RotateAroundY()
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
}

float LazyCamera::RotateAroundX()
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

EAE_Engine::Math::Vector3 LazyCamera::GetAimingPoint()
{
  return _pTarget->GetPos() + EAE_Engine::Math::Vector3(0.0f, 2.0f, 0.0f);
}

bool LazyCamera::HandleCollision()
{
  EAE_Engine::Math::Vector3 playerPos = GetAimingPoint();
  EAE_Engine::Math::Vector3 cameraPos = _pTransform->GetPos();
  std::vector<EAE_Engine::Mesh::TriangleIndex> triangles;
  EAE_Engine::Math::Vector3 collisionNormal = EAE_Engine::Math::Vector3::Zero;
  EAE_Engine::Physics::Physics::GetInstance()->RayCast(playerPos, cameraPos, triangles, collisionNormal);
  if (triangles.size() == 0)
    return false;
  {
    collisionNormal._y = 0.0f;
    collisionNormal.Normalize();
  }
  EAE_Engine::Math::Vector3 relativeXZ = (cameraPos - playerPos);
  relativeXZ._y = 0.0f;

  float dot = EAE_Engine::Math::Vector3::Dot(relativeXZ.GetNormalize(), collisionNormal);
  int sign = EAE_Engine::Math::GetSign<float>(dot); 
  EAE_Engine::Math::Vector3 proj = EAE_Engine::Math::Vector3::Project(relativeXZ, collisionNormal);
  if (sign > 0)
  {
    proj = EAE_Engine::Math::Vector3::Project(relativeXZ, collisionNormal * -1.0f);
  }
  /*
  proj = relativeXZ + proj * 1.20f;
  EAE_Engine::Math::Vector3 targetPos = playerPos + proj.GetNormalize() * relativeXZ.Magnitude();
  */
  EAE_Engine::Math::Vector3 targetPos = playerPos + (_lastVisiblePos - GetAimingPoint()) + proj.GetNormalize() * 0.2f;
  float deltaTime = EAE_Engine::Time::GetSecondsElapsedThisFrame();
  EAE_Engine::Math::Vector3 newPos = EAE_Engine::Math::Vector3::Lerp(cameraPos, targetPos, deltaTime * 1.0f);
  if ((newPos - targetPos).SqMagnitude() < 0.0016f)
    newPos = targetPos;
  _pTransform->SetPos(newPos);
  return true;
}
