#ifndef EAE_ENGINE_GRAPHICS_MESHHEADER
#define EAE_ENGINE_GRAPHICS_MESHHEADER

#include "../Graphics.h"
#include "Engine/Mesh/MeshLoader.h"
#include "Engine/General/MemoryOp.h"
#include <vector>
#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif


namespace EAE_Engine
{
	namespace Graphics
	{
		/*
		* Array Of Strcture Mesh.
		* Structure will be the Vertex type we will pass in.
		* These kind of Vertices info should be used to pass data to GPU.

		* Read https://developer.apple.com/library/ios/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
		*/
		class AOSMesh;//whatever the platform is, we must have an implemented AOSMesh.
#if defined( EAEENGINE_PLATFORM_D3D9 )
		enum UsageInfo
		{
			Default = 0x0L,
			Dynamic = D3DUSAGE_DYNAMIC,
			Depth = D3DUSAGE_DEPTHSTENCIL,
			RenderTarget = D3DUSAGE_RENDERTARGET,
		};

		struct BufferInfo
		{
			BufferInfo() = default;
			WORD _stride;
			D3DPRIMITIVETYPE _primitiveType;
			DWORD _usage;
		};

		struct MeshD3DVertexElements
		{
			MeshD3DVertexElements() = default;
			~MeshD3DVertexElements() {}
			D3DVERTEXELEMENT9* _pGlements;
			WORD _elementCount;
			BufferInfo _bufferInfo;
		};

		class AOSMesh
		{
		public:
			AOSMesh() : _pVertexBuffer(NULL), _pVertexDeclaration(NULL), 
				_vertexCount(0), _pIndexBuffer(NULL), _indexCount(0), _bufferInfo({0, D3DPT_TRIANGLELIST, 0x0L}) {}
			~AOSMesh();

			bool CreateBuffers(IDirect3DDevice9* pDevice, MeshD3DVertexElements vertexElement,
				void* pVertices, uint32_t vertexDataCount, 
				uint32_t* pIndeices, uint32_t indexCount, 
				Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);
			void Release();
			void ChangeWholeBuffers(void* pVertices, uint32_t vertexDataCount, uint32_t* pIndeices, uint32_t indexCount, Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);;

			inline IDirect3DVertexBuffer9* GetVertexBuffer() const { return _pVertexBuffer; };
			inline uint32_t GetVertexCount() const { return _vertexCount; }
			inline IDirect3DIndexBuffer9* GetIndexBuffer() const { return _pIndexBuffer; }
			inline uint32_t GetIndexCount() const { return _indexCount; }
			inline uint32_t GetVertexStride() const { return _bufferInfo._stride; }
			inline D3DPRIMITIVETYPE GetPrimitiveType() const { return _bufferInfo._primitiveType; }
			inline IDirect3DVertexDeclaration9* GetVertexDeclaration() const { return _pVertexDeclaration; }
			inline uint32_t GetSubMeshCount() const { return (uint32_t)_subMeshes.size(); }
			Mesh::sSubMesh* GetSubMesh(uint32_t SubMeshIndex);
			void SetSubMeshes(std::vector<Mesh::sSubMesh>& subMeshes);

		private:
			bool InitVertexElements(IDirect3DDevice9* pDevice, MeshD3DVertexElements vertexElement);
			bool InitVertexBuffer(IDirect3DDevice9* pDevice, void* pVertices, uint32_t vertexCount);
			bool InitIndexBuffer(IDirect3DDevice9* pDevice, uint32_t* pIndeices, uint32_t indexCount,
				Mesh::sSubMesh* pSubMeshes, uint32_t SubMeshCount);
			bool InitUsage(IDirect3DDevice9* pDevice, UsageInfo usageInfo);

		private:
			uint32_t _indexCount;
			IDirect3DIndexBuffer9* _pIndexBuffer;
			uint32_t _vertexCount;
			IDirect3DVertexBuffer9* _pVertexBuffer;
			IDirect3DVertexDeclaration9* _pVertexDeclaration;
			BufferInfo _bufferInfo;
			std::vector<Mesh::sSubMesh> _subMeshes;
		};

		AOSMesh* CreateAOSMeshInternal(MeshD3DVertexElements elements,
			void* pVertices, uint32_t vertexCount,
			uint32_t* pIndices, uint32_t indexCount,
			Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);

#elif defined( EAEENGINE_PLATFORM_GL )
		struct MeshGLVertexElement
		{
			MeshGLVertexElement() = default;
			~MeshGLVertexElement() {}
			GLuint _vertexElementLocation; // the location must match the one you used in the vertex shader for the input
			GLint _elementCount;
			GLenum _type;
			GLboolean _normalized;
			GLuint _sizeOfType;
		};

		struct BufferInfo
		{
			BufferInfo() = default;
			GLsizei _stride;
			GLenum _primitiveMode;
			GLsizei _usage;
		};

		struct MeshGLVertexElements 
		{
			MeshGLVertexElements() = default;
			~MeshGLVertexElements() {}
			MeshGLVertexElement* _pGlements;
			GLsizei _elementCount;
			BufferInfo _bufferInfo;
		};

		class AOSMesh
		{
		public:
			AOSMesh();
			~AOSMesh();

			bool CreateBuffers(MeshGLVertexElements elements,
				void* pVertices, uint32_t vertexCount,
				uint32_t* pIndexData, uint32_t indexCount, 
				Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);
			void Release();
			void ChangeWholeBuffers(void* pVertices, uint32_t vertexCount, 
				uint32_t* pIndexData, uint32_t indexCount, Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);
			inline GLuint GetVertexArrayID() const { return _vertexArrayId; }
			inline GLuint GetVertexCount() const { return _vertexCount; }
			inline GLuint GetIndexCount() const { return _indexCount; }
			inline GLenum GetPrimitiveMode() const { return _bufferInfo._primitiveMode; }
			inline uint32_t GetSubMeshCount() const { return (uint32_t)_subMeshes.size(); }
			Mesh::sSubMesh* GetSubMesh(uint32_t SubMeshIndex);
			void SetSubMeshes(std::vector<Mesh::sSubMesh>& subMeshes);

		private:
			bool CreateVertexBufferElements(MeshGLVertexElements elements, void* pVertices, uint32_t vertexCount);
			bool CreateIndexBuffer(uint32_t* pIndexData, uint32_t indexCount, Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);
			////////////////////////////////////Member Variables/////////////////////////
		private:
			// A vertex array encapsulates both the vertex and index data as well as the vertex format
			GLuint _vertexArrayId;
			GLuint _vertexBufferId;
			GLuint _indexBufferId;
			GLuint _vertexCount;
			GLuint _indexCount;
			BufferInfo _bufferInfo;
			std::vector<Mesh::sSubMesh> _subMeshes;
		};

		AOSMesh* CreateAOSMeshInternal(MeshGLVertexElements elements,
			void* pVertices, uint32_t vertexCount,
			uint32_t* pIndices, uint32_t indexCount,
			Mesh::sSubMesh* pSubMeshes, uint32_t subMeshCount);

#endif
		bool RenderAOSMeshInternal(AOSMesh* pAOSMesh, uint32_t submeshindex = 0);
	}
}

#endif//EAE_ENGINE_GRAPHICS_MESHHEADER