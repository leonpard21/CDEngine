#include "MeshFilter.h"
#include "AOSMeshData.h"
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine
{
  namespace Mesh
  {
    MeshFilter::MeshFilter(Common::ITransform* pTransform, const char* pKey) :
      _pTransform(pTransform), 
      _pSharedAOSMeshData(nullptr), _pLocalAOSMeshData(nullptr)
    {
      _pSharedAOSMeshData = AOSMeshDataManager::GetInstance()->GetAOSMeshData(pKey);
    }

    MeshFilter::~MeshFilter() 
    {
      if (_pLocalAOSMeshData)
        SAFE_DELETE(_pLocalAOSMeshData);
    }
    
    void MeshFilter::SetSharedMesh(AOSMeshData* pNewAOSMeshData)
    {
      if (_pLocalAOSMeshData == nullptr) 
      {
        _pSharedAOSMeshData = pNewAOSMeshData;
        return;
      }
      if (_pLocalAOSMeshData)
        SAFE_DELETE(_pLocalAOSMeshData);
      _pSharedAOSMeshData = pNewAOSMeshData;
      _pLocalAOSMeshData = pNewAOSMeshData;
    }

    void MeshFilter::SetMesh(AOSMeshData* pNewAOSMeshData)
    {
      if (_pLocalAOSMeshData)
        SAFE_DELETE(_pLocalAOSMeshData);
      _pLocalAOSMeshData = pNewAOSMeshData;
      _pSharedAOSMeshData = pNewAOSMeshData;
    }

    AOSMeshData* MeshFilter::GetSharedMesh()
    {
      return _pSharedAOSMeshData;
    }

    AOSMeshData* MeshFilter::GetMesh()
    {
      if (!_pLocalAOSMeshData)
        _pLocalAOSMeshData = new AOSMeshData(*_pSharedAOSMeshData);
      return _pLocalAOSMeshData;
    }

    ///////////////////////////////MeshFilterManager/////////////////////////////////////

    MeshFilterManager::~MeshFilterManager()
    {
      for (auto it = _meshFilters.begin(); it != _meshFilters.end(); )
      {
        MeshFilter* pMeshFilter = *it++;
        SAFE_DELETE(pMeshFilter);
      }
      _meshFilters.clear();
    }

    void MeshFilterManager::AddMeshFilter(Common::ITransform* pTransform, const char* pKey)
    {
      MeshFilter* pMeshFilter = new MeshFilter(pTransform, pKey);
      _meshFilters.push_back(pMeshFilter);
    }


  }
}