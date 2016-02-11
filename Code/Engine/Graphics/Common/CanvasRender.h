#ifndef EAE_ENGINE_GRAPHICS_CANVARENDER_H
#define EAE_ENGINE_GRAPHICS_CANVARENDER_H

#include "ImageRender.h"
#include "Text.h"
#include "RectTransform.h"
#include <vector>
#include "Engine/General/Singleton.hpp"


namespace EAE_Engine
{
	namespace Graphics
	{
		enum eCanvasRender
		{
			TEXT,
			IMAGE,
		};

		struct TextRender
		{
			TextRender(Text* pText, Common::ITransform* pTrans);
			Text* _pText;
			RectTransform _rectTransform;
			Common::ITransform* _pTrans;
		};

		struct CanvasRenderData 
		{
			CanvasRenderData() = default;
			eCanvasRender _renderType;
			void* _pCanvasRender;
		};

		class CanvasRenderManager : public Singleton<CanvasRenderManager>
		{
		public:
			CanvasRenderManager();
			~CanvasRenderManager();
			void Init();
			void Clean();

			ImageRender* AddImageRender(Image* pImage, Common::ITransform* pTransform);
			TextRender* AddTextRender(Text* pText, Common::ITransform* pTransform);
			void UpdateRenderDataList();
			MaterialDesc* GetMaterial() { return _pMaterial; }

		private:
			MaterialDesc* _pMaterial;
			std::vector<CanvasRenderData> _canvasRenderDatas;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_CANVARENDER_H