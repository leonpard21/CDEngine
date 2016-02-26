#ifndef EAE_ENGINE_GRAPHICS_VERTEXFORMAT_H
#define EAE_ENGINE_GRAPHICS_VERTEXFORMAT_H

#include <cstdint>

namespace EAE_Engine
{
	namespace Mesh
	{
#if defined( EAEENGINE_PLATFORM_D3D9 )
		// This struct determines the layout of the data that the CPU will send to the GPU
		struct sVertex
		{
			// POSITION
			// 3 floats == 12 bytes
			// Offset = 0
			float x, y, z;
			// NORMAL,
			// 3 floats = 12 bytes
			// Offset = 12
			float nx, ny, nz;
			// TextureCoordinate
			// 2 floats == 8 bytes
			// Offset = 24
			float u, v;
			// COLOR0
			// 4 uint8_ts == 4 bytes
			// Offset = 32
			uint8_t b, g, r, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
		};
#elif defined( EAEENGINE_PLATFORM_GL )
		// This struct determines the layout of the data that the CPU will send to the GPU
		struct sVertex
		{
			// POSITION
			// 3 floats == 12 bytes
			// Offset = 0
			float x, y, z;
			// NORMAL,
			// 3 floats = 12 bytes
			// Offset = 12
			float nx, ny, nz;
			// TextureCoordinate
			// 2 floats == 8 bytes
			// Offset = 24
			float u, v;
			// COLOR0
			// 4 uint8_ts == 4 bytes
			// Offset = 32
			uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
		};
#endif

		// The structure represents the subMesh.
		// _first is the first of the index of the Submesh,
		// _last is the last of the index of the Submesh.
		struct sSubMesh
		{
			uint32_t _firstIndex;
			uint32_t _lastIndex;
			sSubMesh() :_firstIndex(0), _lastIndex(0) {}
			sSubMesh(uint32_t firstIndex, uint32_t lastIndex) :_firstIndex(firstIndex), _lastIndex(lastIndex) {}
			inline uint32_t GetIndicesCount() { return _lastIndex + 1 - _firstIndex; }
		};

		enum eVertexElementFormat
		{
			UNKNOW = 0x0,
			FLOAT,
			BYTE,
		};

		enum eVertexSyntax
		{
			POSITION = 0x0,
			NORMAL,
			TEXTCOORD,
			COLOR,
		};

		struct VertexElement
		{
			VertexElement() = default;
			uint32_t _elementCount;
			eVertexElementFormat _type;
			bool _normalized;
			eVertexSyntax _syntax;
		};

		struct VertexFormat 
		{
			VertexFormat() = default;
			VertexElement* _pElements;
			uint32_t _vertexCount;
		};

		bool LoadMeshData(const char* i_binaryMeshFile);
	}
}

#endif//EAE_ENGINE_GRAPHICS_VERTEXFORMAT_H