#include "SpriteRender.h"

#include "AOSMesh.h"
#include "AOSMeshOp.h"
#include "Device.h"
#include "BinaryFileLoader.h"
#include "Material.h"
#include "Texture.h"

#include "Math/MathTool.h"
#include "Math/ColMatrix.h"
#include "General/MemoryOp.h"
#include <string>



namespace EAE_Engine
{
	namespace Graphics
	{
		bool Sprite::SetData(Rectangle* i_position, Rectangle* i_texcoords)
		{
			Rectangle textcoordRect;
			if (i_texcoords == nullptr)
				textcoordRect = { 0.0f, 1.0f, 0.0f, 1.0f };
			else
				textcoordRect = { i_texcoords->_left, i_texcoords->_right, i_texcoords->_bottom, i_texcoords->_top };
			std::vector<SpriteVertex> _vertices;
			SpriteVertex vertex0 = { i_position->_left, i_position->_bottom, textcoordRect._left, textcoordRect._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex0);
			SpriteVertex vertex1 = { i_position->_right, i_position->_bottom, textcoordRect._right, textcoordRect._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex1);
			SpriteVertex vertex2 = { i_position->_right, i_position->_top, textcoordRect._right, textcoordRect._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex2);
			SpriteVertex vertex3 = { i_position->_left, i_position->_top, textcoordRect._left, textcoordRect._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex3);

			IDirect3DDevice9* pDevice = GetD3DDevice();
/*		
#if defined( EAEENGINE_PLATFORM_D3D9 )
			D3DVERTEXELEMENT9 elemnt_arr[] = {
				// Stream: 0, POSITION: 2 floats == 8 bytes, Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				// Stream: 0, TextureCoordinate0: 2 float == 8 bytes, Offset = 8
				{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				// Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 16
				{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				// The following marker signals the end of the vertex declaration
			};
#elif defined( EAEENGINE_PLATFORM_GL )
			EAE_Engine::Graphics::MeshGLVertexElement element_arr[] = {
				{ 0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // Position
				{ 1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // UV
				{ 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) }  // Color
			};
#endif
#if defined( EAEENGINE_PLATFORM_D3D9 )
			EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 3, sizeof(SpriteVertex), D3DPT_TRIANGLESTRIP };
#elif defined( EAEENGINE_PLATFORM_GL )
			EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr , 3, sizeof(SpriteVertex), GL_TRIANGLE_STRIP };
#endif
			_pSpriteMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &_vertices[0], 4, nullptr, 0, nullptr, 0);
*/

			_pSpriteMesh = new SpriteMesh();
			_pSpriteMesh->InitSprite(pDevice, &_vertices[0]);

			return true;
		}


		//////////////////////////////////////SpriteRender///////////////////////////////////////////////

		SpriteRender::SpriteRender(Sprite& sprite, Common::ITransform* pTransform):
			_sprite(sprite), _pTransform(pTransform)
		{
		}

		SpriteRender::~SpriteRender()
		{
		}



		//////////////////////////////////////SpriteManager///////////////////////////////////////////////

		void SpriteManager::Init() 
		{
			LoadMaterial("data/Materials/spriteRender.material");
			std::vector<std::string> materialList;
			materialList.push_back("spriteRender");

		}

		void SpriteManager::Clean()
		{
			for (std::vector<SpriteRender*>::iterator iter = _spriteRenders.begin(); iter != _spriteRenders.end(); ++iter)
			{
				SpriteRender* pObj = *iter++;
				SAFE_DELETE(pObj);
			}
			_spriteRenders.clear();
		}

		SpriteRender* SpriteManager::AddSprite(const char* pName, Common::ITransform* pTransform)
		{
			//MaterialDesc* pMaterial = MaterialManager::GetInstance()->GetMaterial("spriteRender");
			tTexture texture = TextureManager::GetInstance()->LoadTexture(pName);
			Sprite sprite(texture);
			SpriteRender* pSpriteRender = new SpriteRender(sprite, pTransform);
			_spriteRenders.push_back(pSpriteRender);
			return pSpriteRender;
		}

	}
}
