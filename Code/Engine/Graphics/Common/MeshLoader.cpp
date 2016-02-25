#include "BinaryFileLoader.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include "AOSMesh.h"
#include "MeshManager.h"
#include "Mesh/MeshLoader.h"
#include "Engine/Math/Vector.h"
#include "UserOutput/Source/Assert.h"
#include "UserOutput/UserOutput.h"
#include "General/MemoryOp.h"

#if defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

// Helper Function Declarations
//=============================
namespace EAE_Engine
{
	uint8_t* LoadMeshInfo(const char* i_pFile, uint32_t& o_vertexOffset, uint32_t& o_vertexCount, 
		uint32_t& o_indexOffset, uint32_t& o_indexCount, 
		uint32_t& o_subMeshOffset, uint32_t& o_subMeshCount)
	{
		if (!i_pFile)
			return nullptr;
		std::ifstream infile(i_pFile, std::ifstream::binary);
		if (!infile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to load binary mesh file: " << i_pFile;
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			return nullptr;
		}
		// get size of file
		infile.seekg(0, infile.end);
		std::ifstream::pos_type size = infile.tellg();
		infile.seekg(0);
		// allocate memory for file content
		char* pBuffer = new char[(unsigned int)size];
		// read content of infile
		infile.read(pBuffer, size);
		if (!infile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to load all data from: " << i_pFile <<", only " << infile.gcount() << "could be load.";
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			infile.close();
			delete[] pBuffer;
			return nullptr;
		}
		uint32_t offset = 0;
		{
			uint32_t o_elementCount = *reinterpret_cast<uint32_t*>(pBuffer + offset);
			offset += sizeof(uint32_t);
			EAE_Engine::Mesh::VertexElement* o_pVertexElements = reinterpret_cast<EAE_Engine::Mesh::VertexElement*>(pBuffer + offset);
			offset += sizeof(EAE_Engine::Mesh::VertexElement) * o_elementCount;
			// Get VertexCount
			uint32_t vertexCount = *reinterpret_cast<uint32_t*>(pBuffer + offset);
			o_vertexCount = vertexCount;
			offset += sizeof(uint32_t);
			// Get IndexCount
			uint32_t indexCount = *reinterpret_cast<uint32_t*>(pBuffer + offset);
			o_indexCount = indexCount;
			offset += sizeof(uint32_t);
			// Get SubMeshCount
			uint32_t subMeshCount = *reinterpret_cast<uint32_t*>(pBuffer + offset);
			o_subMeshCount = subMeshCount;
			offset += sizeof(uint32_t);
			// Set vertexOffset
			o_vertexOffset = offset;
			offset += sizeof(Mesh::sVertex) * vertexCount;
			// Set indexOffset
			o_indexOffset = offset;
			offset += sizeof(uint32_t) * indexCount;
			// Set subMeshOffset
			o_subMeshOffset = offset;
			offset += sizeof(Graphics::sSubMesh) * subMeshCount;
		}
		infile.close();
		return reinterpret_cast<uint8_t*>(pBuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace EAE_Engine
{
	namespace Graphics
	{
		bool LoadMeshFromBinary(const char* i_binaryMeshFile)
		{
			AOSMesh* pAOSMesh = LoadMeshData(i_binaryMeshFile);
			if (!pAOSMesh)
			{
				SAFE_RELEASE(pAOSMesh);
				SAFE_DELETE(pAOSMesh);
				return false;
			}
			MeshManager::GetMeshManager()->AddAOSMesh(i_binaryMeshFile, pAOSMesh);
			return true;
		}

		AOSMesh* LoadMeshData(const char* i_binaryMeshFile)
		{
			uint32_t vertexOffset = 0;
			uint32_t vertexCount = 0;
			uint32_t indexOffset = 0;
			uint32_t indexCount = 0;
			uint32_t subMeshOffset = 0;
			uint32_t subMeshCount = 0;
			uint8_t* pBuffer = LoadMeshInfo(i_binaryMeshFile, vertexOffset, vertexCount, 
				indexOffset, indexCount, subMeshOffset, subMeshCount);
			if (!pBuffer)
				return nullptr;
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
			MeshD3DVertexElements elements = { elemnt_arr, 4, { sizeof(sVertex), D3DPT_TRIANGLELIST,  Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
			MeshGLVertexElement element_arr[4] = {
				{ 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // Position
				{ 1, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat) },         // Normal
				{ 2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // TextureCoordinate
				{ 3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) }  // Color
			};
			MeshGLVertexElements elements = { element_arr, 4, {sizeof(Mesh::sVertex), GL_TRIANGLES, GL_STATIC_DRAW } };
#endif
			pAOSMesh = CreateAOSMeshInternal(elements, reinterpret_cast<Mesh::sVertex*>(pBuffer + vertexOffset), vertexCount,
				reinterpret_cast<uint32_t*>(pBuffer + indexOffset), indexCount,
				reinterpret_cast<sSubMesh*>(pBuffer + subMeshOffset), subMeshCount);
			SAFE_DELETE_ARRAY(pBuffer);
			return pAOSMesh;
		}

	}
}