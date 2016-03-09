#include "Octree.h"
#include "Engine/CollisionDetection/CollisionDetectionFunctions.h"

namespace EAE_Engine 
{
	namespace Core 
	{
		CompleteOctree::CompleteOctree() :
			_min(Math::Vector3::Zero), _max(Math::Vector3::Zero),
			_level(0), _countOfNode(0), _pNodes(nullptr)
		{}

		CompleteOctree::~CompleteOctree()
		{
			SAFE_DELETE_ARRAY(_pNodes);
		}

		std::vector<OctreeNode*> CompleteOctree::GetLeavesCollideWithSegment(Math::Vector3 start, Math::Vector3 end)
		{
			std::vector<OctreeNode*> nodesCollided;
			nodesCollided.push_back(&_pNodes[0]);
			for (; nodesCollided.size() > 0; )
			{
				std::vector<OctreeNode*> newNodesCollided;
				if (IsLeaf(nodesCollided[0]))
				{
					for (std::vector<OctreeNode*>::iterator it = nodesCollided.begin(); it < nodesCollided.end(); ++it)
					{
						OctreeNode* pNode = *it;
						Math::AABBV1 aabb;
						aabb._min = pNode->GetMin();
						aabb._max = pNode->GetMax();
						if (Collision::TestSegmentAABB(start, end, aabb))
						{
							newNodesCollided.push_back(pNode);
						}
					}
					nodesCollided.clear();
					nodesCollided = newNodesCollided;
					break;
				}
				for (std::vector<OctreeNode*>::iterator it = nodesCollided.begin(); it < nodesCollided.end(); ++it)
				{
					OctreeNode* pNode = *it;
					Math::AABBV1 aabb;
					aabb._min = pNode->GetMin();
					aabb._max = pNode->GetMax();
					if (Collision::TestSegmentAABB(start, end, aabb))
					{
						OctreeNode* pChild = GetChildOfNode(pNode);
						for (uint32_t i = 0; i < 8; ++i)
							newNodesCollided.push_back(&pChild[i]);
					}
				}
				nodesCollided.clear();
				nodesCollided = newNodesCollided;
			}
			return nodesCollided;
		}

		OctreeNode* CompleteOctree::GetChildOfNode(OctreeNode* pNode)
		{
			uint32_t indexOfNode = ((uint32_t)pNode - (uint32_t)_pNodes) / (uint32_t)sizeof(OctreeNode);
			uint32_t baseOfNodesOfLeaves = (uint32_t)(std::pow(8.0f, _level - 1) - 1) / (8 - 1);
			if (pNode < _pNodes || indexOfNode >= baseOfNodesOfLeaves)
				return nullptr;
			return &_pNodes[indexOfNode * 8 + 1];
		}

		bool CompleteOctree::IsLeaf(OctreeNode* pNode)
		{
			uint32_t member = (uint32_t)pNode - (uint32_t)_pNodes;
			uint32_t indexOfNode = member / (uint32_t)sizeof(OctreeNode);
			uint32_t baseOfNodesOfLeaves = (uint32_t)(std::pow(8.0f, _level - 1) - 1) / (8 - 1);
			if (indexOfNode >= baseOfNodesOfLeaves && indexOfNode < _countOfNode)
				return true;
			return false;
		}

	}
}
