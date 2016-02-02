#ifndef EAE_Engine_GRAPHICS_INCLUDE_H
#define EAE_Engine_GRAPHICS_INCLUDE_H

#include <cstdint>
#include "Engine/General/MemoryOp.h"

#ifdef _DEBUG
#define EAEENGINE_GRAPHICS_AREDEBUGSHADERSENABLED
#endif


// Interface
//==========

namespace EAE_Engine
{
	namespace Graphics
	{
		// The structure represents the subMesh.
		// _first is the first of the index of the Submesh,
		// _last is the last of the index of the Submesh.
		struct sSubMesh 
		{
			uint32_t _firstIndex;
			uint32_t _lastIndex;
			sSubMesh():_firstIndex(0), _lastIndex(0){}
			sSubMesh(uint32_t firstIndex, uint32_t lastIndex) :_firstIndex(firstIndex), _lastIndex(lastIndex) {}
			inline uint32_t GetIndicesCount() { return _lastIndex + 1 - _firstIndex; }
		};

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

		/*
		* This enum describes the type of the shader so that we can know how to compile it.
		*/
		enum ShaderTypes
		{
			Unknown = 0,
			Vertex,
			Fragment,
		};

		/*
		 * This enum describes the render state of an effect
		 */
		enum RenderState
		{
			Alpha = 1 << 0,
			DepthTesting = 1 << 1,
			DepthWriting = 1 << 2,
			FaceCulling = 1 <<3,
		};
		struct sEffect
		{
			char* _pSourcePath;
			char* _pVS;
			char* _pFS;
			uint32_t _renderState;
			sEffect() : _pSourcePath(nullptr), _pVS(nullptr), _pFS(nullptr), _renderState(0){}
			~sEffect() 
			{
				SAFE_DELETE(_pSourcePath);
				SAFE_DELETE(_pVS);
				SAFE_DELETE(_pFS);
			}
		};
	}
}

#endif EAE_Engine_GRAPHICS_INCLUDE_H