#include "RenderDatas.h"
#include "Material.h"
#include "Effect.h"
#include "MeshRender.h"
#include "AOSMesh.h"
#include "GraphicsInternal.h"
#include "AOSMeshOp.h"
#include "SpriteRender.h"
#include "Device.h"

namespace EAE_Engine
{
	namespace Graphics 
	{

		///////////////////////////////////////////RenderData3D////////////////////////////////////
		MaterialDesc* RenderData3D::s_pCurrentMaterial = nullptr;
		Effect* RenderData3D::s_pCurrentEffect = nullptr;
		AOSMesh* RenderData3D::s_pCurrentAOSMesh = nullptr;
		//MeshRender* RenderObj::s_pCurrentMeshRender = nullptr;

		MaterialDesc* RenderData3D::GetMaterial()
		{
			MaterialDesc* pMaterial = _pMeshRender->GetMaterial(_subMeshIndex);
			return pMaterial;
		}

		void RenderData3D::ChangeEffectVariables()
		{
			// Leo: I think for the transform matrix, I should set the uniform variable through the Effect directly.
			// Set the Transform
			UniformVariableManager::GetInstance().ChangeValue<Math::ColMatrix44>("g_local_world_matrix", &_localToWorld, 1);
			UniformVariableManager::GetInstance().NotifyOwners("g_local_world_matrix");

			// Update all of the uniform variables changed so far for the effect.
			if (s_pCurrentEffect)
				s_pCurrentEffect->Update();
		}

		void RenderData3D::Render()
		{
			//If we need to change material, change the material
			MaterialDesc* pMaterial =_pMeshRender->GetMaterial(_subMeshIndex);
			if (s_pCurrentMaterial != pMaterial)
			{
				s_pCurrentMaterial = pMaterial;
				//If we need to change effect, change the effect
				Effect* pEffect = s_pCurrentMaterial->_pEffect;
				if (s_pCurrentEffect != pEffect)
				{
					s_pCurrentEffect = pEffect;
					BindCurrentEffect(s_pCurrentEffect);
					ChangeEffectRenderState(s_pCurrentEffect->GetRenderState());
				}
				// Becareful that we must set the varibale of the effect after we bind the new Effect program,
				// or we will not be able to set the uniform variables successfully.
				s_pCurrentMaterial->SetUniformForEffect();
				s_pCurrentMaterial->SetTexturesForEffect();
			}
			// updated the parameters for the material
			ChangeEffectVariables();
			//If we need to change mesh, change mesh
			AOSMesh* pAOSMesh = _pMeshRender->GetMesh();
			if (s_pCurrentAOSMesh != pAOSMesh)
			{
				s_pCurrentAOSMesh = pAOSMesh;
				SetCurrentRenderMesh(s_pCurrentAOSMesh);
			}
			// Before rendering, we need to make sure that the effect and material exists.
			if (!s_pCurrentMaterial || !s_pCurrentMaterial->_pEffect)
				return;
			// now start the rendering:
			s_pCurrentMaterial->_pEffect->BeginRender();
			bool renderMeshResult = RenderAOSMeshInternal(s_pCurrentAOSMesh, _subMeshIndex);
			assert(renderMeshResult);
			s_pCurrentMaterial->_pEffect->EndRender();
		}


		///////////////////////////////////////////RenderData2D////////////////////////////////////
		MaterialDesc* RenderData2D::s_pCurrentMaterial = nullptr;
		Effect* RenderData2D::s_pCurrentEffect = nullptr;

		void RenderData2D::ChangeEffectVariables()
		{
			// Leo: I think for the transform matrix, I should set the uniform variable through the Effect directly.
			// Set the Transform
			UniformVariableManager::GetInstance().ChangeValue<Math::ColMatrix44>("g_local_world_matrix", &_localToWorld, 1);
			UniformVariableManager::GetInstance().NotifyOwners("g_local_world_matrix");

			// Update all of the uniform variables changed so far for the effect.
			if (s_pCurrentEffect)
				s_pCurrentEffect->Update();
		}

		bool SetD3DRenderMesh(const SpriteMesh *pSMesh)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			HRESULT result;
			// Set Vertex Declaration
			{
				pD3DDevice->SetVertexDeclaration(pSMesh->GetVertexDeclaration());
			}
			// Bind a specific vertex buffer to the device as a data source
			{
				// There can be multiple streams of data feeding the display adaptor at the same time
				const unsigned int streamIndex = 0;
				// It's possible to start streaming data in the middle of a vertex buffer
				const unsigned int bufferOffset = 0;
				// The "stride" defines how large a single vertex is in the stream of data
				const unsigned int bufferStride = sizeof(SpriteVertex);
				result = pD3DDevice->SetStreamSource(streamIndex, pSMesh->GetVertexBuffer(), bufferOffset, bufferStride);
				assert(SUCCEEDED(result));
			}
			return SUCCEEDED(result);
		}

		bool RenderD3DNoIndexPremitives(SpriteMesh *pSMesh)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			HRESULT result = true;
			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLESTRIP;
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = 0;
				// We are drawing the AOSMesh
				{
					const unsigned int vertexCountToRender = 4;// How vertices from the vertex buffer will be used?
					result = pD3DDevice->DrawPrimitive(primitiveType, indexOfFirstVertexToRender, vertexCountToRender);
					assert(SUCCEEDED(result));
				}
			}
			return SUCCEEDED(result);
		}

		void RenderData2D::Render() 
		{
			//If we need to change material, change the material
			MaterialDesc* pMaterial = SpriteRenderManager::GetInstance()->GetMaterial();
			s_pCurrentMaterial = pMaterial;
			s_pCurrentEffect = pMaterial->_pEffect;
			BindCurrentEffect(s_pCurrentEffect);
			ChangeEffectRenderState(s_pCurrentEffect->GetRenderState());
			s_pCurrentMaterial->SetUniformForEffect();
			s_pCurrentMaterial->SetTexturesForEffect();

			// updated the parameters for the material
			ChangeEffectVariables();

			SpriteMesh* pSMesh = SpriteRenderManager::GetInstance()->GetSpriteMesh();
			SetD3DRenderMesh(pSMesh);

			s_pCurrentEffect->BeginRender();
			bool renderMeshResult = RenderD3DNoIndexPremitives(pSMesh);
			assert(renderMeshResult);
			s_pCurrentEffect->EndRender();
		}


	}
}

