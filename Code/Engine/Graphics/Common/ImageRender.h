#ifndef EAE_ENGINE_GRAPHICS_IMAGERENDER_H
#define EAE_ENGINE_GRAPHICS_IMAGERENDER_H

#include "Image.h"
#include "RectTransform.h"
#include "Color.h"

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
			ImageRender(MaterialDesc* pMaterial, Image* pImage, RectTransform* pRectTransform);
			~ImageRender();

			Image* GetImage() { return _pImage; }
			MaterialDesc* GetMaterial() { return _pMaterial; }
			AOSMesh* GetMesh() { return _pImageMesh; }
			void SetColor(Color color) { _color = color; }
			void SetImageRect(uint32_t index = 0) { _index = index; }

			void UpdateImageRect();

		private:
			void UpdateImageMesh(Rectangle i_rect, Rectangle i_texcoord);
		
		private:
			MaterialDesc* _pMaterial;
			Image* _pImage;
			AOSMesh* _pImageMesh;
			RectTransform* _pRectTransform;
			Color _color;
			uint32_t _index;
		};
	}
}

#endif//EAE_ENGINE_GRAPHICS_IMAGERENDER_H