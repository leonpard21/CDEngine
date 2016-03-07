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
//#define DRAW_DEBUG_SHAPES
#endif

// Interface
//==========
namespace EAE_Engine
{
	namespace Graphics
	{
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
	//ImageManager::GetInstance()->Init();
	CanvasRenderManager::GetInstance()->Init();
	FontManager::GetInstance()->Init();
	UIElementManager::GetInstance()->Init();
#ifdef DRAW_DEBUG_SHAPES
	DebugMeshes::GetInstance().Init();
#endif
	return true;
OnError:
	ShutDown();
	return false;
}

void EAE_Engine::Graphics::PreRender()
{
	CleanBuffer(ClearFlag::COLOR | ClearFlag::DEPTH);
}

void EAE_Engine::Graphics::SetLightParameters()
{
	// Set light position
	Math::Vector3 lightPos(0.0f, 50.0f, 0.0f);
	UniformVariableManager::GetInstance().ChangeValue("_light_pos", &lightPos, sizeof(Math::Vector3));
	UniformVariableManager::GetInstance().NotifyOwners("_light_pos");
	// Set specular information
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
	CameraManager::GetInstance().GetCam()->Update();
	SetLightParameters();
	PreRender();
	// The actual function calls that draw geometry
	{
		// Clean the data of the last frame.
		RenderObjManager::GetInstance().Clean();
		// Add the MeshRender to the RenderData3D list.
		AOSMeshRenderManager::GetInstance().UpdateRenderDataList();
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
		RenderRawData3D::s_pCurrentEffect = nullptr;
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
	AOSMeshRenderManager::CleanInstance();
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
	AOSMeshRenderManager::CleanInstance();
	RenderObjManager::CleanInstance();
	CanvasRenderManager::Destroy();
}

void EAE_Engine::Graphics::RemoveMeshRender(Common::ITransform* pTrans)
{
	AOSMeshRenderManager::GetInstance().Remove(pTrans);
}