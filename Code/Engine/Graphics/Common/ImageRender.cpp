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
			_pMaterial(pMaterial), _pImage(pImage), _pTrans(pTransform), _pImageMesh(nullptr), 
			_anchorPoint({0.5f, 0.5f, 0.5f, 0.5f}), _pivot(Math::Vector2(0.5f, 0.5f))
		{
		}

		ImageRender::~ImageRender()
		{
			if (_pImageMesh)
				_pImageMesh->Release();
			SAFE_DELETE(_pImageMesh);
		}

		/*
		Now the GUI image has 2 modes:
		1. position-mode (the anchor min and max are the same)
		The 4 parameters of the screenRect are:
		x, y, the screen position of the anchor point).
		width, height, the width and the height of the rect we want to render.
		In this case, the screenRect size will be fixed when we change the window size.

		2. stretch-mode (the anchor min and max are not the same)
		The 4 parameters of the screenRect are:
		_left, _right, how far the left edge and right edge from the anchorMinX and anchorMaxX,
		_bottom, _top, how far the bottom edge and top edge from the anchorMinY and anchorMaxY
		*/
		void ImageRender::SetImagePos(Math::Vector4 values, uint32_t index)
		{
			ScreenRect screenInfo = GetScreenRect();
			float screenWidth = screenInfo._width;
			float screenHeight = screenInfo._height;
			Math::Vector2 _anchorMinScreen(_anchorPoint._left * screenWidth,  _anchorPoint._bottom * screenHeight);
			Math::Vector2 _anchorMaxScreen(_anchorPoint._right * screenWidth, _anchorPoint._top * screenHeight);
			Rectangle imageScreenRectangle; 
			// if the anchorPositionX is the same, then based on pivot, 
			// we call it position-mode
			if (_anchorPoint._right - _anchorPoint._left == 0.0f)
			{
				ScreenRect imageScreenRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				float screenRectX = imageScreenRect._x + (_anchorMinScreen._x + _anchorMaxScreen._x) * 0.5f;
				imageScreenRectangle._left = screenRectX - _pivot._x * imageScreenRect._width;
				imageScreenRectangle._right = screenRectX + (1.0f - _pivot._x) * imageScreenRect._width;
			}
			else // or use stretch-mode
			{
				Rectangle imageStretchRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				imageScreenRectangle._left = _anchorMinScreen._x + imageStretchRect._left;
				imageScreenRectangle._right = _anchorMaxScreen._x - imageStretchRect._right;
			}
			// if the anchorPositionY is the same, then based on pivot,
			// we call it position-mode
			if (_anchorPoint._top - _anchorPoint._bottom == 0.0f)
			{	
				ScreenRect imageScreenRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				float screenRectY = imageScreenRect._y + (_anchorMinScreen._y + _anchorMaxScreen._y) * 0.5f;
				imageScreenRectangle._bottom = screenRectY - _pivot._y * imageScreenRect._height;
				imageScreenRectangle._top = screenRectY + (1.0f - _pivot._y) * imageScreenRect._height;
			}
			else  // or use stretch-mode
			{
				Rectangle imageStretchRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				imageScreenRectangle._bottom = _anchorMinScreen._y + imageStretchRect._bottom;
				imageScreenRectangle._top = _anchorMaxScreen._y - imageStretchRect._top;
			}
			// Since we have got the 4 positions of the 4 vertices in screen pos, 
			// we can convert the screen pos to clip pos. 
			Rectangle clipPos; 
			{
				clipPos._left = imageScreenRectangle._left / screenWidth * 2.0f - 1.0f; // the screen vertex coordinate is from (-1.0f, 1.0f)
				clipPos._right = imageScreenRectangle._right / screenWidth * 2.0f - 1.0f;
				clipPos._bottom = imageScreenRectangle._bottom / screenHeight * 2.0f - 1.0f;
				clipPos._top = imageScreenRectangle._top / screenHeight* 2.0f - 1.0f;
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
			UpdateImageMesh(clipPos, texcoord);
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
			if (_pImageMesh == nullptr)
				_pImageMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &_vertices[0], 4, nullptr, 0, nullptr, 0);
			else
				_pImageMesh->ChangeWholeBuffers(&_vertices[0], 4, nullptr, 0, nullptr, 0);
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
