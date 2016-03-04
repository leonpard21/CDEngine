#ifndef EAE_ENGINE_SPATIAL_PARTITION_OCTREE
#define EAE_ENGINE_SPATIAL_PARTITION_OCTREE

#include "Engine/Math/Vector.h"
#include "Engine/Mesh/AOSMeshData.h"
#include "Engine/General/MemoryOp.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Core 
	{
		struct TriangleIndex
		{
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
		};


		class CompleteOctree
		{
		public:  
			enum { _level = 4 };
			CompleteOctree();
			~CompleteOctree();
			inline void Init(Math::Vector3 min, Math::Vector3 max);
			inline OctreeNode* GetNodesInLevel(uint32_t levelIndex);
			inline uint32_t GetCountOfNodesInLevel(uint32_t levelIndex) { return (uint32_t)std::pow(8.0f, levelIndex); }

		private:
			OctreeNode* _pNodes;
			uint32_t _countOfNode;
			Math::Vector3 _min;
			Math::Vector3 _max;
		};

		CompleteOctree::CompleteOctree() :
			_min(Math::Vector3::Zero), _max(Math::Vector3::Zero)
		{ 
			_countOfNode = (uint32_t)((std::pow(8.0f, _level) - 1) / (8 - 1));
			_pNodes = new OctreeNode[_countOfNode];
		}

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

		inline void CompleteOctree::Init(Math::Vector3 min, Math::Vector3 max)
		{
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

	}
}

#include "Octree.inl"
#endif//EAE_ENGINE_SPATIAL_PARTITION_OCTREE