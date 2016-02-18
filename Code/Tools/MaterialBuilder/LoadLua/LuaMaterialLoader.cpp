#include "LuaMaterialLoader.h"
#include <cassert>
#include <sstream>
#include "External/Lua/Includes.h"
#include "Engine/Math/Vector.h"
#include "Engine/General/MemoryOp.h"
#include "Tools/BuilderHelper/cbBuilder.h"
#include "Engine/Graphics/Common/UniformDesc.h"// Get the UniformDesc
#include "Engine/Graphics/Common/Material.h"// Get the Material
#include "Engine/Graphics/Common/Texture.h"

// Helper Function Definitions
//============================
namespace EAE_Engine
{
	namespace Tools
	{
		struct UniformData
		{
			char* _pName;
			float* _pValue;
			uint32_t _valueCount;
			Graphics::ShaderTypes _shaderType;
			UniformData() : _pName(nullptr), _pValue(nullptr),
				_valueCount(0), _shaderType(Graphics::ShaderTypes::Unknown) {}
			~UniformData()
			{
				SAFE_DELETE(_pName);
				SAFE_DELETE_ARRAY(_pValue);
			}
		};

		struct UniformBlockData
		{
			char* _pName;
			float* _pValue;
			uint32_t _valueCount;
			Graphics::ShaderTypes _shaderType;
			UniformBlockData() : _pName(nullptr), _pValue(nullptr),
				_valueCount(0), _shaderType(Graphics::ShaderTypes::Unknown) {}
			~UniformBlockData()
			{
				SAFE_DELETE(_pName);
				SAFE_DELETE_ARRAY(_pValue);
			}
		};

		struct TextureData
		{
			char* _pName;
			char* _pTexPath;
			Graphics::ShaderTypes _shaderType;
			TextureData() : _pName(nullptr), _pTexPath(nullptr) {}
			~TextureData()
			{
				SAFE_DELETE(_pName);
				SAFE_DELETE(_pTexPath);
			}
		};

		struct MaterialData
		{
			char* _pEffectPath;
			char* _pEffectSource;
			uint32_t _materialCost;
			UniformData* _pUniformData;
			uint32_t _uniformCount;
			TextureData* _pTexData;
			uint32_t _texCount;
			MaterialData() :_pEffectPath(nullptr), _pEffectSource(nullptr), _materialCost(0), 
				_pUniformData(nullptr), _uniformCount(0), _pTexData(nullptr), _texCount(0) {}
			~MaterialData()
			{
				SAFE_DELETE(_pEffectPath);
				SAFE_DELETE(_pEffectSource);
				SAFE_DELETE_ARRAY(_pUniformData);
				SAFE_DELETE_ARRAY(_pTexData);
			}
		};
	}
}

namespace
{
	bool LoadTableValues_filenames(lua_State& io_luaState, EAE_Engine::Tools::MaterialData*& o_pMaterial);
	bool LoadMaterial(const char* const i_path, EAE_Engine::Tools::MaterialData*& o_pMaterial);
	bool LoadMaterialData(lua_State& io_luaState, EAE_Engine::Tools::MaterialData*& o_pMaterial);

	inline EAE_Engine::Graphics::ShaderTypes GetShaderType(const char* pTypeName)
	{
		EAE_Engine::Graphics::ShaderTypes result = EAE_Engine::Graphics::ShaderTypes::Unknown;
		if (strcmp("fragment", pTypeName) == 0)
			result = EAE_Engine::Graphics::ShaderTypes::Fragment;
		else if (strcmp("vertex", pTypeName) == 0)
			result = EAE_Engine::Graphics::ShaderTypes::Vertex;
		return result;
	}

	bool LoadTableValues(lua_State& io_luaState, EAE_Engine::Tools::MaterialData*& o_pMaterial)
	{
		if (!LoadTableValues_filenames(io_luaState, o_pMaterial))
		{
			return false;
		}
		return true;
	}

