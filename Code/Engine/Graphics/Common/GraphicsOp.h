#ifndef EAE_ENGINE_GRAPHICS_GRAPHICSOP_H
#define EAE_ENGINE_GRAPHICS_GRAPHICSOP_H


#include "AOSMesh.h"
#include <cstdint>
#include <cassert>

namespace EAE_Engine 
{
	namespace Graphics 
	{
#if defined( EAEENGINE_PLATFORM_D3D9 )

		HRESULT GetVertexProcessingUsage(IDirect3DDevice9* pDevice, DWORD& o_usage);
		bool CreateVertexDeclaration(IDirect3DDevice9* pDevice, D3DVERTEXELEMENT9* pVertexElement, IDirect3DVertexDeclaration9*& o_pVertexDeclaration);
		bool CreateVertexBuffer(IDirect3DVertexBuffer9*& o_pVertexBuffer, IDirect3DDevice9* pDevice, const DWORD usage, const uint32_t bufferSize);
		bool FillVertexBuffer(IDirect3DVertexBuffer9*& io_pVertexBuffer, IDirect3DDevice9* pDevice, void* pVertices, const uint32_t bufferSize);
		bool CreateIndexBuffer(IDirect3DIndexBuffer9*& o_pIndexBuffer, IDirect3DDevice9* pDevice, const DWORD usage, const uint32_t bufferSize);
		bool FillIndexBuffer(IDirect3DIndexBuffer9*& io_pVertexBuffer, IDirect3DDevice9* pDevice, void* pIndices, const uint32_t bufferSize);
		
#elif defined( EAEENGINE_PLATFORM_GL )
		// Buffer Helpers
		bool CreateVertexArrayObj(GLuint& io_vertexArrayId);
		bool DeleteVertexArrayObj(GLuint& o_vertexArrayId);
		bool CreateBindBufferObj(GLuint& o_bufferId, GLenum bufferTarget);
		bool DeleteBufferObj(GLuint& io_BufferID, GLsizei bufferCount = 1);
		bool InitVertexFormat(const MeshGLVertexElements& elements, GLsizei stride);
		bool FillGLBuffer(GLenum bufferTarget, void* pVertices, const uint32_t bufferSize, GLenum usage);
		// Effect Helpers
		bool CreateProgram(GLuint& o_programId);
		bool DeleteProgram(GLuint& i_programId);
		bool IsRunTimeCompilingShdSupported();
		bool LoadCompileShader(GLuint& o_shaderId, const char* i_pShaderFilePath, GLenum shaderType);
		bool AttachShaderToProgram(GLuint& io_programId, GLuint& i_shaderId);
		bool DeleteShader(GLuint& i_shaderId);
		bool LinkProgramAfterAttached(GLuint& io_programId);
#endif

	}
}

#endif//EAE_ENGINE_GRAPHICS_GRAPHICSOP_H