#include "RenderObj.h"
#include "RenderDatas.h"
#include "Material.h"
#include "Effect.h"
#include "MeshRender.h"
#include "AOSMesh.h"
#include "SpriteRender.h"

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
			if(_renderWeight._layer == RenderDataLayer::Object3D)
			{
				RenderData3D* pRenderData = reinterpret_cast<RenderData3D*>(_pRenderData);
				pRenderData->Render();
			}
			if (_renderWeight._layer == RenderDataLayer::Object2D)
			{
				RenderData2D* pRenderData = reinterpret_cast<RenderData2D*>(_pRenderData);
				pRenderData->Render();
			}
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
				weight._layer = RenderDataLayer::Object3D;
				weight._material = pMaterial ? pMaterial->_materialCost._cost : 0;
				RenderObj obj = { weight, &(*it) };
				_renderObjs.push_back(obj);
			}

			for (std::vector<RenderData2D>::iterator it = _renderData2Ds.begin(); it != _renderData2Ds.end(); ++it)
			{
				SpriteRender* pSpriteRender = (it)->_pSpriteRender;
				MaterialDesc* pMaterial = pSpriteRender->GetMaterial();
				RenderWeight weight;
				weight._layer = RenderDataLayer::Object2D;
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

