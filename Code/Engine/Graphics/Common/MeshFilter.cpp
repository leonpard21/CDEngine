#include "MeshFilter.h"
#include "Mesh/AOSMeshData.h"
#include "BinaryFileLoader.h"

namespace EAE_Engine
{
  namespace Graphics
  {

    MeshFilter::MeshFilter(const char* pKey)
    {
      _key = std::string(pKey);
      Mesh::AOSMeshData* pAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(pKey);
      _pMesh = CreateAOSMesh(pAOSMeshData);
    }

    AOSMesh* MeshFilter::GetMesh() const 
    {
      if (_pMesh == nullptr)
        return nullptr;
      Mesh::AOSMeshData* pAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(_key.c_str());
      AOSMesh* pMesh = CreateAOSMesh(pAOSMeshData);
      return pMesh;
    }

  }
}