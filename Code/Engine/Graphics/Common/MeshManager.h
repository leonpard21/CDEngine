#ifndef EAE_ENGINE_MESH_MANAGER_H
#define EAE_ENGINE_MESH_MANAGER_H

#include <map>
#include "GraphicsInternal.h"

namespace EAE_Engine 
{
	namespace Graphics 
	{
		class AOSMesh;
		class MeshManager
		{
		public:
			void AddAOSMesh(const char* pMeshKey, AOSMesh* pAOSMesh);
			void CleanList();
			const std::map<const char*, AOSMesh*>& GetAOSMeshes() const;
			AOSMesh* GetMesh(const char* pKeyName);
		private:
			std::map<const char*, AOSMesh*> _meshMap;

			//////////////////////////////////static_members////////////////////////////////
		public:
			static MeshManager* GetMeshManager();
			static void CleanMeshManager();
		private:
			static MeshManager* s_pMeshManager;
		};
	}
}



#endif//EAE_ENGINE_MESH_MANAGER_H
