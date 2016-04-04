#ifndef EAE_ENGINE_SPATIAL_PARTITION_OCTREE
#define EAE_ENGINE_SPATIAL_PARTITION_OCTREE

#include "Engine/Math/Vector.h"
#include "Engine/Mesh/AOSMeshData.h"
#include "Engine/General/MemoryOp.h"
#include "Engine/General/Singleton.hpp"
#include <vector>
#include <fstream>


namespace EAE_Engine 
{
	namespace Core 
	{

		struct OctreeNode 
		{
			Math::Vector3 _pos;
			Math::Vector3 _extent;
			std::vector<Mesh::TriangleIndex> _triangles;
			Math::Vector3 GetMin() { return _pos - _extent; }
			Math::Vector3 GetMax() { return _pos + _extent; }
		};

		class CompleteOctree
		{
		public: 
			CompleteOctree();
			~CompleteOctree();
			inline void InitFromRange(uint32_t level, Math::Vector3 min, Math::Vector3 max);
			void InitFromFile(const char* pOctreeFile, const char* pMeshKey);
			inline OctreeNode* GetNodesInLevel(uint32_t levelIndex);
			inline uint32_t GetCountOfNodesInLevel(uint32_t levelIndex) { return (uint32_t)std::pow(8.0f, levelIndex); }
			inline uint32_t GetNodeCount() { return _countOfNode; }
			inline OctreeNode* GetNodes() { return _pNodes; }
			inline Math::Vector3 GetMin() { return _min; }
			inline Math::Vector3 GetMax() { return _max; }
			inline uint32_t Level() { return _level; }
			std::vector<OctreeNode*> GetNodesCollideWithSegment(Math::Vector3 start, Math::Vector3 end, uint32_t levelIndex);
			std::vector<OctreeNode*> GetLeavesCollideWithSegment(Math::Vector3 start, Math::Vector3 end);
      void GetTrianlgesCollideWithSegment(Math::Vector3 start, Math::Vector3 end, std::vector<Mesh::TriangleIndex>& o_triangles);
      inline Mesh::AOSMeshData* GetCollisionMesh() { return _pMeshData; }
      bool IsLeaf(OctreeNode* pNode);
			bool IsInLevel(OctreeNode* pNode, uint32_t levelIndex);

		private:
			OctreeNode* GetChildOfNode(OctreeNode* pNode);

		private:
			uint32_t _level;
			OctreeNode* _pNodes;
			uint32_t _countOfNode;
			Math::Vector3 _min;
			Math::Vector3 _max;
			Mesh::AOSMeshData* _pMeshData;
		};


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



		class OctreeManager : public Singleton<OctreeManager>
		{
		public:
			~OctreeManager();
			void AddOctree(CompleteOctree* pCompleteOctree);
			CompleteOctree* GetOctree();

		private:
			std::vector<CompleteOctree*> _octrees;
		};

	}
}

#endif//EAE_ENGINE_SPATIAL_PARTITION_OCTREE