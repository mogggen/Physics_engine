#include "config.h"
#include "Debug.h"
#include "GL/glew.h"

#ifndef MAX_SIZE
#define MAX_SIZE 1024
#endif

namespace Debug
{
    struct Vertex
    {
        V4 pos;
        V4 color;
    };
    struct DebugRenderBuffer
    {
        GLuint vbo = 0;
        int verticesAmount = 0;
        Vertex vertices[MAX_SIZE];
        ShaderResource sr;
    };

    DebugRenderBuffer triBuf;
    DebugRenderBuffer lineBuf;

    inline void PushVertex(DebugRenderBuffer *buf, Vertex vert)
    {
        buf->vertices[buf->verticesAmount++] = vert;
    }

    void DrawBB(const MeshResource& mesh, V4 color, M4 modelMatrix)
    {
         const float left = mesh.left;
         const float bottom = mesh.bottom;
         const float front = mesh.front;

         const float right = mesh.right;
         const float top = mesh.top;
         const float back = mesh.back;
         
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, top, front, 1), color});
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, top, back, 1), color});
         
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, top, front, 1), color});
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, top, back, 1), color});

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, top, front, 1), color});
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, top, front, 1), color});

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, top, back, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, top, back, 1), color });


         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, bottom, front, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, bottom, back, 1), color });

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, bottom, front, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, bottom, back, 1), color });

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, bottom, front, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, bottom, front, 1), color });

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, bottom, back, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, bottom, back, 1), color });


         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, bottom, front, 1), color });

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, bottom, front, 1), color });

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, top, back, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(left, bottom, back, 1), color });

         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, top, back, 1), color });
         PushVertex(&lineBuf, Vertex{ Transpose(modelMatrix) * V4(right, bottom, back, 1), color });
    }

    void DrawAABB(const MeshResource& mesh, V4 color, M4 modelMatrix)
     {
		 float data[6] = { mesh.left, mesh.right, mesh.bottom, mesh.top, mesh.back, mesh.front };
		 V3 current = (Transpose(modelMatrix) * V4(data[0], data[2], data[4], 1)).toV3();
          float left = current.x;
          float bottom = current.y;
          float front = current.z;

          float right = current.x;
          float top = current.y;
          float back = current.z;

         
         for (size_t i = 1; i < 8; i++)
         {
             V3 current = (Transpose(modelMatrix) * V4(data[i / 4], data[2 + (i / 2) % 2], data[4 + i % 2], 1)).toV3();
             if (current.x < left)
                 left = current.x;
             if (current.y < bottom)
                 bottom = current.y;
             if (current.z < front)
                 front = current.z;

             if (current.x > right)
                 right = current.x;
             if (current.y > top)
                 top = current.y;
             if (current.z > back)
                 back = current.z;
         }
         
         PushVertex(&lineBuf, Vertex{V4(left, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(left, top, back, 1), color });

         PushVertex(&lineBuf, Vertex{V4(right, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, top, back, 1), color });

         PushVertex(&lineBuf, Vertex{V4(left, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, top, front, 1), color });

         PushVertex(&lineBuf, Vertex{V4(left, top, back, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, top, back, 1), color });


         PushVertex(&lineBuf, Vertex{V4(left, bottom, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(left, bottom, back, 1), color });

         PushVertex(&lineBuf, Vertex{V4(right, bottom, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, bottom, back, 1), color });

         PushVertex(&lineBuf, Vertex{V4(left, bottom, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, bottom, front, 1), color });

         PushVertex(&lineBuf, Vertex{V4(left, bottom, back, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, bottom, back, 1), color });


         PushVertex(&lineBuf, Vertex{V4(left, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(left, bottom, front, 1), color });

         PushVertex(&lineBuf, Vertex{V4(right, top, front, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, bottom, front, 1), color });

         PushVertex(&lineBuf, Vertex{V4(left, top, back, 1), color });
         PushVertex(&lineBuf, Vertex{V4(left, bottom, back, 1), color });

         PushVertex(&lineBuf, Vertex{V4(right, top, back, 1), color });
         PushVertex(&lineBuf, Vertex{V4(right, bottom, back, 1), color });
     }

    void DrawLine(V4 start, V4 end, V4 color)
    {
        PushVertex(&lineBuf, Vertex{start, color});
        PushVertex(&lineBuf, Vertex{end, color});
    }

    void DrawSquare(V4 center, float size, V4 color)
    {
        PushVertex(&triBuf, Vertex{center + V3(-size/2, 0, -size/2), color});
        PushVertex(&triBuf, Vertex{center + V3(-size/2, 0, size/2), color});
        PushVertex(&triBuf, Vertex{center + V3(size/2, 0, size/2), color});

        PushVertex(&triBuf, Vertex{center + V3(-size/2, 0, -size/2), color});
        PushVertex(&triBuf, Vertex{center + V3(size/2, 0, -size/2), color});
        PushVertex(&triBuf, Vertex{center + V3(size/2, 0, size/2), color});
    }

    void Render(M4 cameraVPMatrix)
    {
        // Rendering debug triangles
        if (triBuf.vbo != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, triBuf.vbo);
        }
        else
        {
            triBuf.sr.LoadShader(triBuf.sr.vs, triBuf.sr.ps, "engine/render/DebugVShader.glsl", "engine/render/DebugPShader.glsl");
            glGenBuffers(1, &triBuf.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, triBuf.vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_SIZE, nullptr, GL_DYNAMIC_DRAW);
        }
        triBuf.sr.bindShaderResource();
        triBuf.sr.setM4(cameraVPMatrix.data, "matrix");
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * triBuf.verticesAmount, triBuf.vertices);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 4));
        glDrawArrays(GL_TRIANGLES, 0, triBuf.verticesAmount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        triBuf.verticesAmount = 0;

        // Rendering debug lines
        if (lineBuf.vbo != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, lineBuf.vbo);
        }
        else
        {
            lineBuf.sr.LoadShader(lineBuf.sr.vs, lineBuf.sr.ps, "engine/render/DebugVShader.glsl", "engine/render/DebugPShader.glsl");
            glGenBuffers(1, &lineBuf.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, lineBuf.vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_SIZE, nullptr, GL_DYNAMIC_DRAW);
        }
        lineBuf.sr.bindShaderResource();
        lineBuf.sr.setM4(cameraVPMatrix.data, "matrix");
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * lineBuf.verticesAmount, lineBuf.vertices);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));
        glDrawArrays(GL_LINES, 0, lineBuf.verticesAmount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        lineBuf.verticesAmount = 0;
    }
}
