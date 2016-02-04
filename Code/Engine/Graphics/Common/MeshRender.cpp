#include "MeshRender.h"
#include "../Common/AOSMesh.h"
#include "../Common/Effect.h"
#include "../Common/MeshManager.h"
#include "../Common/Camera.h"
#include "../Common/Material.h"
#include "RenderObj.h"
#include "RenderDatas.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		Graphics::MaterialDesc* MeshRender::GetMaterial(uint32_t index)
		{ 
			if (_materials.size() == 0)
				return nullptr;
			else if (index >= _materials.size()) 
				return _materials[0];
			return _materials[index];
		}

		void MeshRender::SetMesh(const char* pMeshName)
		{
			_pMesh = MeshManager::GetMeshManager()->GetMesh(pMeshName);
		}

		AOSMesh* MeshRender::GetMesh()
		{
			return _pMesh;
		}

		void MeshRender::AddMaterial(const std::vector<std::string>& materialKeys)
		{
			for (std::vector<std::string>::const_iterator it = materialKeys.begin(); it != materialKeys.end(); ++it)
			{
				MaterialDesc* pMaterial = MaterialManager::GetInstance()->GetMaterial((*it).c_str());
				_materials.push_back(pMaterial);
			}
		}


		//////////////////////////////RenderObjManager///////////////////////////
		MeshRender* MeshRenderManager::AddMeshRender(const std::vector<std::string>& materialKeys)
		{
			MeshRender* pMeshRender = new MeshRender();
			pMeshRender->AddMaterial(materialKeys);
			_meshRenders.push_back(pMeshRender);
			return pMeshRender;
		}

		MeshRender* MeshRenderManager::AddMeshRender(const char* pAOSMesh, const std::vector<std::string>& materialKeys, Common::ITransform* pTransform)
		{
			MeshRender* pRO = new MeshRender();
			pRO->SetMesh(pAOSMesh);
			pRO->AddMaterial(materialKeys);
			pRO->SetTrans(pTransform);
			_meshRenders.push_back(pRO);
			return pRO;
		}
		void MeshRenderManager::UpdateRenderDataList()
		{
			std::vector<RenderData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderData3DList();
			for (std::vector<MeshRender*>::iterator it = _meshRenders.begin(); it != _meshRenders.end(); ++it)
			{
				AOSMesh* pAOSMesh = (*it)->GetMesh();
				if (pAOSMesh == nullptr) 
					continue;
				for (uint32_t submeshindex = 0; submeshindex < pAOSMesh->GetSubMeshCount(); ++submeshindex)
				{
					Common::ITransform* pTrans = (*it)->GetTransform();
					Math::ColMatrix44 colMat = pTrans? pTrans->GetLocalToWorldMatrix() : Math::ColMatrix44::Identity ;
					RenderData3D renderData = { (*it), submeshindex, colMat };
					renderDataList.push_back(renderData);
				}
			}
		}

		void MeshRenderManager::Clean() 
		{
			for (std::vector<MeshRender*>::iterator iter = _meshRenders.begin(); iter != _meshRenders.end();)
			{
				MeshRender* pObj = *iter++;
				SAFE_DELETE(pObj);
			}
			_meshRenders.clear();
		}

		void MeshRenderManager::Remove(Common::ITransform* pTransform)
		{
			for (std::vector<MeshRender*>::iterator iter = _meshRenders.begin(); iter != _meshRenders.end(); ++iter)
			{
				MeshRender* pObj = *iter;
				if (pObj->GetTransform() == pTransform)
				{
					_meshRenders.erase(iter);
					SAFE_DELETE(pObj);
					break;
				}
			}
		}

		////////////////////////////////static_members/////////////////////////////////
		MeshRenderManager* MeshRenderManager::s_pInternalInstance = nullptr;

		MeshRenderManager& MeshRenderManager::GetInstance()
		{ 
			if (!s_pInternalInstance)
				s_pInternalInstance = new MeshRenderManager();
			return *s_pInternalInstance; 
		}

		void MeshRenderManager::CleanInstance() 
		{ 
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean(); 
			SAFE_DELETE(s_pInternalInstance);
		}

	}
}