#ifndef EAE_ENGINE_GRAPHICS_MESH
#define EAE_ENGINE_GRAPHICS_MESH

#include "General/EngineObj.h"
#include "Math/Vector.h"
#include "Color.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Graphics 
	{
		template<typename Vertex>
		class TTriangle 
		{
		public:
			union 
			{
				Vertex _v[3];
				struct 
				{
					Vertex _v0, _v1, _v2;
				};
			};
		};
		typedef TTriangle<uint32_t> TriangleIndex;

		/*
		 * Mesh's Topology.
		 */
		enum MeshTopology 
		{
			TriangleType = 0,
			//Quads,
			//Lines,
			//LineStrip,
			//Points,
		};

		/*
		 * Structure of Array Mesh.
		 * Each array contains a specific attribute, like Position, Normal, Color...
		 
		 * Before we pass the vertiecs the GPU, 
		 * it can also use the same index to load all attributes from each array.
		 * Sometimes, this method are more suitable to deal with data from different 3D format files.

		 * Read http://ogldev.atspace.co.uk/www/tutorial32/tutorial32.html
		 */
		class SOAMesh : public EngineObj
		{
		public:
			SOAMesh();
			~SOAMesh();

			inline size_t SubMeshCount() { return _subMeshes.size(); }
			inline size_t GetVertexCount() { return _vertexCount; }
			void SetVertices(std::vector<Math::Vector3>& vertices);//Assigns a new vertex positions array.
			inline Math::Vector3* GetVertices() { return _pVertices; }

			void SetIndices(std::vector<uint32_t>& indices, uint32_t submesh);// , MeshTopology topology = MeshTopology.TriangleType);
			uint32_t* GetIndices(uint32_t submesh);//Returns the triangle list for the submesh.

			void SetColors(std::vector<Color>& colors);//Set colors of the mesh
			inline Color* GetColors() { return _pColors; }
			//void SetNormals(Math::Vector3* pNormals);
			//void SetTangents(Math::Vector4* pTengents);

		private:
			size_t _vertexCount;               // The whole count of the vertices in this Mesh.
			Math::Vector3* _pVertices;         // Positions of all vertices in this Mesh.
			Color* _pColors;                   // The color of the vertices in this Mesh.
			/*
			 * A submesh is simply a separate indices array. 
			 * Notice that, for a triangle mesh, submesh will be a Triangle List!

			 * When the mesh renderer uses multiple materials, 
			 * you should ensure that there are as many submeshes as materials.
			 */
			struct SubMesh
			{
				SubMesh() : _pIndices(nullptr), _indexCount(0) {}
				~SubMesh(); 
				
				size_t _indexCount;        // Count of Submesh(Triangle List)'s indices
				uint32_t* _pIndices;       // Submesh(Triangle List)'s Indices
				//MeshTopology _topology;    //Topology of Mesh faces. Could be Point, Line, Triangle, Quad...
			};
		
			// If this Mesh is not empty, then it should contains at least one SubMesh.
			// There is no child/mothe relationship between different SubMeshes.
			// They should relying on different Materials, that's the reason we have SubMesh.
			std::vector<SubMesh*> _subMeshes;
			//uint32_t _subMeshCount;                //The number of submeshes. Every material has a separate triangle list.

			
		
			//Math::Vector3* _pNormals; //Normals
			//Vector2* _uv;             //UV
			//Vertec4* _pTangents;      //Tangents in Unity are represented as Vector4, with x,y,z components defining the vector, and w used to flip the binormal if needed
			
			
			//BoneWeight* _pBoneWeight; 
			//Vector3 _bounds;          //Bounds Info of this Mesh
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_MESH