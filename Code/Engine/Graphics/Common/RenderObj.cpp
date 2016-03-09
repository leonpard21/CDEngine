#include "RenderObj.h"
#include "RenderDatas.h"
#include "Material.h"
#include "Effect.h"
#include "MeshRender.h"
#include "AOSMesh.h"
#include "ImageRender.h"
#include "CanvasRender.h"

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
			else if (_renderWeight._layer == RenderDataLayer::DebugMesh)
			{
				RenderRawData3D* pRenderData = reinterpret_cast<RenderRawData3D*>(_pRenderData);
				pRenderData->Render();
			}
			else if (_renderWeight._layer == RenderDataLayer::UIElement)
			{
				RenderDataUI* pRenderData = reinterpret_cast<RenderDataUI*>(_pRenderData);
				pRenderData->Render();
			}
		}

		////////////////////////////////RenderObjManager/////////////////////////////////
		void RenderObjManager::Clean()
		{
			_renderData3Ds.clear();
			_renderRawData3Ds.clear();
			_renderDataUIs.clear();
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
			for (std::vector<RenderRawData3D>::iterator itRaw = _renderRawData3Ds.begin(); itRaw != _renderRawData3Ds.end(); ++itRaw)
			{
				MaterialDesc* pMaterial = itRaw->_pMeshRender->GetSharedMaterial();
				RenderWeight weight;
				weight._layer = RenderDataLayer::DebugMesh;
				weight._material = pMaterial ? pMaterial->_materialCost._cost : 0;
				RenderObj obj = { weight, &(*itRaw) };
				_renderObjs.push_back(obj);
			}
			for (std::vector<RenderDataUI>::iterator it = _renderDataUIs.begin(); it != _renderDataUIs.end(); ++it)
			{
				CanvasRenderData* pCanvasRenderData = (it)->_pCanvasRenderData;
				RenderWeight weight;
				weight._layer = RenderDataLayer::UIElement;
				// I haven't decide how to deal with the material in 2D.
				// So I leave these code at here right now.
				/*
				if (pCanvasRenderData->_renderType == eCanvasRender::IMAGE)
				{
					ImageRender* pImageRender = (ImageRender*)pCanvasRenderData->_pCanvasRender;
					MaterialDesc* pMaterial = pImageRender->GetMaterial();
					weight._material = pMaterial ? pMaterial->_materialCost._cost : 0;
				}
				else if (pCanvasRenderData->_renderType == eCanvasRender::TEXT)
				{
				
				}
				*/
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

