#include "RenderObj.h"
#include "Material.h"
#include "Effect.h"
#include "MeshRender.h"
#include "AOSMesh.h"

#include "AOSMeshOp.h"
#include "GraphicsInternal.h"
#include <algorithm>


namespace 
{
}

namespace EAE_Engine
{
	namespace Graphics 
	{

		//////////////////////////////////////RenderObj////////////////////////////////////
		void RenderObj::Render()
		{
			//switch()
			if(_renderWeight._layer == RenderDataLayer::Scene)
			{
				RenderData3D* pRenderData = reinterpret_cast<RenderData3D*>(_pRenderData);
				pRenderData->Render();
			}
		}

		///////////////////////////////////////////RenderData3D////////////////////////////////////
		MaterialDesc* RenderData3D::s_pCurrentMaterial = nullptr;
		Effect* RenderData3D::s_pCurrentEffect = nullptr;
		AOSMesh* RenderData3D::s_pCurrentAOSMesh = nullptr;
		//MeshRender* RenderObj::s_pCurrentMeshRender = nullptr;

		MaterialDesc* RenderData3D::GetMaterial()
		{
			MaterialDesc* pMaterial = _pMeshRender->GetMaterial(_subMeshIndex);
			return pMaterial;
		}

		void RenderData3D::ChangeEffectVariables()
		{
			// Leo: I think for the transform matrix, I should set the uniform variable through the Effect directly.
			// Set the Transform
			UniformVariableManager::GetInstance().ChangeValue<Math::ColMatrix44>("g_local_world_matrix", &_localToWorld, 1);
			UniformVariableManager::GetInstance().NotifyOwners("g_local_world_matrix");

			// Update all of the uniform variables changed so far for the effect.
			if (s_pCurrentEffect)
				s_pCurrentEffect->Update();
		}

		void RenderData3D::Render()
		{
			//If we need to change material, change the material
			MaterialDesc* pMaterial =_pMeshRender->GetMaterial(_subMeshIndex);
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
			AOSMesh* pAOSMesh = _pMeshRender->GetMesh();
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

		////////////////////////////////RenderObjManager/////////////////////////////////
		void RenderObjManager::Clean()
		{
			_renderData3Ds.clear();
			_renderObjs.clear();
		}

		static RenderObjManager::RenderObjLess sortFunc;
		void RenderObjManager::UpdateRenderObjList()
		{
			for (std::vector<RenderData3D>::iterator it = _renderData3Ds.begin(); it != _renderData3Ds.end(); ++it)
			{
				MaterialDesc* pMaterial = it->GetMaterial();
				RenderWeight weight;
				weight._layer = RenderDataLayer::Scene;
				weight._material = pMaterial ? pMaterial->_materialCost._cost : 0;
				RenderObj obj = { weight, &(*it) };
				_renderObjs.push_back(obj);
			}
			std::sort(_renderObjs.begin(), _renderObjs.end(), sortFunc);
		}

		////////////////////////////////static_members/////////////////////////////////
		RenderObjManager* RenderObjManager::s_pInternalInstance = nullptr;

		RenderObjManager& RenderObjManager::GetInstance()
		{
			if (!s_pInternalInstance)
				s_pInternalInstance = new RenderObjManager();
			return *s_pInternalInstance;
		}

		void RenderObjManager::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}

	}
}

