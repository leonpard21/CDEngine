#include "MeshManager.h"
#include "AOSMesh.h"
#include "General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Graphics
	{
    AOSMeshManager::~AOSMeshManager()
    {
      CleanList();
    }

		void AOSMeshManager::AddAOSMesh(const char* pMeshKey, AOSMesh* pAOSMesh)
		{
			if(pAOSMesh)
				_meshMap.insert(std::pair<const char*, AOSMesh*>(pMeshKey, pAOSMesh));
		}

		void AOSMeshManager::CleanList()
		{
			if (_meshMap.size() == 0)  return;
			//clean all of the submeshes.
			for (std::unordered_map<const char*, AOSMesh*>::iterator iter = _meshMap.begin(); iter != _meshMap.end();)
			{
				AOSMesh* pAOSMesh = iter++->second;
				pAOSMesh->Release();
				SAFE_DELETE(pAOSMesh);
			}
			_meshMap.clear();
		}


		const std::unordered_map<const char*, AOSMesh*>& AOSMeshManager::GetAOSMeshes() const
		{
			return _meshMap;
		}

		AOSMesh* AOSMeshManager::GetMesh(const char* pKeyName)
		{
			auto iter = _meshMap.find(pKeyName);
			if (iter == _meshMap.end())
				return nullptr;
			return iter->second;
		}

	}
}