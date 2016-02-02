#include "MeshManager.h"
#include "AOSMesh.h"
#include "General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		void MeshManager::AddAosMesh(const char* pMeshKey, AOSMesh* pAOSMesh)
		{
			if(pAOSMesh)
				_meshMap.insert(std::pair<const char*, AOSMesh*>(pMeshKey, pAOSMesh));
		}

		void MeshManager::CleanList()
		{
			if (_meshMap.size() == 0)  return;
			//clean all of the submeshes.
			for (std::map<const char*, AOSMesh*>::iterator iter = _meshMap.begin(); iter != _meshMap.end();)
			{
				AOSMesh* pAOSMesh = iter++->second;
				pAOSMesh->Release();
				SAFE_DELETE(pAOSMesh);
			}
			_meshMap.clear();
		}


		const std::map<const char*, AOSMesh*>& MeshManager::GetAOSMeshes() const
		{
			return _meshMap;
		}

		AOSMesh* MeshManager::GetMesh(const char* pKeyName)
		{
			std::map<const char*, AOSMesh*>::iterator iter = _meshMap.find(pKeyName);
			if (iter == _meshMap.end())
				return nullptr;
			return iter->second;
		}

////////////////////////////static_members/////////////////////////////
		MeshManager* MeshManager::s_pMeshManager = nullptr;

		MeshManager* MeshManager::GetMeshManager()
		{
			if (s_pMeshManager == NULL)
			{
				s_pMeshManager = new EAE_Engine::Graphics::MeshManager();
			}
			return s_pMeshManager;
		}

		void MeshManager::CleanMeshManager()
		{
			if (s_pMeshManager == nullptr) return;
			s_pMeshManager->CleanList();
			SAFE_DELETE(s_pMeshManager);
		}

	}
}