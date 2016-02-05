#ifndef EAE_ENGINE_GRAPHICS_SPRITE_MESH_H
#define EAE_ENGINE_GRAPHICS_SPRITE_MESH_H

#include <cstdint>

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )

#endif	

namespace EAE_Engine
{
	namespace Graphics
	{
		struct SpriteVertex
		{
			float x, y, z;
			float u, v;
#if defined( EAEENGINE_PLATFORM_D3D9 )
			uint8_t b, g, r, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#elif defined( EAEENGINE_PLATFORM_GL )
			uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif	
			SpriteVertex() = default;
		};

#if defined( EAEENGINE_PLATFORM_D3D9 )
		class SpriteMesh
		{
		public:
			bool Init(void* pVertices);
			void Release();

			void ChangeVertexBuffer(void* pVertices);

			inline IDirect3DVertexBuffer9* GetVertexBuffer() const { return _pVertexBuffer; };
			inline IDirect3DVertexDeclaration9* GetVertexDeclaration() const { return _pVertexDeclaration; }

		private:
			IDirect3DVertexBuffer9* _pVertexBuffer;
			IDirect3DVertexDeclaration9* _pVertexDeclaration;
		};


#elif defined(EAEENGINE_PLATFORM_GL)
		class SpriteMesh
		{
		public:
			SpriteMesh() {}
			~SpriteMesh() {}

			//	bool InitSprite(IDirect3DDevice9* pDevice, void* pVertices);
			void Release();


		private:
		};
#endif
		bool RenderSpriteMeshInternal(SpriteMesh *pSpriteMesh);
	}
}



#endif //  EAE_ENGINE_GRAPHICS_SPRITE_MESH_H

