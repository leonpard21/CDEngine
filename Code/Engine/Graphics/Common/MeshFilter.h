#ifndef EAE_ENGINE_GRAPHICS_MESH_FILTER_H
#define EAE_ENGINE_GRAPHICS_MESH_FILTER_H

#include "AOSMesh.h"
#include "Engine/General/Singleton.hpp"
#include <string>
#include <vector>

namespace EAE_Engine
{
  namespace Mesh 
  {
    struct AOSMeshData;
  }

  namespace Graphics
  {
    // In Unity3D, 
    // if MeshFilter is a part of an asset object, 
    // quering mesh property is not allowed and only asset mesh can be assigned.
    class MeshFilter 
    {
    public:
      MeshFilter(const char* pKey);
      ~MeshFilter();
      AOSMesh* GetLocalRenderMesh();
      AOSMesh* GetSharedRenderMesh();
    private: 

    private: 
      std::string _aosMeshDataKey;
      AOSMesh* _pLocalAOSMesh;
    };

    class MeshFilterManager : public Singleton<MeshFilterManager> 
    {
    public:
      ~MeshFilterManager();
      void AddMeshFilter(MeshFilter* pMeshFilter);
      bool Contains(MeshFilter* pMeshFilter);

    private:
      std::vector<MeshFilter*> _meshFilters;
    };

  }
}

#endif//EAE_ENGINE_GRAPHICS_MESH_FILTER_H