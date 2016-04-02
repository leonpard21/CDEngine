#ifndef EAE_ENGINE_GRAPHICS_DEBUGMESH_H
#define EAE_ENGINE_GRAPHICS_DEBUGMESH_H
#include "../Common/AOSMesh.h"

namespace EAE_Engine 
{
	namespace Graphics
	{
		class AOSMeshRender;
		class DebugMeshes 
		{
		public:
			void Init();
			void Clean();
			void Update();

		private:
			void GenerateDebugSegments();
      void GenerateDebugCircles();
			void GenerateDebugBoxes();
			void GenerateDebugSpheres();
			void GenerateDebugMeshes();
			///////////////////////////Members/////////////////////////////
		private:
			AOSMeshRender* _pSegmentsMeshRender;
      AOSMeshRender* _pCirclesMeshRender;
			AOSMeshRender* _pBoxesMeshRender;
			AOSMeshRender* _pShperesMeshRender;
			AOSMeshRender* _pTempMeshRender;
			// We should Manage the Debug Meshes by ourself, because we need to update them every frame.
			AOSMesh* _pSegmentsMesh;
      AOSMesh* _pCircleMesh;
			AOSMesh* _pBoxesMesh;
			AOSMesh* _pShperesMesh;
			AOSMesh* _pTempMesh;

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