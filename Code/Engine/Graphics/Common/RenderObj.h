#ifndef EAE_ENGINE_RENDEROBJ_H
#define EAE_ENGINE_RENDEROBJ_H
#include <vector>
#include "Engine/Math/Vector.h"
#include "Engine/Math/ColMatrix.h"
#include "Engine/Common/Interfaces.h"
#include "CommonDeclare.h"

namespace EAE_Engine
{
	namespace Graphics 
	{
		class AOSMesh;
		class Effect;
		struct MaterialDesc;
		class MeshRender;
		class SpriteRender;

		// The design of this structure based on the idea of:
		// Christer Ericson, http://realtimecollisiondetection.net/blog/?p=86
		// We use a weight to represent the weight of different RenderObjs.
		// in this case we can sort the rendering order easily.
		struct RenderWeight
		{
			union 
			{
				uint64_t _weight;
				struct
				{
					/* 
					 * Sometimes we want to sort on depth before material, 
					 * as is done with this key layout
					 * (for example when sorting translucent objects back-to-front)
					 * Other times we might want to sort on material before depth 
					 * (typically for opaque geometry, as we want to reduce state settings as much as possible)
					 * so this bottom area should be used base on the real cases.
					 */
					uint64_t _material : 24; // sort by Material
					uint64_t _depth : 24; // sort by Depth
					/* Here are some more important sorting aspects*/
					uint64_t _translucency : 2; // whether the RenderObj is transparent or not.
					uint64_t _viewportLayer : 3;//Each viewport could have their own skybox layer, world layer, effects layer, HUD layer.
					uint64_t _viewport : 3; // multiplayer splitscreen, for mirrors or portals in the scene, etc.
					uint64_t _layer : 2;    // sort by Background, Scene, HUD...
				};
			};
			bool operator < (RenderWeight& i_other)
			{
				return _weight < i_other._weight;
			}
			RenderWeight(uint64_t weight) : _weight(weight) {}
			RenderWeight(): _weight(0) {}
		};

		enum RenderDataLayer 
		{
			Object3D = 0,
			Object2D = 2, 
		};

		/*
		 * The design ideas are:
		 * For the rendering order, we need to have 2 features:
		 * 1. We want to sort all of the RenderObjs based on their differences of types/slots/layers/materials...,
		 * but we don't want to consider their differences of rendering method.
		 * So we don't care what they are, and how they are rendered,
		 * we just render the RenderObj one after another.
		 * 2. We want to manage the RenderObjs by differnt types/slots/layers, 
		 * so taht we can manage the renderObjs by the 3D render things/sprite/...
		 * Because for each type/slot/layer, 
		 * their rendering method are different with each other.
		 *
		 * As a result:
		 * We have 2 layers in the Rendering System:
		 * The upper layer is the RenderObj, which contains the RenderWeight and a void* _pRenderData.
		 * The bottom layer is the RenderData, which can be RenderData3D, RenderData2D...
		 * On each frame, we generate the RenderData3D, RenderData2D seperately,
		 * then we add those RenderDatas to the RenderObjList,
		 * and we set the RenderWeight based on the RenderDatas' attributes.
		 * At last, we just need to sort the RenderObjList by the RenderWeight.
		 */
		struct RenderData3D;
		struct RenderData2D;
		struct RenderObj
		{
			RenderObj() = default;
			void Render();

			RenderWeight _renderWeight;    // weight helps us to sort in Rendering
			void* _pRenderData;
		};

		class RenderObjManager 
		{
		public:
			struct RenderObjLess
			{
				bool operator()(RenderObj& i_objA, RenderObj& i_objB)
				{
					return i_objA._renderWeight < i_objB._renderWeight;
				}
			};
			std::vector<RenderObj>& GetRenderObjList() { return _renderObjs; }
			std::vector<RenderData3D>& GetRenderData3DList() { return _renderData3Ds; }
			std::vector<RenderData2D>& GetRenderData2DList() { return _renderData2Ds; }
			void Clean();
			void UpdateRenderObjList();
		private:
			std::vector<RenderData3D> _renderData3Ds;
			std::vector<RenderData2D> _renderData2Ds;
			std::vector<RenderObj> _renderObjs;

		/////////////////////static_members////////////////////////////
		private:
			RenderObjManager() {}
			static RenderObjManager* s_pInternalInstance;
		public:
			static RenderObjManager& GetInstance();
			static void CleanInstance();
		};


	}
}


#endif//EAE_ENGINE_RENDEROBJ_H