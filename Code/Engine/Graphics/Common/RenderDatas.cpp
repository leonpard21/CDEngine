#include "RenderDatas.h"
#include "Material.h"
#include "Effect.h"
#include "MeshRender.h"
#include "AOSMesh.h"
#include "GraphicsInternal.h"
#include "GraphicsOp.h"
#include "Device.h"

#include "ImageRender.h"
#include "CanvasRender.h"
#include "Font.h"
#include "Camera.h"

namespace EAE_Engine
{
	namespace Graphics 
	{

		///////////////////////////////////////////RenderData3D////////////////////////////////////
		MaterialDesc* RenderData3D::s_pCurrentMaterial = nullptr;
		Effect* RenderData3D::s_pCurrentEffect = nullptr;
		AOSMesh* RenderData3D::s_pCurrentAOSMesh = nullptr;
		//MeshRender* RenderObj::s_pCurrentMeshRender = nullptr;

		MaterialDesc* RenderData3D::GetSharedMaterial()
		{
			MaterialDesc* pMaterial = _pMeshRender->GetSharedMaterial(_subMeshIndex);
			return pMaterial;
		}

		void RenderData3D::ChangeEffectVariables()
		{
			// Leo: I think for the transform matrix, I should set the uniform variable through the Effect directly.
			// Set the Transform
			Math::ColMatrix44 colMat = _pTrans ? _pTrans->GetLocalToWorldMatrix() : Math::ColMatrix44::Identity;
#if defined( EAEENGINE_PLATFORM_D3D9 )
			UniformVariableManager::GetInstance().ChangeValue("g_local_world_matrix", &colMat.GetTranspose(), sizeof(Math::ColMatrix44));
#elif defined( EAEENGINE_PLATFORM_GL )
			UniformVariableManager::GetInstance().ChangeValue("g_local_world_matrix", &colMat, sizeof(Math::ColMatrix44));
#endif
			UniformVariableManager::GetInstance().NotifyOwners("g_local_world_matrix");

			// Update all of the uniform variables changed so far for the effect.
			if (s_pCurrentEffect)
				s_pCurrentEffect->Update();
		}

		void RenderData3D::Render()
		{
			//If we need to change material, change the material
			MaterialDesc* pMaterial =_pMeshRender->GetSharedMaterial(_subMeshIndex);
			if (s_pCurrentMaterial != pMaterial)
			{
				s_pCurrentMaterial = pMaterial;
				//If we need to change effect, change the effect
				Effect* pEffect = s_pCurrentMaterial->_pEffect;
				if (s_pCurrentEffect != pEffect)
				{
					s_pCurrentEffect = pEffect;
					BindCurrentEffect(s_pCurrentEffect);
					ChangeEffectRenderState(s_pCurrentEffect->GetRenderState());
				}
				// Becareful that we must set the varibale of the effect after we bind the new Effect program,
				// or we will not be able to set the uniform variables successfully.
				s_pCurrentMaterial->SetUniformForEffect();
				s_pCurrentMaterial->SetTexturesForEffect();
			}
			// updated the parameters for the material
			ChangeEffectVariables();
			//If we need to change mesh, change mesh
			AOSMesh* pAOSMesh = _pMeshRender->GetMeshFilter()->GetSharedRenderMesh();
			if (s_pCurrentAOSMesh != pAOSMesh)
			{
				s_pCurrentAOSMesh = pAOSMesh;
				SetCurrentRenderMesh(s_pCurrentAOSMesh);
			}
			// Before rendering, we need to make sure that the effect and material exists.
			if (!s_pCurrentMaterial || !s_pCurrentMaterial->_pEffect)
				return;
			// now start the rendering:
			s_pCurrentMaterial->_pEffect->BeginRender();
			bool renderMeshResult = RenderAOSMeshInternal(s_pCurrentAOSMesh, _subMeshIndex);
			assert(renderMeshResult);
			s_pCurrentMaterial->_pEffect->EndRender();
		}
		///////////////////////////////////////////RenderRawData3D////////////////////////////////////
		Effect* RenderRawData3D::s_pCurrentEffect = nullptr;
		//MeshRender* RenderObj::s_pCurrentMeshRender = nullptr;


