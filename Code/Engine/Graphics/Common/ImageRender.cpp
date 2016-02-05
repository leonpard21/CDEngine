#include "ImageRender.h"

#include "AOSMesh.h"
#include "MeshOp.h"
#include "BinaryFileLoader.h"
#include "Material.h"
#include "Texture.h"
#include "RenderObj.h"
#include "RenderDatas.h"
#include "Device.h"
#include "Screen.h"

#include "Math/ColMatrix.h"
#include "General/MemoryOp.h"
#include <string>

#include "ImageMesh.h"
#include "Camera.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		//////////////////////////////////////ImageRender///////////////////////////////////////////////

		ImageRender::ImageRender(MaterialDesc* pMaterial, Image* pImage, Common::ITransform* pTransform):
			_pMaterial(pMaterial), _pImage(pImage), _pTrans(pTransform)
		{
		}

		ImageRender::~ImageRender()
		{
			if (_pImageMesh)
				_pImageMesh->Release();
			SAFE_DELETE(_pImageMesh);
		}

		void ImageRender::SetImagePos(float x, float y, uint32_t index)
		{
			float _defaultScreenWidth = 800.0f;
			float _defaultScreenHeight = 600.0f;
			Rect screenRect = GetScreenRect();
			float defaultRaitoX = _pImage->_width / _defaultScreenWidth * 2.0f; // the screen vertex coordinate is from (-1.0f, 1.0f)
			//float defaultRaitoY = _pImage->_height / _defaultScreenHeight * 2.0f;// the screen vertex coordinate is from (-1.0f, 1.0f)
			float scaleRaitoX = _defaultScreenWidth / screenRect._width;
			//float scaleRaitoY = _defaultScreenHeight / screenRect._height;
			//float aspect = screenRect._width / screenRect._height;
			Rectangle pos; 
			{
				float widthOfImage = _pImage->_width / (float)_pImage->_cols;
				float heightOfImage = _pImage->_height / (float)_pImage->_rows;
				float leftInClip = x / screenRect._width - 1.0f;
				float rightInClip = (x + widthOfImage) / screenRect._width - 1.0f;
				float bottomInClip = y / screenRect._height - 1.0f;
				float topInClip = (y + heightOfImage) / screenRect._height - 1.0f;
				pos._left = leftInClip * defaultRaitoX * scaleRaitoX;
				pos._right = rightInClip * defaultRaitoX * scaleRaitoX;
				pos._bottom = bottomInClip * defaultRaitoX * scaleRaitoX;
				pos._top = topInClip * defaultRaitoX * scaleRaitoX;
			}
			Rectangle texcoord;
			{
				uint32_t rowIndex = index / _pImage->_rows;
				uint32_t colIndex = index % _pImage->_cols;
				texcoord._left = (float)colIndex / (float)_pImage->_cols;
				texcoord._right = (float)(colIndex + 1) / (float)_pImage->_cols;
				texcoord._bottom = (float)rowIndex / (float)_pImage->_rows;
				texcoord._top = (float)(rowIndex + 1) / (float)_pImage->_rows;
			}
			UpdateImageMesh(pos, texcoord);
		}

		void ImageRender::UpdateImageMesh(Rectangle i_pos, Rectangle i_texcoord)
		{
			std::vector<ImageVertex> _vertices;
			ImageVertex vertex0 = { i_pos._left, i_pos._bottom, 0.0f, i_texcoord._left, i_texcoord._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex0);
			ImageVertex vertex1 = { i_pos._left, i_pos._top, 0.0f, i_texcoord._left, i_texcoord._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex1);
			ImageVertex vertex2 = { i_pos._right, i_pos._top, 0.0f, i_texcoord._right, i_texcoord._bottom,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex2);
			ImageVertex vertex3 = { i_pos._right, i_pos._bottom, 0.0f, i_texcoord._right, i_texcoord._top,
				(uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)255 };
			_vertices.push_back(vertex3);

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
			EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr , 3, sizeof(ImageVertex), GL_TRIANGLE_STRIP };
#endif
			_pImageMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &_vertices[0], 4, nullptr, 0, nullptr, 0);
		}

		//////////////////////////////////////ImageManager///////////////////////////////////////////////

		ImageRenderManager::ImageRenderManager()
		{}

		ImageRenderManager::~ImageRenderManager()
		{
			Clean();
		}

		void ImageRenderManager::Init() 
		{
			LoadMaterial("data/Materials/imageRender.material");
			_pMaterial = MaterialManager::GetInstance()->GetMaterial("imageRender");
		}

		void ImageRenderManager::Clean()
		{
			for (std::vector<ImageRender*>::iterator iter = _imageRenders.begin(); iter != _imageRenders.end(); )
			{
				ImageRender* pObj = *iter++;
				SAFE_DELETE(pObj);
			}
			_imageRenders.clear();

		}

		ImageRender* ImageRenderManager::AddImageRender(Image* pImage, Common::ITransform* pTransform)
		{
			ImageRender* pImageRender = new ImageRender(_pMaterial, pImage, pTransform);
			_imageRenders.push_back(pImageRender);
			return pImageRender;
		}

		void ImageRenderManager::UpdateRenderDataList()
		{
			std::vector<RenderDataUI>& renderDataList = RenderObjManager::GetInstance().GetRenderData2DList();
			for (std::vector<ImageRender*>::iterator it = _imageRenders.begin(); it != _imageRenders.end(); ++it)
			{
				Common::ITransform* pTrans = (*it)->GetTransform();
				Math::ColMatrix44 colMat = pTrans ? pTrans->GetLocalToWorldMatrix() : Math::ColMatrix44::Identity;
				RenderDataUI renderData = { (*it), colMat };
				renderDataList.push_back(renderData);
			}
		}

	}
}
