#ifndef EAE_ENGINE_GRAPHICS_MESH_FILTER_H
#define EAE_ENGINE_GRAPHICS_MESH_FILTER_H

#include "AOSMesh.h"

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
      void SetSharedMesh(Mesh::AOSMeshData* pNewAOSMeshData);
      void SetMesh(Mesh::AOSMeshData* pNewAOSMeshData);
      AOSMesh* GetSharedMesh();
      // like Unity3D, we will return a duplicated mesh
      AOSMesh* GetMesh();
    private: 

    private: 
      Mesh::AOSMeshData* _pAOSMeshData;
      AOSMesh* _pSharedAOSMesh;
      AOSMesh* _pLocalAOSMesh;
    };
  }
}

#endif//EAE_ENGINE_GRAPHICS_MESH_FILTER_H