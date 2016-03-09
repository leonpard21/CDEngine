#include "Octree.h"
#include "Engine/CollisionDetection/CollisionDetectionFunctions.h"

namespace EAE_Engine 
{
	namespace Core 
	{
		CompleteOctree::CompleteOctree() :
			_min(Math::Vector3::Zero), _max(Math::Vector3::Zero),
			_level(0), _countOfNode(0), _pNodes(nullptr), _pMeshData(nullptr) 
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

		std::vector<TriangleIndex> CompleteOctree::GetTrianlgesCollideWithSegment(Math::Vector3 start, Math::Vector3 end)
		{
			std::vector<TriangleIndex> result;
			std::vector<OctreeNode*> leavesCollided = GetLeavesCollideWithSegment(start, end);
			for (std::vector<OctreeNode*>::iterator it = leavesCollided.begin(); it != leavesCollided.end(); ++it)
			{
				OctreeNode* pLeaf = *it;
				for (std::vector<TriangleIndex>::iterator itTrianlge = pLeaf->_triangles.begin(); itTrianlge != pLeaf->_triangles.end(); ++itTrianlge)
				{
					uint32_t index0 = itTrianlge->_index0;
					uint32_t index1 = itTrianlge->_index1;
					uint32_t index2 = itTrianlge->_index2;
					Mesh::sVertex& svertex0 = _pMeshData->_vertices[index0];
					Mesh::sVertex& svertex1 = _pMeshData->_vertices[index1];
					Mesh::sVertex& svertex2 = _pMeshData->_vertices[index2];
					Math::Vector3 vertex0(svertex0.x, svertex0.y, svertex0.z);
					Math::Vector3 vertex1(svertex1.x, svertex1.y, svertex1.z);
					Math::Vector3 vertex2(svertex2.x, svertex2.y, svertex2.z);
					float u = 0, v = 0, w = 0, t = 0;
					int collided = Collision::IntersectSegmentTriangle(start, end, vertex0, vertex1, vertex2, u, v, w, t);
					if (collided)
					{
						result.push_back(*itTrianlge);
					}
				}
			}
			return result;
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
