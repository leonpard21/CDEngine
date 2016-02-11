#include "ImageRender.h"

#include "AOSMesh.h"
#include "GraphicsOp.h"
#include "Material.h"
#include "Texture.h"
#include "Device.h"
#include "ImageMesh.h"
#include "Screen.h"

#include "Math/ColMatrix.h"
#include "General/MemoryOp.h"
#include <string>
#include "CanvasRender.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		//////////////////////////////////////ImageRender///////////////////////////////////////////////

		ImageRender::ImageRender(MaterialDesc* pMaterial, Image* pImage, RectTransform* pRectTransform):
			_pMaterial(pMaterial), _pImage(pImage), _pImageMesh(nullptr),
			_pRectTransform(pRectTransform)
		{
		}

		ImageRender::~ImageRender()
		{
			if (_pImageMesh)
				_pImageMesh->Release();
			SAFE_DELETE(_pImageMesh);
		}

		void ImageRender::SetImageRect(uint32_t index)
		{
			Rectangle imageRect = _pRectTransform->GetClipRect();
			Rectangle texcoord;
			{
				uint32_t rowIndex = index / _pImage->_rows;
				uint32_t colIndex = index % _pImage->_cols;
				texcoord._left = (float)colIndex / (float)_pImage->_cols;
				texcoord._right = (float)(colIndex + 1) / (float)_pImage->_cols;
				texcoord._bottom = (float)rowIndex / (float)_pImage->_rows;
				texcoord._top = (float)(rowIndex + 1) / (float)_pImage->_rows;
			}
			UpdateImageMesh(imageRect, texcoord);
		}

		void ImageRender::UpdateImageMesh(Rectangle i_pos, Rectangle i_texcoord)
		{
			std::vector<ImageVertex> _vertices;
			ImageVertex vertex0 = { i_pos._left, i_pos._bottom, 0.0f, i_texcoord._left, i_texcoord._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex0);
#if defined( EAEENGINE_PLATFORM_D3D9 )
			ImageVertex vertex1 = { i_pos._left, i_pos._top, 0.0f, i_texcoord._left, i_texcoord._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex1);
			ImageVertex vertex2 = { i_pos._right, i_pos._top, 0.0f, i_texcoord._right, i_texcoord._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex2);
			ImageVertex vertex3 = { i_pos._right, i_pos._bottom, 0.0f, i_texcoord._right, i_texcoord._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex3);
#elif defined( EAEENGINE_PLATFORM_GL )
			// We are working under the glFrontFace(GL_CCW) mode.
			ImageVertex vertex1 = { i_pos._right, i_pos._bottom, 0.0f, i_texcoord._right, i_texcoord._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex1);
			ImageVertex vertex2 = { i_pos._left, i_pos._top, 0.0f, i_texcoord._left, i_texcoord._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex2);
			ImageVertex vertex3 = { i_pos._right, i_pos._top, 0.0f, i_texcoord._right, i_texcoord._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex3);
#endif
#if defined( EAEENGINE_PLATFORM_D3D9 )
			D3DVERTEXELEMENT9 elemnt_arr[] = {
				// Stream: 0, POSITION: 3 floats == 12 bytes, Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				// Stream: 0, TextureCoordinate0: 2 float == 8 bytes, Offset = 8
				{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				// Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 16
				{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			};
			EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 3,{ sizeof(ImageVertex), D3DPT_TRIANGLEFAN, Dynamic } };
#elif defined( EAEENGINE_PLATFORM_GL )
			EAE_Engine::Graphics::MeshGLVertexElement element_arr[] = {
				{ 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // Position
				{ 1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // UV
				{ 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) }  // Color
			};
			EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr , 3, { sizeof(ImageVertex), GL_TRIANGLE_STRIP, GL_DYNAMIC_DRAW } };
#endif
			if (_pImageMesh == nullptr)
				_pImageMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &_vertices[0], 4, nullptr, 0, nullptr, 0);
			else
				_pImageMesh->ChangeWholeBuffers(&_vertices[0], 4, nullptr, 0, nullptr, 0);
		}
	}
}
