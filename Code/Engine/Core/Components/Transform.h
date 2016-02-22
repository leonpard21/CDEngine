#ifndef EAE_ENGINE_CORE_TRANSFORM_H
#define EAE_ENGINE_CORE_TRANSFORM_H
#include "Common/Interfaces.h"
#include "Engine/General/EngineObj.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/ColMatrix.h"

namespace EAE_Engine
{
	namespace Core
	{
		class Transform : public Common::ITransform, public EngineObj
		{
			Transform() = delete;
			Transform(const Transform& i_other) = delete;
			Transform& operator=(const Transform& i_other) = delete;
		public:
			Transform(Common::IGameObj* pGamObj);
			virtual ~Transform();
			ITransform* GetTransform() { return this; }
			// Inherted
			virtual Common::IGameObj* GetGameObj();
			virtual Common::ICompo* GetComponent(typeid_t type);
			// global transform
			Math::Vector3 GetPos();
			void SetPos(const Math::Vector3&);
			Math::Quaternion GetRotation();
			void SetRotation(const Math::Quaternion& i_other);
			void SetScale(const Math::Vector3& scale);
			Math::Vector3 GetScale();
			// local transform
			Math::Vector3& GetLocalPos();
			void SetLocalPos(const Math::Vector3&);
			Math::Quaternion& GetLocalRotation();
			void SetLocalRotation(const Math::Quaternion& i_other);
			void Rotate(const Math::Quaternion& i_other);
			void SetLocalScale(const Math::Vector3& localScale);
			Math::Vector3 LocalScale();
			// Velocity
			Math::Vector3& GetVelocity();
			void SetVelocity(Math::Vector3& velocity);
			// Matrix && Direction
			Math::ColMatrix44 GetRotateTransformMatrix();
			Math::ColMatrix44 GetLocalToWorldMatrix();//read only, so return value
			Math::Vector3 GetForward();
			// Children
			uint32_t GetChildCount() { return _childCount; }
			Common::ITransform* pGetChildren(uint32_t index = 0) { return _ppChildren[index]; }
			// Parent
			void SetParent(Common::ITransform* parent);
			Common::ITransform* GetParent() { return _pParent; }
			Common::ITransform* _root();

		protected:
			Common::IGameObj* _pGamObj;//The game object this component is attached to. A component is always attached to a game object.

		private:
			uint32_t _childCount;
			Transform** _ppChildren;
			ITransform* _pParent;

			Math::Vector3 _localScale;
			Math::Quaternion _localRotation;
			Math::Vector3 _localPosition;
			Math::Vector3 _velocity;
		};
	}
}

RTTI_DECLARE_META_TYPE(EAE_Engine::Core::Transform)
#endif//EAE_ENGINE_CORE_TRANSFORM_H