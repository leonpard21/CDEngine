#include "Transform.h"
#include "../Individual/GameObj.h"
#include "Math/EulerAngle.h"
#include "Math/MathTool.h"
#include <cassert>

namespace EAE_Engine
{
  namespace Core
  {
    Transform::Transform(Common::IGameObj* pGamObj) : _pGamObj(pGamObj), _pParent(nullptr), 
      _localPosition(Math::Vector3::Zero), _localRotation(Math::Quaternion::Identity), 
      _localScale(Math::Vector3(1.0f, 1.0f, 1.0f))
    {
    }

    Transform::~Transform()
    {
    }

    Common::IGameObj* Transform::GetGameObj()
    {
      return _pGamObj;
    }
    Common::ICompo* Transform::GetComponent(typeid_t type)
    {
      return _pGamObj ? _pGamObj->GetComponent(type) : nullptr;
    }

    void Transform::RemoveChild(ITransform* pParent)
    {
      for (std::vector<Common::ITransform*>::iterator it = _children.begin(); it != _children.end(); ++it)
      {
        if (*it == pParent)
        {
          _children.erase(it);
          break;
        }
      }
    }

    // Parent
    void Transform::SetParent(ITransform* parent)
    {
      if (parent == this) {
        _pParent = nullptr;
        return;
      }
      if (_pParent && parent == nullptr)
      {
        _localPosition = GetPos();
        _localRotation = GetRotation();
        _pParent->RemoveChild(this);
        _pParent = nullptr;
        return;
      }
      _pParent = parent;
      if (_pParent)
      {
        parent->AddChild(this);
      }
    }
    Common::ITransform* Transform::Root() 
    {
      ITransform* pRoot = this;
      while (pRoot->GetParent())
      {
        pRoot = pRoot->GetParent();
      }
      return pRoot;
    }

    // global transofrm
    Math::Vector3 Transform::GetPos() const
    { 
      if (_pParent == nullptr)
        return _localPosition;
      else
      {
        Math::Vector4 localPos(_localPosition._x, _localPosition._y, _localPosition._z, 1.0f);
        Math::Vector4 result = _pParent->GetLocalToWorldMatrix() * localPos;
        return result;
      }
    }

    void Transform::SetPos(const Math::Vector3& pos) 
    {
      if (_pParent == nullptr)
        _localPosition = pos;
      else
        _localPosition = pos - _pParent->GetPos();
    }

    Math::Quaternion Transform::GetRotation() const
    { 
      if (_pParent == nullptr)
        return _localRotation; 
      else 
      {
        // Always remember that: 
        // the quaternion multiplication should be read from right to left!
        Math::Quaternion result = _localRotation * _pParent->GetRotation();
        return result;
      }
    }

    void Transform::SetRotation(const Math::Quaternion& i_other) 
    { 
      if (_pParent == nullptr)
        _localRotation = i_other;
      else 
      {
        // We need to calculate the difference between the local to the parent, 
        // which is _localQuaternion * _parentQuaternion.inverse()
        Math::Quaternion result = i_other * _pParent->GetRotation().GetInverse();
        _localRotation = result;
      }
    }

    void Transform::SetScale(const Math::Vector3& scale) 
    {
      if (scale.Magnitude() < FLT_EPSILON) 
      {
        _localScale = Math::Vector3::Zero;
        return;
      }
      if (_pParent == nullptr)
        _localScale = scale;
      else 
      {
        Math::Vector3 parentScale = _pParent->GetScale();
        if (parentScale.Magnitude() < FLT_EPSILON)
        {
          _localScale = scale;
          return;
        }
        _localScale = Math::Vector3(scale.x() / parentScale.x(), scale.y() / parentScale.y(), scale.z() / parentScale.z());
      }
    }
    Math::Vector3 Transform::GetScale() const
    {
      if (_pParent == nullptr)
        return _localScale;
      else
      {
        Math::Vector3 parentScale = _pParent->GetScale();
        Math::Vector3 result(parentScale.x()* _localScale.x(), parentScale.y()* _localScale.y(), parentScale.z()* _localScale.z());
        return result;
      }
    }

