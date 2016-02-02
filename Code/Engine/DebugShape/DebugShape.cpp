#include "DebugShape.h"
#include "General/MemoryOp.h"
//#include "Graphics/Common/AOSMesh.h"
//#include "Graphics/Common/Effect.h"
//#include "Graphics/Common/MeshManager.h"




namespace EAE_Engine 
{
	namespace DebugShape 
	{
		void DebugShapes::AddSegment(Math::Vector3 i_start, Math::Vector3 i_end, Math::Vector3 i_color)
		{
			_segments.push_back({ i_start, i_end, i_color});
		}

		void DebugShapes::AddBox(Math::Vector3 i_extents, Math::Vector3 i_pos, Math::Quaternion i_rotation, Math::Vector3 i_color)
		{
			_boxes.push_back({ i_extents, i_pos, i_rotation, i_color});
		}

		void DebugShapes::AddSphere(Math::Vector3 i_pos, float radius, Math::Vector3 i_color)
		{
			_spheres.push_back({ i_pos, i_color, radius });
		}

		void DebugShapes::Clean()
		{
			_segments.clear();
			_boxes.clear();
			_spheres.clear();
		}

		////////////////////////////////static_members/////////////////////////////////
		DebugShapes* DebugShapes::s_pInternalInstance = nullptr;

		DebugShapes& DebugShapes::GetInstance()
		{
			if (!s_pInternalInstance)
				s_pInternalInstance = new DebugShapes();
			return *s_pInternalInstance;
		}

		void DebugShapes::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			//s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}
	}
}