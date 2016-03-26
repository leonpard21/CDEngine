#include "Camera.h"
#include "Engine/General/MemoryOp.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/ColMatrix.h"
#include "Screen.h"
#include "Device.h"
#include <cassert>
#include "UniformBlock.h"
#include "UniformVariable.h"

namespace EAE_Engine
{
  namespace Graphics
  {
    Camera::Camera(Common::ITransform* pTrans) : _pTransform(pTrans),
      _fieldOfView_y(Math::ConvertDegreesToRadians(60.0f)), _aspectRatio(1.0f), _z_nearPlane(0.1f), _z_farPlane(1000.0f)
    {
    }

    void Camera::SetWindowSize(float windowWidth, float windowHeight)
    {
      _windowWidth = windowWidth;
      _windowHeight = windowHeight;
    }

    void Camera::Init(const float i_fieldOfView_y, const float i_aspectRatio,
      const float i_z_nearPlane, const float i_z_farPlane)
    {
      _fieldOfView_y = i_fieldOfView_y;
      _aspectRatio = i_aspectRatio;
      _z_nearPlane = i_z_nearPlane;
      _z_farPlane = i_z_farPlane;
    }

    struct CameraMatrices
    {
      Math::ColMatrix44 _worldViewMatrix;
      Math::ColMatrix44 _viewProjMatrix;
    };

    void Camera::Update()
    {
      CameraMatrices viewprojmatrices;
      //Set the camera materices for D3D or GL
#if defined( EAEENGINE_PLATFORM_D3D9 )
      const char* pCamBlockName = "g_CameraMatrices";
      // The standard Direct3D stores matrices as rows but we are storing them as columns. 
      // (Confusingly, standard Direct3D interprets matrices as being stored in columns on the GPU
      // Since we're copying ColMatrices in the Engine to RowMatrices in the D3D, 
      // we need to offer the Transpose of the matrices.
      viewprojmatrices._worldViewMatrix = GetWroldToViewMatrix().GetTranspose();
      viewprojmatrices._viewProjMatrix = GetProjClipMatrix().GetTranspose();
#elif defined( EAEENGINE_PLATFORM_GL )
      const char* pCamBlockName = "CameraMatrices";
      viewprojmatrices._worldViewMatrix = GetWroldToViewMatrix();
      viewprojmatrices._viewProjMatrix = GetProjClipMatrix();
#endif
      UniformBlock* pUB = UniformBlockManager::GetInstance()->GetUniformBlock(pCamBlockName);
      UniformBlockData data[] = { { 0, &(viewprojmatrices._worldViewMatrix), sizeof(viewprojmatrices._worldViewMatrix) },
      { 0 + sizeof(viewprojmatrices._worldViewMatrix), &(viewprojmatrices._viewProjMatrix), sizeof(viewprojmatrices._viewProjMatrix) } };
      pUB->SetBlockData(data, 2);
      UniformBlockManager::GetInstance()->NotifyOwners(pCamBlockName);
      // Set the CameraPos
      Math::Vector3 camPos = _pTransform->GetPos();
      UniformVariableManager::GetInstance().ChangeValue("_camera_pos", &camPos, sizeof(Math::Vector3));
      UniformVariableManager::GetInstance().NotifyOwners("_camera_pos");
    }

    Math::ColMatrix44 Camera::GetWroldToViewMatrix() 
    {
      Math::ColMatrix44 result = CreateWorldToViewMatrix(_pTransform->GetRotation(), _pTransform->GetPos());
      return result;
    }

    Math::ColMatrix44 Camera::GetProjClipMatrix() 
    { 
      return CreateProjectClipMatrix(_fieldOfView_y, _aspectRatio, _z_nearPlane, _z_farPlane); 
    }

