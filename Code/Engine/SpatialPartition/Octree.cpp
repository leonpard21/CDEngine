#include "Octree.h"
#include "Engine/CollisionDetection/CollisionDetectionFunctions.h"
#include <algorithm>
#include "General/Implements.h"
#include "Windows/WindowsFunctions.h"

namespace EAE_Engine 
{
	namespace Core 
	{

    ///////////////////////////////CompleteOctree///////////////////////////////////////
		CompleteOctree::CompleteOctree() :
			_min(Math::Vector3::Zero), _max(Math::Vector3::Zero),
			_level(0), _countOfNode(0), _pNodes(nullptr), _pMeshData(nullptr) 
		{}

		CompleteOctree::~CompleteOctree()
		{
			SAFE_DELETE_ARRAY(_pNodes);
		}

    void CompleteOctree::InitFromFile(const char* pOctreeFile, const char* pCollisionMesh)
    {
      std::ifstream infile(pOctreeFile, std::ifstream::binary);
      // get size of file
      infile.seekg(0, infile.end);
      std::streamoff size = infile.tellg();
      infile.seekg(0);
      // allocate memory for file content
      char* pBuffer = new char[(uint32_t)size];
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
        // Build the Octree Architecture
        InitFromRange(_level, _min, _max);
        // Now let's fill in the trianlges information
        OctreeNode* pLeaves = GetNodesInLevel(_level - 1);
        uint32_t countOfLeaves = GetCountOfNodesInLevel(_level - 1);
        for (uint32_t leafIndex = 0; leafIndex < countOfLeaves; ++leafIndex)
        {
          // record count of trianlges in this node
          uint32_t triangleCountInLeaf = 0;
          CopyMem((uint8_t*)pBuffer + offset, (uint8_t*)&triangleCountInLeaf, sizeof(uint32_t));
          offset += sizeof(uint32_t);
          if (triangleCountInLeaf > 0)
          {
            for (uint32_t trianlgeIndex = 0; trianlgeIndex < triangleCountInLeaf; ++trianlgeIndex)
            {
              Mesh::TriangleIndex tempTraiangle;
              CopyMem((uint8_t*)pBuffer + offset, (uint8_t*)&tempTraiangle, sizeof(Mesh::TriangleIndex));
              pLeaves[leafIndex]._triangles.push_back(tempTraiangle);
              offset += sizeof(Mesh::TriangleIndex);
            }
          }
        }
      }
      // release dynamically-allocated memory
      delete[] pBuffer;
      infile.close();

      Mesh::LoadMeshData(pCollisionMesh);
      std::string mesh_path(pCollisionMesh);
      std::string key = GetFileNameWithoutExtension(mesh_path.c_str());
      _pMeshData = Mesh::AOSMeshDataManager::GetInstance()->GetAOSMeshData(key.c_str());
    }


		std::vector<OctreeNode*> CompleteOctree::GetNodesCollideWithSegment(Math::Vector3 start, Math::Vector3 end, uint32_t levelIndex)
		{
			std::vector<OctreeNode*> nodesCollided;
			if (levelIndex > _level - 1)
				return nodesCollided;
			nodesCollided.push_back(&_pNodes[0]);
			for (; nodesCollided.size() > 0; )
			{
				std::vector<OctreeNode*> newNodesCollided;
				if (IsInLevel(nodesCollided[0], levelIndex))
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
					// sort the nodes based on the distance from the Node
          // we're using lambad at here.
					std::sort(nodesCollided.begin(), nodesCollided.end(), 
            [&](OctreeNode* i_pObjA, OctreeNode* i_pObjB) { return (i_pObjA->_pos - start).Magnitude() < (i_pObjB->_pos - start).Magnitude(); });
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
					// sort the nodes based on the distance from the Node
					std::sort(nodesCollided.begin(), nodesCollided.end(), 
            [&](OctreeNode* i_pObjA, OctreeNode* i_pObjB) { return (i_pObjA->_pos - start).Magnitude() < (i_pObjB->_pos - start).Magnitude(); });
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
		
		struct TriangleList 
		{
			TriangleList() = default;
			float _t;
      Mesh::TriangleIndex* _pTriangle;
		};

		void CompleteOctree::GetTrianlgesCollideWithSegment(Math::Vector3 start, Math::Vector3 end, std::vector<Mesh::TriangleIndex>& o_triangles)
		{
			std::vector<TriangleList> needToSort;
			std::vector<OctreeNode*> leavesCollided = GetLeavesCollideWithSegment(start, end);
			for (std::vector<OctreeNode*>::iterator it = leavesCollided.begin(); it != leavesCollided.end(); ++it)
			{
				OctreeNode* pLeaf = *it;
				for (std::vector<Mesh::TriangleIndex>::iterator itTrianlge = pLeaf->_triangles.begin(); itTrianlge != pLeaf->_triangles.end(); ++itTrianlge)
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
						needToSort.push_back({t, &*itTrianlge});
					}
				}
			}
			// sort all of the triangles by t, it means we want to have the first collision t
      // notice that we're using the lambda at here.
      std::sort(needToSort.begin(), needToSort.end(), [](TriangleList& i_objA, TriangleList& i_objB) { return i_objA._t < i_objB._t; });
			// get rid of the duplicated triangles
			std::vector<TriangleList>::iterator itTrianlge = needToSort.begin();
			std::vector<TriangleList>::iterator previousTriangle = itTrianlge;
			for (; itTrianlge != needToSort.end(); ++itTrianlge)
			{
				if (Implements::AlmostEqual2sComplement(previousTriangle->_t, itTrianlge->_t, 4) && o_triangles.size() > 0)
				{
					if (*(previousTriangle->_pTriangle) == *(itTrianlge->_pTriangle))
					{
						continue;
					}
				}
				previousTriangle = itTrianlge;
        o_triangles.push_back(*itTrianlge->_pTriangle);
			}
		}

