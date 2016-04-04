#include "AOSMeshData.h"
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine 
{
	namespace Mesh
	{	
    inline Math::Vector3 GetPos(const sVertex& vertex) 
    {
      return Math::Vector3(vertex.x, vertex.y, vertex.z);
    }

    inline Math::Vector3 GetNormal(const sVertex& vertex)
    {
      return Math::Vector3(vertex.nx, vertex.ny, vertex.nz);
    }

    ///////////////////////////////TriangleIndex//////////////////////////////////////////
    bool TriangleIndex::operator == (const TriangleIndex& i_other)
    {
      if (_index0 != i_other._index0)
        return false;
      if (_index1 != i_other._index1)
        return false;
      if (_index2 != i_other._index2)
        return false;
      return true;
    }

    /////////////////////////////////AOSMeshData////////////////////////////////////////
		Math::Vector3 AOSMeshData::GetVertex(uint32_t vertexIndex)
		{
			uint32_t index = _indices[vertexIndex];
			return Math::Vector3(_vertices[index].x, _vertices[index].y, _vertices[index].z);
		}

		std::vector<sVertex> AOSMeshData::GetVertices(std::vector<uint32_t> indices)
		{
			std::vector<sVertex> result;
			for (std::vector<uint32_t>::iterator indexIt = indices.begin(); indexIt != indices.end(); ++indexIt)
			{
				result.push_back(_vertices[*indexIt]);
			}
			return result;
		}

		std::vector<Math::Vector3> AOSMeshData::GetVertexPoses(std::vector<uint32_t> indices)
		{
			std::vector<Math::Vector3> result;
			for (std::vector<uint32_t>::iterator indexIt = indices.begin(); indexIt != indices.end(); ++indexIt)
			{
				Math::Vector3 vertex(_vertices[*indexIt].x, _vertices[*indexIt].y, _vertices[*indexIt].z);
				result.push_back(vertex);
			}
			return result;
		}

    Math::Vector3 AOSMeshData::GetNormal(TriangleIndex triangle) const
    {
      uint32_t index0 = _indices[triangle._index0];
      uint32_t index1 = _indices[triangle._index1];
      uint32_t index2 = _indices[triangle._index2];
      sVertex v0 = _vertices[index0];
      sVertex v1 = _vertices[index1];
      sVertex v2 = _vertices[index2];
      Math::Vector3 edge0 = (GetPos(v1) - GetPos(v0)).GetNormalize();
      Math::Vector3 edge1 = (GetPos(v2) - GetPos(v1)).GetNormalize();
      return Math::Vector3::Cross(edge0, edge1);
    }

		/////////////////////////////////////////////AOSMeshDataManager//////////////////////////////////////////////
		AOSMeshDataManager::~AOSMeshDataManager() 
		{
			for (std::map<const char*, AOSMeshData*>::iterator iter = _aosMeshDatas.begin(); iter != _aosMeshDatas.end(); )
			{
				char* pKey = const_cast<char*>(iter->first);
				SAFE_DELETE(pKey);
				AOSMeshData* pData = iter++->second;
				SAFE_DELETE(pData);
			}
			_aosMeshDatas.clear();
		}

		bool AOSMeshDataManager::AddAOSMeshData(const char* i_pKey, AOSMeshData* pData)
		{
			for (std::map<const char*, AOSMeshData*>::iterator iter = _aosMeshDatas.begin(); iter != _aosMeshDatas.end(); ++iter)
			{
				const char* pKey = iter->first;
				AOSMeshData* pData = iter->second;
				if (strcmp(pKey, i_pKey) == 0)
					return false;
			}
			_aosMeshDatas.insert(std::pair<const char*, AOSMeshData*>(_strdup(i_pKey), pData));
			return true;
		}

		AOSMeshData* AOSMeshDataManager::GetAOSMeshData(const char* i_pKey)
		{
			for (std::map<const char*, AOSMeshData*>::iterator iter = _aosMeshDatas.begin(); iter != _aosMeshDatas.end(); ++iter)
			{
				const char* pKey = iter->first;
				AOSMeshData* pData = iter->second;
				if (strcmp(pKey, i_pKey) == 0)
					return pData;
			}
			return nullptr;
		}

	}
}
