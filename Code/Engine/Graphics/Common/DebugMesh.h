#ifndef EAE_ENGINE_GRAPHICS_DEBUGMESH_H
#define EAE_ENGINE_GRAPHICS_DEBUGMESH_H
#include "../Common/AOSMesh.h"

namespace EAE_Engine 
{
	namespace Graphics
	{
		class MeshRender;
		class DebugMeshes 
		{
		public:
			void Init();
			void Clean();
			void Update();

		private:
			void GenerateDebugSegments();
			void GenerateDebugBoxes();
			void GenerateDebugSpheres();

			///////////////////////////Members/////////////////////////////
		private:
			MeshRender* _pSegmentsMeshRender;
			MeshRender* _pBoxesMeshRender;
			MeshRender* _pShperesMeshRender;
			// We should Manage the Debug Meshes by ourself, because we need to update them every frame.
			AOSMesh* _pSegmentsMesh;
			AOSMesh* _pBoxesMesh;
			AOSMesh* _pShperesMesh;

			/////////////////////static_members////////////////////////////
		private:
			DebugMeshes();
			static DebugMeshes* s_pInternalInstance;
		public:
			static DebugMeshes& GetInstance();
			static void CleanInstance();
		};
	
	}
}

#endif//EAE_ENGINE_GRAPHICS_DEBUGMESH_H