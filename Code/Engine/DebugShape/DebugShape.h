#ifndef EAE_ENGINE_DEBUG_SHAPE_H
#define EAE_ENGINE_DEBUG_SHAPE_H
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Debug 
	{
		struct DebugSegment
		{
			DebugSegment() = default;

			Math::Vector3 _start;
			Math::Vector3 _end;
			Math::Vector3 _color;
		};

        struct DebugCircle
        {
          DebugCircle() = default;

          Math::Vector3 _pos;
          float _radius;
          Math::Vector3 _color;
        };

		struct DebugBox 
		{
			DebugBox() = default;

			Math::Vector3 _extents;
			Math::Vector3 _pos;
			Math::Quaternion _rotation;
			Math::Vector3 _color;
		};

		struct DebugSphere 
		{
			DebugSphere() = default;

			Math::Vector3 _pos;
			Math::Vector3 _color;
			float _radius;
		};

		struct DebugMesh
		{
			DebugMesh() = default;

			std::vector<Math::Vector3> _vertices;
			Math::Vector3 _color;
		};

		class DebugShapes 
		{
		public:
			void AddSegment(Math::Vector3 i_start, Math::Vector3 i_end, Math::Vector3 i_color);
            void AddCircle(Math::Vector3 center, float radius, Math::Vector3 i_color);
			void AddBox(Math::Vector3 i_extents, Math::Vector3 i_pos, Math::Quaternion i_rotation, Math::Vector3 i_color);
			void AddSphere(Math::Vector3 i_pos, float radius, Math::Vector3 i_color);
			void AddMesh(std::vector<Math::Vector3> i_vertices, Math::Vector3 i_color);
			inline std::vector<DebugSegment>& GetSegments() { return _segments; }
            inline std::vector<DebugCircle>& GetCircles() { return _circles; }
			inline std::vector<DebugBox>& GetBoxes() { return _boxes; }
			inline std::vector<DebugSphere>& GetSpheres() { return _spheres; }
			inline std::vector<DebugMesh>& GetMeshes() { return _meshes; }
			void Clean();
		private:
			std::vector<DebugSegment> _segments;
            std::vector<DebugCircle> _circles;
			std::vector<DebugBox> _boxes;
			std::vector<DebugSphere> _spheres;
			std::vector<DebugMesh> _meshes;

			/////////////////////static_members////////////////////////////
		private:
			DebugShapes() {}
			static DebugShapes* s_pInternalInstance;
		public:
			static DebugShapes& GetInstance();
			static void CleanInstance();
		};
		
		inline void CleanDebugShapes() 
		{
			DebugShapes::GetInstance().Clean(); 
		}

		inline void AddSegment(Math::Vector3 i_start, Math::Vector3 i_end, Math::Vector3 i_color) 
		{
			return DebugShapes::GetInstance().AddSegment(i_start, i_end, i_color);
		}

		inline void AddBox(Math::Vector3 i_extents, Math::Vector3 i_pos, Math::Quaternion i_rotation, Math::Vector3 i_color)
		{
			return DebugShapes::GetInstance().AddBox(i_extents, i_pos, i_rotation, i_color);
		}

		inline void AddSphere(Math::Vector3 i_pos, float radius, Math::Vector3 i_color)
		{
			return DebugShapes::GetInstance().AddSphere(i_pos, radius, i_color);
		}

	}
}

#endif//EAE_ENGINE_DEBUG_SHAPE_H
