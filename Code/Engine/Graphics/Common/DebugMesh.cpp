#include "DebugMesh.h"
#include "DebugShape/DebugShape.h"
#include "Math/MathTool.h"
#include "Math/ColMatrix.h"
#include <vector>
#include "BinaryFileLoader.h"
#include "GraphicsOp.h"
#include "RenderObj.h"
#include "RenderDatas.h"
#include "MeshRender.h"
#include "BasicMeshes.h"
#include "Material.h"

namespace 
{
    struct DebugVertex
    {
        // POSITION: 3 floats == 12 bytes, Offset = 0
        float x, y, z;
        // COLOR0: 4 uint8_ts == 4 bytes, Offset = 12
#if defined( EAEENGINE_PLATFORM_D3D9 )
        uint8_t b, g, r, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#elif defined( EAEENGINE_PLATFORM_GL )
        uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif
        DebugVertex() = default;
        DebugVertex(EAE_Engine::Math::Vector3 i_pos) :
            x(i_pos.x()), y(i_pos.y()), z(i_pos.z())
        {
            r = (uint8_t)255;
            g = (uint8_t)255;
            b = (uint8_t)255;
            a = (uint8_t)255;
        }
        DebugVertex(EAE_Engine::Math::Vector3 i_pos, EAE_Engine::Math::Vector4 i_color) :
            x(i_pos.x()), y(i_pos.y()), z(i_pos.z())
        {
            r = (uint8_t)(EAE_Engine::Math::clamp<float>(i_color.x(), 0.0f, 1.0f) * 255.0f);
            g = (uint8_t)(EAE_Engine::Math::clamp<float>(i_color.y(), 0.0f, 1.0f) * 255.0f);
            b = (uint8_t)(EAE_Engine::Math::clamp<float>(i_color.z(), 0.0f, 1.0f) * 255.0f);
            a = (uint8_t)(EAE_Engine::Math::clamp<float>(i_color.w(), 0.0f, 1.0f) * 255.0f);
        }
    };
}

namespace EAE_Engine
{
    namespace Graphics
    {
        DebugMeshes::DebugMeshes():_pSegmentsMeshRender(nullptr), _pBoxesMeshRender(nullptr), _pShperesMeshRender(nullptr), 
            _pSegmentsMesh(nullptr), _pBoxesMesh(nullptr), _pShperesMesh(nullptr) {}

        void DebugMeshes::Init()
        {
            LoadMaterial("data/Materials/debugPrimitives.material");
            _pSegmentsMeshRender = new AOSMeshRender(); 
            _pSegmentsMeshRender->AddMaterial("debugPrimitives");
            _pCirclesMeshRender = new AOSMeshRender();
            _pCirclesMeshRender->AddMaterial("debugPrimitives");
            _pBoxesMeshRender = new AOSMeshRender();
            _pBoxesMeshRender->AddMaterial("debugPrimitives");
            _pShperesMeshRender = new AOSMeshRender();
            _pShperesMeshRender->AddMaterial("debugPrimitives");
            _pTempMeshRender = new AOSMeshRender();
            _pTempMeshRender->AddMaterial("debugPrimitives");

            // Setup vertices format.
#if defined( EAEENGINE_PLATFORM_D3D9 )
            D3DVERTEXELEMENT9 elemnt_arr[] = {
                // Stream: 0, POSITION: 3 floats == 12 bytes, Offset = 0
                { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
                // Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 12
                { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }
            };
#elif defined( EAEENGINE_PLATFORM_GL )
            EAE_Engine::Graphics::MeshGLVertexElement element_arr[] = {
                { 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) },        // Position
                { 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) }  // Color
            };
#endif
            // For Segments
            {
#if defined( EAEENGINE_PLATFORM_D3D9 )
              EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 2, {sizeof(DebugVertex), D3DPT_LINELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
              EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr, 2, {sizeof(DebugVertex), GL_LINES, GL_STREAM_COPY } };
#endif
              DebugVertex vertices;
              Mesh::sSubMesh subMesh(0, 0);
              {
                vertices.x = 0.0f;
                vertices.y = 0.0f;
                vertices.z = -1.0f;
                vertices.r = (uint8_t)255;
                vertices.g = (uint8_t)255;
                vertices.b = (uint8_t)255;
                vertices.a = (uint8_t)255;
              }
              _pSegmentsMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, &vertices, 1, nullptr, 0, nullptr, 0);
              _pSegmentsMeshRender->SetMesh(_pSegmentsMesh);
            }
            // Circle
            {
#if defined( EAEENGINE_PLATFORM_D3D9 )
              EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 2,{ sizeof(DebugVertex), D3DPT_LINELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
              EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr, 2,{ sizeof(DebugVertex), GL_LINES, GL_STREAM_COPY } };
#endif
              CircleSOAMesh standardCircle;
              uint32_t vertexCount = (uint32_t)standardCircle._vertices.size();
              DebugVertex* pVertices = new DebugVertex[vertexCount];
              // Set the Vertices Information.
              for (uint32_t verticesIndex = 0; verticesIndex < standardCircle._vertices.size(); ++verticesIndex)
              {
                pVertices[verticesIndex] = DebugVertex(standardCircle._vertices[verticesIndex]);
                pVertices[verticesIndex].a = (uint8_t)100;
              }
              _pCircleMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, pVertices, vertexCount, nullptr, 0, nullptr, 0);
              SAFE_DELETE_ARRAY(pVertices);
              _pCirclesMeshRender->SetMesh(_pCircleMesh);
            }

