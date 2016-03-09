#ifndef EAE_ENGINE_MATH_GEOMETRY_H
#define EAE_ENGINE_MATH_GEOMETRY_H

#include "Vector.h"

namespace EAE_Engine
{
	namespace Math 
	{
		struct Ray
		{
			Ray() = default;
			Vector3 _pos;
			Vector3 _dir;
		};

		struct Plane 
		{
			Plane() = default;
			Vector3 _normal; // Plane normal. Points x on the plane satisfy Dot(n,x) = d
			float _d; //d = dot(n,p) for a given point p on the plane
		};

		// Given three noncollinear points (ordered ccw), compute plane equation
		inline Plane ComputePlane(Vector3 a, Vector3 b, Vector3 c)
		{
			Plane p;
			p._normal = Vector3::Cross(b - a, c - a).GetNormalize();
			p._d = Vector3::Dot(p._normal, a);
			return p;
		}

		struct Triangle 
		{
			Triangle() = default;
			union 
			{
				Vector3 _vertices[3];
				struct 
				{
					Vector3 _vertex0;
					Vector3 _vertex1;
					Vector3 _vertex2;
				};
			};
		};

		struct Sphere
		{
			Sphere() = default;
			Vector3 _center;
			float _radius;
		};

		struct Capsule
		{
			Capsule() = default;
			Vector3 _a;
			Vector3 _b;
			float _radius;
		};

		struct AABBV1
		{
			AABBV1() = default;
			Vector3 _min;
			Vector3 _max;
		};

		struct AABBV2
		{
			AABBV2() = default;
			Vector3 _pos;
			Vector3 _extents;
		};

		struct OBB 
		{
			OBB() = default;
			Vector3 _pos;
			Vector3 _extent;
			Vector3 _axis[3];// Local x-, y-, and z-axes
		};

	}
}

#endif//EAE_ENGINE_MATH_GEOMETRY_H