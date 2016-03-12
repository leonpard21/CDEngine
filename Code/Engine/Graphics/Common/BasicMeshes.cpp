#include "BasicMeshes.h"
#include "Math/ColMatrix.h"
#include "Math/MathTool.h"
#include <cmath>

namespace EAE_Engine
{
	namespace Graphics
	{
		RectangleSOAMesh::RectangleSOAMesh()
		{
			//We need 4 vertices
			_vertices.push_back(Math::Vector3(-0.5f, -0.5f, 0.0f));
			_vertices.push_back(Math::Vector3(-0.5f, 0.5f, 0.0f));
			_vertices.push_back(Math::Vector3(0.5f, 0.5f, 0.0f));
			_vertices.push_back(Math::Vector3(0.5f, -0.5f, 0.0f));
		}

		RectangleSOAMesh::RectangleSOAMesh(float i_left, float i_right, float i_bottom, float i_top)
		{
			//We need 4 vertices
			_vertices.push_back(Math::Vector3(i_left, i_bottom, 0.0f));
			_vertices.push_back(Math::Vector3(i_right, i_bottom, 0.0f));
			_vertices.push_back(Math::Vector3(i_right, i_top, 0.0f));
			_vertices.push_back(Math::Vector3(i_left, i_top, 0.0f));
		}

