#ifndef EAE_ENGINE_COLLISION_DETECTION_FUNCTIONS_H
#define EAE_ENGINE_COLLISION_DETECTION_FUNCTIONS_H

#include "Engine/Math/Geometry.h"

#define EPSILON FLT_EPSILON

namespace EAE_Engine
{
	namespace Collision 
	{

		// Test whether a point is in the AABB or not
		inline bool TestPointInAABB(const Math::Vector3& min, const Math::Vector3& max, const Math::Vector3& vecPoint)
		{
			//Check if the point is less than max and greater than min
			if (vecPoint._x > min._x && vecPoint._x < max._x &&
				vecPoint._y > min._y && vecPoint._y < max._y &&
				vecPoint._z > min._z && vecPoint._z < max._z)
			{
				return true;
			}
			//If not, then return false
			return false;
		}

		inline bool TestSegmentAABB(Math::Vector3 p0, Math::Vector3 p1, Math::AABBV1 b)
		{
			Math::Vector3 c = (b._min + b._max) * 0.5f; // Box center-point
			Math::Vector3 e = b._max - c; // Box halflength extents
			Math::Vector3 m = (p0 + p1) * 0.5f; // Segment midpoint
			Math::Vector3 d = p1 - m; // Segment halflength vector
			m = m - c; // Segment midpoint relative to box center

			// Try world coordinate axes as separating axes
			float adx = std::fabsf(d._x);
			if (std::fabsf(m._x) > e._x + adx) 
				return false;
			float ady = std::fabsf(d._y);
			if (std::fabsf(m._y) > e._y + ady) 
				return false;
			float adz = std::fabsf(d._z);
			if (std::fabsf(m._z) > e._z + adz) 
				return false;

			// Add in an epsilon term to counteract arithmetic errors when segment is
			// (near) parallel to a coordinate axis
			adx += EPSILON;
			ady += EPSILON;
			adz += EPSILON;

			// Try cross products of segment direction vector with coordinate axes
			if (std::fabsf(m._y * d._z - m._z * d._y) > e._y * adz + e._z * ady) 
				return false;
			if (std::fabsf(m._z * d._x - m._x * d._z) > e._x * adz + e._z * adx) 
				return false;
			if (std::fabsf(m._x * d._y - m._y * d._x) > e._x * ady + e._y * adx) 
				return false;

			// No separating axis found; sement must be overlapping AABB
			return true;
		}

		// Given segment pq and triangle abc, returns whether segment intersects
		// triangle and if so, also returns the barycentric coordinates (u,v,w)
		// of the intersection point
		inline int IntersectSegmentTriangle(Math::Vector3 p, Math::Vector3 q, const Math::Vector3& a, const Math::Vector3& b, const Math::Vector3& c,
			float &u, float &v, float &w, float &t)
		{
			Math::Vector3 ab = b - a;
			Math::Vector3 ac = c - a;
			Math::Vector3 qp = p - q;

			// Compute triangle normal. Can be precalculated or cached if
			// intersecting multiple segments against the same triangle
			Math::Vector3 n = Math::Vector3::Cross(ab, ac);
			// Compute denominator d. If d <= 0, segment is parallel to or points
			// away from triangle, so exit early
			float d = Math::Vector3::Dot(qp, n);
			if (d <= 0.0f) return 0;
			// Compute intersection t value of pq with plane of triangle. A ray
			// intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
			// dividing by d until intersection has been found to pierce triangle
			Math::Vector3 ap = p - a;
			t = Math::Vector3::Dot(ap, n);
			if (t < 0.0f) return 0;
			if (t > d) return 0; // For segment; exclude this code line for a ray test

			// Compute barycentric coordinate components and test if within bounds
			Math::Vector3 e = Math::Vector3::Cross(qp, ap);
			v = Math::Vector3::Dot(ac, e);
			if (v < 0.0f || v > d) return 0;
			w = -Math::Vector3::Dot(ab, e);
			if (w < 0.0f || v + w > d) return 0;

			// Segment/ray intersects triangle. Perform delayed division and
			// compute the last barycentric coordinate component
			float ood = 1.0f / d;
			t *= ood;
			v *= ood;
			w *= ood;
			u = 1.0f - v - w;
			return 1;
		}
		
