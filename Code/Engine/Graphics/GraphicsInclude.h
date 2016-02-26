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