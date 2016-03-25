#include "LuaMeshLoader.h"
#include <cassert>
#include <sstream>
#include "External/Lua/Includes.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/MathTool.h"
#include "Engine/General/MemoryOp.h"
#include "Engine/Mesh/MeshLoader.h"
#include "Tools/BuilderHelper/cbBuilder.h"

// Helper Function Declarations
//=============================

namespace
{
	bool LoadVertexFormat(lua_State& io_luaState, EAE_Engine::Mesh::VertexFormat*& o_pVertexFormat);
	bool LoadEachVertexElement(lua_State& io_luaState, EAE_Engine::Mesh::VertexElement& o_vertexElement);

	bool LoadTableVertices(lua_State& io_luaState, EAE_Engine::Mesh::sVertex*& o_pVertices, uint32_t& o_vertexCount,
		uint32_t*& o_pIndices, uint32_t& o_indexCount, 
		EAE_Engine::Mesh::sSubMesh*& o_pSubMeshes, uint32_t& o_subMeshCount);
	bool LoadTableValues_vertices(lua_State& io_luaState, EAE_Engine::Mesh::sVertex*& o_pVertices, uint32_t& o_vertexCount);
	bool LoadTableValues_vertex(lua_State& io_luaState, EAE_Engine::Mesh::sVertex& o_vertex);
	bool LoadTableValues_indices(lua_State& io_luaState, uint32_t*& o_pIndices, uint32_t& o_indexCount);
	bool LoadTableValues_subMeshes(lua_State& io_luaState, EAE_Engine::Mesh::sSubMesh*& o_pSubMeshes, uint32_t& o_subMeshCount);
	bool LoadMeshInfo(const char* const i_path, EAE_Engine::Mesh::VertexFormat*& o_pVertexFormat, 
		EAE_Engine::Mesh::sVertex*& pVertices, uint32_t& o_vertexCount,
		uint32_t*& o_pIndices, uint32_t& o_indexCount, 
		EAE_Engine::Mesh::sSubMesh*& o_pSubMeshes, uint32_t& o_subMeshCount);
}

// Helper Function Definitions
//============================

