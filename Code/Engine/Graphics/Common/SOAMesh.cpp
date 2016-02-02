#include "SOAMesh.h"
#include "General/MemoryOp.h"
#include "UserOutput/Source/Assert.h"
#include <cmath>

namespace EAE_Engine
{
	namespace Graphics
	{
		//Clean the submesh
		SOAMesh::SubMesh::~SubMesh()
		{
			SAFE_DELETE_ARRAY(_pIndices);
			_indexCount = 0;
		}

		SOAMesh::SOAMesh() :
			_pVertices(nullptr), _vertexCount(0), 
			_pColors(nullptr)
		{}

		//Clean the SOAMesh
		SOAMesh::~SOAMesh()
		{
			//clean all of the submeshes.
			for (std::vector<SubMesh*>::iterator iter = _subMeshes.begin(); iter != _subMeshes.end();)
			{
				SubMesh* pSubMesh = *iter++;
				SAFE_DELETE(pSubMesh);
			}
			_subMeshes.clear();

			//clean all of the vertices.
			SAFE_DELETE_ARRAY(_pVertices);

			//clean all of the colors.
			SAFE_DELETE_ARRAY(_pColors);
		}

		//Assigns a new vertex positions array.
		void SOAMesh::SetVertices(std::vector<Math::Vector3>& vertices)
		{
			if (vertices.size() == 0)  return;

			_vertexCount = vertices.size();
			_pVertices = new Math::Vector3[_vertexCount];
			for (size_t i = 0; i < _vertexCount; ++i)
			{
				_pVertices[i] = vertices[i];  
			}
		}
		
		// Sets the index buffer for the submesh.
		void SOAMesh::SetIndices(std::vector<uint32_t>& indices, uint32_t submesh)// , MeshTopology topology)
		{
			if (submesh != _subMeshes.size())
			{
				MessagedAssert(submesh == _subMeshes.size(), "Cannot install submesh, because the submesh count is incorrect.");
				return;
			}
			//if topology == TriangleType
			
			if (indices.size() == 0)
			{
				return;
			}
			SubMesh* pSubMesh = new SubMesh();
			pSubMesh->_indexCount = indices.size();
			pSubMesh->_pIndices = new uint32_t[pSubMesh->_indexCount];
			//Copy the indices
			for (size_t i = 0; i < indices.size(); ++i)
			{
				pSubMesh->_pIndices[i] = indices[i];
			}
			_subMeshes.push_back(pSubMesh);
		}

		// Returns the index buffer for the submesh.
		uint32_t* SOAMesh::GetIndices(uint32_t submesh)
		{
			if (_subMeshes.size() <= submesh) return nullptr;

			SubMesh* pSubMesh = _subMeshes[submesh];
			return pSubMesh->_pIndices;
		}
	
		//Vertex colors of the mesh.
		void SOAMesh::SetColors(std::vector<Color>& colors)
		{
			if (colors.size() == 0)
			{
				return;
			}
			MessagedAssert(_vertexCount == colors.size(), "colors' count should be the same with the positions.");
			_pColors = new Color[colors.size()];
			for (size_t i = 0; i < colors.size(); ++i)
			{
				_pColors[i] = colors[i];
			}
		}

	}
}