    Math::Vector3 Camera::ConvertWorldToViewport(const Math::Vector3& i_worldPos)
    {
      Math::Vector4 worldPos(i_worldPos.x(), i_worldPos.y(), i_worldPos.z(), 1.0f);
      // Get Matrices
      Math::ColMatrix44 viewMat = CreateWorldToViewMatrix(_pTransform->GetRotation(), _pTransform->GetPos());
      Math::ColMatrix44 projClipMat = CreateProjectClipMatrix(_fieldOfView_y, _aspectRatio, _z_nearPlane, _z_farPlane);
      Math::Vector4 clipSpacePos = projClipMat * viewMat * worldPos;
      // From here, the point will be in the Clip Volume Space.
      // And very important for the range of the Clip Volumn space:
      // x is -1f to 1f,
      // y is also -1f to 1f,
      // but z is from 0.0f to 1.0f, not -1.0f.
      // it is a left-hand coordinate either GL or D3D.
      // Now we Convert the point from clip-volume homogeneous to normal device coordinate
      Math::Vector4 normaldevicePos = clipSpacePos * (1.0f / clipSpacePos.w());
      // convert normal device coordinate to viewport coordinate
      Math::ColMatrix44 clipToViewportMat = CreateClipVolumeToViewportMatrix();
      Math::Vector4 result = clipToViewportMat * normaldevicePos;
      {
        Math::ColMatrix44 invMat;
        bool hasInv = clipToViewportMat.GetInverse(invMat);
        assert(hasInv);
        assert(clipToViewportMat * invMat == Math::ColMatrix44::Identity);
        Math::Vector4 test = invMat * result;
        assert((test - normaldevicePos).Magnitude() < 0.1f);
      }
      return result;
    }

    Math::Vector3 Camera::ConvertViewportToWorld(Math::Vector3& i_portPos)
    {
      Math::ColMatrix44 viewportMat = CreateClipVolumeToViewportMatrix();
      Math::ColMatrix44 invMat;
      bool hasInv = viewportMat.GetInverse(invMat);
      assert(hasInv);
      // First we convert the input point from viewport space coordinate to the normal device coordinate,
      // When we conver the world point to the viewport point, we will divide the x,y,z,w by w,
      // so at that time we will use 1.0f to be w value.
      // In this case we should still use 1.0f to be the w.
      Math::Vector4 portPos(i_portPos.x(), i_portPos.y(), i_portPos.z(), 1.0f);
      Math::Vector4 clipPos = invMat * portPos;
      // Second, we convert the point from clipvolume to the world space.
      Math::ColMatrix44 viewMat = CreateWorldToViewMatrix(_pTransform->GetRotation(), _pTransform->GetPos());
      Math::ColMatrix44 projClipMat = CreateProjectClipMatrix(_fieldOfView_y, _aspectRatio, _z_nearPlane, _z_farPlane);
      Math::ColMatrix44 transMat = projClipMat * viewMat;
      hasInv = transMat.GetInverse(invMat);
      assert(hasInv);
      Math::Vector4 result = invMat * clipPos;
      // We convert the w value to 1.0f, because if we want to convert the world to the viewport,
      // we also use 1.0f to be the w value for the world point.
      result = result * (1.0f / result.w());
      return result;
    }
    
    Math::ColMatrix44 Camera::CreateWorldToViewMatrix(
      const Math::Quaternion& i_cameraOrientation, const Math::Vector3& i_cameraPosition)
    {
      Math::ColMatrix44 transform_viewToWorld(i_cameraOrientation, i_cameraPosition);
      // A camera can only ever have rotation and translation
      // and so a lot of simplifying assumptions can be made in order to create the inverse
      // The inverse of transformation matrix [R|t] is [R^T | - R^T t]:
      // See the link here: http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche53.html
      Math::ColMatrix44 inverseOfRotationTransform(
        transform_viewToWorld._m00, transform_viewToWorld._m01, transform_viewToWorld._m02, 0.0f,
        transform_viewToWorld._m10, transform_viewToWorld._m11, transform_viewToWorld._m12, 0.0f,
        transform_viewToWorld._m20, transform_viewToWorld._m21, transform_viewToWorld._m22, 0.0f,
        -(transform_viewToWorld._m00 * transform_viewToWorld._m03 + transform_viewToWorld._m10 * transform_viewToWorld._m13 + transform_viewToWorld._m20 * transform_viewToWorld._m23),
        -(transform_viewToWorld._m01 * transform_viewToWorld._m03 + transform_viewToWorld._m11 * transform_viewToWorld._m13 + transform_viewToWorld._m21 * transform_viewToWorld._m23),
        -(transform_viewToWorld._m02 * transform_viewToWorld._m03 + transform_viewToWorld._m12 * transform_viewToWorld._m13 + transform_viewToWorld._m22 * transform_viewToWorld._m23),
        1.0f);
      return inverseOfRotationTransform;
    }

