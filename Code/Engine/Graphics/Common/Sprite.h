#ifndef EAE_ENGINE_GRAPHICS_SPRITE_H
#define EAE_ENGINE_GRAPHICS_SPRITE_H

#include <vector>
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include "BasicMeshes.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )
//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace EAE_Engine
{
	namespace Graphics
	{
		class SpriteRender 
		{
		public:
			SpriteRender();
			~SpriteRender();

			bool Init(const char* pName);

		private:
			const char* _pName;
		};


		class SpriateManager : public Singleton<SpriateManager>
		{
			std::vector<SpriteRender*> _spriteRenders;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITE_H