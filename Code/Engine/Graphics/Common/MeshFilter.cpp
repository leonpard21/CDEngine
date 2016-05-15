#include "MeshFilter.h"
#include "Mesh/AOSMeshData.h"
#include "BinaryFileLoader.h"
#include "MeshManager.h"
#include "Windows/WindowsFunctions.h"

namespace EAE_Engine
{
  namespace Graphics
  {

    MeshFilter::MeshFilter() : 
      _pLocalAOSMesh(nullptr)
    {
      
    }

    MeshFilter::~MeshFilter() 
    {
      CleanLocalAOSMesh();
    }

    void MeshFilter::SetLocalRenderMesh(AOSMesh* pAOSMesh)
    {
      CleanLocalAOSMesh();
      _pLocalAOSMesh = pAOSMesh;
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

    void MeshFilter::SetSharedRenderMesh(const char* pMeshPath)
    {
      _aosMeshDataKey = GetFileNameWithoutExtension(pMeshPath);
      CleanLocalAOSMesh();
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

    void MeshFilter::CleanLocalAOSMesh()
    {
      if (_pLocalAOSMesh)
      {
        _pLocalAOSMesh->Release();
        SAFE_DELETE(_pLocalAOSMesh);
      }
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