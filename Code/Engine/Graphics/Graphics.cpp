// Header Files
//=============
#include "Graphics.h"
#include "Common/GraphicsInternal.h"
#include "Common/Device.h"
#include "Common/BinaryFileLoader.h"
#include "Common/MeshManager.h"
#include "Common/RenderObj.h"
#include "Common/RenderDatas.h"
#include "Common/Effect.h"
#include "Common/Material.h"
#include "Common/Texture.h"
#include "Common/MeshRender.h"
#include "Common/Camera.h"
#include "Common/DebugMesh.h"
#include "Common/CanvasRender.h"
#include "Common/Font.h"
#include "Common/GUI.h"
#include "Engine/Math/ColMatrix.h"


#ifdef _DEBUG
#define DRAW_DEBUG_SHAPES
#endif

// Interface
//==========
namespace EAE_Engine
{
	namespace Graphics
	{
		void SetCameraParameters(Camera* pCamera);
		void SetLightParameters();
		void PreRender();
		void PostRender();
	}
}

bool EAE_Engine::Graphics::Initialize( const HWND i_renderingWindow )
{
	if (!CreateDevice(i_renderingWindow)) 
	{
		goto OnError;
	}
#ifdef DRAW_DEBUG_SHAPES
	DebugMeshes::GetInstance().Init();
#endif
	//ImageManager::GetInstance()->Init();
	CanvasRenderManager::GetInstance()->Init();
	FontManager::GetInstance()->Init();
	UIElementManager::GetInstance()->Init();
	return true;
OnError:
	ShutDown();
	return false;
}

void EAE_Engine::Graphics::PreRender()
{
	CleanBuffer(ClearFlag::COLOR | ClearFlag::DEPTH);
}

void EAE_Engine::Graphics::SetCameraParameters(Camera* pCamera)
{
	CameraMatrices viewprojmatrices;
	//Set the camera materices for D3D or GL
#if defined( EAEENGINE_PLATFORM_D3D9 )
	const char* pCamBlockName = "g_CameraMatrices";
	viewprojmatrices._worldViewMatrix = pCamera->GetWroldToViewMatrix().GetTranspose();
	viewprojmatrices._viewProjMatrix = pCamera->GetProjClipMatrix().GetTranspose();
#elif defined( EAEENGINE_PLATFORM_GL )
	const char* pCamBlockName = "CameraMatrices";
	viewprojmatrices._worldViewMatrix = pCamera->GetWroldToViewMatrix();
	viewprojmatrices._viewProjMatrix = pCamera->GetProjClipMatrix();
#endif
	UniformBlock* pUB = UniformBlockManager::GetInstance()->GetUniformBlock(pCamBlockName);
	UniformBlockData data[] = { {0, &(viewprojmatrices._worldViewMatrix), sizeof(viewprojmatrices._worldViewMatrix)}, 
	{ 0 + sizeof(viewprojmatrices._worldViewMatrix), &(viewprojmatrices._viewProjMatrix), sizeof(viewprojmatrices._viewProjMatrix) } };
	pUB->SetBlockData(data, 2);
	UniformBlockManager::GetInstance()->NotifyOwners(pCamBlockName);

	// Set the CameraPos
	Math::Vector3 camPos = pCamera->GetTransform()->GetPos();
	UniformVariableManager::GetInstance().ChangeValue<Math::Vector3>("_camera_pos", &camPos, 1);
	UniformVariableManager::GetInstance().NotifyOwners("_camera_pos");
}

void EAE_Engine::Graphics::SetLightParameters()
{
#if defined( EAEENGINE_PLATFORM_D3D9 )
	const char* pLightBlockName = "gSpecularLight";
#elif defined( EAEENGINE_PLATFORM_GL )
	const char* pLightBlockName = "SpecularLight";
#endif
	UniformBlock* pSLUB = UniformBlockManager::GetInstance()->GetUniformBlock(pLightBlockName);
	Math::Vector3 specularAlbedo(1.0f, 1.0f, 1.0f);
	float specularPower = 128.0f;
	UniformBlockData sldata[] = { { 0, &specularAlbedo, sizeof(Math::Vector3) },
	{ 0 + sizeof(Math::Vector3), &specularPower, sizeof(float) } };
	pSLUB->SetBlockData(sldata, 2);
	UniformBlockManager::GetInstance()->NotifyOwners(pLightBlockName);
}

void EAE_Engine::Graphics::Render()
{
	SetCameraParameters(CameraManager::GetInstance().GetCam());
	SetLightParameters();
	PreRender();
	// The actual function calls that draw geometry
	{
		// Clean the data of the last frame.
		RenderObjManager::GetInstance().Clean();
		// Add the MeshRender to the RenderData3D list.
		MeshRenderManager::GetInstance().UpdateRenderDataList();
		UIElementManager::GetInstance()->Update();
		CanvasRenderManager::GetInstance()->UpdateRenderDataList();
#ifdef DRAW_DEBUG_SHAPES
		// Add the DebugMesh to the RenderData3D list.
		DebugMeshes::GetInstance().Update();
#endif
		RenderObjManager::GetInstance().UpdateRenderObjList();
		std::vector<RenderObj>& renderObjList = RenderObjManager::GetInstance().GetRenderObjList();
		RenderData3D::s_pCurrentAOSMesh = nullptr;
		RenderData3D::s_pCurrentEffect = nullptr;
		RenderData3D::s_pCurrentMaterial = nullptr;
		for (std::vector<RenderObj>::iterator it = renderObjList.begin(); it != renderObjList.end(); ++it)
		{
			it->Render();
		}
	}
	PostRender();
}

void EAE_Engine::Graphics::PostRender()
{
	SwapBuffer();
}

bool EAE_Engine::Graphics::ShutDown()
{
	bool wereThereErrors = false;
#ifdef DRAW_DEBUG_SHAPES
	DebugMeshes::CleanInstance();
#endif
	MeshRenderManager::CleanInstance();
	CanvasRenderManager::Destroy();
	UIElementManager::Destroy();
	ImageManager::Destroy();
	FontManager::Destroy();
	UniformVariableManager::CleanInstance();
	UniformBlockManager::Destroy();
	MeshManager::CleanMeshManager();
	RenderObjManager::CleanInstance();
	CameraManager::CleanInstance();
	EffectsManager::CleanInstance();
	MaterialManager::CleanInstance();
	TextureManager::CleanInstance();
	CleanDevice();
	return !wereThereErrors;
}

void EAE_Engine::Graphics::CleanBeforeRestart()
{
	CameraManager::CleanInstance();
	MeshRenderManager::CleanInstance();
	RenderObjManager::CleanInstance();
	CanvasRenderManager::Destroy();
}

void EAE_Engine::Graphics::RemoveMeshRender(Common::ITransform* pTrans)
{
	MeshRenderManager::GetInstance().Remove(pTrans);
}