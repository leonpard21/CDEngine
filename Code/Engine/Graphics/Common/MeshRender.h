#ifndef EAE_ENGINE_MESHRENDEROBJ_H
#define EAE_ENGINE_MESHRENDEROBJ_H
#include <vector>
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"

namespace EAE_Engine
{
	namespace Graphics 
	{
		class AOSMesh;
		class Effect;
		struct MaterialDesc;
		class AOSMeshRender //: public Common::ICompo
		{
		public:
			~AOSMeshRender();
			MaterialDesc* GetSharedMaterial(uint32_t index = 0);
			MaterialDesc* GetMaterial(uint32_t index = 0);
			AOSMesh* GetMesh();
			void AddMaterial(std::string materialkey, bool shared = true);
			void SetMesh(AOSMesh* pAOSMesh) { _pMesh = pAOSMesh; }
			void SetMesh(const char* pMeshName);
			void SetTrans(Common::ITransform*  pTrans) { _pTrans = pTrans; }
			Common::ITransform* GetTransform() { return _pTrans; }

			friend class AOSMeshRenderManager;
		private:
			Graphics::AOSMesh* _pMesh;
			std::vector<Graphics::MaterialDesc*> _materials;
			std::vector<Graphics::MaterialDesc*> _sharedMaterials;
			Common::ITransform* _pTrans;
		};

		struct RenderData3D;
		class AOSMeshRenderManager 
		{
		public:
			AOSMeshRender* AddMeshRender(const char* pAOSMesh, Common::ITransform* pTransform);
			std::vector<AOSMeshRender*>& GetMeshRenderList() { return _meshRenders; }
			void UpdateRenderDataList();

			void Clean();
			void Remove(Common::ITransform* pTransform);
		private:
			std::vector<AOSMeshRender*> _meshRenders;

		/////////////////////static_members////////////////////////////
		private:
			AOSMeshRenderManager() {}
			static AOSMeshRenderManager* s_pInternalInstance;
		public:
			static AOSMeshRenderManager& GetInstance();
			static void CleanInstance();
		};

		inline AOSMeshRender* AddMeshRender(const char* pAOSMesh, Common::ITransform* pTransform)
		{
			AOSMeshRender* pResult = AOSMeshRenderManager::GetInstance().AddMeshRender(pAOSMesh, pTransform);
			return pResult;
		}

	}
}


#endif//EAE_ENGINE_MESHRENDEROBJ_H