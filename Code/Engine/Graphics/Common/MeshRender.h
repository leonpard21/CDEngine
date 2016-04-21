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
			AOSMeshRender();
			~AOSMeshRender();
			MaterialDesc* GetSharedMaterial(uint32_t index = 0);
			AOSMesh* GetMesh();
			void AddMaterial(std::string materialkey);
      // Just like Unity3D, if you set the material in this way, 
      // you need to delete the new material by yourself.
      void SetMaterial(uint32_t index, MaterialDesc* pNewMaterial);
      // This is useful when we want to change the material just this the gameobject 
      // which contains this AOSMeshRender. It will be managed by this AOSMeshRender itself.
      // In unity we need to use .materials and set it back, 
      // so here I add this function which is helpful for those requirement.
      void DuplicateMaterial(uint32_t index);
			void SetMesh(AOSMesh* pAOSMesh) { _pMesh = pAOSMesh; }
			void SetMesh(const char* pMeshName);
			void SetTrans(Common::ITransform*  pTrans) { _pTrans = pTrans; }
			Common::ITransform* GetTransform() { return _pTrans; }

			friend class AOSMeshRenderManager;
		private:
			Graphics::AOSMesh* _pMesh;
			std::vector<MaterialDesc*> _sharedMaterials;
      std::vector<MaterialDesc*> _duplicated_materials;
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

	}
}


#endif//EAE_ENGINE_MESHRENDEROBJ_H