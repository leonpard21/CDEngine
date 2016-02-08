#include "DebugMesh.h"
#include "DebugShape/DebugShape.h"
#include "Math/MathTool.h"
#include "Math/ColMatrix.h"
#include <vector>
#include "BinaryFileLoader.h"
#include "GraphicsOp.h"
#include "RenderObj.h"
#include "RenderDatas.h"
#include "MeshRender.h"
#include "BasicMeshes.h"
#include "Material.h"

namespace 
{
	struct DebugVertex
	{
		// POSITION: 3 floats == 12 bytes, Offset = 0
		float x, y, z;
		// COLOR0: 4 uint8_ts == 4 bytes, Offset = 12
#if defined( EAEENGINE_PLATFORM_D3D9 )
		uint8_t b, g, r, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#elif defined( EAEENGINE_PLATFORM_GL )
		uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif
		DebugVertex() = default;
		DebugVertex(EAE_Engine::Math::Vector3 i_pos, EAE_Engine::Math::Vector4 i_color) :
			x(i_pos.x()), y(i_pos.y()), z(i_pos.z())
		{
			r = (uint8_t)(EAE_Engine::Math::clip<float>(i_color.x(), 0.0f, 1.0f) * 255.0f);
			g = (uint8_t)(EAE_Engine::Math::clip<float>(i_color.y(), 0.0f, 1.0f) * 255.0f);
			b = (uint8_t)(EAE_Engine::Math::clip<float>(i_color.z(), 0.0f, 1.0f) * 255.0f);
			a = (uint8_t)(EAE_Engine::Math::clip<float>(i_color.w(), 0.0f, 1.0f) * 255.0f);
		}
	};
}

namespace EAE_Engine
{
	namespace Graphics
	{
		DebugMeshes::DebugMeshes():_pSegmentsMeshRender(nullptr), _pSegmentsMesh(nullptr), 
		_pBoxesMeshRender(nullptr), _pBoxesMesh(nullptr) {}

		void DebugMeshes::Init()
		{
			LoadMaterial("data/Materials/debugPrimitives.material");
			std::vector<std::string> materialList;
			materialList.push_back("debugPrimitives");
			_pSegmentsMeshRender = new MeshRender(); 
			_pSegmentsMeshRender->AddMaterial(materialList);
			_pBoxesMeshRender = new MeshRender();
			_pBoxesMeshRender->AddMaterial(materialList);
			_pShperesMeshRender = new MeshRender();
			_pShperesMeshRender->AddMaterial(materialList);
			// Setup vertices format.
#if defined( EAEENGINE_PLATFORM_D3D9 )
			D3DVERTEXELEMENT9 elemnt_arr[] = {
				// Stream: 0, POSITION: 3 floats == 12 bytes, Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				// Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 12
				{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }
			};
#elif defined( EAEENGINE_PLATFORM_GL )
			EAE_Engine::Graphics::MeshGLVertexElement element_arr[] = {
				{ 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // Position
				{ 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) }  // Color
			};
#endif
			DebugVertex vertices;
			uint32_t indices = 0;
			sSubMesh subMesh(0, 0);
			{
				{
					vertices.x = 0.0f;
					vertices.y = 0.0f;
					vertices.z = -1.0f;
					vertices.r = (uint8_t)255;
					vertices.g = (uint8_t)255;
					vertices.b = (uint8_t)255;
					vertices.a = (uint8_t)255;
					indices = 0;
				}
			}
			// For Segments
			{
#if defined( EAEENGINE_PLATFORM_D3D9 )
				EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 2, {sizeof(DebugVertex), D3DPT_LINELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
				EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr, 2, {sizeof(DebugVertex), GL_LINES, GL_DYNAMIC_DRAW } };
#endif
				_pSegmentsMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &vertices, 1, nullptr, 0, nullptr, 0);
				_pSegmentsMeshRender->SetMesh(_pSegmentsMesh);
			}

			{
#if defined( EAEENGINE_PLATFORM_D3D9 )
				EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 2, { sizeof(DebugVertex), D3DPT_TRIANGLELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
				EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr, 2, { sizeof(DebugVertex), GL_TRIANGLES, GL_DYNAMIC_DRAW } };
#endif
				_pBoxesMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &vertices, 1, &indices, 1, &subMesh, 1);
				_pBoxesMeshRender->SetMesh(_pBoxesMesh);
			}