    Math::Vector3 Transform::GetEulerAngle() const
    {
      return Math::Quaternion::CreateEulerAngle(GetRotation());
    }

    void Transform::SetEulerAngle(Math::Vector3 eulerAngle)
    {
      Math::Quaternion rotation = Math::EulerAngle::GetQuaternion(eulerAngle);
      SetRotation(rotation);
    }

    // local transform
    Math::Vector3& Transform::GetLocalPos() { return _localPosition; }
    void Transform::SetLocalPos(const Math::Vector3& pos) { _localPosition = pos; }
    Math::Quaternion& Transform::GetLocalRotation() { return _localRotation; }
    void Transform::SetLocalRotation(const Math::Quaternion& i_other) { _localRotation = i_other; }
    void Transform::SetLocalScale(const Math::Vector3& localScale) { _localScale = localScale; }
    Math::Vector3 Transform::LocalScale() { return _localScale; }
    Math::Vector3 Transform::GetLocalEulerAngle() { return Math::Quaternion::CreateEulerAngle(_localRotation); }
    void Transform::SetLocalEulerAngle(const Math::Vector3& eulerAngle)
    {
      Math::Quaternion localrotation = Math::EulerAngle::GetQuaternion(eulerAngle);
      _localRotation = localrotation;
    }
    void Transform::Move(const Math::Vector3& i_movement) { _localPosition = _localPosition + i_movement; }
    void Transform::Rotate(const Math::Quaternion& i_other) { _localRotation = _localRotation * i_other; }
    // Transform Matrices
    Math::ColMatrix44 Transform::GetRotateTransformMatrix() 
    { 
      Math::ColMatrix44 result;
      if (_pParent) 
        result = Math::ColMatrix44(GetRotation(), GetPos());
      else 
        result = Math::ColMatrix44(_localRotation, _localPosition);
      return result;
    }

    Math::ColMatrix44 Transform::GetLocalToWorldMatrix() 
    {
      // When we want to get the transform Matrix of a Transform,
      // we should use the Global Rotaion and Global Position,
      // so we don't need to care the local position and how many parents it has.
      Math::ColMatrix44 rotateTransMat = Math::ColMatrix44(_localRotation, _localPosition);
      Math::ColMatrix44 localTransformMatrix = rotateTransMat * Math::ColMatrix44::CreateScaleMatrix(_localScale);
      if (!_pParent)
        return localTransformMatrix;
      else
        return _pParent->GetLocalToWorldMatrix() * localTransformMatrix;
    }
    Math::Vector3 Transform::GetForward()
    {
      Math::ColMatrix44 rotationMat = GetRotateTransformMatrix();
      // Remember that we are working on the ColumnMatrix and Right hand coordinate.
      // So the 3rd axis of the RotateTransformMatrix is pointing to the backward
      Math::Vector3 forward = rotationMat.GetCol(2) * 1.0f;
      return forward.Normalize();
    }

    void Transform::SetForward(Math::Vector3 forward)
    {
      Math::Vector3 currentForward = GetForward();
      Math::Vector3 currentUp = GetUp();
      // Get the angle from currentForward to the new forward
      float radian = Math::Radian(currentForward, forward);
      Math::Vector3 normalAxis = Math::Vector3::Cross(currentForward, forward).Normalize();
      // Because we're looking from the positive to the negative dirction of the normalAxis, 
      // so we need to rotate the -radian.
      Math::Quaternion rotation(-radian, normalAxis);
      Rotate(rotation);
      currentForward = GetForward();
      assert((currentForward - forward).Magnitude() < 0.0001f);
    }

    Math::Vector3 Transform::GetRight() 
    {
      Math::ColMatrix44 rotationMat = GetRotateTransformMatrix();
      Math::Vector3 right = rotationMat.GetCol(0);
      return right.Normalize();
    }
    Math::Vector3 Transform::GetUp()
    {
      Math::ColMatrix44 rotationMat = GetRotateTransformMatrix();
      Math::Vector3 up = rotationMat.GetCol(1);
      return up.Normalize();
    }

  }
}