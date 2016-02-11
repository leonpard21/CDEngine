#include "BasicMeshes.h"
#include "Math/ColMatrix.h"
#include "Math/MathTool.h"
#include <cmath>

namespace EAE_Engine
{
	namespace Graphics
	{
		RectangleMesh::RectangleMesh()
		{
			//We need 4 vertices
			_vertices.push_back(Math::Vector3(-0.5f, -0.5f, 0.0f));
			_vertices.push_back(Math::Vector3(-0.5f, 0.5f, 0.0f));
			_vertices.push_back(Math::Vector3(0.5f, 0.5f, 0.0f));
			_vertices.push_back(Math::Vector3(0.5f, -0.5f, 0.0f));
		}

		RectangleMesh::RectangleMesh(float i_left, float i_right, float i_bottom, float i_top)
		{
			//We need 4 vertices
			_vertices.push_back(Math::Vector3(i_left, i_bottom, 0.0f));
			_vertices.push_back(Math::Vector3(i_right, i_bottom, 0.0f));
			_vertices.push_back(Math::Vector3(i_right, i_top, 0.0f));
			_vertices.push_back(Math::Vector3(i_left, i_top, 0.0f));
		}

		BoxMesh::BoxMesh()
		{
			// Vertices Data, there are 24 vertices for a box
			{
				//Upper 4 vertices
				// front
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, -0.5f));
				// back
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, 0.5f));
				// top
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, 0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(0.5f, 0.5f, -0.5f));
				// bottom
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(0.5f, -0.5f, 0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, 0.5f));
				// left
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, +0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, +0.5f, +0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, +0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(-0.5f, -0.5f, -0.5f));
				// right
				_vertices.push_back(Math::Vector3(+0.5f, -0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(+0.5f, +0.5f, -0.5f));
				_vertices.push_back(Math::Vector3(+0.5f, +0.5f, +0.5f));
				_vertices.push_back(Math::Vector3(+0.5f, -0.5f, +0.5f));
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
		SphereMesh::SphereMesh(uint32_t sliceCount, uint32_t stackCount, float radius)
		{
			const float phiStep = Math::Pi / stackCount;
			const float thetaStep = 2.0f * Math::Pi / sliceCount;
			// Add vertices
			{
				// north pole vertex
				_vertices.push_back(Math::Vector3(0.0f, radius, 0.0f));
				// vertices on the middle stacks, they are saved from one stack to another.
				for (uint32_t i = 1; i <= stackCount - 1; ++i) {
					float phi = i * phiStep;
					for (uint32_t j = 0; j <= sliceCount; ++j) {
						float theta = j * thetaStep;
						Math::Vector3 p = Math::Vector3((radius*std::sinf(phi)*std::cosf(theta)),
							(radius * std::cosf(phi)),
							(radius* std::sinf(phi)*std::sinf(theta)));
						_vertices.push_back(p);
					}
				}
				// south pole vertex
				_vertices.push_back(Math::Vector3(0.0f, -radius, 0.0f));
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

	}
}