		BoxSOAMesh::BoxSOAMesh()
		{
			// Vertices Data, there are 24 vertices for a box
			{
				//Upper 4 vertices
				// front
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, -0.5f));
				for (uint32_t i = 0; i < 4; ++i)
				{
					_normals.push_back(Math::Vector3(0.0f, 0.0f, -1.0f));
				}
				// back
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, 0.5f));
				for (uint32_t i = 0; i < 4; ++i)
				{
					_normals.push_back(Math::Vector3(0.0f, 0.0f, 1.0f));
				}
				// top
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, -0.5f));
				for (uint32_t i = 0; i < 4; ++i)
				{
					_normals.push_back(Math::Vector3(0.0f, 1.0f, 0.0f));
				}
				// bottom
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, 0.5f));
				for (uint32_t i = 0; i < 4; ++i)
				{
					_normals.push_back(Math::Vector3(0.0f, -1.0f, 0.0f));
				}
				// left
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, +0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, +0.5f, +0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, +0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, -0.5f));
				for (uint32_t i = 0; i < 4; ++i)
				{
					_normals.push_back(Math::Vector3(-1.0f, 0.0f, 0.0f));
				}
				// right
				_vertices.push_back(Math::Vector3(+0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(+0.5f, +0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(+0.5f, +0.5f, +0.5f));
				_vertices.push_back(Math::Vector3(+0.5f, -0.5f, +0.5f));
				for (uint32_t i = 0; i < 4; ++i)
				{
					_normals.push_back(Math::Vector3(1.0f, 0.0f, 0.0f));
				}
				// Insert UVs, add 3 * 8 = 24 uvs
				for (uint32_t i = 0; i < 3; ++i)
				{
					// forward
					_uvs.push_back(Math::Vector2(0.0f, 1.0f));
					_uvs.push_back(Math::Vector2(0.0f, 0.0f));
					_uvs.push_back(Math::Vector2(1.0f, 0.0f));
					_uvs.push_back(Math::Vector2(1.0f, 1.0f));
					// backward
					_uvs.push_back(Math::Vector2(1.0f, 1.0f));
					_uvs.push_back(Math::Vector2(0.0f, 1.0f));
					_uvs.push_back(Math::Vector2(0.0f, 0.0f));
					_uvs.push_back(Math::Vector2(1.0f, 0.0f));
				}
				// Add colors for 24 vertices (we have 3 pairs of 2 faces, each face has 4 vertices, so it is 3 * 2 * 4 = 24 vertices)
				for (uint32_t i = 0; i < 24; ++i)
				{
					Math::TVector4<uint8_t> color(255, 255, 255, 255);
					_colors.push_back(color);
				}
			}
			// Indices Data, there are 36 indices for a box
			{
				_indices = { 0, 1, 2, 0, 2, 3, 
					4, 5, 6, 4, 6, 7,
					8, 9, 10, 8, 10, 11,
					12, 13, 14, 12, 14, 15,
					16, 17, 18, 16, 18, 19, 
					20, 21, 22, 20, 22, 23 };
			}
		}

		/*
		 * The code came from: http://richardssoftware.net/Home/Post/7
		 * stack is the rows, slice is the columns
		 */
		SphereSOAMesh::SphereSOAMesh(uint32_t sliceCount, uint32_t stackCount, float radius)
		{
			const float phiStep = Math::Pi / stackCount;
			const float thetaStep = 2.0f * Math::Pi / sliceCount;
			// Add vertices
			{
				// north pole vertex
				{
					_vertices.push_back(Math::Vector3(0.0f, radius, 0.0f));
					_normals.push_back(Math::Vector3(0.0f, 1.0f, 0.0f));
					// UV of the sphere can be thought as a rectangle covers the surface of the sphere
					_uvs.push_back(Math::Vector2(0.0f, 0.0f));
				}
				// vertices on the middle stacks, they are saved from one stack to another.
				for (uint32_t i = 1; i <= stackCount - 1; ++i) {
					float phi = i * phiStep;
					for (uint32_t j = 0; j <= sliceCount; ++j) {
						float theta = j * thetaStep;
						Math::Vector3 p = Math::Vector3((radius*std::sinf(phi)*std::cosf(theta)),
							(radius * std::cosf(phi)),
							(radius* std::sinf(phi)*std::sinf(theta)));
						_vertices.push_back(p);
						Math::Vector3 t(-radius * std::sinf(phi) * std::sinf(theta), 0.0f, radius * std::sinf(phi) * std::cosf(theta));
						t.Normalize();
						Math::Vector3 n = p.GetNormalize();
						_normals.push_back(n);
						Math::Vector2 uv(theta / (Math::Pi * 2.0f), phi / Math::Pi);
						_uvs.push_back(uv);
					}
				}
				// south pole vertex
				{
					_vertices.push_back(Math::Vector3(0.0f, -radius, 0.0f));
					_normals.push_back(Math::Vector3(0.0f, -1.0f, 0.0f));
					_uvs.push_back(Math::Vector2(0.0f, 1.0f));
				}
				// add color for each vertex
				for (uint32_t i = 0; i < _vertices.size(); ++i)
				{
					Math::TVector4<uint8_t> color(255, 255, 255, 255);
					_colors.push_back(color);
				}
			}
			// Add Indices
			{
				// Add indices for north pole
				for (uint32_t i = 1; i <= sliceCount; ++i) {
					_indices.push_back(0);
					_indices.push_back(i + 1);
					_indices.push_back(i);
				}
				// Add indices for middle stacks,
				// we jump over the North pole Vertex.
				uint32_t baseIndex = 1;
				// in a ring(row), there should have sliceCount +£±vertices, 
				// if we make the surface of the sphere a plane.
				// Also when we created the Sphere vertices, 
				// we treated the first and the last vertex in a row as different vertices.
				// So we wll have sliceCount + 1 vertices in a row.
				// So for Row 1 to Row Stack - 1, 
				// it is a (stackCount - 2) * ringVertexCount matrix.
				const uint32_t ringVertexCount = sliceCount + 1;
				for (uint32_t i = 0; i < stackCount - 2; ++i) {
					for (uint32_t j = 0; j < sliceCount; ++j) {
						// one triangle that 2 vertices in one row and another 1 vertex in the next row.
						_indices.push_back(baseIndex + i * ringVertexCount + j);
						_indices.push_back(baseIndex + i * ringVertexCount + j + 1);
						_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
						// one triangle that 1 vertex in one row and another 2 vertices in the new row.
						_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
						_indices.push_back(baseIndex + i * ringVertexCount + j + 1);
						_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
					}
				}
				// Add indices for south pole
				uint32_t southPoleIndex = (uint32_t)_vertices.size() - 1;
				// The second last row should be southPoleIndex - 1 - sliceCount = southPoleIndex - ringVertexCount.
				baseIndex = southPoleIndex - ringVertexCount;
				for (uint32_t i = 0; i < sliceCount; ++i) {
					_indices.push_back(southPoleIndex);
					_indices.push_back(baseIndex + i);
					_indices.push_back(baseIndex + i + 1);
				}
			}
		}


		/*
		 * Code of calculating the Cylinder, comes from http://richardssoftware.net/Home/Post/7
		 */
		void BuildCylinderTopCap(float topRadius, float height, uint32_t sliceCount, CylinderSOAMesh& o_cylinder) {
			uint32_t baseIndex = (uint32_t)o_cylinder._vertices.size();

			float y = 0.5f * height;
			float dTheta = 2.0f * Math::Pi / sliceCount;
			for (uint32_t i = 0; i <= sliceCount; ++i) {
				float x = topRadius * std::cosf(i * dTheta);
				float z = topRadius* std::sinf(i * dTheta);
				o_cylinder._vertices.push_back(Math::Vector3(x, y, z));
				o_cylinder._normals.push_back(Math::Vector3(0.0f, 1.0f, 0.0f));
				float u = x / height + 0.5f;
				float v = z / height + 0.5f;
				o_cylinder._uvs.push_back(Math::Vector2(u, v));
				Math::TVector4<uint8_t> color(255, 200, 255, 255);
				o_cylinder._colors.push_back(color);
			}
			// Add the center point 
			{
				o_cylinder._vertices.push_back(Math::Vector3(0.0f, y, 0.0f));
				o_cylinder._normals.push_back(Math::Vector3(0.0f, 1.0f, 0.0f));
				o_cylinder._uvs.push_back(Math::Vector2(0.5f, 0.5f));
				Math::TVector4<uint8_t> color(255, 200, 255, 255);
				o_cylinder._colors.push_back(color);
			}
			uint32_t centerIndex = (uint32_t)o_cylinder._vertices.size() - 1;
			for (uint32_t i = 0; i < sliceCount; ++i) {
				o_cylinder._indices.push_back(centerIndex);
				o_cylinder._indices.push_back(baseIndex + i + 1);
				o_cylinder._indices.push_back(baseIndex + i);
			}
		}

		void BuildCylinderBottomCap(float bottomRadius, float height, uint32_t sliceCount, CylinderSOAMesh& o_cylinder) {
			uint32_t baseIndex = (uint32_t)o_cylinder._vertices.size();

			float y = -0.5f * height;
			float dTheta = 2.0f * Math::Pi / sliceCount;

			for (uint32_t i = 0; i <= sliceCount; ++i) {
				float x = bottomRadius * std::cosf(i * dTheta);
				float z = bottomRadius * std::sinf(i * dTheta);
				o_cylinder._vertices.push_back(Math::Vector3(x, y, z));
				o_cylinder._normals.push_back(Math::Vector3(0.0f, -1.0f, 0.0f));
				float u = x / height + 0.5f;
				float v = z / height + 0.5f;
				o_cylinder._uvs.push_back(Math::Vector2(u, v));
				Math::TVector4<uint8_t> color(255, 200, 255, 255);
				o_cylinder._colors.push_back(color);
			}
			// Add the center point 
			{
				o_cylinder._vertices.push_back(Math::Vector3(0.0f, y, 0.0f));
				o_cylinder._normals.push_back(Math::Vector3(0.0f, -1.0f, 0.0f));
				o_cylinder._uvs.push_back(Math::Vector2(0.5f, 0.5f));
				Math::TVector4<uint8_t> color(255, 200, 255, 255);
				o_cylinder._colors.push_back(color);
			}
			uint32_t centerIndex = (uint32_t)o_cylinder._vertices.size() - 1;
			for (uint32_t i = 0; i < sliceCount; ++i) {
				o_cylinder._indices.push_back(centerIndex);
				o_cylinder._indices.push_back(baseIndex + i);
				o_cylinder._indices.push_back(baseIndex + i + 1);
			}
		}

		CylinderSOAMesh::CylinderSOAMesh(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount) {
			float stackHeight = height / stackCount;//height per stack
			float radiusStep = (topRadius - bottomRadius) / stackCount;
			uint32_t ringCount = stackCount + 1;// stack is the gap, so rings will be one more than that
			for (uint32_t i = 0; i < ringCount; ++i) {
				// calculate height from bottom to top
				float y = -0.5f * height + i * stackHeight;
				// calculate radius of the ring from bottom to top
				float r = bottomRadius + i * radiusStep;
				float dTheta = 2.0f * Math::Pi / sliceCount;
				for (uint32_t j = 0; j <= sliceCount; ++j) {
					float c = std::cosf(j * dTheta);
					float s = std::sinf(j * dTheta);
					Math::Vector3 vertex(r*c, y, r*s);
					_vertices.push_back(vertex);
					Math::Vector2 uv((float)j / sliceCount, 1.0f - (float)i / stackCount);
					_uvs.push_back(uv);
					Math::Vector3 t(-s, 0.0f, c);
					float dr = bottomRadius - topRadius;
					Math::Vector3 bitangent(dr*c, -height, dr*s);
					Math::Vector3 n = Math::Vector3::Cross(t, bitangent);
					n.Normalize();
					_normals.push_back(n);
					Math::TVector4<uint8_t> color(255, 200, 255, 255);
					_colors.push_back(color);
				}
			}
			uint32_t ringVertexCount = sliceCount + 1; //vertex count per ring
			for (uint32_t i = 0; i < stackCount; ++i) {
				for (uint32_t j = 0; j < sliceCount; ++j) {
					// the 1st triangle of the rectangle of side
					_indices.push_back(i * ringVertexCount + j);
					_indices.push_back((i + 1) * ringVertexCount + j);
					_indices.push_back((i + 1) * ringVertexCount + j + 1);
					// the 2nd triangle of the rectangle of side
					_indices.push_back(i * ringVertexCount + j);
					_indices.push_back((i + 1) * ringVertexCount + j + 1);
					_indices.push_back(i * ringVertexCount + j + 1);
				}
			}
			BuildCylinderTopCap(topRadius, height, sliceCount, *this);
			BuildCylinderBottomCap(bottomRadius, height, sliceCount, *this);
		}

	}
}