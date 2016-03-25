#ifndef EAE_ENGINE_CORE_INTERFACE
#define EAE_ENGINE_CORE_INTERFACE

#include "Engine/General/RTTI.h"

namespace EAE_Engine
{
	struct Rectangle;

	namespace Math
	{
		template<typename T>
		class TVector4;
		typedef TVector4<float> Vector4;
		template<typename T>
		class TVector3;
		typedef TVector3<float> Vector3;
		template<typename T>
		class TVector2;
		typedef TVector2<float> Vector2;
		class ColMatrix44;
		class Quaternion;
	}

	namespace Common
	{
		class IMessage
		{
		public:
			//virtual void BroadcastMessage(const char* message) = 0;
		};

		class ICompo;
		class IGetCompo
		{
		public:
			virtual ICompo* GetComponent(typeid_t type) = 0;
			//virtual IComponent* GetComponents(typeid_t type) = 0;
			//virtual IComponent* GetComponentInChildren() = 0;
			//virtual IComponent* GetComponentInParent() = 0;
		};

		struct Compo
		{
			Compo() = default;
			Common::ICompo* _pCompo;
			typeid_t _typeId;
		};

		class ITransform;
		class IGameObj : public IGetCompo
		{
		public:
			virtual ~IGameObj() {}
			virtual ITransform* GetTransform() = 0;
			virtual const char* GetName() = 0;
			virtual void AddComponent(Compo) = 0;
			template<typename T>
			void AddComponent(T* pCompo) = 0;
		};

		class ICompo : public IGetCompo
		{
		public:
			virtual ~ICompo() {}
			virtual ITransform* GetTransform() = 0;
		};

		class ITransform : public ICompo
		{
		public:
			virtual ~ITransform() {}
			virtual IGameObj* GetGameObj() = 0;
			// global transform
			virtual Math::Vector3 GetPos() const = 0;
			virtual void SetPos(const Math::Vector3&) = 0;
			virtual void SetRotation(const Math::Quaternion&) = 0;
			virtual Math::Quaternion GetRotation() const = 0;
			virtual void SetScale(const Math::Vector3&) = 0;
			virtual Math::Vector3 GetScale() const = 0;
			// local transform
			virtual Math::Vector3& GetLocalPos() = 0;
			virtual void SetLocalPos(const Math::Vector3&) = 0;
			virtual Math::Quaternion& GetLocalRotation() = 0;
			virtual void SetLocalRotation(const Math::Quaternion&) = 0;
			virtual void SetLocalScale(const Math::Vector3&) = 0;
			virtual Math::Vector3 LocalScale() = 0;
			virtual void Move(const Math::Vector3&) = 0;
			virtual void Rotate(const Math::Quaternion&) = 0;
			// Matrix
			virtual Math::ColMatrix44 GetRotateTransformMatrix() = 0;
			virtual Math::ColMatrix44 GetLocalToWorldMatrix() = 0;
			virtual Math::Vector3 GetForward() = 0;
			virtual Math::Vector3 GetRight() = 0;
			virtual Math::Vector3 GetUp() = 0;
			// Children
			virtual uint32_t GetChildCount() = 0;
			virtual ITransform* GetChild(uint32_t index = 0) = 0;
			virtual void AddChild(ITransform*) = 0;
			virtual void RemoveChild(ITransform* pChild) = 0;
			// Parent
			virtual void SetParent(ITransform* pParent) = 0;
			virtual ITransform* GetParent() = 0;
		};


		class IRectTransform
		{
		public:
			virtual ~IRectTransform() {}
			virtual void SetAnchor(Rectangle anchorPoint) = 0;
			virtual void SetPivot(Math::Vector2 pivot) = 0; 
			virtual void SetRect(Math::Vector4 values) = 0;
			virtual Rectangle GetRect() = 0;
		};

		enum ForceMode 
		{
			Force,
			Momentum,
			Velocity,
		};

		enum CollisionDetectionMode 
		{
			Discrete = 0x0,
			Continuous = 0x1, // Continuous collision detection is on for colliding with static mesh geometry.
			ContinuousDynamic = 0x2,// Continuous collision detection is on for colliding with static and dynamic geometry.
		};

		class IRigidBody : public ICompo
		{
		public:
			virtual ~IRigidBody() {}
			virtual ITransform* GetTransform() = 0;
			virtual Math::Vector3 GetVelocity() const = 0;
			virtual void SetVelocity(const Math::Vector3&) = 0;
			virtual Math::Vector3 GetPos() const = 0;
			virtual void SetPos(const Math::Vector3&) = 0;
			//virtual Math::Quaternion GetRotation() = 0;
			//virtual void SetRotation(Math::Quaternion& rotation) = 0;
			virtual float GetMass() const = 0;
			virtual void SetMass(float) = 0;
			virtual void SetCollisionDetectionMode(CollisionDetectionMode mode) = 0;
			virtual CollisionDetectionMode GetCollisionDetectionMode() const = 0;
			virtual void AddForce(Math::Vector3& force, ForceMode mode = ForceMode::Force) = 0;
			virtual Math::Vector3 PredictPosAfter(float timeStep) = 0;
		};

		class ICamera // : public ICompo
		{
		public:
			virtual ~ICamera() {}
			virtual Math::ColMatrix44 GetWroldToViewMatrix() = 0;
			virtual Math::ColMatrix44 GetProjClipMatrix() = 0;
			virtual ITransform* GetTransform() = 0;
			virtual Math::Vector3 ConvertWorldToViewport(const Math::Vector3& i_worldPos) = 0;
			virtual Math::Vector3 ConvertViewportToWorld(Math::Vector3& i_portPos) = 0;
		};

		class IController// : public ICompo
		{
		public:
			virtual ~IController() {}
			virtual void Update() = 0;
			virtual void FixedUpdate() = 0;
			virtual ITransform* GetTransform() = 0;
			virtual bool IsActive() = 0;
			virtual void SetActive(bool active) = 0;
		};

	}
}
#endif//EAE_ENGINE_CORE_INTERFACE
