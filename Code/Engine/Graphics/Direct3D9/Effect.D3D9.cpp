#include "../Common/Effect.h"
#include "../Common/Device.h"
#include "../Common/UniformVariable.h"
#include "../Common/UniformBlock.h"
#include "../Common/ShaderContent.h"
#include "../Common/BinaryFileLoader.h"
#include <sstream>
#include <cassert>
#include "UserOutput/UserOutput.h"
#include "Math/ColMatrix.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		bool Effect::Init(const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState)
		{
			ShaderContent* pVS = LoadShader(i_pVSfilePath);
			if (!LoadVertexShader(pVS->GetBufferPtr()))
			{
				SAFE_DELETE(pVS);
				return false;
			}
			ExtratUniformsFromTable(_pVSConstantTable, ShaderTypes::Vertex);
			SAFE_DELETE(pVS);
			ShaderContent* pFS = LoadShader(i_pFSfilePath);
			if (!LoadFragmentShader(pFS->GetBufferPtr()))
			{
				Release();
				SAFE_DELETE(pFS);
				return false;
			}
			ExtratUniformsFromTable(_pFSConstantTable, ShaderTypes::Fragment);
			SAFE_DELETE(pFS);
			_renderState = renderState;
			return true;
		}

		bool Effect::Release()
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;
			if (pD3DDevice)
			{
				SAFE_RELEASE(_pVSConstantTable);
				SAFE_RELEASE(_pVertexShader);
				SAFE_RELEASE(_pFragmentShader);
			}

			return true;
		}

		bool Effect::LoadVertexShader(uint8_t* i_pVSBuffer)
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			// Get the vertex shader constant table
			D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(i_pVSBuffer), &_pVSConstantTable);
			// Create the vertex shader object
			bool wereThereErrors = false;
			{
				HRESULT result = pD3DDevice->CreateVertexShader(reinterpret_cast<DWORD*>(i_pVSBuffer),
					&_pVertexShader);
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to create the vertex shader");
					wereThereErrors = true;
				}
			}
			return !wereThereErrors;
		}

		bool Effect::LoadFragmentShader(uint8_t* i_pFSBuffer)
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			// Get the fragment shader constant table
			D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(i_pFSBuffer), &_pFSConstantTable);
			// Create the fragment shader object
			bool wereThereErrors = false;
			{
				HRESULT result = pD3DDevice->CreatePixelShader(reinterpret_cast<DWORD*>(i_pFSBuffer),
					&_pFragmentShader);
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to create the fragment shader");
					wereThereErrors = true;
				}
			}
			return !wereThereErrors;
		}

		bool Effect::BeginRender()
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			assert(pD3DDevice != nullptr);
			
			HRESULT result = pD3DDevice->BeginScene();
			assert(SUCCEEDED(result));

			return SUCCEEDED(result);
		}

		bool Effect::EndRender()
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			assert(pD3DDevice != nullptr);

			HRESULT result = pD3DDevice->EndScene();
			assert(SUCCEEDED(result));
			return SUCCEEDED(result);
		}

		void Effect::ExtratUniformsFromTable(ID3DXConstantTable* pConstantTable, ShaderTypes shaderType)
		{
			D3DXCONSTANTTABLE_DESC pContantTableDesc;
			pConstantTable->GetDesc(&pContantTableDesc);
			UINT count = pContantTableDesc.Constants;
			if (count == 0)
				return;
			for (size_t i = 0; i < count; ++i)
			{
				D3DXHANDLE handle = NULL;
				handle = pConstantTable->GetConstant(NULL, (UINT)i);
				if (handle == NULL) continue;
				D3DXCONSTANT_DESC constDesc;
				UINT countToGet = 1;
				pConstantTable->GetConstantDesc(handle, &constDesc, &countToGet);
				if (handle == NULL) continue;
				UniformVariable* pUV = nullptr;
				if (constDesc.Class == D3DXPC_VECTOR)
				{
					pUV = UniformVariableManager::GetInstance().GetUniformVariable(constDesc.Name, sizeof(Math::Vector3), shaderType);
				}
				else if (constDesc.Class == D3DXPC_MATRIX_COLUMNS)
				{
					pUV = UniformVariableManager::GetInstance().GetUniformVariable(constDesc.Name, sizeof(Math::ColMatrix44), shaderType);
				}
				if (constDesc.Class == D3DXPC_STRUCT)
				{
					uint32_t blockSize = constDesc.Bytes;
					pUV = UniformVariableManager::GetInstance().GetUniformVariable(constDesc.Name, blockSize, shaderType);
				}
				if (pUV)
					pUV->AddOwner(this, handle);
				// Now I'm using struct to handle some feature like Uniform Block.
				// Since in D3D9 we don't have Uniform Block, nor Constant Buffer.
				// So for the strucutre, we will have 2 copys,
				// one is UniformVariable, the other one is UniformBlock.
				// So we can handle different requirement to set the variable.
				// I think it's OK for now.
				if (constDesc.Class == D3DXPC_STRUCT)
				{
					const char* pName = constDesc.Name;
					if (!UniformBlockManager::GetInstance()->Contains(pName))
					{
						uint32_t blockSize = constDesc.Bytes;
						UniformBlock* pUB = new UniformBlock(pName, blockSize, handle, shaderType);
						pUB->AddOwner(this);
						UniformBlockManager::GetInstance()->AddUniformBlock(pUB);
					}
				}
			}
		}

		void Effect::SetUniform(D3DXHANDLE handle, void* pBuffer, uint32_t bufferSize, ShaderTypes shaderType)
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return;
			// check we should set the uniform for vertexshader or fragmentshader
			ID3DXConstantTable* pConstantTable = nullptr;
			if (shaderType == ShaderTypes::Vertex)
				pConstantTable = _pVSConstantTable;
			else
				pConstantTable = _pFSConstantTable;
			if (!pConstantTable)
				return;
			// The handle is what we can get from the constantTable by using:
			// handle = _pVSConstantTable->GetConstantByName(NULL, pName);
			if (handle == NULL) return;
			HRESULT result = pConstantTable->SetValue(pD3DDevice, handle, pBuffer, bufferSize);
			assert(SUCCEEDED(result));
			// Here we should be very careful about an odd thing in setting the matrices:
			// The standard Direct3D stores matrices as rows but we are storing them as columns. 
			// (Confusingly, standard Direct3D interprets matrices as being stored in columns on the GPU
			// Since we're using Right Hand Matrices in the Engine, we need to offer the Transpose of the matrices.

			// Another confusing point for the APIs:
			// On one hand, standard SetMatrix() actually will transpose the values:
			// If we call the transpose version of the the SetMatrix function:
			// Like HRESULT result = pConstantTable->SetValue(pD3DDevice, handle, (void*)pArray, sizeof(D3DXMATRIX));
			// On the other hand, SetMatrixTranspose() function just passes them to the GPU directly
			// If we use the bottom one, we should use Transpose Matrix of ColMatrix44
			// Like: HRESULT result = pConstantTable->SetMatrixTranspose(pD3DDevice, handle, reinterpret_cast<const D3DXMATRIX*>(pBuffer));
		}

		tUniformHandle Effect::GetHandle(const char* pName, ShaderTypes shaderType)
		{
			// check we should set the uniform for vertexshader or fragmentshader
			ID3DXConstantTable* pConstantTable = nullptr;
			if (shaderType == ShaderTypes::Vertex)
				pConstantTable = _pVSConstantTable;
			else
				pConstantTable = _pFSConstantTable;
			if (!pConstantTable) return 0;
			D3DXHANDLE handle = pConstantTable->GetConstantByName(NULL, pName);
			// return the handle of the Uniform Variable.
			return (tUniformHandle)handle;
		}

		tSamplerID Effect::GetSamplerID(const char* pName, ShaderTypes shaderType)
		{
			// check we should set the uniform for vertexshader or fragmentshader
			ID3DXConstantTable* pConstantTable = nullptr;
			if (shaderType == ShaderTypes::Vertex)
				pConstantTable = _pVSConstantTable;
			else if (shaderType == ShaderTypes::Fragment)
				pConstantTable = _pFSConstantTable;
			else
				return 0;
			if (!pConstantTable) return 0;
			D3DXHANDLE handle = pConstantTable->GetConstantByName(NULL, pName);
			if (handle == NULL)
			{
				return UINT_MAX;
			}
			// return the handle of the Uniform Variable.
			return static_cast<tSamplerID>(pConstantTable->GetSamplerIndex(handle));
		}

		void Effect::OnNotify(UniformVariable* pVariable, D3DXHANDLE location)
		{
			_updateVariableList[location] = pVariable;
		}

		void Effect::Update()
		{
			for (std::map<D3DXHANDLE, UniformVariable*>::iterator iter = _updateVariableList.begin(); iter != _updateVariableList.end(); ++iter)
			{
				D3DXHANDLE location = iter->first;
				UniformVariable* pValue = iter->second;
				SetUniform(location, pValue->GetBuffer(), pValue->GetBufferSize(), pValue->GetShaderType());
			}
			_updateVariableList.clear();
			for (std::map<const char*, UniformBlock*>::iterator iter = _updateBlockList.begin(); iter != _updateBlockList.end(); ++iter)
			{
				UniformBlock* pBlock = iter->second;
				ID3DXConstantTable* pConstantTable = nullptr;
				if (pBlock->GetType() == ShaderTypes::Vertex) 
				{
					pConstantTable = _pVSConstantTable;
				}
				else
				{
					pConstantTable = _pFSConstantTable;
				}
				pBlock->UpdateUniformBlockBuffer(pConstantTable);
			}
			_updateBlockList.clear();
		}

/////////////////////////////////////////////////////////////////////////////////

	}
}

