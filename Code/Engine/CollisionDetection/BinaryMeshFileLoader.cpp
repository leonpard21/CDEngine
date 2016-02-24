#include "BinaryMeshFileLoader.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include "Math/Vector.h"
#include "UserOutput/Source/Assert.h"
#include "UserOutput/UserOutput.h"
#include "General/MemoryOp.h"
#include "Graphics/GraphicsInclude.h"

// Helper Function Declarations
//=============================
namespace
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
			offset += sizeof(EAE_Engine::Graphics::sVertex) * vertexCount;
			// Set indexOffset
			o_indexOffset = offset;
			offset += sizeof(uint32_t) * indexCount;
			// Set subMeshOffset
			o_subMeshOffset = offset;
			offset += sizeof(EAE_Engine::Graphics::sSubMesh) * subMeshCount;
		}
		infile.close();
		return reinterpret_cast<uint8_t*>(pBuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace EAE_Engine
{
	namespace General
	{
		void LoadMeshData(const char* i_binaryMeshFile, uint8_t* o_pBuffer)
		{
			o_pBuffer = nullptr;
			uint32_t vertexOffset = 0;
			uint32_t vertexCount = 0;
			uint32_t indexOffset = 0;
			uint32_t indexCount = 0;
			uint32_t subMeshOffset = 0;
			uint32_t subMeshCount = 0;
			uint8_t* o_pBuffer = LoadMeshInfo(i_binaryMeshFile, vertexOffset, vertexCount,
				indexOffset, indexCount, subMeshOffset, subMeshCount);
		}

	}
}