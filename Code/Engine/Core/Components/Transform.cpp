#include "Transform.h"
#include "../Individual/GameObj.h"
#include <cassert>

namespace EAE_Engine
{
	namespace Core
	{
		Transform::Transform(Common::IGameObj* pGamObj) : _pGamObj(pGamObj), _pParent(nullptr), 
			_localPosition(Math::Vector3::Zero), _localRotation(Math::Quaternion::Identity), 
			_localScale(Math::Vector3(1.0f, 1.0f, 1.0f)), _velocity(Math::Vector3::Zero), 
			_ppChildren(nullptr), _childCount(0)
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
			return nullptr;
		}

		// Parent
		void Transform::SetParent(ITransform* parent)
		{
			if (parent == this) {
				_pParent = nullptr;
				return;
			}
			_pParent = parent;
		}
		Common::ITransform* Transform::_root() 
		{
			ITransform* pRoot = this;
			while (pRoot->GetParent())
			{
				pRoot = pRoot->GetParent();
			}
			return pRoot;
		}

		// global transofrm
		Math::Vector3 Transform::GetPos() 
		{ 
			if (_pParent == nullptr)
				return _localPosition;
			else 
			{
				Math::Vector3 result = _pParent->GetPos() + _localPosition;
				return result;
			}
		}
		void Transform::SetPos(const Math::Vector3& pos) 
		{
			if (_pParent == nullptr)
				_localPosition = pos;
			else
			{
				_localPosition = pos - _pParent->GetPos();
			}
		}
		Math::Quaternion Transform::GetRotation() 
		{ 
			if (_pParent == nullptr)
				return _localRotation; 
			else 
			{
				Math::Quaternion result = _pParent->GetRotation() * _localRotation;
				return result;
			}
		}
		void Transform::SetRotation(const Math::Quaternion& i_other) 
		{ 
			if (_pParent == nullptr)
				_localRotation = i_other;
			else 
			{
				Math::Quaternion result = _pParent->GetRotation().CreateInverse() * i_other;
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
		Math::Vector3 Transform::GetScale()
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
		// local transform
		Math::Vector3& Transform::GetLocalPos() { return _localPosition; }
		void Transform::SetLocalPos(const Math::Vector3& pos) { _localPosition = pos; }
		Math::Quaternion& Transform::GetLocalRotation() { return _localRotation; }
		void Transform::SetLocalRotation(const Math::Quaternion& i_other) { _localRotation = i_other; }
		void Transform::Rotate(const Math::Quaternion& i_other) { _localRotation = _localRotation * i_other; }
		void Transform::SetLocalScale(const Math::Vector3& localScale) { _localScale = localScale; }
		Math::Vector3 Transform::LocalScale() { return _localScale; }
		// Velocity
		Math::Vector3& Transform::GetVelocity() { return _velocity; }
		void Transform::SetVelocity(Math::Vector3& velocity) { _velocity = velocity; }
		// Transform Matrices
		Math::ColMatrix44 Transform::GetRotateTransformMatrix() { 
			Math::ColMatrix44 result;
			if(_pParent)
				result = Math::ColMatrix44(_localRotation, _localPosition) * _pParent->GetRotateTransformMatrix();
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
			Math::ColMatrix44 scaleMat = Math::ColMatrix44::Identity;
			{
				Math::Vector3 scale = _localScale;
				scaleMat._m00 = scale.x();
				scaleMat._m11 = scale.y();
				scaleMat._m22 = scale.z();
			}
			Math::ColMatrix44 localTransformMatrix = rotateTransMat * scaleMat;
			if (!_pParent)
				return localTransformMatrix;
			else
				return localTransformMatrix * _pParent->GetLocalToWorldMatrix();
		}
		Math::Vector3 Transform::GetForward()
		{
			Math::ColMatrix44 rotationMat = GetRotateTransformMatrix();
			// Remember that we are working on the ColumnMatrix and Right hand coordinate.
			// So the 3rd axis of the RotateTransformMatrix is pointing to the backward
			// However, because we use i_vec * i_matrix in our engine,
			// and we actually saved the tranpose of column matrix,
			// so instead of getting z-col, we can z-row
			Math::Vector3 forward = rotationMat.GetRow(2) * -1.0f;
			return forward.Normalize();
		}


	}
}