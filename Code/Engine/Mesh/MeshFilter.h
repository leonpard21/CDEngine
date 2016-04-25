#ifndef EAE_ENGINE_MESH_MESHFILTER_H
#define EAE_ENGINE_MESH_MESHFILTER_H

#include "Engine/General/Singleton.hpp"
#include "Engine/Common/Interfaces.h"
#include <vector>

namespace EAE_Engine
{
  namespace Mesh 
  {
    struct AOSMeshData;
    // In Unity3D, 
    // if MeshFilter is a part of an asset object, 
    // quering mesh property is not allowed and only asset mesh can be assigned.
    class MeshFilter 
    {
    public:
      MeshFilter(Common::ITransform* pTransform, const char* pKey);
      ~MeshFilter();
      void SetSharedMesh(AOSMeshData* pNewAOSMeshData);
      void SetMesh(AOSMeshData* pNewAOSMeshData);
      AOSMeshData* GetSharedMesh();
      // like Unity3D, it will return a duplicated mesh
      AOSMeshData* GetMesh();

    private: 
      // The idea is: _pSharedAOSMeshData should 
      AOSMeshData* _pSharedAOSMeshData;
      AOSMeshData* _pLocalAOSMeshData;
      Common::ITransform* _pTransform;
    };

    class MeshFilterManager : public Singleton<MeshFilterManager> 
    {
    public:
      ~MeshFilterManager();
      void AddMeshFilter(Common::ITransform* pTransform, const char* pKey);
    private:
      std::vector<MeshFilter*> _meshFilters;
    };

  }
}

#endif//EAE_ENGINE_MESH_MESHFILTER_H