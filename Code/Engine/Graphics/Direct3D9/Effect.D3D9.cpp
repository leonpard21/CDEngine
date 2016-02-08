#include "../Common/Effect.h"
#include "../Common/Device.h"
#include "../Common/UniformVariable.h"
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
			ExtratVSUniforms();
			SAFE_DELETE(pVS);
			ShaderContent* pFS = LoadShader(i_pFSfilePath);
			if (!LoadFragmentShader(pFS->GetBufferPtr()))
			{
				Release();
				SAFE_DELETE(pFS);
				return false;
			}
			ExtratFSUniforms();
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

		void Effect::ExtratVSUniforms()
		{
			D3DXCONSTANTTABLE_DESC pContantTableDesc;
			_pVSConstantTable->GetDesc(&pContantTableDesc);
			UINT count = pContantTableDesc.Constants;
			if (count == 0)
				return;
			for (size_t i = 0; i < count; ++i)
			{
				D3DXHANDLE handle = NULL;
				handle = _pVSConstantTable->GetConstant(NULL, (UINT)i);
				if (handle == NULL) continue;
				D3DXCONSTANT_DESC constDesc;
				UINT countToGet = 1;
				_pVSConstantTable->GetConstantDesc(handle, &constDesc, &countToGet);
				if (handle == NULL) continue;
				if (constDesc.Class == D3DXPC_VECTOR)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(constDesc.Name, UniformType::float3, ShaderTypes::Vertex);
					pUV->AddOwner(this, handle);
				}
				else if (constDesc.Class == D3DXPC_MATRIX_COLUMNS)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(constDesc.Name, UniformType::mat4, ShaderTypes::Vertex);
					pUV->AddOwner(this, handle);
				}
				else if (strcmp(constDesc.Name, "g_CameraMatrices") == 0)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(constDesc.Name, UniformType::CameraMatricesStruct, ShaderTypes::Vertex);
					pUV->AddOwner(this, handle);
				}
			}
		}

		void Effect::ExtratFSUniforms() 
		{
			/*
			D3DXCONSTANTTABLE_DESC pContantTableDesc;
			_pFSConstantTable->GetDesc(&pContantTableDesc);
			UINT count = pContantTableDesc.Constants;
			if (count == 0)
				return;
			for (size_t i = 0; i < count; ++i)
			{
				D3DXHANDLE handle = NULL;
				handle = _pFSConstantTable->GetConstant(NULL, (UINT)i);
				if (handle == NULL) continue;
				D3DXCONSTANT_DESC constDesc;
				UINT countToGet = 1;
				_pFSConstantTable->GetConstantDesc(handle, &constDesc, &countToGet);
				if (handle == NULL) 
					continue;
			}
			*/
		}

		void Effect::SetUniform(UniformType type, D3DXHANDLE handle, void* pArray, size_t count, ShaderTypes shaderType)
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
			// handle = _pVSConstantTable->GetConstantByName(NULL, pArrayName);
			if (handle == NULL) return;
			if (type == UniformType::float3) 
			{
				HRESULT result = pConstantTable->SetFloatArray(pD3DDevice, handle, (float*)pArray, (UINT)count);
			}
			else if (type == UniformType::mat4) 
			{
				// Calling the transpose version of the the SetMatrix function.
				// Because standard Direct3D stores matrices as rows but we are storing them as columns. 
				// (Confusingly, standard Direct3D interprets matrices as being stored in columns on the GPU, 
				// and so the standard SetMatrix() actually will transpose the values, 
				// but the SetMatrixTranspose() function we use here just passes them to the GPU directly.)
				HRESULT result = pConstantTable->SetMatrixTranspose(pD3DDevice, handle, reinterpret_cast<const D3DXMATRIX*>(pArray));
				// If we use the bottom one, we should use Transpose Matrix of ColMatrix44
				//HRESULT result = pConstantTable->SetValue(pD3DDevice, handle, (void*)pArray, sizeof(D3DXMATRIX));
			}
			else if (type == UniformType::CameraMatricesStruct)
			{
				// set the camera matrices
				HRESULT result = pConstantTable->SetValue(pD3DDevice, handle, pArray, sizeof(CameraMatrices));
			}
		}

		void Effect::SetUniform(D3DXHANDLE handle, void* pArray, size_t count, ShaderTypes shaderType)
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
			// handle = _pVSConstantTable->GetConstantByName(NULL, pArrayName);
			if (handle == NULL) return;
			if (count <= 4)
			{
				HRESULT result = pConstantTable->SetFloatArray(pD3DDevice, handle, (float*)pArray, (UINT)count);
			}
			else if (count == 16)
			{
				// Calling the transpose version of the the SetMatrix function.
				// Because standard Direct3D stores matrices as rows but we are storing them as columns. 
				// (Confusingly, standard Direct3D interprets matrices as being stored in columns on the GPU, 
				// and so the standard SetMatrix() actually will transpose the values, 
				// but the SetMatrixTranspose() function we use here just passes them to the GPU directly.)
				HRESULT result = pConstantTable->SetMatrixTranspose(pD3DDevice, handle, reinterpret_cast<const D3DXMATRIX*>(pArray));
			}
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
			_updateList[location] = pVariable;
		}

		void Effect::Update()
		{
			for (std::map<D3DXHANDLE, UniformVariable*>::iterator iter = _updateList.begin(); iter != _updateList.end(); ++iter)
			{
				D3DXHANDLE location = iter->first;
				UniformVariable* pValue = iter->second;
				SetUniform(pValue->GetUniformType(), location, pValue->GetElements(), pValue->GetElementCount(), pValue->GetShaderType());
			}
			_updateList.clear();
		}

/////////////////////////////////////////////////////////////////////////////////

	}
}

