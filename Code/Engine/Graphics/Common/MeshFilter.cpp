#include "MeshFilter.h"
#include "Mesh/AOSMeshData.h"
#include "BinaryFileLoader.h"
#include "MeshManager.h"
#include "Windows/WindowsFunctions.h"

namespace EAE_Engine
{
  namespace Graphics
  {

    MeshFilter::MeshFilter(const char* pKey) : 
      _pLocalAOSMesh(nullptr)
    {
      _aosMeshDataKey = GetFileNameWithoutExtension(pKey);
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

    /////////////////////////////////////////////////////////////////////////

    MeshFilterManager::~MeshFilterManager()
    {
      for (std::vector<MeshFilter*>::iterator iter = _meshFilters.begin(); iter != _meshFilters.end(); )
      {
        MeshFilter* pData = *(iter++);
        SAFE_DELETE(pData);
      }
      _meshFilters.clear();
    }

    void MeshFilterManager::AddMeshFilter(MeshFilter* pMeshFilter)
    {
      if (!Contains(pMeshFilter))
      {
        _meshFilters.push_back(pMeshFilter);
      }
    }

    bool MeshFilterManager::Contains(MeshFilter* pMeshFilter)
    {
      for (auto iter : _meshFilters)
      {
        if (iter == pMeshFilter)
        {
          return true;
        }
      }
      return false;
    }


  }
}