		void RenderRawData3D::ChangeEffectVariables()
		{
			// Leo: I think for the transform matrix, I should set the uniform variable through the Effect directly.
			// Set the Transformx() : Math::ColMatrix44::Identity;
#if defined( EAEENGINE_PLATFORM_D3D9 )
			UniformVariableManager::GetInstance().ChangeValue("g_local_world_matrix", &_transMat.GetTranspose(), sizeof(Math::ColMatrix44));
#elif defined( EAEENGINE_PLATFORM_GL )
			UniformVariableManager::GetInstance().ChangeValue("g_local_world_matrix", &_transMat, sizeof(Math::ColMatrix44));
#endif
			UniformVariableManager::GetInstance().NotifyOwners("g_local_world_matrix");
			UniformVariableManager::GetInstance().ChangeValue("g_DebugMeshColor", &_meshColor, sizeof(Math::Vector3));
			UniformVariableManager::GetInstance().NotifyOwners("g_DebugMeshColor");
			// Update all of the uniform variables changed so far for the effect.
			if (s_pCurrentEffect)
				s_pCurrentEffect->Update();
		}

		void RenderRawData3D::Render()
		{
			//If we need to change material, change the material
			MaterialDesc* pMaterial = _pMeshRender->GetSharedMaterial(0);
			//If we need to change effect, change the effect
			Effect* pEffect = pMaterial->_pEffect;
			if (s_pCurrentEffect != pEffect)
			{
				s_pCurrentEffect = pEffect;
				BindCurrentEffect(s_pCurrentEffect);
				ChangeEffectRenderState(s_pCurrentEffect->GetRenderState());
			}
			// updated the parameters for the material
			ChangeEffectVariables();
			//If we need to change mesh, change mesh
			AOSMesh* pAOSMesh = _pMeshRender->GetMeshFilter()->GetSharedRenderMesh();
			SetCurrentRenderMesh(pAOSMesh);
			// Before rendering, we need to make sure that the effect and material exists.
			if (!pMaterial || !pMaterial->_pEffect)
				return;
			// now start the rendering:
			pMaterial->_pEffect->BeginRender();
			bool renderMeshResult = RenderAOSMeshInternal(pAOSMesh);
			assert(renderMeshResult);
			pMaterial->_pEffect->EndRender();
		}

		///////////////////////////////////////////RenderData2D////////////////////////////////////
		MaterialDesc* RenderDataUI::s_pCurrentMaterial = nullptr;
		Effect* RenderDataUI::s_pCurrentEffect = nullptr;

		void RenderDataUI::ChangeEffectVariables()
		{
			// Update all of the uniform variables changed so far for the effect.
			if (s_pCurrentEffect)
				s_pCurrentEffect->Update();
		}

		void RenderDataUI::Render()
		{
			if (_pCanvasRenderData->_renderType == eCanvasRender::IMAGE) 
			{
				RenderImage((ImageRender*)(_pCanvasRenderData->_pCanvasRender));
			}
			else if (_pCanvasRenderData->_renderType == eCanvasRender::TEXT) 
			{
				TextRender* pTextRender = (TextRender*)(_pCanvasRenderData->_pCanvasRender);
				RenderText(pTextRender);
			}
		}

		void RenderDataUI::RenderImage(ImageRender* pImageRender) 
		{
			//If we need to change material, change the material
			MaterialDesc* pMaterial = CanvasRenderManager::GetInstance()->GetSharedMaterial();
			s_pCurrentMaterial = pMaterial;
			s_pCurrentEffect = pMaterial->_pEffect;
			BindCurrentEffect(s_pCurrentEffect);
			ChangeEffectRenderState(s_pCurrentEffect->GetRenderState());
			s_pCurrentMaterial->SetUniformForEffect();
			s_pCurrentMaterial->ChangeTexture(0, pImageRender->GetImage()->_pTextureInfo);
			s_pCurrentMaterial->SetTexturesForEffect();
			// updated the parameters for the material
			ChangeEffectVariables();
			pImageRender->UpdateImageRect();
			AOSMesh* pSMesh = pImageRender->GetMesh();
			
			SetCurrentRenderMesh(pSMesh);

			s_pCurrentEffect->BeginRender();
			bool renderMeshResult = RenderAOSMeshInternal(pSMesh);
			assert(renderMeshResult);
			s_pCurrentEffect->EndRender();
		}

		void RenderDataUI::RenderText(TextRender* pTextRender)
		{
			Rectangle rect = pTextRender->_pRectTransform->GetScreenSpaceRect();
			Text* pText = pTextRender->_pText;
			ScreenRect screenInfo = GetScreenRect();
			float screenHeight = screenInfo._height;
			rect._bottom = screenHeight - rect._bottom;
			rect._top = screenHeight - rect._top;
			FontManager::GetInstance()->DrawString(rect, pText->_value.c_str());
		}
	}
}