#if defined( EAEENGINE_PLATFORM_D3D9 )
            EAE_Engine::Graphics::MeshD3DVertexElements elements = { elemnt_arr, 2,{ sizeof(DebugVertex), D3DPT_TRIANGLELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
            EAE_Engine::Graphics::MeshGLVertexElements elements = { element_arr, 2,{ sizeof(DebugVertex), GL_TRIANGLES, GL_STREAM_COPY } };
#endif
            {
                BoxSOAMesh standardBox;
                uint32_t vertexCount = (uint32_t)standardBox._vertices.size();
                uint32_t indexCount = (uint32_t)standardBox._indices.size();
                // alloc memory for vertices and indices buffers
                DebugVertex* pVertices = new DebugVertex[vertexCount];
                uint32_t* pIndices = new uint32_t[indexCount];
                {
                    // Set the Vertices Information.
                    for (uint32_t verticesIndex = 0; verticesIndex < standardBox._vertices.size(); ++verticesIndex)
                    {
                        pVertices[verticesIndex] = DebugVertex(standardBox._vertices[verticesIndex]);
                        pVertices[verticesIndex].a = (uint8_t)100;
                    }
                    // Set the Indices Information.
                    for (uint32_t indicesIndex = 0; indicesIndex < standardBox._indices.size(); indicesIndex += 3)
                    {
                        pIndices[indicesIndex + 0] = standardBox._indices[indicesIndex + 0];
#if defined( EAEENGINE_PLATFORM_D3D9 )
                        pIndices[indicesIndex + 2] = standardBox._indices[indicesIndex + 1];
                        pIndices[indicesIndex + 1] = standardBox._indices[indicesIndex + 2];
#elif defined( EAEENGINE_PLATFORM_GL )
                        pIndices[indicesIndex + 1] = standardBox._indices[indicesIndex + 1];
                        pIndices[indicesIndex + 2] = standardBox._indices[indicesIndex + 2];
#endif
                    }
                }
                Mesh::sSubMesh subMesh(0, indexCount - 1);
                _pBoxesMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, pVertices, vertexCount, pIndices, indexCount, &subMesh, 1);
                SAFE_DELETE_ARRAY(pVertices);
                SAFE_DELETE_ARRAY(pIndices);
                _pBoxesMeshRender->SetMesh(_pBoxesMesh);
            }
            {

                // alloc memory for buffers
                SphereSOAMesh standardSphere(6, 6);
                uint32_t vertexCount = (uint32_t)standardSphere._vertices.size();
                uint32_t indexCount = (uint32_t)standardSphere._indices.size();
                DebugVertex* pVertices = new DebugVertex[vertexCount];
                uint32_t* pIndices = new uint32_t[indexCount];
                {
                    // Set the Vertices Information.
                    for (uint32_t verticesIndex = 0; verticesIndex < vertexCount; ++verticesIndex)
                    {
                        Math::Vector3 currentVertex = standardSphere._vertices[verticesIndex];
                        pVertices[verticesIndex] = DebugVertex(currentVertex);
                        pVertices[verticesIndex].a = (uint8_t)100;
                    }
                    // Set the Indices Information.
                    for (uint32_t indicesIndex = 0; indicesIndex < indexCount; indicesIndex += 3)
                    {
                        pIndices[indicesIndex + 0] = standardSphere._indices[indicesIndex + 0];
#if defined( EAEENGINE_PLATFORM_D3D9 )
                        pIndices[indicesIndex + 2] = standardSphere._indices[indicesIndex + 1];
                        pIndices[indicesIndex + 1] = standardSphere._indices[indicesIndex + 2];
#elif defined( EAEENGINE_PLATFORM_GL )
                        pIndices[indicesIndex + 1] = standardSphere._indices[indicesIndex + 1];
                        pIndices[indicesIndex + 2] = standardSphere._indices[indicesIndex + 2];
#endif
                    }
                }
                Mesh::sSubMesh subMesh(0, indexCount - 1);
                _pShperesMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(elements, pVertices, vertexCount, pIndices, indexCount, &subMesh, 1);
                SAFE_DELETE_ARRAY(pVertices);
                SAFE_DELETE_ARRAY(pIndices);		
                _pShperesMeshRender->SetMesh(_pShperesMesh);
            }
            
            {
#if defined( EAEENGINE_PLATFORM_D3D9 )
                EAE_Engine::Graphics::MeshD3DVertexElements tempMeshelements = { elemnt_arr, 2,{ sizeof(DebugVertex), D3DPT_TRIANGLELIST, Default } };
#elif defined( EAEENGINE_PLATFORM_GL )
                EAE_Engine::Graphics::MeshGLVertexElements tempMeshelements = { element_arr, 2,{ sizeof(DebugVertex), GL_TRIANGLES, GL_STREAM_COPY } };
#endif
                DebugVertex vertices;
                {
                    vertices.x = 0.0f;
                    vertices.y = 0.0f;
                    vertices.z = -1.0f;
                    vertices.r = (uint8_t)255;
                    vertices.g = (uint8_t)255;
                    vertices.b = (uint8_t)255;
                    vertices.a = (uint8_t)255;
                }
                _pTempMesh = EAE_Engine::Graphics::CreateAOSMeshInternal(tempMeshelements, &vertices, 1, nullptr, 0, nullptr, 0);
                _pTempMeshRender->SetMesh(_pTempMesh);
            }
        }

        void DebugMeshes::Clean()
        {
            // DELETE all of the MeshRenders
            SAFE_DELETE(_pSegmentsMeshRender);
            SAFE_DELETE(_pCirclesMeshRender);
            SAFE_DELETE(_pBoxesMeshRender);
            SAFE_DELETE(_pShperesMeshRender);
            SAFE_DELETE(_pTempMeshRender);
            // becareful that we cannot use SAFE_RELEASE, 
            // because this marco will set _pSegmentsMesh to NULL.
            // However we need to delete this mesh memory by ourself.
            if (_pSegmentsMesh)
                _pSegmentsMesh->Release();
            SAFE_DELETE(_pSegmentsMesh);
            if (_pCircleMesh)
              _pCircleMesh->Release();
            SAFE_DELETE(_pCircleMesh);
            if (_pBoxesMesh)
                _pBoxesMesh->Release();
            SAFE_DELETE(_pBoxesMesh);
            if (_pShperesMesh)
                _pShperesMesh->Release();
            SAFE_DELETE(_pShperesMesh);
            if (_pTempMesh)
                _pTempMesh->Release();
            SAFE_DELETE(_pTempMesh);
        }

        void DebugMeshes::Update()
        {

            GenerateDebugSegments();
            GenerateDebugCircles();
            GenerateDebugBoxes();
            GenerateDebugSpheres();
            GenerateDebugMeshes();
        }

        void DebugMeshes::GenerateDebugSegments()
        {
            std::vector<Debug::DebugSegment>& debugSegments = Debug::DebugShapes::GetInstance().GetSegments();
            //  Make sure that the debugSegments has primitives to draw
            if (debugSegments.size() == 0) return;
            // Setup the Vertices Information
            DebugVertex* pVertices = new DebugVertex[debugSegments.size() * 2];
            uint32_t vertexCount = 0;
            for (uint32_t segmentIndex = 0; segmentIndex < debugSegments.size(); ++segmentIndex)
            {
                // Vertex 
                {
                    DebugVertex vertex0(debugSegments[segmentIndex]._start, debugSegments[segmentIndex]._color);
                    vertex0.a = (uint8_t)255;
                    pVertices[vertexCount++] = vertex0;
                }
                {
                    DebugVertex vertex1(debugSegments[segmentIndex]._end, debugSegments[segmentIndex]._color);
                    vertex1.a = (uint8_t)255;
                    pVertices[vertexCount++] = vertex1;
                }
            }
            if (_pSegmentsMesh)
            {
                _pSegmentsMesh->ChangeWholeBuffers(pVertices, vertexCount, nullptr, 0, nullptr, 0);
            }
            SAFE_DELETE_ARRAY(pVertices);

            std::vector<RenderRawData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderRawData3DList();
            // Get the TransformMatrix
            Math::Vector3 white(1.0f, 1.0f, 1.0f);
            RenderRawData3D renderData = {_pSegmentsMeshRender, white, Math::ColMatrix44::Identity };
            renderDataList.push_back(renderData);
        }

        void DebugMeshes::GenerateDebugCircles()
        {
          std::vector<Debug::DebugCircle>& debugCircles = Debug::DebugShapes::GetInstance().GetCircles();
          if (debugCircles.size() == 0) return;
          std::vector<RenderRawData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderRawData3DList();
          for (uint32_t circleIndex = 0; circleIndex < debugCircles.size(); ++circleIndex)
          {
            Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44(Math::Quaternion::Identity, debugCircles[circleIndex]._pos);
            Math::Vector3 scale(debugCircles[circleIndex]._radius, debugCircles[circleIndex]._radius, debugCircles[circleIndex]._radius);
            Math::ColMatrix44 scaleMatrix = Math::ColMatrix44::CreateScaleMatrix(scale);
            tranformsMatrix = tranformsMatrix * scaleMatrix;
            // Get the TransformMatrix
            RenderRawData3D renderData = { _pCirclesMeshRender, debugCircles[circleIndex]._color, tranformsMatrix };
            renderDataList.push_back(renderData);
          }
        }

        void DebugMeshes::GenerateDebugBoxes()
        {
            std::vector<Debug::DebugBox>& debugboxes = Debug::DebugShapes::GetInstance().GetBoxes();
            if (debugboxes.size() == 0) return;
            std::vector<RenderRawData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderRawData3DList();
            for (uint32_t boxIndex = 0; boxIndex < debugboxes.size(); ++boxIndex)
            {
                Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44(debugboxes[boxIndex]._rotation, debugboxes[boxIndex]._pos);
                Math::ColMatrix44 scaleMatrix = Math::ColMatrix44::CreateScaleMatrix(debugboxes[boxIndex]._extents * 2.0f);
                tranformsMatrix = tranformsMatrix * scaleMatrix;
                // Get the TransformMatrix
                RenderRawData3D renderData = { _pBoxesMeshRender, debugboxes[boxIndex]._color, tranformsMatrix };
                renderDataList.push_back(renderData);
            }
        }

        void DebugMeshes::GenerateDebugSpheres()
        {
            std::vector<Debug::DebugSphere>& debugSpheres = Debug::DebugShapes::GetInstance().GetSpheres();
            if (debugSpheres.size() == 0) return;
            std::vector<RenderRawData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderRawData3DList();
            // Get vertices and indices information for all of the debug meshes
            for (uint32_t sphereIndex = 0; sphereIndex < debugSpheres.size(); ++sphereIndex)
            {
                Math::Quaternion rotation = Math::Quaternion::Identity;
                Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44(rotation, debugSpheres[sphereIndex]._pos);
                Math::ColMatrix44 scaleMatrix = Math::ColMatrix44::CreateScaleMatrix(Math::Vector3(debugSpheres[sphereIndex]._radius *2.0f, debugSpheres[sphereIndex]._radius * 2.0f, debugSpheres[sphereIndex]._radius * 2.0f));
                tranformsMatrix = tranformsMatrix * scaleMatrix;
                // Get the TransformMatrix
                RenderRawData3D renderData = { _pShperesMeshRender, debugSpheres[sphereIndex]._color, tranformsMatrix };
                renderDataList.push_back(renderData);
            }
        }

        void DebugMeshes::GenerateDebugMeshes() 
        {
            std::vector<Debug::DebugMesh>& debugMeshes = Debug::DebugShapes::GetInstance().GetMeshes();
            if (debugMeshes.size() == 0) return;
            std::vector<DebugVertex> _debugVertices;
            // Get vertices and indices information for all of the debug meshes
            for (uint32_t meshIndex = 0; meshIndex < debugMeshes.size(); ++meshIndex)
            {
                Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44::Identity;
                for (uint32_t index = 0; index < debugMeshes[meshIndex]._vertices.size(); ++index)
                {
                    DebugVertex vertex(debugMeshes[meshIndex]._vertices[index], debugMeshes[meshIndex]._color);
                    vertex.a = (uint8_t)100;
                    _debugVertices.push_back(vertex);
                }
            }
            if (_debugVertices.size() < 3)
                return;
            _pTempMesh->ChangeWholeBuffers(&_debugVertices[0], (uint32_t)_debugVertices.size(), nullptr, 0, nullptr, 0);
            _pTempMeshRender->SetMesh(_pTempMesh);
            std::vector<RenderRawData3D>& renderDataList = RenderObjManager::GetInstance().GetRenderRawData3DList();
            Math::Vector3 white(1.0f, 1.0f, 1.0f);
            Math::ColMatrix44 tranformsMatrix = Math::ColMatrix44::Identity;
            RenderRawData3D renderData = { _pTempMeshRender, white, tranformsMatrix };
            renderDataList.push_back(renderData);
        }

        ////////////////////////////////static_members/////////////////////////////////
        DebugMeshes* DebugMeshes::s_pInternalInstance = nullptr;

        DebugMeshes& DebugMeshes::GetInstance()
        {
            if (!s_pInternalInstance)
                s_pInternalInstance = new DebugMeshes();
            return *s_pInternalInstance;
        }

        void DebugMeshes::CleanInstance()
        {
            if (!s_pInternalInstance) return;
            s_pInternalInstance->Clean();
            SAFE_DELETE(s_pInternalInstance);
        }

    }
}
