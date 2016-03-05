#ifndef EAE_ENGINE_SPATIAL_PARTITION_OCTREE
#define EAE_ENGINE_SPATIAL_PARTITION_OCTREE

#include "Engine/Math/Vector.h"
#include "Engine/Mesh/AOSMeshData.h"
#include "Engine/General/MemoryOp.h"
#include <vector>
#include <fstream>

namespace EAE_Engine 
{
	namespace Core 
	{
		struct TriangleIndex
		{
			TriangleIndex() = default;
			union
			{
				uint32_t _indices[3];
				struct
				{
					uint32_t _index0;
					uint32_t _index1;
					uint32_t _index2;
				};
			};
		};

		struct OctreeNode 
		{
			Math::Vector3 _pos;
			Math::Vector3 _extent;
			std::vector<TriangleIndex> _triangles;
			Math::Vector3 GetMin() { return _pos - _extent; }
			Math::Vector3 GetMax() { return _pos + _extent; }
		};


		class CompleteOctree
		{
		public: 
			CompleteOctree();
			~CompleteOctree();
			inline void InitFromRange(uint32_t level, Math::Vector3 min, Math::Vector3 max);
			inline void InitFromFile(const char* pFile);
			inline OctreeNode* GetNodesInLevel(uint32_t levelIndex);
			inline uint32_t GetCountOfNodesInLevel(uint32_t levelIndex) { return (uint32_t)std::pow(8.0f, levelIndex); }
			inline uint32_t GetNodeCount() { return _countOfNode; }
			inline OctreeNode* GetNodes() { return _pNodes; }
			inline Math::Vector3 GetMin() { return _min; }
			inline Math::Vector3 GetMax() { return _max; }
			inline uint32_t Level() { return _level; }

			inline void SetBuffer(OctreeNode* pNodes, uint32_t countOfNode);
			inline void LoadFromFile();

		private:
			uint32_t _level;
			OctreeNode* _pNodes;
			uint32_t _countOfNode;
			Math::Vector3 _min;
			Math::Vector3 _max;
		};

		CompleteOctree::CompleteOctree() :
			_min(Math::Vector3::Zero), _max(Math::Vector3::Zero), 
			_level(0), _countOfNode(0), _pNodes(nullptr)
		{}

		CompleteOctree::~CompleteOctree()
		{
			SAFE_DELETE_ARRAY(_pNodes);
		}

		inline OctreeNode* CompleteOctree::GetNodesInLevel(uint32_t levelIndex)
		{
			uint32_t baseOfNodesInLevel = (uint32_t)(std::pow(8.0f, levelIndex) - 1) / (8 - 1);
			uint32_t countOfNodesInLevel = (uint32_t)std::pow(8.0f, levelIndex);
			return &_pNodes[baseOfNodesInLevel];
		}

		inline void CompleteOctree::InitFromRange(uint32_t level, Math::Vector3 min, Math::Vector3 max)
		{
			_level = level;
			_countOfNode = (uint32_t)((std::pow(8.0f, _level) - 1) / (8 - 1));
			_pNodes = new OctreeNode[_countOfNode];
			_min = min;
			_max = max;
			_pNodes[0]._pos = (_min + _max) * 0.5f;
			_pNodes[0]._extent = (_max - _min) * 0.5f;
			for (uint32_t levelIndex = 0; levelIndex < _level - 1; ++levelIndex)
			{
				uint32_t baseOfNodesInLevel = (uint32_t)(std::pow(8.0f, levelIndex) - 1) / (8 - 1);
				uint32_t countOfNodesInLevel = (uint32_t)std::pow(8.0f, levelIndex);
				Math::Vector3 extentInNextLevel = _pNodes[baseOfNodesInLevel]._extent * 0.5f;
				for (uint32_t nodeIndex = 0; nodeIndex < countOfNodesInLevel; ++nodeIndex)
				{
					uint32_t indexOfNode = baseOfNodesInLevel + nodeIndex;
					Math::Vector3 centerPos = _pNodes[indexOfNode]._pos;
					OctreeNode* pChildren = &_pNodes[indexOfNode * 8 + 1];
					pChildren[0]._pos = centerPos + Math::Vector3(-extentInNextLevel._x, -extentInNextLevel._y, -extentInNextLevel._z);
					pChildren[1]._pos = centerPos + Math::Vector3(-extentInNextLevel._x, -extentInNextLevel._y, +extentInNextLevel._z);
					pChildren[2]._pos = centerPos + Math::Vector3(+extentInNextLevel._x, -extentInNextLevel._y, +extentInNextLevel._z);
					pChildren[3]._pos = centerPos + Math::Vector3(+extentInNextLevel._x, -extentInNextLevel._y, -extentInNextLevel._z);
					pChildren[4]._pos = centerPos + Math::Vector3(-extentInNextLevel._x, +extentInNextLevel._y, -extentInNextLevel._z);
					pChildren[5]._pos = centerPos + Math::Vector3(-extentInNextLevel._x, +extentInNextLevel._y, +extentInNextLevel._z);
					pChildren[6]._pos = centerPos + Math::Vector3(+extentInNextLevel._x, +extentInNextLevel._y, +extentInNextLevel._z);
					pChildren[7]._pos = centerPos + Math::Vector3(+extentInNextLevel._x, +extentInNextLevel._y, -extentInNextLevel._z);
					for (uint32_t childIndex = 0; childIndex < 8; ++childIndex)
					{
						pChildren[childIndex]._extent = extentInNextLevel;
					}
				}
			}
		}

		inline void CompleteOctree::InitFromFile(const char* pFile)
		{
			std::ifstream infile(pFile, std::ifstream::binary);
			// get size of file
			infile.seekg(0, infile.end);
			std::streamoff size = infile.tellg();
			infile.seekg(0);
			// allocate memory for file content
			char* pBuffer = new char[size];
			// read content of infile
			infile.read(pBuffer, size);
			{
				uint32_t offset = 0;
				EAE_Engine::CopyMem((uint8_t*)(pBuffer + offset), (uint8_t*)&_level, sizeof(uint32_t));
				offset += sizeof(uint32_t);
				EAE_Engine::CopyMem((uint8_t*)(pBuffer + offset), (uint8_t*)&_countOfNode, sizeof(uint32_t));
				offset += sizeof(uint32_t);
				EAE_Engine::CopyMem((uint8_t*)(pBuffer + offset), (uint8_t*)&_min, sizeof(EAE_Engine::Math::Vector3));
				offset += sizeof(EAE_Engine::Math::Vector3);
				EAE_Engine::CopyMem((uint8_t*)(pBuffer + offset), (uint8_t*)&_max, sizeof(EAE_Engine::Math::Vector3));
				offset += sizeof(EAE_Engine::Math::Vector3);


				EAE_Engine::Math::Vector3 min(-100.0f, -25.0f, -100.0f);
				EAE_Engine::Math::Vector3 max(100.0f, 25.0f, 100.0f);
				InitFromRange(_level, min, max);
			}
			// release dynamically-allocated memory
			delete[] pBuffer;
			infile.close();
		}

		inline void CompleteOctree::SetBuffer(OctreeNode* pNodes, uint32_t countOfNode)
		{
			_countOfNode = countOfNode;
			EAE_Engine::CopyMem((uint8_t*)pNodes, (uint8_t*)_pNodes, sizeof(EAE_Engine::Core::OctreeNode) * countOfNode);
		}

	}
}

#include "Octree.inl"
#endif//EAE_ENGINE_SPATIAL_PARTITION_OCTREE