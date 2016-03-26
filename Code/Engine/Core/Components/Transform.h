#ifndef EAE_ENGINE_CORE_TRANSFORM_H
#define EAE_ENGINE_CORE_TRANSFORM_H
#include "Common/Interfaces.h"
#include "Engine/General/EngineObj.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/ColMatrix.h"
#include <vector>

namespace EAE_Engine
{
  namespace Core
  {
    class Transform : public Common::ITransform, public Reflection<Transform>
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
      Math::Vector3 GetPos() const;
      void SetPos(const Math::Vector3&);
      Math::Quaternion GetRotation() const;
      void SetRotation(const Math::Quaternion& i_other);
      void SetScale(const Math::Vector3& scale);
      Math::Vector3 GetScale() const;
      // local transform
      Math::Vector3& GetLocalPos();
      void SetLocalPos(const Math::Vector3&);
      Math::Quaternion& GetLocalRotation();
      void SetLocalRotation(const Math::Quaternion& i_other);
      void SetLocalScale(const Math::Vector3& localScale);
      Math::Vector3 LocalScale();
      void Move(const Math::Vector3& i_movement);
      void Rotate(const Math::Quaternion& i_other);
      // Matrix && Direction
      Math::ColMatrix44 GetRotateTransformMatrix();
      Math::ColMatrix44 GetLocalToWorldMatrix();//read only, so return value
      Math::Vector3 GetForward();
      Math::Vector3 GetRight();
      Math::Vector3 GetUp();
      // Children
      void AddChild(Common::ITransform* pChild) { _children.push_back(pChild); }
      void RemoveChild(Common::ITransform* pChild);
      uint32_t GetChildCount() { return (uint32_t)_children.size(); }
      Common::ITransform* GetChild(uint32_t index = 0) { return _children[index]; }
      // Parent
      void SetParent(Common::ITransform* pParent);
      Common::ITransform* GetParent() { return _pParent; }
      Common::ITransform* Root();

    protected:
      Common::IGameObj* _pGamObj;//The game object this component is attached to. A component is always attached to a game object.

    private:
      std::vector<Common::ITransform*> _children;
      Common::ITransform* _pParent;

      Math::Vector3 _localScale;
      Math::Quaternion _localRotation;
      Math::Vector3 _localPosition;
    };
  }
}

RTTI_DECLARE_META_TYPE(EAE_Engine::Core::Transform)
#endif//EAE_ENGINE_CORE_TRANSFORM_H