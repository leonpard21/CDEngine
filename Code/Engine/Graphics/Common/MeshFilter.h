#ifndef EAE_ENGINE_GRAPHICS_MESH_FILTER_H
#define EAE_ENGINE_GRAPHICS_MESH_FILTER_H

#include "AOSMesh.h"

namespace EAE_Engine
{
  namespace Graphics
  {
    class MeshFilter 
    {
    public:
      MeshFilter(const char* pKey);
      void SetMesh(AOSMesh* pMesh) { _pMesh = pMesh; }
      // like Unity3D, we will return a duplicated mesh
      AOSMesh* GetMesh() const;
      AOSMesh* GetSharedMesh() { return _pMesh; }
    private: 
      std::string _key;
      AOSMesh* _pMesh;
    };
  }
}

#endif//EAE_ENGINE_GRAPHICS_MESH_FILTER_H