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
		bool CreateVertexDeclaration(IDirect3DDevice9* pDevice, D3DVERTEXELEMENT9* pVertexElement, IDirect3DVertexDeclaration9*& o_pVertexDeclaration);
		bool CreateVertexBuffer(IDirect3DVertexBuffer9*& o_pVertexBuffer, IDirect3DDevice9* pDevice, const DWORD usage, const uint32_t bufferSize);
		bool FillVertexBuffer(IDirect3DVertexBuffer9*& io_pVertexBuffer, IDirect3DDevice9* pDevice, void* pVertices, const uint32_t bufferSize);
		bool CreateIndexBuffer(IDirect3DIndexBuffer9*& o_pIndexBuffer, IDirect3DDevice9* pDevice, const DWORD usage, const uint32_t bufferSize);
		bool FillIndexBuffer(IDirect3DIndexBuffer9*& io_pVertexBuffer, IDirect3DDevice9* pDevice, void* pIndices, const uint32_t bufferSize);

#elif defined( EAEENGINE_PLATFORM_GL )
		AOSMesh* CreateAOSMeshInternal(MeshGLVertexElements elements,
			void* pVertices, uint32_t vertexCount, 
			uint32_t* pIndices, uint32_t indexCount, 
			sSubMesh* pSubMeshes, uint32_t subMeshCount);
		bool CreateVertexArrayObj(GLuint& io_vertexArrayId);
		bool DeleteVertexArrayObj(GLuint& o_vertexArrayId);
		bool CreateBindBufferObj(GLuint& o_bufferId, GLenum bufferTarget);
		bool DeleteBufferObj(GLuint& io_BufferID, GLsizei bufferCount = 1);
		bool InitVertexFormat(const MeshGLVertexElements& elements, GLsizei stride);
		bool FillGLBuffer(GLenum bufferTarget, void* pVertices, const uint32_t bufferSize, GLenum usage);
#endif
		bool RenderAOSMeshInternal(AOSMesh* pAOSMesh, uint32_t submeshindex = 0);
	}
}

#endif//EAE_ENGINE_GRAPHICS_AOSMESHOP_H