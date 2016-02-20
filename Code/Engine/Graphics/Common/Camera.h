#ifndef EAEENGINE_GRPHICS_CAMERAH
#define EAEENGINE_GRPHICS_CAMERAH
#include "Engine/Common/Interfaces.h"
#include "Engine/Math/MathFunctions.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class Camera : public Common::ICamera
		{
		public:
			Camera(Common::ITransform* pTrans);
			void Init(const float i_fieldOfView_y, const float i_aspectRatio, const float i_z_nearPlane, const float i_z_farPlane);
			void SetWindowSize(float windowWidth, float windowHeight);
			void Update();

			Common::ITransform* GetTransform() { return _pTransform; }
			Math::ColMatrix44 GetWroldToViewMatrix();
			Math::ColMatrix44 GetProjClipMatrix();
			Math::Vector3 ConvertWorldToViewport(const Math::Vector3& i_worldPos);
			Math::Vector3 ConvertViewportToWorld(Math::Vector3& i_portPos);
		private:
			static Math::ColMatrix44 CreateWorldToViewMatrix(
			const Math::Quaternion& i_cameraOrientation, const Math::Vector3& i_cameraPosition );
			Math::ColMatrix44 CreateProjMatrix();
			static Math::ColMatrix44 CreateProjectClipMatrix(
			const float i_fieldOfView_y, const float i_aspectRatio,
			const float i_z_nearPlane, const float i_z_farPlane );
			Math::ColMatrix44 CreateClipVolumeToViewportMatrix();

		private:
			Common::ITransform* _pTransform;
			float _fieldOfView_y;
			float _aspectRatio;
			float _z_nearPlane;
			float _z_farPlane;

			float _windowWidth;
			float _windowHeight;
		};

		class CameraManager 
		{
		public:
			Camera* AddCamera(Common::ITransform* pTransform);
			void Clean();
			Camera* GetCam() { return _pCam; }

		private:
			Camera* _pCam;
			CameraManager() {}
			static CameraManager* s_pInternalInstance;
		public:
			static CameraManager& GetInstance();
			static void CleanInstance();
			static bool Valid();
		};

		inline Camera* AddCamrea(float windowWidth, float windowHeight, Common::ITransform* pTransform)
		{
			Camera* pResult = CameraManager::GetInstance().AddCamera(pTransform);
			pResult->SetWindowSize(windowWidth, windowHeight);
			pResult->Init(Math::ConvertDegreesToRadians(60.0f), windowWidth / windowHeight, 0.1f, 500.0f);
			return pResult;
		}
	}
}

#endif//EAEENGINE_GRPHICS_CAMERAH