	bool LoadTableValues_filenames(lua_State& io_luaState, EAE_Engine::Tools::MaterialData*& o_pMaterial)
	{
		bool wereThereErrors = false;
		o_pMaterial = new EAE_Engine::Tools::MaterialData();
		if (!o_pMaterial)
		{
			return false;
		}
		// Right now the effect is at -1.
		// After the following table operation it will be at -2.
		// and the "vsSource" table will be at -1:
		{
			const char* const key = "sourcePath";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			const char* pSourcePath = lua_tostring(&io_luaState, -1);
			lua_pop(&io_luaState, 1);
			o_pMaterial->_pEffectPath = _strdup(pSourcePath);
		}
		{
			const char* const key = "effectSource";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			const char* peffectSource = lua_tostring(&io_luaState, -1);
			lua_pop(&io_luaState, 1);
			o_pMaterial->_pEffectSource = _strdup(peffectSource);
		}
		{
			const char* const key = "materialCost";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			uint32_t materialCost = 0;
			if (!lua_isnil(&io_luaState, -1))
			{
				materialCost = (uint32_t)lua_tointeger(&io_luaState, -1);
			}
			lua_pop(&io_luaState, 1);
			o_pMaterial->_materialCost = materialCost;
		}
		{
			const char* const key = "uniformData";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				const int arrayLength = luaL_len(&io_luaState, -1);
				o_pMaterial->_uniformCount = arrayLength;
				if (arrayLength > 0)
				{
					o_pMaterial->_pUniformData = new EAE_Engine::Tools::UniformData[o_pMaterial->_uniformCount];
					// Remember that Lua arrays are 1-based and not 0-based!
					for (int i = 1; i <= arrayLength; ++i)
					{
						lua_pushinteger(&io_luaState, i);
						lua_gettable(&io_luaState, -2);
						{
							const char* const key = "name";
							lua_pushstring(&io_luaState, key);
							lua_gettable(&io_luaState, -2);
							const char* pName = lua_tostring(&io_luaState, -1);
							o_pMaterial->_pUniformData[i - 1]._pName = _strdup(pName);
							lua_pop(&io_luaState, 1);
						}
						{
							const char* const key = "value";
							lua_pushstring(&io_luaState, key);
							lua_gettable(&io_luaState, -2);
							assert(lua_istable(&io_luaState, -1));
							const int valueTableLength = luaL_len(&io_luaState, -1);
							o_pMaterial->_pUniformData[i - 1]._valueCount = valueTableLength;
							o_pMaterial->_pUniformData[i - 1]._pValue = new float[valueTableLength];
							for (int j = 1; j <= valueTableLength; ++j)
							{
								lua_pushinteger(&io_luaState, j);
								lua_gettable(&io_luaState, -2);
								o_pMaterial->_pUniformData[i - 1]._pValue[j - 1] = (float)lua_tonumber(&io_luaState, -1);
								lua_pop(&io_luaState, 1);
							}
							lua_pop(&io_luaState, 1);
						}
						{
							const char* const key = "shaderType";
							lua_pushstring(&io_luaState, key);
							lua_gettable(&io_luaState, -2);
							const char* pShaderType = lua_tostring(&io_luaState, -1);
							o_pMaterial->_pUniformData[i - 1]._shaderType = GetShaderType(pShaderType);
							lua_pop(&io_luaState, 1);
						}
						lua_pop(&io_luaState, 1);
					}
				}
			}
			lua_pop(&io_luaState, 1);
		}
		{
			const char* const key = "textureData";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				const int arrayLength = luaL_len(&io_luaState, -1);
				o_pMaterial->_texCount = arrayLength;
				if (arrayLength > 0)
				{
					o_pMaterial->_pTexData = new EAE_Engine::Tools::TextureData[o_pMaterial->_texCount];
					// Remember that Lua arrays are 1-based and not 0-based!
					for (int i = 1; i <= arrayLength; ++i)
					{
						lua_pushinteger(&io_luaState, i);
						lua_gettable(&io_luaState, -2);
						{
							const char* const key = "name";
							lua_pushstring(&io_luaState, key);
							lua_gettable(&io_luaState, -2);
							const char* pName = lua_tostring(&io_luaState, -1);
							o_pMaterial->_pTexData[i - 1]._pName = _strdup(pName);
							lua_pop(&io_luaState, 1);
						}
						{
							const char* const key = "path";
							lua_pushstring(&io_luaState, key);
							lua_gettable(&io_luaState, -2);
							const char* pPath = lua_tostring(&io_luaState, -1);
							o_pMaterial->_pTexData[i - 1]._pTexPath = _strdup(pPath);
							lua_pop(&io_luaState, 1);
						}
						{
							const char* const key = "shaderType";
							lua_pushstring(&io_luaState, key);
							lua_gettable(&io_luaState, -2);
							const char* pShaderType = lua_tostring(&io_luaState, -1);
							o_pMaterial->_pTexData[i - 1]._shaderType = GetShaderType(pShaderType);
							lua_pop(&io_luaState, 1);
						}
						lua_pop(&io_luaState, 1);
					}
				}
			}
			lua_pop(&io_luaState, 1);
		}
		return true;
	}

	bool LoadMaterial(const char* const i_path, EAE_Engine::Tools::MaterialData*& o_pMaterial)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = NULL;
		{
			luaState = luaL_newstate();
			if (!luaState)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "Failed to create a new Lua state\n";
				EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str());
				goto OnExit;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_path);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				{
					std::stringstream errorMessage;
					errorMessage << "failed to load the luaeffectfile: " << lua_tostring(luaState, -1) << ".\n";
					EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str());
				}
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			const int argumentCount = 0;
			const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			const int noMessageHandler = 0;
			const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
			if (luaResult == LUA_OK)
			{
				// A well-behaved asset file will only return a single value
				const int returnedValueCount = lua_gettop(luaState);
				if (returnedValueCount == 1)
				{
					// A correct asset file _must_ return a table, this is our Material file's regular.
					if (!lua_istable(luaState, -1))
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "Material files must return a table (instead of a " <<
							luaL_typename(luaState, -1) << ")\n";
						EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str());
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "Material files must return a single table (instead of " <<
						returnedValueCount << " values)\n";
					EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str());
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << lua_tostring(luaState, -1) << ".\n";
				EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str());
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}
		// Load the MaterialData
		{
			const char* const key = "materialData";
			lua_pushstring(luaState, key);
			lua_gettable(luaState, -2);
			if (lua_istable(luaState, -1))
			{
				if (!LoadMaterialData(*luaState, o_pMaterial))
				{
					wereThereErrors = true;
				}
				// Pop the table
				lua_pop(luaState, 1);
			}
		}

		// Pop the table
		lua_pop(luaState, 1);

	OnExit:
		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack,
			// regardless of any errors encountered while loading the file:
			assert(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = NULL;
		}

		return !wereThereErrors;
	}

	bool LoadMaterialData(lua_State& io_luaState, EAE_Engine::Tools::MaterialData*& o_pMaterial)
	{
		bool wereThereErrors = false;
		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1
		if (!LoadTableValues(io_luaState, o_pMaterial))
		{
			wereThereErrors = true;
		}
		return !wereThereErrors;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////


namespace EAE_Engine
{
	namespace Tools
	{
		bool GenerateBinaryMaterialData(const char* i_luaMaterialFile, char*& o_pBuffer, uint32_t& o_sizeOfBuffer)
		{
			MaterialData* pMaterialData = nullptr;
			uint32_t o_sizeOfUniformNameBuffer = 0;
			uint32_t o_sizeOfValueBuffer = 0;
			uint32_t o_sizeOfTexPathBuffer = 0;
			uint32_t o_sizeOfTexUniformNameBuffer = 0;
			if (!LoadMaterial(i_luaMaterialFile, pMaterialData))
			{
				return false;
			}
			uint32_t sourcePathLength = (uint32_t)strlen(pMaterialData->_pEffectPath);
			uint32_t effectSourceLength = (uint32_t)strlen(pMaterialData->_pEffectSource);
			uint32_t lengthOfEffectPath = (sourcePathLength + effectSourceLength + 1);
			// 1. We need to save the sUniformDesc for each Uniform Variable.
			uint32_t uniformDescArrayLength = (uint32_t)(pMaterialData->_uniformCount * sizeof(Graphics::UniformDesc));
			// 2. Get the general information of the UniformDesc Data
			for (uint32_t index = 0; index < pMaterialData->_uniformCount; ++index)
			{
				o_sizeOfValueBuffer += pMaterialData->_pUniformData[index]._valueCount * sizeof(float);
				o_sizeOfUniformNameBuffer += (uint32_t)strlen(pMaterialData->_pUniformData[index]._pName) + 1;
			}
			// 1. We need to save the sTextureDesc for each texture.
			uint32_t textureDescArrayLength = (uint32_t)(pMaterialData->_texCount * sizeof(Graphics::TextureDesc));
			// 2. Get the general information of the TextureDesc Data
			for (uint32_t index = 0; index < pMaterialData->_texCount; ++index)
			{
				o_sizeOfTexPathBuffer += (uint32_t)strlen(pMaterialData->_pTexData[index]._pTexPath) + 1;
				o_sizeOfTexUniformNameBuffer += (uint32_t)strlen(pMaterialData->_pTexData[index]._pName) + 1;
			}
			// Here we need to alloc the memory for the data we need to ouput to the binary file
			// First, we will save a MaterialDesc and the information.
			// In this MaterialDesc:
			// _pEffect will just be null. 
			// the _sizeOfMaterialBuffer will be the size of this buffer.
			// the _offsetOfUniformVariableValueBuffer will be the offset to the all value buffer of the Uniform Variables. 
			// the _offsetOfUniformVariableNameBuffer will be the offset to the all names buffer of the Uniform Variables.
			// Third, uniformDescArrayLength will be the space for all of the UniformDesc structure.
			// Forth, o_sizeOfValueBuffer will be the space for all of the UniformVariables Values,
			// and o_sizeOfNameBuffer will be the space for all of the UniformVariables Names.
			// At the end, lengthOfEffectPath = (sourcePathLength + effectSourceLength + 1) will be the path name of the effect, it ends with a /0.
			o_sizeOfBuffer = sizeof(Graphics::MaterialDesc) + uniformDescArrayLength + textureDescArrayLength + o_sizeOfValueBuffer + 
				o_sizeOfUniformNameBuffer + o_sizeOfTexPathBuffer + o_sizeOfTexUniformNameBuffer + lengthOfEffectPath;
			// allocate memory for file content
			o_pBuffer = new char[o_sizeOfBuffer];
			SetMem(reinterpret_cast<uint8_t*>(o_pBuffer), o_sizeOfBuffer, 0);
			uint32_t offset = 0;
			// Write the offset of the uniform variable value buffer, 
			// and the offset of the uniform variable name buffer
			Graphics::MaterialDesc* pMaterialDesc = (Graphics::MaterialDesc*)(reinterpret_cast<uint8_t*>(o_pBuffer) + offset);
			{
				// Write the effect file path & name to the buffer
				{
					uint32_t offsetForEffectPath = sizeof(Graphics::MaterialDesc) + uniformDescArrayLength + textureDescArrayLength + o_sizeOfValueBuffer +
						o_sizeOfUniformNameBuffer + o_sizeOfTexPathBuffer + o_sizeOfTexUniformNameBuffer;
					// Since we save the MaterialDesc at first and EffectPathName at the end, we should have:
					assert(o_sizeOfBuffer - lengthOfEffectPath == offsetForEffectPath);
					// Set the content of the Effect Pointer to be the Offset to the effect name path
					*reinterpret_cast<size_t*>(&(pMaterialDesc->_pEffect)) = offsetForEffectPath;
					// Write effectFilePath
					CopyMem(reinterpret_cast<uint8_t*>(pMaterialData->_pEffectPath), reinterpret_cast<uint8_t*>(o_pBuffer) + offsetForEffectPath, sourcePathLength);
					offsetForEffectPath += sourcePathLength;
					// Write effectFileName
					CopyMem(reinterpret_cast<uint8_t*>(pMaterialData->_pEffectSource), reinterpret_cast<uint8_t*>(o_pBuffer) + offsetForEffectPath, effectSourceLength);
				}
				pMaterialDesc->_sizeOfMaterialBuffer = o_sizeOfBuffer - lengthOfEffectPath;
				pMaterialDesc->_uniformCount = pMaterialData->_uniformCount;
				pMaterialDesc->_textureCount = pMaterialData->_texCount;
				// we need to jump over the MaterialDesc the uniformDescArrayLength for the Uniform Variable Value Buffer
				// 1. We put the offset of the UniformVarible Value Buffer
				// 2. We put the offset of the UniofrmVariable Name Buffer
				pMaterialDesc->_offsetOfUniformVariableValueBuffer = sizeof(Graphics::MaterialDesc) + uniformDescArrayLength + textureDescArrayLength;
				pMaterialDesc->_offsetOfUniformVariableNameBuffer = pMaterialDesc->_offsetOfUniformVariableValueBuffer + o_sizeOfValueBuffer;
				// 1. We put the offset of the path of the textures at first, 
				// 2. We put the offset of the Uniform Variable Names of the texture sampler.
				pMaterialDesc->_offsetOfTexturePathBuffer = pMaterialDesc->_offsetOfUniformVariableNameBuffer + o_sizeOfUniformNameBuffer;
				pMaterialDesc->_offsetOfTextureSamplerBuffer = pMaterialDesc->_offsetOfTexturePathBuffer + o_sizeOfTexPathBuffer;
				// We also need to set the value of the MaterialCost
				pMaterialDesc->_materialCost = pMaterialData->_materialCost;
				// Now we have finished the setting of the MaterialDesc, so we add sizeof(MaterialDesc) to the offset.
				offset += sizeof(Graphics::MaterialDesc);
			}
			// Now the offset should point to the start of the UniformDesc buffers.
			// So Let's set all of UniformDesc information.
			{
				uint32_t offsetInValueBuffer = 0;
				uint32_t offsetInNameBuffer = 0;
				uint32_t totalOffsetOfUniformVariableValueBuffer = pMaterialDesc->_offsetOfUniformVariableValueBuffer;
				uint32_t totalOffsetOfUniformVariableNameBuffer = pMaterialDesc->_offsetOfUniformVariableNameBuffer;
				for (size_t i = 0; i < pMaterialData->_uniformCount; ++i)
				{
					Graphics::UniformDesc* pUniformDesc = (Graphics::UniformDesc*)(reinterpret_cast<uint8_t*>(o_pBuffer) + offset);
					// set the shaderType
					pUniformDesc->_shaderType = pMaterialData->_pUniformData[i]._shaderType;
					// Set the UniformVariable Value Buffer
					{
						pUniformDesc->_offsetInValueBuffer = offsetInValueBuffer;
						uint32_t sizeOfValueBufferToCopy = pMaterialData->_pUniformData[i]._valueCount * sizeof(float);
						pUniformDesc->_valueBufferSize = sizeOfValueBufferToCopy;
						// Copy the value to the target position
						CopyMem((uint8_t*)pMaterialData->_pUniformData[i]._pValue, reinterpret_cast<uint8_t*>(o_pBuffer) + totalOffsetOfUniformVariableValueBuffer + offsetInValueBuffer, sizeOfValueBufferToCopy);
						offsetInValueBuffer += sizeOfValueBufferToCopy;
					}
					// Set the UniformVariable Name Buffer
					{
						// Be careful, I am using a tricky method that I save the offset to the Name in the _handle.
						// But be careful that the _handle will be different on x64 and x86,
						// so we really really need to take care the size of it.
						*reinterpret_cast<size_t*>(&(pUniformDesc->_handle)) = offsetInNameBuffer;
						uint32_t sizeOfValueBufferToCopy = (uint32_t)strlen(pMaterialData->_pUniformData[i]._pName);
						// Copy the name to the target position
						CopyMem((uint8_t*)pMaterialData->_pUniformData[i]._pName, reinterpret_cast<uint8_t*>(o_pBuffer) + totalOffsetOfUniformVariableNameBuffer + offsetInNameBuffer, sizeOfValueBufferToCopy);
						offsetInNameBuffer += sizeOfValueBufferToCopy + 1;
					}
					// move to set the next UniformDesc
					offset += sizeof(Graphics::UniformDesc);
				}
			}
			// Now the offset should point to the start of the TextureDesc buffers.
			// So Let's set all of TextureDesc information.
			{
				uint32_t offsetInPathBuffer = 0;
				uint32_t offsetInSamplerBuffer = 0;
				uint32_t totalOffsetOfTexturePathBuffer = pMaterialDesc->_offsetOfTexturePathBuffer;
				uint32_t totalOffsetOfTextureSamplerBuffer = pMaterialDesc->_offsetOfTextureSamplerBuffer;
				for (size_t i = 0; i < pMaterialData->_texCount; ++i)
				{
					Graphics::TextureDesc* pTexDesc = (Graphics::TextureDesc*)(reinterpret_cast<uint8_t*>(o_pBuffer) + offset);
					// Set the Texture Path Buffer
					{
						*reinterpret_cast<size_t*>(&(pTexDesc->_texture)) = offsetInPathBuffer;
						uint32_t sizeOfPathBufferToCopy = (uint32_t)strlen(pMaterialData->_pTexData[i]._pTexPath);
						CopyMem((uint8_t*)pMaterialData->_pTexData[i]._pTexPath, reinterpret_cast<uint8_t*>(o_pBuffer) + totalOffsetOfTexturePathBuffer + offsetInPathBuffer, sizeOfPathBufferToCopy);
						offsetInPathBuffer += sizeOfPathBufferToCopy + 1;
					}
					// Set the Texture Sampler UniformVariable Buffer
					{
						*reinterpret_cast<tSamplerID*>(&(pTexDesc->_samplerID)) = offsetInSamplerBuffer;
						uint32_t sizeOfSamplerNameToCopy = (uint32_t)strlen(pMaterialData->_pTexData[i]._pName);
						CopyMem((uint8_t*)pMaterialData->_pTexData[i]._pName, reinterpret_cast<uint8_t*>(o_pBuffer) + totalOffsetOfTextureSamplerBuffer + offsetInSamplerBuffer, sizeOfSamplerNameToCopy);
						offsetInSamplerBuffer += sizeOfSamplerNameToCopy + 1;
					}
					// set the shaderType
					pTexDesc->_shaderType = pMaterialData->_pTexData[i]._shaderType;
					// move to set the next UniformDesc
					offset += sizeof(Graphics::TextureDesc);
				}
			}
			SAFE_DELETE(pMaterialData);
			return true;
		}

	}
}