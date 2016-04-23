#ifndef EAE_ENGINE_MESH_MANAGER_H
#define EAE_ENGINE_MESH_MANAGER_H

#include <unordered_map>
#include "GraphicsInternal.h"
#include "Engine/General/Singleton.hpp"

namespace EAE_Engine 
{
	namespace Graphics 
	{
		class AOSMesh;
		class AOSMeshManager : public Singleton<AOSMeshManager>
		{
		public:
      ~AOSMeshManager();
			void AddAOSMesh(const char* pMeshKey, AOSMesh* pAOSMesh);
			void CleanList();
			const std::unordered_map<const char*, AOSMesh*>& GetAOSMeshes() const;
			AOSMesh* GetMesh(const char* pKeyName);
		private:
			std::unordered_map<const char*, AOSMesh*> _meshMap;
		};
	}
}



#endif//EAE_ENGINE_MESH_MANAGER_H
