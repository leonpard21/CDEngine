#include "MeshFilter.h"
#include "Mesh/AOSMeshData.h"
#include "BinaryFileLoader.h"
#include "MeshManager.h"

namespace EAE_Engine
{
  namespace Graphics
  {

    MeshFilter::MeshFilter(const char* pKey) : 
      _pLocalAOSMesh(nullptr)
    {
      _aosMeshDataKey = std::string(pKey);
    }

    MeshFilter::~MeshFilter() 
    {
      if (_pLocalAOSMesh)
      {
        _pLocalAOSMesh->Release();
        SAFE_DELETE(_pLocalAOSMesh);
      }
    }

    AOSMesh* MeshFilter::GetLocalRenderMesh()
    {
      if (_pLocalAOSMesh == nullptr)
      {
        Mesh::AOSMeshData* pSharedAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(_aosMeshDataKey.c_str());
        if (!pSharedAOSMeshData)
          _pLocalAOSMesh = nullptr;
        else
          _pLocalAOSMesh = CreateAOSMesh(pSharedAOSMeshData);
      }
      return _pLocalAOSMesh;
    }

    AOSMesh* MeshFilter::GetSharedRenderMesh()
    {
      if (_pLocalAOSMesh)
        return _pLocalAOSMesh;
      Mesh::AOSMeshData* pSharedAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(_aosMeshDataKey.c_str());
      if (!pSharedAOSMeshData)
        return nullptr;
      return AOSMeshManager::GetInstance()->GetMesh(_aosMeshDataKey.c_str());
    }

  }
}