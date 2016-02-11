#ifndef EAE_ENGINE_GRAPHICS_IMAGERENDER_H
#define EAE_ENGINE_GRAPHICS_IMAGERENDER_H

#include "Engine/Math/Vector.h"
#include "Engine/General/BasicShapes.h"
#include "Image.h"
#include "RectTransform.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		struct MaterialDesc;
		class AOSMesh;
		class ImageRender
		{
			ImageRender();
		public:
			ImageRender(MaterialDesc* pMaterial, Image* pImage, Common::ITransform* _pTransform);
			~ImageRender();
			void SetImageRect(Math::Vector4 values, uint32_t index = 0);

			Image* GetImage() { return _pImage; }
			MaterialDesc* GetMaterial() { return _pMaterial; }
			AOSMesh* GetMesh() { return _pImageMesh; }
			void SetAnchor(Rectangle anchor) { _rectTransform.SetAnchor(anchor); }
			void SetPivot(Math::Vector2 pivot) { _rectTransform.SetPivot(pivot); }

		private:
			void ImageRender::UpdateImageMesh(Rectangle i_rect, Rectangle i_texcoord);

		private:
			MaterialDesc* _pMaterial;
			Image* _pImage;
			AOSMesh* _pImageMesh;
			RectTransform _rectTransform;
			Common::ITransform* _pTrans;
		};
	}
}

#endif//EAE_ENGINE_GRAPHICS_IMAGERENDER_H