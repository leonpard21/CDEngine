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
      virtual ~Transform() override;
      ITransform* GetTransform() override { return this; }
      // Inherted
      virtual Common::IGameObj* GetGameObj() override;
      virtual Common::ICompo* GetComponent(typeid_t type) override;
      // global transform
      Math::Vector3 GetPos() const override;
      void SetPos(const Math::Vector3&) override;
      Math::Quaternion GetRotation() const override;
      void SetRotation(const Math::Quaternion& i_other) override;
      void SetScale(const Math::Vector3& scale) override;
      Math::Vector3 GetScale() const override;
      // euler angle
      Math::Vector3 GetEulerAngle() const override;
      void SetEulerAngle(Math::Vector3 eulerAngle) override;
      // local transform
      Math::Vector3& GetLocalPos() override;
      void SetLocalPos(const Math::Vector3&) override;
      Math::Quaternion& GetLocalRotation() override;
      void SetLocalRotation(const Math::Quaternion& i_other) override;
      void SetLocalScale(const Math::Vector3& localScale) override;
      Math::Vector3 LocalScale() override;
      Math::Vector3 GetLocalEulerAngle() override;
      void SetLocalEulerAngle(const Math::Vector3& eulerAngle) override;
      void Move(const Math::Vector3& i_movement) override;
      void Rotate(const Math::Quaternion& i_other) override;
      // Matrix && Direction
      Math::ColMatrix44 GetRotateTransformMatrix() override;
      Math::ColMatrix44 GetLocalToWorldMatrix() override;//read only, so return value
      Math::Vector3 GetForward() override;
      Math::Vector3 GetRight() override;
      Math::Vector3 GetUp() override;
      // Children
      void AddChild(Common::ITransform* pChild) override { _children.push_back(pChild); }
      void RemoveChild(Common::ITransform* pChild) override;
      uint32_t GetChildCount() override { return (uint32_t)_children.size(); }
      Common::ITransform* GetChild(uint32_t index = 0) override { return _children[index]; }
      // Parent
      void SetParent(Common::ITransform* pParent) override;
      Common::ITransform* GetParent() override { return _pParent; }
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