#ifndef EAE_ENGINE_MESH_AOSMESHDATA_H
#define EAE_ENGINE_MESH_AOSMESHDATA_H

#include "MeshLoader.h"
#include "General/Singleton.hpp"
#include <vector>
#include <map>

namespace EAE_Engine 
{
	namespace Mesh
	{	
		struct AOSMeshData 
		{
			std::vector<sVertex> _vertices;
			std::vector<uint32_t> _indices;
			std::vector<sSubMesh> _subMeshes;
		};

		class AOSMeshDataManager : public Singleton<AOSMeshDataManager>
		{
		public:
			~AOSMeshDataManager();
			bool AddAOSMeshData(const char* i_pKey, AOSMeshData* pData);
			AOSMeshData* GetAOSMeshData(const char* i_pKey);

		private:
			std::map<const char*, AOSMeshData*> _aosMeshDatas;
		};

	}
}

#endif//EAE_ENGINE_MESH_AOSMESHDATA_H