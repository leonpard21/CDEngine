#ifndef EAE_ENGINE_GRAPHICS_SPRITERENDER_H
#define EAE_ENGINE_GRAPHICS_SPRITERENDER_H

#include <vector>
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"
#include "BasicShapes.h"

#include "Sprite.h"

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
		class SpriteRender 
		{
			SpriteRender();
		public:
			SpriteRender(MaterialDesc* pMaterial, Sprite* pSprite, Common::ITransform* _pTransform);
			~SpriteRender();
			Math::ColMatrix44 GetSpriteMatrix();

			void SetTrans(Common::ITransform*  pTrans) { _pTrans = pTrans; }
			Common::ITransform* GetTransform() { return _pTrans; }
			Sprite* GetSprite() { return _pSprite; }
			MaterialDesc* GetMaterial() { return _pMaterial; }
		private:
			MaterialDesc* _pMaterial;
			Sprite* _pSprite;
			Common::ITransform* _pTrans;
		};

		class AOSMesh;
		class SpriteRenderManager : public Singleton<SpriteRenderManager>
		{
		public:
			SpriteRenderManager();
			~SpriteRenderManager();
			void Init();
			void Clean();

			
			SpriteRender* AddSpriteRender(Sprite* pSprite, Common::ITransform* pTransform);
			void UpdateRenderDataList();

			MaterialDesc* GetMaterial() { return _pMaterial; }
			AOSMesh* GetSpriteMesh(){ return _pSpriteMesh; }
		private:
			MaterialDesc* _pMaterial;
			AOSMesh* _pSpriteMesh;
			std::vector<SpriteRender*> _spriteRenders;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITERENDER_H