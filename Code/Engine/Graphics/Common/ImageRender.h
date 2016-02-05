#ifndef EAE_ENGINE_GRAPHICS_IMAGERENDER_H
#define EAE_ENGINE_GRAPHICS_IMAGERENDER_H

#include <vector>
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"
#include "BasicShapes.h"
#include "Screen.h"
#include "Image.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )
//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace EAE_Engine
{
	namespace Math
	{
		class ColMatrix44;
	}
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
			// If the min and max anchor point is the same,
			// then we should use offsetMode, 
			// in that case the @values describe the position of the pivot point, 
			// and the width and height of the Image.
			// Else if the min or max anchor point is not the same,
			// then we should use the stretch mode.
			// the image will be scaled based on the window size.
			void SetImagePos(Math::Vector4 values, uint32_t index = 0);

			void SetTrans(Common::ITransform*  pTrans) { _pTrans = pTrans; }
			Common::ITransform* GetTransform() { return _pTrans; }
			Image* GetImage() { return _pImage; }
			MaterialDesc* GetMaterial() { return _pMaterial; }
			AOSMesh* GetMesh() { return _pImageMesh; }
			void SetAnchor(Rectangle anchor) { _anchorPoint  = anchor; }

		private:
			void ImageRender::UpdateImageMesh(Rectangle i_rect, Rectangle i_texcoord);

		private:
			MaterialDesc* _pMaterial;
			Image* _pImage;
			AOSMesh* _pImageMesh;
			Common::ITransform* _pTrans;
			// @_pivot is the local ratio of the Image.
			Math::Vector2 _pivot;
			// anchor point uses the Viewport coordinate, 
			// it contains a pair of min and max points.
			Rectangle _anchorPoint;
		};

		class ImageRenderManager : public Singleton<ImageRenderManager>
		{
		public:
			ImageRenderManager();
			~ImageRenderManager();
			void Init();
			void Clean();

			ImageRender* AddImageRender(Image* pImage, Common::ITransform* pTransform);
			void UpdateRenderDataList();
			MaterialDesc* GetMaterial() { return _pMaterial; }

		private:
			MaterialDesc* _pMaterial;
			std::vector<ImageRender*> _imageRenders;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_IMAGERENDER_H