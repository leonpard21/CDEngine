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
			_pMesh(nullptr), _pTrans(nullptr)
		{
		}

		AOSMeshRender::~AOSMeshRender() 
		{
			for (std::vector<MaterialDesc*>::iterator it = _materials.begin(); it != _materials.end();)
			{
				MaterialDesc* pLocalMaterial = *it++;
				uint8_t* pBuffer = (uint8_t*)pLocalMaterial;
				SAFE_DELETE_ARRAY(pBuffer);
			}
			_materials.clear();
		}

		Graphics::MaterialDesc* AOSMeshRender::GetSharedMaterial(uint32_t index)
		{ 
			if (_materials.size() == 0)
				return nullptr;
			else if (index >= _materials.size())
				return _materials[0];
			return _materials[index];
		}


		void AOSMeshRender::SetMesh(const char* pMeshName)
		{
			_pMesh = MeshManager::GetMeshManager()->GetMesh(pMeshName);
		}

		AOSMesh* AOSMeshRender::GetMesh()
		{
			return _pMesh;
		}

		void AOSMeshRender::AddMaterial(std::string materialkey)
		{
			MaterialDesc* pMaterial = MaterialManager::GetInstance()->GetMaterialInfo(materialkey.c_str());
      // Copy the material and save it to the list.
      {
        uint32_t materialBufferSize = pMaterial->_sizeOfMaterialBuffer;
        uint8_t* pMaterialBuffer = new uint8_t[materialBufferSize];
        CopyMem((uint8_t*)pMaterial, pMaterialBuffer, materialBufferSize);
        _materials.push_back((MaterialDesc*)pMaterialBuffer);
      }
		}


		//////////////////////////////RenderObjManager///////////////////////////

		AOSMeshRender* AOSMeshRenderManager::AddMeshRender(const char* pAOSMesh, Common::ITransform* pTransform)
		{
			AOSMeshRender* pRO = new AOSMeshRender();
			pRO->SetMesh(pAOSMesh);
			pRO->SetTrans(pTransform);
			_meshRenders.push_back(pRO);
			return pRO;
		}
		void AOSMeshRenderManager::UpdateRenderDataList()
		{
			std::vector<RenderData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderData3DList();
			for (std::vector<AOSMeshRender*>::iterator it = _meshRenders.begin(); it != _meshRenders.end(); ++it)
			{
				AOSMesh* pAOSMesh = (*it)->GetMesh();
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