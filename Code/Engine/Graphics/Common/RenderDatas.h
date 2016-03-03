#ifndef EAE_ENGINE_GRAPHICS_RENDERDATAS_H
#define EAE_ENGINE_GRAPHICS_RENDERDATAS_H
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
		class AOSMeshRender;

		struct RenderData3D
		{
			AOSMeshRender* _pMeshRender;// which MeshRender we will render.
			uint32_t _subMeshIndex;  // which subMesh of the MeshRender we will render.
			Common::ITransform* _pTrans;

			RenderData3D() = default;
			MaterialDesc* GetMaterial();
			void ChangeEffectVariables();
			void Render();

			// we will save the current Mesh, Effect and Material and compare it with the new one.
			// if the new one is still the same,
			// then we don't need to switch the context when rendering.
			static MaterialDesc* s_pCurrentMaterial;
			static Effect* s_pCurrentEffect;
			static AOSMesh* s_pCurrentAOSMesh;
			//static MeshRender* s_pCurrentMeshRender;
		};

		struct RenderRawData3D
		{
			AOSMeshRender* _pMeshRender;// which MeshRender we will render.
			Math::Vector3 _meshColor;
			Math::ColMatrix44 _transMat;

			RenderRawData3D() = default;
			void ChangeEffectVariables();
			void Render();

			// we will save the current Mesh, Effect and Material and compare it with the new one.
			// if the new one is still the same,
			// then we don't need to switch the context when rendering.
			static Effect* s_pCurrentEffect;
			//static MeshRender* s_pCurrentMeshRender;
		};

		struct CanvasRenderData;
		struct TextRender;
		class ImageRender;
		struct RenderDataUI
		{
			CanvasRenderData* _pCanvasRenderData;

			RenderDataUI() = default;
			void ChangeEffectVariables();
			void Render();
			void RenderImage(ImageRender* pImageRender);
			void RenderText(TextRender* pTextRender);

			static MaterialDesc* s_pCurrentMaterial;
			static Effect* s_pCurrentEffect;
		};


	}
}


#endif//EAE_ENGINE_GRAPHICS_RENDERDATAS_H