#include "BinaryFileLoader.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include "AOSMesh.h"
#include "MeshManager.h"
#include "Engine/Mesh/AOSMeshData.h"
#include "Engine/Math/Vector.h"
#include "General/MemoryOp.h"
#include "Windows/WindowsFunctions.h"
#include "Mesh/AOSMeshData.h"

#if defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace EAE_Engine
{
	namespace Graphics
	{
		bool LoadMeshFromBinary(const char* i_binaryMeshFile)
		{
			Mesh::LoadMeshData(i_binaryMeshFile);
			AOSMesh* pAOSMesh = LoadMeshData(i_binaryMeshFile);
			if (!pAOSMesh)
			{
				SAFE_RELEASE(pAOSMesh);
				SAFE_DELETE(pAOSMesh);
				return false;
			}
			AOSMeshManager::GetInstance()->AddAOSMesh(i_binaryMeshFile, pAOSMesh);
			return true;
		}

		AOSMesh* LoadMeshData(const char* i_binaryMeshFile)
		{
			std::string key = GetFileNameWithoutExtension(i_binaryMeshFile);
			Mesh::AOSMeshData* pData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(key.c_str());
			if (!pData)
				return nullptr;
			AOSMesh* pAOSMesh = CreateAOSMesh(pData);
      return pAOSMesh;
		}

    AOSMesh* CreateAOSMesh(Mesh::AOSMeshData* pAOSMeshData)
    {
      AOSMesh* pAOSMesh = nullptr;
#if defined( EAEENGINE_PLATFORM_D3D9 )
      // These elements must match the VertexFormat::sVertex layout struct exactly.
      // They instruct Direct3D how to match the binary data in the vertex buffer
      // to the input elements in a vertex shader
      // (by using D3DDECLUSAGE enums here and semantics in the shader,
      // so that, for example, D3DDECLUSAGE_POSITION here matches with POSITION in shader code).
      // Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
      D3DVERTEXELEMENT9 elemnt_arr[] = {
        // Stream: 0, POSITION: 3 floats == 12 bytes, Offset = 0
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        // Stream: 0, Normal: 3 floats == 12 bytes, Offsets = 12
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        // Stream: 0, TextureCoordinate0: 2 float == 8 bytes, Offset = 20
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        // Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 12
        { 0, 32, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }
      };
      MeshD3DVertexElements elements = { elemnt_arr, 4,{ sizeof(Mesh::sVertex), D3DPT_TRIANGLELIST,  Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
      MeshGLVertexElement element_arr[4] = {
        { 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // Position
        { 1, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat) },         // Normal
        { 2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // TextureCoordinate
        { 3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) }  // Color
      };
      MeshGLVertexElements elements = { element_arr, 4,{ sizeof(Mesh::sVertex), GL_TRIANGLES, GL_STATIC_DRAW } };
#endif
      uint32_t* pIndices = nullptr;
      uint32_t indexCount = (uint32_t)pAOSMeshData->_indices.size();
      Mesh::sSubMesh* pSubMeshes = nullptr;
      uint32_t subMeshCount = (uint32_t)pAOSMeshData->_subMeshes.size();
      if (indexCount > 0)
      {
        pIndices = &pAOSMeshData->_indices[0];
        indexCount = (uint32_t)pAOSMeshData->_indices.size();
      }
      if(subMeshCount > 0)
      {
        pSubMeshes = &pAOSMeshData->_subMeshes[0];
        subMeshCount = (uint32_t)pAOSMeshData->_subMeshes.size();
      }
      pAOSMesh = CreateAOSMeshInternal(elements, &pAOSMeshData->_vertices[0], (uint32_t)pAOSMeshData->_vertices.size(),
        pIndices, indexCount, pSubMeshes, subMeshCount);
      return pAOSMesh;
    }

	}
}