namespace
{
	bool LoadVertexFormat(lua_State& io_luaState, EAE_Engine::Mesh::VertexFormat*& o_pVertexFormat)
	{
		o_pVertexFormat = nullptr;
		// Right now the mesh table is at -1.
		// After the following table operation it will be at -2.
		// and the "vertices" table will be at -1:
		const char* const key = "vertexFormat";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		bool wereThereErrors = false;
		if (lua_istable(&io_luaState, -1))
		{
			o_pVertexFormat = new EAE_Engine::Mesh::VertexFormat();
			int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength > 0)
			{
				EAE_Engine::Mesh::VertexElement* pVertexElements = new EAE_Engine::Mesh::VertexElement[arrayLength];
				o_pVertexFormat->_pElements = pVertexElements;
				o_pVertexFormat->_vertexCount = (uint32_t)arrayLength;
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (!LoadEachVertexElement(io_luaState, pVertexElements[i - 1]) || !lua_istable(&io_luaState, -1))
					{
						wereThereErrors = true;
						lua_pop(&io_luaState, 1);//this pop works for the vertex itself
						SAFE_DELETE_ARRAY(pVertexElements);
						SAFE_DELETE(o_pVertexFormat);
						goto OnExit;
					}
					lua_pop(&io_luaState, 1);//this pop works for the vertex itself
				}
			}
			else 
			{
				o_pVertexFormat->_pElements = nullptr;
				o_pVertexFormat->_vertexCount = (uint32_t)0;
			}
		}

	OnExit:
		// Pop the vertices table
		lua_pop(&io_luaState, 1);
		return !wereThereErrors;
	}

	bool LoadEachVertexElement(lua_State& io_luaState, EAE_Engine::Mesh::VertexElement& o_vertexElement)
	{
		bool wereThereErrors = false;
		int arrayLength = luaL_len(&io_luaState, -2);
		assert(arrayLength == 4); 
		// load count
		{
			lua_pushinteger(&io_luaState, 1);
			lua_gettable(&io_luaState, -2);
			o_vertexElement._elementCount = (uint32_t)lua_tointeger(&io_luaState, -1);
			lua_pop(&io_luaState, 1);
		}
		// load type
		{
			lua_pushinteger(&io_luaState, 2);
			lua_gettable(&io_luaState, -2);
			const char* pTypeStr= lua_tostring(&io_luaState, -1);
			if (strcmp(pTypeStr, "FLOAT") == 0) 
				o_vertexElement._type = EAE_Engine::Mesh::FLOAT;
			else if (strcmp(pTypeStr, "BYTE") == 0)
				o_vertexElement._type = EAE_Engine::Mesh::BYTE;
			lua_pop(&io_luaState, 1);
		}
		// load NORMALIZED
		{
			lua_pushinteger(&io_luaState, 3);
			lua_gettable(&io_luaState, -2);
			int normolized = lua_toboolean(&io_luaState, -1);
			o_vertexElement._normalized = normolized == 0 ? false : true;
			lua_pop(&io_luaState, 1);
		}
		// load Syntax
		{
			lua_pushinteger(&io_luaState, 4);
			lua_gettable(&io_luaState, -2);
			const char* pStr = lua_tostring(&io_luaState, -1);
			if (strcmp(pStr, "POSITION") == 0)
				o_vertexElement._syntax = EAE_Engine::Mesh::POSITION;
			else if (strcmp(pStr, "NORMAL") == 0)
				o_vertexElement._syntax = EAE_Engine::Mesh::NORMAL;
			else if (strcmp(pStr, "TEXTCOORD") == 0)
				o_vertexElement._syntax = EAE_Engine::Mesh::TEXTCOORD;
			else if (strcmp(pStr, "COLOR") == 0)
				o_vertexElement._syntax = EAE_Engine::Mesh::COLOR;
			lua_pop(&io_luaState, 1);
		}
		return !wereThereErrors;
	}

	bool LoadTableVertices(lua_State& io_luaState, EAE_Engine::Mesh::sVertex*& o_pVertices, uint32_t& o_vertexCount, 
		uint32_t*& o_pIndices, uint32_t& o_indexCount, 
		EAE_Engine::Mesh::sSubMesh*& o_pSubMeshes, uint32_t& o_subMeshCount)
	{
		if (!LoadTableValues_vertices(io_luaState, o_pVertices, o_vertexCount))
		{
			return false;
		}
		if (!LoadTableValues_indices(io_luaState, o_pIndices, o_indexCount))
		{
			return false;
		}
		if (!LoadTableValues_subMeshes(io_luaState, o_pSubMeshes, o_subMeshCount))
		{
			return false;
		}
		return true;
	}

	bool LoadTableValues_vertices(lua_State& io_luaState, EAE_Engine::Mesh::sVertex*& o_pVertices, uint32_t& o_vertexCount)
	{
		bool wereThereErrors = false;

		// Right now the mesh table is at -1.
		// After the following table operation it will be at -2.
		// and the "vertices" table will be at -1:
		const char* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{
			const int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength > 0)
			{
				o_vertexCount = arrayLength;
				o_pVertices = new EAE_Engine::Mesh::sVertex[arrayLength];

				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (!LoadTableValues_vertex(io_luaState, o_pVertices[i-1]))
					{
						wereThereErrors = true;
						lua_pop(&io_luaState, 1);//this pop works for the vertex itself
						SAFE_DELETE_ARRAY(o_pVertices);
						goto OnExit;
					}
					lua_pop(&io_luaState, 1);//this pop works for the vertex itself
				}
			}
		}

	OnExit:
		// Pop the vertices table
		lua_pop(&io_luaState, 1);
		return !wereThereErrors;
	}

	bool LoadTableValues_vertex(lua_State& io_luaState, EAE_Engine::Mesh::sVertex& o_vertex)
	{
		bool wereThereErrors = false;
		// Get the value of "position"
		{
			const char* key = "position";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (!lua_istable(&io_luaState, -1) || lua_isnil(&io_luaState, -2))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			EAE_Engine::Math::Vector4 position = EAE_Engine::Math::Vector4::Zero;
			const int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength > 0)
			{
				// Remember that Lua arrays are 1-based, not 0-based!
				for (int i = 1; i <= arrayLength && i <= 4; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					position._u[i-1] = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
				}
			}
			lua_pop(&io_luaState, 1);
			o_vertex.x = position._u[0];
			o_vertex.y = position._u[1];
			o_vertex.z = position._u[2];
		}
		// Get the value of "color"
		{
			const char* key = "color";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (!lua_istable(&io_luaState, -1) || lua_isnil(&io_luaState, -2))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			EAE_Engine::Math::Vector4 color = EAE_Engine::Math::Vector4::Zero;
			const int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength > 0)
			{
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength && i <= 4; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					color._u[i-1] = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
				}
			}
			lua_pop(&io_luaState, 1);
			o_vertex.r = (uint8_t)(EAE_Engine::Math::clamp<float>(color._u[0], 0.0f, 1.0f) * 255.0f);
			o_vertex.g = (uint8_t)(EAE_Engine::Math::clamp<float>(color._u[1], 0.0f, 1.0f) * 255.0f);
			o_vertex.b = (uint8_t)(EAE_Engine::Math::clamp<float>(color._u[2], 0.0f, 1.0f) * 255.0f);
			o_vertex.a = (uint8_t)(EAE_Engine::Math::clamp<float>(color._u[3], 0.0f, 1.0f) * 255.0f);
		}
		// Get the value of "Normal"
		{
			const char* key = "normal";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_isnil(&io_luaState, -2) || !lua_istable(&io_luaState, -1))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			EAE_Engine::Math::Vector3 normal = EAE_Engine::Math::Vector3::Zero;
			const int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength > 0)
			{
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength && i <= 3; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					normal._u[i - 1] = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
				}
			}
			lua_pop(&io_luaState, 1);
			o_vertex.nx = normal._u[0];
			o_vertex.ny = normal._u[1];
			o_vertex.nz = normal._u[2];
		}
		// Get the value of "textureCoordinate"
		{
			const char* key = "textureCoordinate";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (!lua_istable(&io_luaState, -1) || lua_isnil(&io_luaState, -2))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			EAE_Engine::Math::Vector2 uv = EAE_Engine::Math::Vector2::Zero;
			const int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength > 0)
			{
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength && i <= 2; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					uv._u[i - 1] = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
				}
			}
			lua_pop(&io_luaState, 1);
			o_vertex.u = uv._u[0];
			o_vertex.v = uv._u[1];
		}
		return !wereThereErrors;

	OnExit:
		// Pop the textures table
		lua_pop(&io_luaState, 1);
		return !wereThereErrors;
	}

	bool LoadTableValues_indices(lua_State& io_luaState, uint32_t*& o_pIndices, uint32_t& o_indexCount)
	{
		bool wereThereErrors = false;

		const char* const key = "triangleIndices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{
			const int triangleArrayLength = luaL_len(&io_luaState, -1);
			if (triangleArrayLength > 0)
			{
				uint32_t indexCount = 0;
				o_indexCount = triangleArrayLength * 3;
				o_pIndices = new uint32_t[o_indexCount];
				const int triangleArrayLength = luaL_len(&io_luaState, -1);
				for (int triangleIndex = 1; triangleIndex <= triangleArrayLength; ++triangleIndex)
				{
					lua_pushinteger(&io_luaState, triangleIndex);
					lua_gettable(&io_luaState, -2);
					// For this triangle, we iterate its 3 indices
					const int arrayLength = luaL_len(&io_luaState, -1);
					assert(arrayLength == 3);
					for (int i = 1; i <= arrayLength; ++i)
					{
						lua_pushinteger(&io_luaState, i);
						lua_gettable(&io_luaState, -2);
						o_pIndices[(i - 1) + indexCount] = (uint32_t)lua_tointeger(&io_luaState, -1);
						lua_pop(&io_luaState, 1);
					}
					indexCount += 3;//because we loaded 3 indices of each triangle, so we need to add 3
									// To iterate to the next key/value pair, pop the value, but leave the key:
									// pop to next trianlge
					lua_pop(&io_luaState, 1);
				}
			}
		}
		// Pop the indices table
		lua_pop(&io_luaState, 1);
		return !wereThereErrors;
	}

	bool LoadTableValues_subMeshes(lua_State& io_luaState, EAE_Engine::Mesh::sSubMesh*& o_pSubMeshes, uint32_t& o_subMeshCount)
	{
		bool wereThereErrors = false;

		const char* const key = "subMeshes";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{
			const int subMeshArrayLength = luaL_len(&io_luaState, -1);
			if (subMeshArrayLength > 0)
			{
				// record the information of subMeshes and count the Indices.
				o_subMeshCount = subMeshArrayLength;
				o_pSubMeshes = new EAE_Engine::Mesh::sSubMesh[o_subMeshCount];
				uint32_t subMeshIndex = 0;
				// iterate all of the SubMesh tables
				lua_pushnil(&io_luaState);	// This tells lua_next() to return the first key
				const int tableIndex = -2;
				while (lua_next(&io_luaState, tableIndex))
				{
					// After lua_next() the key is at -2 and the value is at -1:
					assert(lua_istable(&io_luaState, -1));
					{
						// Iterate each of the SubMeshTable
						const int arrayLength = luaL_len(&io_luaState, -1);
						assert(arrayLength == 2);
						// Get firstIndex, remember that Lua count starts from 1.
						lua_pushinteger(&io_luaState, 1);
						lua_gettable(&io_luaState, -2);
						o_pSubMeshes[subMeshIndex]._firstIndex = (uint32_t)lua_tointeger(&io_luaState, -1);
						lua_pop(&io_luaState, 1);
						// Get lastIndex
						lua_pushinteger(&io_luaState, 2);
						lua_gettable(&io_luaState, -2);
						o_pSubMeshes[subMeshIndex]._lastIndex = (uint32_t)lua_tointeger(&io_luaState, -1);
						lua_pop(&io_luaState, 1);
					}
					++subMeshIndex;
					// To iterate to the next key/value pair, pop the value, but leave the key:
					lua_pop(&io_luaState, 1);
				}
			}
		}
		// Pop the indices table
		lua_pop(&io_luaState, 1);
		return !wereThereErrors;
	}

	bool LoadMeshInfo(const char* const i_path, EAE_Engine::Mesh::VertexFormat*& o_pVertexFormat, 
		EAE_Engine::Mesh::sVertex*& o_pVertices, uint32_t& o_vertexCount,
		uint32_t*& o_pIndices, uint32_t& o_indexCount, 
		EAE_Engine::Mesh::sSubMesh*& o_pSubMeshes, uint32_t& o_subMeshCount)
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
					errorMessage << "failed to load the luameshfile: " << lua_tostring(luaState, -1) << ".\n";
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
						errorMessage << "Mesh files must return a table (instead of a " <<
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
					errorMessage << "Mesh files must return a single table (instead of " <<
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
		if (!LoadVertexFormat(*luaState, o_pVertexFormat))
		{
			wereThereErrors = true;
		}
		if (!LoadTableVertices(*luaState, o_pVertices, o_vertexCount, o_pIndices, o_indexCount, o_pSubMeshes, o_subMeshCount))
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
		bool GenerateBinaryMeshData(const char* i_luaMeshFile, char*& o_pBuffer, uint32_t& sizeOfBuffer)
		{
			EAE_Engine::Mesh::VertexFormat* pVertexFormat = nullptr;
			EAE_Engine::Mesh::sVertex* pVertices = nullptr;
			uint32_t vertexCount = 0;
			uint32_t* pIndices = nullptr;
			uint32_t indexCount = 0;
			Mesh::sSubMesh* pSubMeshes = nullptr;
			uint32_t subMeshCount = 0;
			if (!LoadMeshInfo(i_luaMeshFile, pVertexFormat, 
				pVertices, vertexCount, pIndices, indexCount, pSubMeshes, subMeshCount))
			{
				return false;
			}

			// Because we will use the DDS texture, which is a D3D texture.
			// In its textureCoordinate, (0, 0) is top_left and (1, 1) is right_bottom.
			// However, in openGL and maya textureCoordinate, the (0, 0) is the left_bottom and (1, 1) is the right_top.
			// So we will convert the v value to 1.0f- v; for the binary mesh file.
			for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				EAE_Engine::Mesh::sVertex& pVertex = pVertices[vertexIndex];
				pVertex.v = 1.0f - pVertex.v;
			}
			uint32_t vertexElementCount = pVertexFormat ? pVertexFormat->_vertexCount : 0;
			sizeOfBuffer = sizeof(uint32_t) + sizeof(Mesh::VertexElement) * vertexElementCount + // VertexElement Count, VertexFormat,
				sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + // Count of Vertices, Count of Indices, Count of SubMeshes
				sizeof(EAE_Engine::Mesh::sVertex) * vertexCount +  // Vertices Buffer
				sizeof (uint32_t) * indexCount +                       // Indices Buffer
				sizeof(EAE_Engine::Mesh::sSubMesh) * subMeshCount; // SubMeshes Buffer

			// allocate memory for file content
			o_pBuffer = new char[sizeOfBuffer];
			SetMem((uint8_t*)o_pBuffer, sizeOfBuffer, 0);
			uint32_t offset = 0;
			// Write VertexElement Count
			CopyMem(reinterpret_cast<uint8_t*>(&vertexElementCount), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			// Write VertexElements(Vertex Format)
			CopyMem(reinterpret_cast<uint8_t*>(pVertexFormat->_pElements), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(Mesh::VertexElement) * vertexElementCount);
			offset += sizeof(Mesh::VertexElement) * vertexElementCount;
			SAFE_DELETE_ARRAY(pVertexFormat->_pElements);
			SAFE_DELETE(pVertexFormat);
			// Write VertexCount
			CopyMem(reinterpret_cast<uint8_t*>(&vertexCount), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			// Write IndexCount
			CopyMem(reinterpret_cast<uint8_t*>(&indexCount), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			// Write SubMeshCount
			CopyMem(reinterpret_cast<uint8_t*>(&subMeshCount), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			// Write Vertices
			CopyMem(reinterpret_cast<uint8_t*>(pVertices), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(EAE_Engine::Mesh::sVertex) * vertexCount);
			offset += sizeof(EAE_Engine::Mesh::sVertex) * vertexCount;
			// Before write the indices, we need to convert it for D3D since it uses clockwise indices.
#if defined( EAEENGINE_PLATFORM_D3D9 )
			// Convert the counterclockwise indices to to clockwise
			for (uint32_t triangle_index = 0; triangle_index < indexCount / 3; ++triangle_index)
			{
				uint32_t temp = pIndices[triangle_index * 3 + 1];
				pIndices[triangle_index * 3 + 1] = pIndices[triangle_index * 3 + 2];
				pIndices[triangle_index * 3 + 2] = temp;
			}
#endif
			// Write Indices
			CopyMem(reinterpret_cast<uint8_t*>(pIndices), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(uint32_t) * indexCount);
			offset += sizeof(uint32_t) * indexCount;
			// Write SubMeshes
			CopyMem(reinterpret_cast<uint8_t*>(pSubMeshes), reinterpret_cast<uint8_t*>(o_pBuffer) + offset, sizeof(EAE_Engine::Mesh::sSubMesh) * subMeshCount);
			offset += sizeof(uint32_t) * subMeshCount;

			SAFE_DELETE_ARRAY(pVertices);
			SAFE_DELETE_ARRAY(pIndices);
			SAFE_DELETE_ARRAY(pSubMeshes);
			return true;
		}

	}
}