    Math::ColMatrix44 Camera::CreateProjMatrix()
    {
      return Math::ColMatrix44(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, -1.0f,
        0.0f, 0.0f, 0.0f, 0.0f);
    }

    Math::ColMatrix44 Camera::CreateProjectClipMatrix(
      const float i_fieldOfView_y, const float i_aspectRatio,
      const float i_z_nearPlane, const float i_z_farPlane)
    {
      // Convert the Y FOV angle to ZoomY.
      const float yZoom = 1.0f / std::tan(i_fieldOfView_y * 0.5f);
      // yZoom / xZoom = winPhysX / winPhysY = windows_aspect_raito
      const float xZoom = yZoom / i_aspectRatio;
#if defined( EAEENGINE_PLATFORM_D3D9 )
      const float zDistanceScale = i_z_farPlane / (i_z_nearPlane - i_z_farPlane);
      return Math::ColMatrix44(
        xZoom, 0.0f, 0.0f, 0.0f,
        0.0f, yZoom, 0.0f, 0.0f,
        0.0f, 0.0f, zDistanceScale, -1.0f,
        0.0f, 0.0f, i_z_nearPlane * zDistanceScale, 0.0f);
#elif defined( EAEENGINE_PLATFORM_GL )
      const float zDistanceScale = 1.0f / (i_z_nearPlane - i_z_farPlane);
      return Math::ColMatrix44(
        xZoom, 0.0f, 0.0f, 0.0f,
        0.0f, yZoom, 0.0f, 0.0f,
        0.0f, 0.0f, (i_z_nearPlane + i_z_farPlane) * zDistanceScale, -1.0f,
        0.0f, 0.0f, (2.0f * i_z_nearPlane * i_z_farPlane) * zDistanceScale, 0.0f);
#endif
      // From here, the vertices will be in the Clip Volume Space.
      // And very important:
      // x is -1f to 1f,
      // y is also -1f to 1f,
      // but z is from 0.0f to 1.0f, not -1.0f.
      // it is a left-hand coordinate either GL or D3D.
    }

    Math::ColMatrix44 Camera::CreateClipVolumeToViewportMatrix()
    {
      Math::ColMatrix44 tranlation, scaleXYZMatirx, translateOrigin = Math::ColMatrix44::Identity;
      //If the viewport cover the entire screen, minX=minY=minZ=0, w=screenWidth and h=screenHeight.
      float minX = 0.0f;
      float maxX = 1.0f;
      float minY = 0.0f;
      float maxY = 1.0f;
      float minZ = 0.0f;
      float maxZ = 1.0f;
      float halfWidth = (maxX - minX) * 0.5f;
      float halfHeight = (maxX - minX) * 0.5f;
      Math::ColMatrix44 result(
        halfWidth, 0.0f, 0.0f, 0.0f, 
        0.0f, halfHeight, 0.0f, 0.0f, 
        0.0f, 0.0f, maxZ - minZ, 0.0f,
        minX + halfWidth, minY + halfHeight, minZ, 1.0f);
      return result;
    }

//////////////////////////////CameraManager////////////////////////

    Camera* CameraManager::AddCamera(Common::ITransform* pTransform)
    {
      _pCam = new Camera(pTransform);
      return _pCam;
    }

    void CameraManager::Clean()
    {
      SAFE_DELETE(_pCam);
    }

    
////////////////////////////////static_members/////////////////////////////////
    CameraManager* CameraManager::s_pInternalInstance = nullptr;

    CameraManager& CameraManager::GetInstance()
    {
      if (!s_pInternalInstance)
        s_pInternalInstance = new CameraManager();
      return *s_pInternalInstance;
    }

    void CameraManager::CleanInstance()
    {
      if (!s_pInternalInstance) 
        return;
      s_pInternalInstance->Clean();
      SAFE_DELETE(s_pInternalInstance);
    }

    bool CameraManager::Valid()
    {
      if (!s_pInternalInstance)
        return false;
      return true;
    }
  }
}