		// Test if AABB b intersects plane p
		inline bool TestAABBPlane(Math::AABBV1 b, Math::Plane p)
		{
			// These two lines not necessary with a (center, extents) AABB representation
			Math::Vector3 c = (b._max + b._min) * 0.5f; // Compute AABB center
			Math::Vector3 e = b._max - c; // Compute positive extents
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			float r = e._u[0] * std::fabsf(p._normal._u[0]) + e._u[1] * std::fabsf(p._normal._u[1]) + e._u[2] * std::fabsf(p._normal._u[2]);
			// Compute distance of box center from plane
			float s = Math::Vector3::Dot(p._normal, c) - p._d;			// Intersection occurs when distance s falls within [-r,+r] interval
			return std::fabsf(s) <= r;
		}

		// Read Real-time collision detection P169
		inline bool TestTriangleAABB(Math::Vector3 v0, Math::Vector3 v1, Math::Vector3 v2, Math::AABBV1 b)
		{
			// Compute box center and extents (if not already given in that format)
			Math::Vector3 c = (b._min + b._max) * 0.5f;
			float e0 = (b._max._x - b._min._x) * 0.5f;
			float e1 = (b._max._y - b._min._y) * 0.5f;
			float e2 = (b._max._z - b._min._z) * 0.5f;
			// Translate triangle as conceptually moving AABB to origin
			v0 = v0 - c;
			v1 = v1 - c;
			v2 = v2 - c;
			// Compute edge vectors for triangle
			Math::Vector3 axis[3];
			axis[0] = Math::Vector3::Right; axis[1] = Math::Vector3::Up; axis[2] = Math::Vector3::Forward;
			Math::Vector3 edge[3];
			edge[0] = v1 - v0, edge[1] = v2 - v1, edge[2] = v0 - v2;
			// Test axes a00..a22 (category 3)
			float p0, p1, p2, r;
			for (int i = 0; i < 3;  ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					Math::Vector3 projectionSATAxis = Math::Vector3::Cross(axis[i], edge[j]);
					r = std::fabsf(Math::Vector3::Dot(axis[0], projectionSATAxis)) * e0 + std::fabsf(Math::Vector3::Dot(axis[1], projectionSATAxis)) * e1 + std::fabsf(Math::Vector3::Dot(axis[2], projectionSATAxis)) * e2;
					p0 = Math::Vector3::Dot(v0, projectionSATAxis);
					p1 = Math::Vector3::Dot(v1, projectionSATAxis);
					p2 = Math::Vector3::Dot(v2, projectionSATAxis);
					float max = std::fmaxf(p0, std::fmaxf(p1, p2));
					float min = std::fminf(p0, std::fminf(p1, p2));
					//If the projection intervals [?r, r] and min(p0, p1, p2), max(p0, p1, p2) are disjoint for the given axis, 
					//the axis is a separating axis and the triangle and the AABB do not overlap.
					if  (max < -r || min > r)
						return false; // Axis is a separating axis
				}
			}
			// Test the three axes corresponding to the face normals of AABB b (category 1).
			// Exit if...
			// ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
			if (std::fmaxf(v0._x, std::fmaxf(v1._x, v2._x)) < -e0 || std::fminf(v0._x, std::fminf(v1._x, v2._x)) > e0) return 0;
			// ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
			if (std::fmaxf(v0._y, std::fmaxf(v1._y, v2._y)) < -e1 || std::fminf(v0._y, std::fminf(v1._y, v2._y)) > e1) return 0;
			// ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
			if (std::fmaxf(v0._z, std::fmaxf(v1._z, v2._z)) < -e2 || std::fminf(v0._z, std::fminf(v1._z, v2._z)) > e2) return 0;
			// Test separating axis corresponding to triangle face normal (category 2)
			Math::Plane p;
			p._normal = Math::Vector3::Cross(edge[0], edge[1]);
			p._d = Math::Vector3::Dot(p._normal, v0);
			Math::AABBV1 b2;
			b2._min = b._min - c;
			b2._max = b._max - c;
			return TestAABBPlane(b2, p);
		}
	}
}

#endif//EAE_ENGINE_COLLISION_DETECTION_FUNCTIONS_H

