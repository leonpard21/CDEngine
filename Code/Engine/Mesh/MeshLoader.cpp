#include "MeshLoader.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include "Engine/Math/Vector.h"
#include "General/MemoryOp.h"
#include "AOSMeshData.h"
#include "Windows/WindowsFunctions.h"

// Helper Function Declarations
//=============================
namespace EAE_Engine
{
	uint8_t* LoadMeshInfo(const char* i_pFile,
		uint32_t& o_vertexElementCount, Mesh::VertexElement*& o_pVertexElements, 
		uint32_t& o_vertexOffset, uint32_t& o_vertexCount,
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
			decoratedErrorMessage << "Failed to load all data from: " << i_pFile << ", only " << infile.gcount() << "could be load.";
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			infile.close();
			delete[] pBuffer;
			return nullptr;
		}
		uint32_t offset = 0;
		{
			o_vertexElementCount = *reinterpret_cast<uint32_t*>(pBuffer + offset);
			offset += sizeof(uint32_t);
			o_pVertexElements = reinterpret_cast<EAE_Engine::Mesh::VertexElement*>(pBuffer + offset);
			offset += sizeof(EAE_Engine::Mesh::VertexElement) * o_vertexElementCount;
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
			offset += sizeof(Mesh::sSubMesh) * subMeshCount;
		}
		infile.close();
		return reinterpret_cast<uint8_t*>(pBuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace EAE_Engine
{
	namespace Mesh
	{
		bool LoadMeshData(const char* i_binaryMeshFile)
		{
			uint32_t vertexElementCount = 0;
			VertexElement* pVertexElement = nullptr;
			uint32_t vertexOffset = 0;
			uint32_t vertexCount = 0;
			uint32_t indexOffset = 0;
			uint32_t indexCount = 0;
			uint32_t subMeshOffset = 0;
			uint32_t subMeshCount = 0;
			uint8_t* pBuffer = LoadMeshInfo(i_binaryMeshFile, vertexElementCount, pVertexElement,
				vertexOffset, vertexCount, indexOffset, indexCount, subMeshOffset, subMeshCount);
			AOSMeshData* pAOSMeshData = new AOSMeshData();
			sVertex* pVertices = (sVertex*)(pBuffer + vertexOffset);
			for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				sVertex& vertex = pVertices[vertexIndex];
				pAOSMeshData->_vertices.push_back(vertex);
			}
			for (uint32_t index = 0; index < indexCount; ++index)
			{
				uint32_t& indexValue = ((uint32_t*)(pBuffer + indexOffset))[index];
				pAOSMeshData->_indices.push_back(indexValue);
			}
			sSubMesh* pSubMeshes = (sSubMesh*)(pBuffer + subMeshOffset);
			for (uint32_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
			{
				sSubMesh* pSubMesh = pSubMeshes + subMeshIndex;
				pAOSMeshData->_subMeshes.push_back(*pSubMesh);
			}
			std::string mesh_path(i_binaryMeshFile);
			std::string key = GetFileNameWithoutExtension(mesh_path.c_str());
			SAFE_DELETE_ARRAY(pBuffer);
			bool result = AOSMeshDataManager::GetInstance()->AddAOSMeshData(key.c_str(), pAOSMeshData);
			return result;
		}


	}
}