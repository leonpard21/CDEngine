#include "AOSMeshData.h"
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine 
{
	namespace Mesh
	{	
		AOSMeshDataManager::~AOSMeshDataManager() 
		{
			for (std::map<const char*, AOSMeshData*>::iterator iter = _aosMeshDatas.begin(); iter != _aosMeshDatas.end(); )
			{
				AOSMeshData* pData = iter++->second;
				SAFE_DELETE(pData);
			}
		}

		bool AOSMeshDataManager::AddAOSMeshData(const char* i_pKey, AOSMeshData* pData)
		{
			for (std::map<const char*, AOSMeshData*>::iterator iter = _aosMeshDatas.begin(); iter != _aosMeshDatas.end(); )
			{
				const char* pKey = iter->first;
				AOSMeshData* pData = iter++->second;
				if (strcmp(pKey, i_pKey) == 0)
					return false;
			}
			_aosMeshDatas.insert(std::pair<const char*, AOSMeshData*>(_strdup(i_pKey), pData));
			return true;
		}

		AOSMeshData* AOSMeshDataManager::GetAOSMeshData(const char* i_pKey)
		{
			for (std::map<const char*, AOSMeshData*>::iterator iter = _aosMeshDatas.begin(); iter != _aosMeshDatas.end(); )
			{
				const char* pKey = iter->first;
				AOSMeshData* pData = iter++->second;
				if (strcmp(pKey, i_pKey) == 0)
					return pData;
			}
			return nullptr;
		}

	}
}
