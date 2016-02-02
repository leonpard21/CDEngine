#ifndef EAE_ENGINE_GRAPHICS_AOSMESHOP_H
#define EAE_ENGINE_GRAPHICS_AOSMESHOP_H


#include "AOSMesh.h"
#include <cstdint>
#include <cassert>

namespace EAE_Engine 
{
	namespace Graphics 
	{
#if defined( EAEENGINE_PLATFORM_D3D9 )
		AOSMesh* CreateAOSMeshInternal(MeshD3DVertexElements elements,
			void* pVertices, uint32_t vertexCount, 
			uint32_t* pIndices, uint32_t indexCount,
			sSubMesh* pSubMeshes, uint32_t subMeshCount);
#elif defined( EAEENGINE_PLATFORM_GL )
		AOSMesh* CreateAOSMeshInternal(MeshGLVertexElements elements,
			void* pVertices, uint32_t vertexCount, 
			uint32_t* pIndices, uint32_t indexCount, 
			sSubMesh* pSubMeshes, uint32_t subMeshCount);
#endif
		bool RenderAOSMeshInternal(AOSMesh* pAOSMesh, uint32_t submeshindex = 0);
	}
}

#endif//EAE_ENGINE_GRAPHICS_AOSMESHOP_H