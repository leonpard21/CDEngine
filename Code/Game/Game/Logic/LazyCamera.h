#ifndef LAZY_CAMERA_H
#define LAZY_CAMERA_H

#include "Engine/Controller/Controller.h"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/Engine/Engine.h"

#include "FlagController.h"
#include "Networking.h"

class RelativeScreenInput : public EAE_Engine::Controller::Controller
{
public:
  RelativeScreenInput(EAE_Engine::Common::ITransform* pTransform) :
    EAE_Engine::Controller::Controller(pTransform)
  {
  }

  void Update();
private:
  // This is the same example using the cVector class from the provided Math library instead:
  EAE_Engine::Math::Vector3 GetInput(EAE_Engine::Common::ICamera* pCam);
};


class FlagScore : public EAE_Engine::Controller::Controller
{
public:
  FlagScore(EAE_Engine::Common::ITransform* pTransform, EAE_Engine::Math::Vector3 initFlagPoint) :
    EAE_Engine::Controller::Controller(pTransform), _scorePoint(initFlagPoint), _pOther(nullptr)
  {
    _pTargetFlag = nullptr;
  }

  virtual typeid_t GetTypeID() const
  {
    return getTypeID<FlagScore>();
  }

  void Init(const char* targetFlag);
  void SetTarget(FlagScore* pOther);
  void Update();

  FlagScore* _pOther;

  static int s_score;
  static int s_otherScore;
private:
  EAE_Engine::Math::Vector3 _scorePoint;
  EAE_Engine::Common::ITransform* _pTargetFlag;
};


class LazyCamera : public EAE_Engine::Controller::Controller
{
public:
  LazyCamera(EAE_Engine::Common::ICamera* pCamera) :
    EAE_Engine::Controller::Controller(pCamera->GetTransform()),
    _phi(20.0f), _theta(20.0f)
  {
    _inner = 6.0f;
    _outter = 16.0f;
  }

  void ResetCamera(EAE_Engine::Common::ITransform* pTarget);
  void Update();

private:
  void UpdateInputRotation();
  void UpdateOrientation();
  void UpdatePosition();
  void Reset();
  float RotateAroundY();
  float RotateAroundX();

  EAE_Engine::Math::Vector3 GetAimingPoint();
  bool HandleCollision();

private:
  EAE_Engine::Common::ITransform* _pTarget;
  EAE_Engine::Math::Vector3 _lastVisiblePos;
  float _phi;
  float _theta;

  float _inner;
  float _outter;
};

#endif// LAZY_CAMERA_H