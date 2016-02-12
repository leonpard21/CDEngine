#ifndef EAE_ENGINE_GRAPHICS_FONTS_H
#define EAE_ENGINE_GRAPHICS_FONTS_H

#include "Engine/General/BasicShapes.h"
#include "Engine/General/Singleton.hpp"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3dx9.h>
#elif defined( EAEENGINE_PLATFORM_GL )

#endif

namespace EAE_Engine
{
	namespace Graphics
	{

#if defined( EAEENGINE_PLATFORM_D3D9 )
		class FontManager : public Singleton<FontManager>
		{
		public:
			~FontManager();
			void Init();
			void DrawString(Rectangle rect, const char* pSentense);
		private:
			ID3DXFont* _pFont;
		};
#elif defined( EAEENGINE_PLATFORM_GL )
		class FontManager : public Singleton<FontManager>
		{
		public:
			inline void Init() {}
			inline void DrawString(Rectangle rect, const char* pSentense) {}
		};
#endif

	}
}

#endif//EAE_ENGINE_GRAPHICS_FONTS_H