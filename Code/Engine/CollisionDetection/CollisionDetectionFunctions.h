#ifndef EAE_ENGINE_COLLISION_DETECTION_FUNCTIONS_H
#define EAE_ENGINE_COLLISION_DETECTION_FUNCTIONS_H

#include "Math/Vector.h"

namespace EAE_Engine
{
	namespace Collision 
	{
		typedef Math::Vector3 Point;

		// Given segment pq and triangle abc, returns whether segment intersects
		// triangle and if so, also returns the barycentric coordinates (u,v,w)
		// of the intersection point
		int IntersectSegmentTriangle(Point p, Point q, Point a, Point b, Point c,
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

	}
}

#endif//EAE_ENGINE_COLLISION_DETECTION_FUNCTIONS_H

