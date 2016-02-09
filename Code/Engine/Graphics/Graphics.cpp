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
#include "Common/ImageRender.h"
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
	ImageRenderManager::GetInstance()->Init();
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
	viewprojmatrices._worldViewMatrix = pCamera->GetWroldToViewMatrix().GetTranspose();
	viewprojmatrices._viewProjMatrix = pCamera->GetProjClipMatrix().GetTranspose();
	UniformVariableManager::GetInstance().ChangeValue<CameraMatrices>("g_CameraMatrices", &viewprojmatrices, 1);
	UniformVariableManager::GetInstance().NotifyOwners("g_CameraMatrices");
#elif defined( EAEENGINE_PLATFORM_GL )
	viewprojmatrices._worldViewMatrix = pCamera->GetWroldToViewMatrix();
	viewprojmatrices._viewProjMatrix = pCamera->GetProjClipMatrix();
	UniformBlock* pUB = UniformBlockManager::GetInstance()->GetUniformBlock("CameraMatrices");
	pUB->SetBlockValue(&viewprojmatrices, sizeof(viewprojmatrices));
	UniformBlockManager::GetInstance()->NotifyOwners("CameraMatrices");
#endif
}

void EAE_Engine::Graphics::Render()
{
	SetCameraParameters(CameraManager::GetInstance().GetCam());
	PreRender();
	// The actual function calls that draw geometry
	{
		// Clean the data of the last frame.
		RenderObjManager::GetInstance().Clean();
		// Add the MeshRender to the RenderData3D list.
		MeshRenderManager::GetInstance().UpdateRenderDataList();
		ImageRenderManager::GetInstance()->UpdateRenderDataList();
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
	ImageRenderManager::Destroy();
	ImageManager::Destroy();
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
	ImageRenderManager::Destroy();
}

void EAE_Engine::Graphics::RemoveMeshRender(Common::ITransform* pTrans)
{
	MeshRenderManager::GetInstance().Remove(pTrans);
}