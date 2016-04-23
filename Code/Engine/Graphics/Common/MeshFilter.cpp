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
    /*
    void MeshFilter::SetSharedMesh(Mesh::AOSMeshData* pNewAOSMeshData)
    {
      _pSharedAOSMeshData = CreateAOSMesh(_pAOSMeshData);
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
    */

    AOSMesh* MeshFilter::GetSharedMesh() 
    {
      Mesh::AOSMeshData* pSharedAOSMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(_aosMeshDataKey.c_str());
      if (!pSharedAOSMeshData)
        return nullptr;
      return AOSMeshManager::GetInstance()->GetMesh(_aosMeshDataKey.c_str());
    }

    AOSMesh* MeshFilter::GetMesh() 
    {
      if (_pLocalAOSMesh == nullptr)
      {
        Mesh::AOSMeshData aosMeshData;
        Mesh::sVertex sVertex;
        aosMeshData._vertices.push_back(sVertex);
        aosMeshData._indices.push_back(0);
        Mesh::sSubMesh subMesh(0, 1);
        aosMeshData._subMeshes.push_back(subMesh);
        _pLocalAOSMesh = CreateAOSMesh(&aosMeshData);
      }
      return _pLocalAOSMesh;
    }

  }
}