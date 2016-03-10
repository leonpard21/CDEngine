#include "AOSMeshData.h"
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine 
{
	namespace Mesh
	{	
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
