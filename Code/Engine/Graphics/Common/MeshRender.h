#ifndef EAE_ENGINE_MESHRENDEROBJ_H
#define EAE_ENGINE_MESHRENDEROBJ_H
#include <vector>
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"
#include "MeshFilter.h"

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
      AOSMesh* GetSharedMesh();
      void SetSharedMesh(AOSMesh* pAOSMesh) { _pSharedMesh = pAOSMesh; }
      void SetSharedMesh(const char* pMeshName);

      void SetMeshFilter(MeshFilter* pMeshFilter) { _pMeshFilter = pMeshFilter; }
      MeshFilter* GetMeshFilter() { return _pMeshFilter; }
      void AddMaterial(std::string materialkey);
      // In untiy3d, if the material is used by any other renderers, 
      // this will clone the shared material and start using it from now on.
      MaterialDesc* GetMaterial(uint32_t index = 0);
      // In unity3d, modifying sharedMaterial will change the appearance of all objects using this material, 
      // and change material settings that are stored in the project too.
			MaterialDesc* GetSharedMaterial(uint32_t index = 0);
      void SetSharedMaterial(uint32_t index, std::string materialkey);
      void SetMaterial(uint32_t index, MaterialDesc* pNewMaterial);
			void SetTrans(Common::ITransform*  pTrans) { _pTrans = pTrans; }
			Common::ITransform* GetTransform() { return _pTrans; }
    private: 
      // copy the shared material to local material
      bool CopySharedMaterialToLocal(uint32_t index);
			friend class AOSMeshRenderManager;
		private:
      MeshFilter* _pMeshFilter;
			AOSMesh* _pSharedMesh;
			std::vector<MaterialDesc*> _sharedMaterials;
      std::vector<MaterialDesc*> _localMaterials;
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