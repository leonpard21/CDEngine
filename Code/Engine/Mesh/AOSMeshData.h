#ifndef EAE_ENGINE_MESH_AOSMESHDATA_H
#define EAE_ENGINE_MESH_AOSMESHDATA_H

#include "MeshLoader.h"
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include <vector>
#include <map>

namespace EAE_Engine 
{
	namespace Mesh
	{	
		struct sVertex;
		struct AOSMeshData 
		{
			std::vector<sVertex> _vertices;
			std::vector<uint32_t> _indices;
			std::vector<sSubMesh> _subMeshes;
			Math::Vector3 GetVertex(uint32_t vertexIndex);
			std::vector<sVertex> GetVertices(std::vector<uint32_t> indices);
			std::vector<Math::Vector3> GetVertexPoses(std::vector<uint32_t> indices);
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