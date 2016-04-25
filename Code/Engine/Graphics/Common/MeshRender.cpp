#include "MeshRender.h"
#include "../Common/AOSMesh.h"
#include "../Common/Effect.h"
#include "../Common/MeshManager.h"
#include "../Common/Camera.h"
#include "../Common/Material.h"
#include "RenderObj.h"
#include "RenderDatas.h"
#include  <cassert>

namespace EAE_Engine
{
	namespace Graphics
	{
		AOSMeshRender::AOSMeshRender() :
			_pSharedMesh(nullptr), _pTrans(nullptr)
		{
		}

		AOSMeshRender::~AOSMeshRender() 
		{
			for (std::vector<MaterialDesc*>::iterator it = _localMaterials.begin(); it != _localMaterials.end(); )
			{
        MaterialDesc* pLocalMaterial = *it++;
        uint8_t* pBuffer = (uint8_t*)pLocalMaterial;
        SAFE_DELETE_ARRAY(pBuffer);
			}
      _localMaterials.clear();
      _sharedMaterials.clear();
		}

		void AOSMeshRender::SetSharedMesh(const char* pMeshName)
		{
			_pSharedMesh = AOSMeshManager::GetInstance()->GetMesh(pMeshName);
		}

		AOSMesh* AOSMeshRender::GetSharedMesh()
		{
			return _pSharedMesh;
		}

		void AOSMeshRender::AddMaterial(std::string materialkey)
		{
			MaterialDesc* pMaterial = MaterialManager::GetInstance()->GetMaterialDesc(materialkey.c_str());
      _sharedMaterials.push_back(pMaterial);
      _localMaterials.push_back(nullptr);
		}

    MaterialDesc* AOSMeshRender::GetSharedMaterial(uint32_t index)
    {
      if (_sharedMaterials.size() == 0)
        return nullptr;
      else if (index >= _sharedMaterials.size())
        return _sharedMaterials[0];
      return _sharedMaterials[index];
    }

    MaterialDesc* AOSMeshRender::GetMaterial(uint32_t index)
    {
      if (_localMaterials.size() == 0 || index >= _sharedMaterials.size())
        return nullptr;
      else if (index < _localMaterials.size())
        return _localMaterials[index];
      // Copy the material and save it to the local material list.
      CopySharedMaterialToLocal(index);
      return _localMaterials[index];
    }

    void AOSMeshRender::SetSharedMaterial(uint32_t index, std::string materialkey)
    {
      if (index >= _sharedMaterials.size())
        return;
      MaterialDesc* pNewMaterial = MaterialManager::GetInstance()->GetMaterialDesc(materialkey.c_str());
      _sharedMaterials[index] = pNewMaterial;
      // also remember to clean the localMaterial(if instantiated)
      if (_localMaterials[index] != nullptr)
      {
        uint8_t* pBuffer = (uint8_t*)_localMaterials[index];
        SAFE_DELETE_ARRAY(pBuffer);
        _localMaterials[index] = nullptr;
        // generate the new local material in the local list. 
        CopySharedMaterialToLocal(index);
      }
    }

    void AOSMeshRender::SetMaterial(uint32_t index, MaterialDesc* pNewMaterial)
    {
      if (index >= _localMaterials.size())
        return;
      // delete the previous local material, if exists. 
      if (_localMaterials[index] != nullptr)
      {
        uint8_t* pBuffer = (uint8_t*)_localMaterials[index];
        SAFE_DELETE_ARRAY(pBuffer);
      }
      _localMaterials[index] = pNewMaterial;
    }

    bool AOSMeshRender::CopySharedMaterialToLocal(uint32_t index)
    {
        MaterialDesc* pMaterial = _sharedMaterials[index];
        if (!pMaterial)
          return false;
        uint32_t materialBufferSize = pMaterial->_sizeOfMaterialBuffer;
        uint8_t* pNewMaterial = new uint8_t[materialBufferSize];
        CopyMem((uint8_t*)pMaterial, pNewMaterial, materialBufferSize);
        _localMaterials[index] = (MaterialDesc*)pNewMaterial;
        return true;
    }

		//////////////////////////////RenderObjManager///////////////////////////

		AOSMeshRender* AOSMeshRenderManager::AddMeshRender(const char* pAOSMesh, Common::ITransform* pTransform)
		{
			AOSMeshRender* pRO = new AOSMeshRender();
			pRO->SetSharedMesh(pAOSMesh);
			pRO->SetTrans(pTransform);
			_meshRenders.push_back(pRO);
			return pRO;
		}
		void AOSMeshRenderManager::UpdateRenderDataList()
		{
			std::vector<RenderData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderData3DList();
			for (std::vector<AOSMeshRender*>::iterator it = _meshRenders.begin(); it != _meshRenders.end(); ++it)
			{
				AOSMesh* pAOSMesh = (*it)->GetSharedMesh();
				if (pAOSMesh == nullptr) 
					continue;
				for (uint32_t submeshindex = 0; submeshindex < pAOSMesh->GetSubMeshCount(); ++submeshindex)
				{
					Common::ITransform* pTrans = (*it)->GetTransform();
					RenderData3D renderData = { (*it), submeshindex, pTrans };
					renderDataList.push_back(renderData);
				}
			}
		}

		void AOSMeshRenderManager::Clean() 
		{
			for (std::vector<AOSMeshRender*>::iterator iter = _meshRenders.begin(); iter != _meshRenders.end();)
			{
				AOSMeshRender* pObj = *iter++;
				SAFE_DELETE(pObj);
			}
			_meshRenders.clear();
		}

		void AOSMeshRenderManager::Remove(Common::ITransform* pTransform)
		{
			for (std::vector<AOSMeshRender*>::iterator iter = _meshRenders.begin(); iter != _meshRenders.end(); ++iter)
			{
				AOSMeshRender* pObj = *iter;
				if (pObj->GetTransform() == pTransform)
				{
					_meshRenders.erase(iter);
					SAFE_DELETE(pObj);
					break;
				}
			}
		}

		////////////////////////////////static_members/////////////////////////////////
		AOSMeshRenderManager* AOSMeshRenderManager::s_pInternalInstance = nullptr;

		AOSMeshRenderManager& AOSMeshRenderManager::GetInstance()
		{ 
			if (!s_pInternalInstance)
				s_pInternalInstance = new AOSMeshRenderManager();
			return *s_pInternalInstance; 
		}

		void AOSMeshRenderManager::CleanInstance() 
		{ 
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean(); 
			SAFE_DELETE(s_pInternalInstance);
		}

	}
}