			{
#if defined( EAEENGINE_PLATFORM_D3D9 )
				EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 2, { sizeof(DebugVertex), D3DPT_TRIANGLELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
				EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr, 2, { sizeof(DebugVertex), GL_TRIANGLES, GL_DYNAMIC_DRAW } };
#endif
				_pShperesMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &vertices, 1, &indices, 1, &subMesh, 1);
				_pShperesMeshRender->SetMesh(_pShperesMesh);
			}

		}

		void DebugMeshes::Clean()
		{
			// DELETE all of the MeshRenders
			SAFE_DELETE(_pSegmentsMeshRender);
			SAFE_DELETE(_pBoxesMeshRender);
			SAFE_DELETE(_pShperesMeshRender);
			// becareful that we cannot use SAFE_RELEASE, 
			// because this marco will set _pSegmentsMesh to NULL.
			// However we need to delete this mesh memory by ourself.
			if (_pSegmentsMesh)
				_pSegmentsMesh->Release();
			SAFE_DELETE(_pSegmentsMesh);
			if (_pBoxesMesh)
				_pBoxesMesh->Release();
			SAFE_DELETE(_pBoxesMesh);
			if (_pShperesMesh)
				_pShperesMesh->Release();
			SAFE_DELETE(_pShperesMesh);
		}

		void DebugMeshes::Update()
		{
			GenerateDebugSegments();
			GenerateDebugBoxes();
			GenerateDebugSpheres();
		}

		void DebugMeshes::GenerateDebugSegments()
		{
			std::vector<DebugShape::DebugSegment>& debugSegments = DebugShape::DebugShapes::GetInstance().GetSegments();
			//  Make sure that the debugSegments has primitives to draw
			if (debugSegments.size() == 0) return;
			// Setup the Vertices Information
			DebugVertex* pVertices = new DebugVertex[debugSegments.size() * 2];
			uint32_t vertexCount = 0;
			for (uint32_t segmentIndex = 0; segmentIndex<debugSegments.size(); ++segmentIndex)
			{
				// Vertex 
				{
					DebugVertex vertex0(debugSegments[segmentIndex]._start, debugSegments[segmentIndex]._color);
					vertex0.a = (uint8_t)255;
					pVertices[vertexCount++] = vertex0;
				}
				{
					DebugVertex vertex1(debugSegments[segmentIndex]._end, debugSegments[segmentIndex]._color);
					vertex1.a = (uint8_t)255;
					pVertices[vertexCount++] = vertex1;
				}
			}
			if (_pSegmentsMesh)
			{
				_pSegmentsMesh->ChangeWholeBuffers(pVertices, vertexCount, nullptr, 0, nullptr, 0);
			}
			SAFE_DELETE_ARRAY(pVertices);

			std::vector<RenderData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderData3DList();
			// Get the TransformMatrix
			RenderData3D renderData = {_pSegmentsMeshRender, 0, Math::ColMatrix44::Identity };
			renderDataList.push_back(renderData);
		}

		void DebugMeshes::GenerateDebugBoxes()
		{
			std::vector<DebugShape::DebugBox>& debugboxes = DebugShape::DebugShapes::GetInstance().GetBoxes();
			//  Make sure that the debugSegments has primitives to draw
			if (debugboxes.size() == 0) return;
			BoxMesh standardBox;
			uint32_t verticesPerBox = (uint32_t)standardBox._vertices.size();
			uint32_t indicesPerBox = (uint32_t)standardBox._indices.size();
			// alloc memory for vertices and indices buffers
			DebugVertex* pVertices = new DebugVertex[debugboxes.size() * verticesPerBox];
			uint32_t* pIndices = new uint32_t[debugboxes.size() * indicesPerBox];
			// Get vertices and indices information for all of the debug meshes
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			for (uint32_t boxIndex = 0; boxIndex < debugboxes.size(); ++boxIndex)
			{
				DebugShape::DebugBox debugbox = debugboxes[boxIndex];
				Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44(debugbox._rotation, debugbox._pos);
				Math::ColMatrix44 scaleMatrix = Math::ColMatrix44(debugbox._extents);
				tranformsMatrix = scaleMatrix * tranformsMatrix;
				// Set the Vertices Information.
				for (uint32_t verticesIndex = 0; verticesIndex < standardBox._vertices.size(); ++verticesIndex)
				{
					Math::Vector4 tempPos = Math::Vector4(standardBox._vertices[verticesIndex].x(), standardBox._vertices[verticesIndex].y(), standardBox._vertices[verticesIndex].z(), 1.0f);
					pVertices[vertexCount] = DebugVertex(tempPos * tranformsMatrix, debugbox._color);
					pVertices[vertexCount].a = (uint8_t)100;
					++vertexCount;
				}
				// Set the Indices Information.
				uint32_t currentIndexCount = boxIndex * verticesPerBox;
				for (uint32_t indicesIndex = 0; indicesIndex < standardBox._indices.size(); ++indicesIndex)
				{
					pIndices[indexCount] = standardBox._indices[indicesIndex] + currentIndexCount;
					++indexCount;
				}
			}
#if defined( EAEENGINE_PLATFORM_D3D9 )
			for (uint32_t index = 0; index < indexCount; index += 3)
			{
				uint32_t temp = pIndices[index + 1];
				pIndices[index + 1] = pIndices[index + 2];
				pIndices[index + 2] = temp;
			}
#endif
			sSubMesh subMesh(0, indexCount - 1);
			if (_pBoxesMesh)
				_pBoxesMesh->ChangeWholeBuffers(pVertices, vertexCount, pIndices, indexCount, &subMesh, 1);
			SAFE_DELETE_ARRAY(pVertices);
			SAFE_DELETE_ARRAY(pIndices);
			std::vector<RenderData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderData3DList();
			// Get the TransformMatrix
			RenderData3D renderData = { _pBoxesMeshRender, 0, Math::ColMatrix44::Identity };
			renderDataList.push_back(renderData);
			/*
			uint32_t vertexCount = standardBox._vertices.size();
			uint32_t indexCount = standardBox._indices.size();
			std::vector<DebugVertex> vertices;
			// Set the Vertices Information.
			for (uint32_t verticesIndex = 0; verticesIndex < standardBox._vertices.size(); ++verticesIndex)
			{
				Math::Vector3 color(1.0f, 1.0f, 1.0f);
				DebugVertex debugVertex = DebugVertex(standardBox._vertices[verticesIndex], color);
				debugVertex.a = (uint8_t)100;
				vertices[verticesIndex] = debugVertex;
			}
#if defined( EAEENGINE_PLATFORM_D3D9 )
			for (uint32_t index = 0; index < indexCount; index += 3)
			{
				uint32_t temp = standardBox._indices[index + 1];
				standardBox._indices[index + 1] = standardBox._indices[index + 2];
				standardBox._indices[index + 2] = temp;
			}
#endif
			sSubMesh subMesh(0, indexCount - 1);
			if (_pBoxesMesh)
			{
				_pBoxesMesh->ChangeWholeBuffers(&standardBox._vertices[0], vertexCount,
					&standardBox._indices[0], indexCount, &subMesh, 1);
				std::vector<sSubMesh> subMeshList;
				subMeshList.push_back(subMesh);
				_pSegmentsMesh->SetSubMeshes(subMeshList);
			}
			std::vector<RenderObj>& renderObjList = RenderObjManager::GetInstance().GetRenderObjList();
			for (uint32_t boxIndex = 0; boxIndex < debugboxes.size(); ++boxIndex)
			{
				// Get the TransformMatrix
				DebugShape::DebugBox debugbox = debugboxes[boxIndex];
				Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44(debugbox._rotation, debugbox._pos);
				Math::ColMatrix44 scaleMatrix = Math::ColMatrix44(debugbox._extents);
				tranformsMatrix = Math::ColMatrix44::Identity;// tranformsMatrix * scaleMatrix;
				// Reset the color.
				for (uint32_t verticesIndex = 0; verticesIndex < standardBox._vertices.size(); ++verticesIndex)
				{
					vertices[verticesIndex].r = debugbox._color.x();
					vertices[verticesIndex].g = debugbox._color.y();
					vertices[verticesIndex].b = debugbox._color.z();
				}

				// Get Material
				MaterialDesc* pMaterial = _pBoxesMeshRender->GetMaterial(0);
				RenderWeight weight;
				weight._material = pMaterial ? pMaterial->_materialCost._cost : 0;
				// Add renderObj
				RenderObj renderObj = { weight, _pBoxesMeshRender, 0, tranformsMatrix };
				renderObjList.push_back(renderObj);
			}
			*/
		}

		void DebugMeshes::GenerateDebugSpheres()
		{
			std::vector<DebugShape::DebugSphere>& debugSpheres = DebugShape::DebugShapes::GetInstance().GetSpheres();
			//  Make sure that the debugSegments has primitives to draw
			if (debugSpheres.size() == 0) return;
			// alloc memory for buffers
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			for (uint32_t sphereIndex = 0; sphereIndex < debugSpheres.size(); ++sphereIndex)
			{
				SphereMesh standardSphere(6, 6);
				vertexCount += (uint32_t)standardSphere._vertices.size();
				indexCount += (uint32_t)standardSphere._indices.size();
			}
			DebugVertex* pVertices = new DebugVertex[vertexCount];
			uint32_t* pIndices = new uint32_t[indexCount];
			// Now let's fill in the Datas
			vertexCount = 0;
			indexCount = 0;
			uint32_t currentIndexOffset = 0;
			// Get vertices and indices information for all of the debug meshes
			for (uint32_t sphereIndex = 0; sphereIndex < debugSpheres.size(); ++sphereIndex)
			{
				DebugShape::DebugSphere debugSphere = debugSpheres[sphereIndex];
				SphereMesh standardSphere(6, 6);
				uint32_t verticesOfThisBox = (uint32_t)standardSphere._vertices.size();
				uint32_t indicesOfThisBox = (uint32_t)standardSphere._indices.size();
				Math::Quaternion identityRotation = Math::Quaternion::Identity;
				Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44(identityRotation, debugSphere._pos);
				{
					Math::ColMatrix44 scaleMatrix = Math::ColMatrix44(Math::Vector3(debugSphere._radius, debugSphere._radius, debugSphere._radius));
					tranformsMatrix = scaleMatrix * tranformsMatrix;
				}
				// Set the Vertices Information.
				for (uint32_t verticesIndex = 0; verticesIndex < verticesOfThisBox; ++verticesIndex)
				{
					Math::Vector3 currentVertex = standardSphere._vertices[verticesIndex];
					Math::Vector4 tempPos = Math::Vector4(currentVertex.x(), currentVertex.y(), currentVertex.z(), 1.0f);
					pVertices[vertexCount] = DebugVertex( tempPos * tranformsMatrix, debugSphere._color);
					pVertices[vertexCount].a = (uint8_t)100;
					++vertexCount;
				}
				// Set the Indices Information.
				for (uint32_t indicesIndex = 0; indicesIndex < indicesOfThisBox; ++indicesIndex)
				{
					pIndices[indexCount] = standardSphere._indices[indicesIndex] + currentIndexOffset;
					++indexCount;
				}
				currentIndexOffset += verticesOfThisBox;
			}
#if defined( EAEENGINE_PLATFORM_D3D9 )
			for (uint32_t index = 0; index < indexCount; index += 3)
			{
				uint32_t temp = pIndices[index + 1];
				pIndices[index + 1] = pIndices[index + 2];
				pIndices[index + 2] = temp;
			}
#endif
			sSubMesh subMesh(0, indexCount - 1);
			if (_pShperesMesh)
				_pShperesMesh->ChangeWholeBuffers(pVertices, vertexCount, pIndices, indexCount, &subMesh, 1);
			SAFE_DELETE_ARRAY(pVertices);
			SAFE_DELETE_ARRAY(pIndices);
			std::vector<RenderData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderData3DList();
			// Get the TransformMatrix
			RenderData3D renderData = {_pShperesMeshRender, 0, Math::ColMatrix44::Identity };
			renderDataList.push_back(renderData);
		}

		////////////////////////////////static_members/////////////////////////////////
		DebugMeshes* DebugMeshes::s_pInternalInstance = nullptr;

		DebugMeshes& DebugMeshes::GetInstance()
		{
			if (!s_pInternalInstance)
				s_pInternalInstance = new DebugMeshes();
			return *s_pInternalInstance;
		}

		void DebugMeshes::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}

	}
}
