#include "MeshFilter.h"
#include "Mesh/AOSMeshData.h"
#include "BinaryFileLoader.h"

namespace EAE_Engine
{
  namespace Graphics
  {

    MeshFilter::MeshFilter(const char* pKey) : 
      _pSharedAOSMesh(nullptr), _pLocalAOSMesh(nullptr)
    {
      _pAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(pKey);
      if (!_pAOSMeshData)
        return;
      _pSharedAOSMesh = CreateAOSMesh(_pAOSMeshData);
    }

    MeshFilter::~MeshFilter() 
    {
      if (_pLocalAOSMesh)
      {
        _pLocalAOSMesh->Release();
        SAFE_DELETE(_pLocalAOSMesh);
      }
    }

    void MeshFilter::SetSharedMesh(Mesh::AOSMeshData* pNewAOSMeshData)
    {
      _pSharedAOSMesh = CreateAOSMesh(_pAOSMeshData);
      if (_pLocalAOSMesh)
      {
        _pLocalAOSMesh->Release();
        SAFE_DELETE(_pLocalAOSMesh);
        _pLocalAOSMesh = CreateAOSMesh(_pAOSMeshData);
      }
    }

    void MeshFilter::SetMesh(Mesh::AOSMeshData* pNewAOSMeshData)
    {
      // clear the old local mesh, if instantiated
      if (_pLocalAOSMesh)
      {
        _pLocalAOSMesh->Release();
        SAFE_DELETE(_pLocalAOSMesh);
      }
      _pAOSMeshData = pNewAOSMeshData;
      if (pNewAOSMeshData == nullptr) 
      {
        return;
      }
      _pLocalAOSMesh = CreateAOSMesh(_pAOSMeshData);
    }

    AOSMesh* MeshFilter::GetSharedMesh() 
    {
      return _pSharedAOSMesh;
    }

    AOSMesh* MeshFilter::GetMesh() 
    {
      if (_pSharedAOSMesh == nullptr || _pAOSMeshData == nullptr)
        return nullptr;
      if (!_pLocalAOSMesh)
        _pLocalAOSMesh = CreateAOSMesh(_pAOSMeshData);
      return _pLocalAOSMesh;
    }

  }
}