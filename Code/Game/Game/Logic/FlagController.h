#ifndef FLAG_CONTROLLER_H
#define FLAG_CONTROLLER_H

#include "Engine/Controller/Controller.h"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"

class FlagController : public EAE_Engine::Controller::Controller
{
public:
  FlagController(EAE_Engine::Common::ITransform* pTransform) :
    EAE_Engine::Controller::Controller(pTransform), _pTarget(nullptr)
  {
    _initPos = pTransform->GetPos();
  }

  void SetTarget(EAE_Engine::Common::ITransform* pTarget) 
  {
    _pTarget = pTarget;
  }

  void Update()
  {
    if (!_pTarget)
      return;
    EAE_Engine::Math::Vector3 relativePos = _pTarget->GetPos() - _pTransform->GetPos();
    float distanceSq = relativePos.SqMagnitude();
    if(distanceSq < 1.0f)
    {
      _pTransform->SetParent(_pTarget->GetTransform());
      _pTransform->SetLocalPos(EAE_Engine::Math::Vector3::Zero);
    }
    


  }

  void Reset() 
  {
    _pTransform->SetParent(nullptr);
    _pTransform->SetPos(_initPos);
  }

private:
  EAE_Engine::Common::ITransform* _pTarget;
  EAE_Engine::Math::Vector3 _initPos;
};

#endif//FLAG_CONTROLLER_H