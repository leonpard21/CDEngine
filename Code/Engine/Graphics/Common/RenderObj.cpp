#include "RenderObj.h"
#include "RenderDatas.h"
#include "Material.h"
#include "Effect.h"
#include "MeshRender.h"
#include "AOSMesh.h"

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

