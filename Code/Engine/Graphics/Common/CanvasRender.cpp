#include "CanvasRender.h"
#include "ImageRender.h"
#include "AOSMesh.h"
#include "GraphicsOp.h"
#include "BinaryFileLoader.h"
#include "Material.h"
#include "RenderObj.h"
#include "RenderDatas.h"



namespace EAE_Engine
{
	namespace Graphics
	{
		TextRender::TextRender(Text* pText, RectTransform* pTrans) :
			_pText(pText), _pRectTransform(pTrans)
		{
		}

		//////////////////////////////////////CanvasRenderManager///////////////////////////////////////////////

		CanvasRenderManager::CanvasRenderManager()
		{}

		CanvasRenderManager::~CanvasRenderManager()
		{
			Clean();
		}

		void CanvasRenderManager::Init() 
		{
			LoadMaterial("data/Materials/imageRender.material");
			_pMaterial = MaterialManager::GetInstance()->GetMaterial("imageRender");
		}

		void CanvasRenderManager::Clean()
		{
			for (std::vector<CanvasRenderData>::iterator iter = _canvasRenderDatas.begin(); iter != _canvasRenderDatas.end(); )
			{
				CanvasRenderData& crData = *iter++;
				if (crData._renderType == eCanvasRender::IMAGE) 
				{
					ImageRender* pRender = (ImageRender*)(crData._pCanvasRender);
					SAFE_DELETE(pRender);
				}
				else if (crData._renderType == eCanvasRender::TEXT)
				{
					TextRender* pRender = (TextRender*)(crData._pCanvasRender);
					SAFE_DELETE(pRender);
				}
			}
			_canvasRenderDatas.clear();
		}

		ImageRender* CanvasRenderManager::AddImageRender(Image* pImage, RectTransform* pRectTransform)
		{
			ImageRender* pImageRender = new ImageRender(_pMaterial, pImage, pRectTransform);
			CanvasRenderData newData = {eCanvasRender::IMAGE, pImageRender };
			_canvasRenderDatas.push_back(newData);
			return pImageRender;
		}

		TextRender* CanvasRenderManager::AddTextRender(Text* pText, RectTransform* pRectTransform)
		{
			TextRender* pTextRender = new TextRender(pText, pRectTransform);
			CanvasRenderData newData = { eCanvasRender::TEXT, pTextRender };
			_canvasRenderDatas.push_back(newData);
			return pTextRender;
		}

		void CanvasRenderManager::UpdateRenderDataList()
		{
			std::vector<RenderDataUI>& renderDataList = RenderObjManager::GetInstance().GetRenderData2DList();
			for (std::vector<CanvasRenderData>::iterator it = _canvasRenderDatas.begin(); it != _canvasRenderDatas.end(); ++it)
			{
				RenderDataUI renderData = { &(*it)};
				renderDataList.push_back(renderData);
			}
		}

	}
}
