#include "LuaEffectLoader.h"
#include <cassert>
#include <sstream>
#include "External/Lua/Includes.h"
#include "Engine/Math/Vector.h"
#include "Engine/General/MemoryOp.h"
#include "Engine/Graphics/GraphicsInclude.h"
#include "Tools/BuilderHelper/cbBuilder.h"

// Helper Function Declarations
//=============================

namespace
{
	bool LoadTableValues(lua_State& io_luaState, EAE_Engine::Graphics::sEffect*& o_pEffect);
	bool LoadTableValues_filenames(lua_State& io_luaState, EAE_Engine::Graphics::sEffect*& o_pEffect);

	bool LoadEffectInfo(const char* const i_path, EAE_Engine::Graphics::sEffect*& o_pEffect);
}

// Helper Function Definitions
//============================

namespace
{
	bool LoadTableValues(lua_State& io_luaState, EAE_Engine::Graphics::sEffect*& o_pEffect)
	{
		if (!LoadTableValues_filenames(io_luaState, o_pEffect))
		{
			return false;
		}
		return true;
	}

	bool LoadTableValues_filenames(lua_State& io_luaState, EAE_Engine::Graphics::sEffect*& o_pEffect)
	{
		bool wereThereErrors = false;
		o_pEffect = new EAE_Engine::Graphics::sEffect();
		if (!o_pEffect)
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
			o_pEffect->_pSourcePath = _strdup(pSourcePath);
		}
		{
			const char* const key = "vsSource";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			const char* pVS = lua_tostring(&io_luaState, -1);
			lua_pop(&io_luaState, 1);
			o_pEffect->_pVS = _strdup(pVS);
		}
		{
			const char* const key = "fsSource";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			const char* pFS = lua_tostring(&io_luaState, -1);
			lua_pop(&io_luaState, 1);
			o_pEffect->_pFS = _strdup(pFS);
		}

		int alpha = false;
		int depthtesting = true;
		int depthwriting = true;
		int faceculling = true;
		{
			const char* const key = "renderState";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				// alpha 
				const char* const alphakey = "Alpha";
				lua_pushstring(&io_luaState, alphakey);
				if (!lua_isnil(&io_luaState, -1))
				{
					lua_gettable(&io_luaState, -2);
					alpha = lua_toboolean(&io_luaState, -1);
				}
				lua_pop(&io_luaState, 1);
				// Depth Testing
				const char* const dtkey = "DepthTesting";
				lua_pushstring(&io_luaState, dtkey);
				if (!lua_isnil(&io_luaState, -1))
				{
					lua_gettable(&io_luaState, -2);
					depthtesting = lua_toboolean(&io_luaState, -1);
				}
				lua_pop(&io_luaState, 1);
				// Depth Writing
				const char* const dwkey = "DepthWriting";
				lua_pushstring(&io_luaState, dwkey);
				if (!lua_isnil(&io_luaState, -1))
				{
					lua_gettable(&io_luaState, -2);
					depthwriting = lua_toboolean(&io_luaState, -1);
				}
				lua_pop(&io_luaState, 1);
				// Face Culling
				const char* const fckey = "FaceCulling";
				lua_pushstring(&io_luaState, fckey);
				if (!lua_isnil(&io_luaState, -1))
				{
					lua_gettable(&io_luaState, -2);
					faceculling = lua_toboolean(&io_luaState, -1);
				}
				lua_pop(&io_luaState, 1);
			}
			lua_pop(&io_luaState, 1);
		}
		o_pEffect->_renderState = (alpha ? EAE_Engine::Graphics::RenderState::Alpha : 0x0) |
			(depthtesting ? EAE_Engine::Graphics::RenderState::DepthTesting : 0x0) |
			(depthwriting ? EAE_Engine::Graphics::RenderState::DepthWriting : 0x0) |
			(faceculling ? EAE_Engine::Graphics::RenderState::FaceCulling : 0x0);
		return true;
	}

	bool LoadEffectInfo(const char* const i_path, EAE_Engine::Graphics::sEffect*& o_pEffect)
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
					// A correct asset file _must_ return a table, this is our AOSMesh file's regular.
					if (!lua_istable(luaState, -1))
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "Effect files must return a table (instead of a " <<
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
					errorMessage << "Effect files must return a single table (instead of " <<
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
				errorMessage << lua_tostring(luaState, -1)<<".\n";
				EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str());
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1
		if (!LoadTableValues(*luaState, o_pEffect))
		{
			wereThereErrors = true;
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////


namespace EAE_Engine
{
	namespace Tools
	{
		bool GenerateBinaryEffectData(const char* i_luaEffectFile, char*& o_pBuffer, size_t& o_sizeOfBuffer)
		{
			EAE_Engine::Graphics::sEffect* pEffect = nullptr;
			if (!LoadEffectInfo(i_luaEffectFile, pEffect))
			{
				return false;
			}
			size_t sourcePathLength = strlen(pEffect->_pSourcePath);
			size_t vsSourceLength = strlen(pEffect->_pVS);
			size_t fsSourceLength = strlen(pEffect->_pFS);
			size_t renderStateLength = sizeof(pEffect->_renderState);
			o_sizeOfBuffer = sourcePathLength + 1 + vsSourceLength + 1 + fsSourceLength + 1 + renderStateLength + 1;
			// allocate memory for file content
			o_pBuffer = new char[o_sizeOfBuffer];
			SetMem(reinterpret_cast<uint8_t*>(o_pBuffer), o_sizeOfBuffer, 0);
			size_t offset = 0;
			// Write vsSource
			CopyMem(reinterpret_cast<uint8_t*>(pEffect->_pSourcePath), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sourcePathLength);
			offset += sourcePathLength + 1;
			// Write vsSource
			CopyMem(reinterpret_cast<uint8_t*>(pEffect->_pVS), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, vsSourceLength);
			offset += vsSourceLength + 1;
			// Write fsSource
			CopyMem(reinterpret_cast<uint8_t*>(pEffect->_pFS), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, fsSourceLength);
			offset += fsSourceLength + 1;
			// Write renderStateSource
			CopyMem(reinterpret_cast<uint8_t*>(&pEffect->_renderState), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, renderStateLength);
			offset += renderStateLength + 1;
			SAFE_DELETE(pEffect);
			return true;
		}

	}
}