		OctreeNode* CompleteOctree::GetChildOfNode(OctreeNode* pNode)
		{
			size_t member = reinterpret_cast<size_t>(pNode) - reinterpret_cast<size_t>(_pNodes);
			uint32_t indexOfNode = (uint32_t)member / (uint32_t)sizeof(OctreeNode);
			uint32_t baseOfNodesOfLeaves = (uint32_t)(std::pow(8.0f, _level - 1) - 1) / (8 - 1);
			if (pNode < _pNodes || indexOfNode >= baseOfNodesOfLeaves)
				return nullptr;
			return &_pNodes[indexOfNode * 8 + 1];
		}

		bool CompleteOctree::IsLeaf(OctreeNode* pNode)
		{
			size_t member = reinterpret_cast<size_t>(pNode) - reinterpret_cast<size_t>(_pNodes);
			uint32_t indexOfNode = (uint32_t)member / (uint32_t)sizeof(OctreeNode);
			uint32_t baseOfNodesOfLeaves = (uint32_t)(std::pow(8.0f, _level - 1) - 1) / (8 - 1);
			if (indexOfNode >= baseOfNodesOfLeaves && indexOfNode < _countOfNode)
				return true;
			return false;
		}

		bool CompleteOctree::IsInLevel(OctreeNode* pNode, uint32_t levelIndex)
		{
			if (levelIndex > _level - 1)
				return false;
			else if (levelIndex == _level - 1)
				return IsLeaf(pNode);
			size_t member = reinterpret_cast<size_t>(pNode) - reinterpret_cast<size_t>(_pNodes);
			uint32_t indexOfNode = (uint32_t)member / (uint32_t)sizeof(OctreeNode);
			uint32_t baseOfNodesInLevel = (uint32_t)(std::pow(8.0f, levelIndex) - 1) / (8 - 1);
			uint32_t baseOfNodesInNextLevel = (uint32_t)(std::pow(8.0f, levelIndex + 1) - 1) / (8 - 1);
			if (indexOfNode >= baseOfNodesInLevel && indexOfNode < baseOfNodesInNextLevel)
				return true;
			return false;
		}

		//////////////////////////////////////OctreeManager////////////////////////////////////////////////
		OctreeManager::~OctreeManager()
		{
			for (std::vector<CompleteOctree*>::iterator octreeIt = _octrees.begin(); octreeIt != _octrees.end(); )
			{
				CompleteOctree* pOctree = *octreeIt++;
				SAFE_DELETE(pOctree);
			}
			_octrees.clear();
		}

		void OctreeManager::AddOctree(CompleteOctree* pCompleteOctree) 
		{
			_octrees.push_back(pCompleteOctree);
		}

		CompleteOctree* OctreeManager::GetOctree()
		{
			if (_octrees.size() == 0)
				return nullptr;
			return _octrees[0];
		}

	}
}
