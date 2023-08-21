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

    inline void PushRay(DebugRenderBuffer *buf, Vertex vert)
    {
        if (buf->verticesAmount < 1024)
            buf->vertices[buf->verticesAmount] = vert;
    }

    inline void PushVertex(DebugRenderBuffer *buf, Vertex vert)
    {
        if (buf->verticesAmount < 1024)
            buf->vertices[buf->verticesAmount++] = vert;
    }

    void DrawBB(MeshResource &mesh, V4 color, M4 modelMatrix)
    {
        V3 &min = mesh.min;
        V3 &max = mesh.max;

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], max[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], max[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], max[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], max[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], max[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], max[1], min[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], max[1], max[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], max[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], min[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], min[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], min[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], min[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], min[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], min[1], min[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], min[1], max[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], min[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], max[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], min[1], min[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], max[1], min[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], min[1], min[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], max[1], max[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(min[0], min[1], max[2], 1), color});

        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], max[1], max[2], 1), color});
        PushVertex(&lineBuf, Vertex{Transpose(modelMatrix) * V4(max[0], min[1], max[2], 1), color});
    }

    void DrawAABB(MeshResource &mesh, V4 color, M4 modelMatrix)
    {
        mesh.find_bounds();

        float data[6] = {mesh.min[0], mesh.max[0], mesh.min[1], mesh.max[1], mesh.min[2], mesh.max[2]};
        V3 current = (Transpose(modelMatrix) * V4(data[0], data[2], data[4], 1)).toV3();
        float left = current[0];
        float bottom = current[1];
        float front = current[2];

        float right = current[0];
        float top = current[1];
        float back = current[2];

        for (size_t i = 1; i < 8; i++)
        {
            V3 current = (Transpose(modelMatrix) * V4(data[i / 4], data[2 + (i / 2) % 2], data[4 + i % 2], 1)).toV3();
            if (current[0] < left)
                left = current[0];
            if (current[1] < bottom)
                bottom = current[1];
            if (current[2] < front)
                front = current[2];

            if (current[0] > right)
                right = current[0];
            if (current[1] > top)
                top = current[1];
            if (current[2] > back)
                back = current[2];
        }

        PushVertex(&lineBuf, Vertex{V4(left, top, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(left, top, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(right, top, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, top, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, top, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, top, front, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, top, back, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, top, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, bottom, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(left, bottom, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(right, bottom, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, bottom, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, bottom, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, bottom, front, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, bottom, back, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, bottom, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, top, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(left, bottom, front, 1), color});

        PushVertex(&lineBuf, Vertex{V4(right, top, front, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, bottom, front, 1), color});

        PushVertex(&lineBuf, Vertex{V4(left, top, back, 1), color});
        PushVertex(&lineBuf, Vertex{V4(left, bottom, back, 1), color});

        PushVertex(&lineBuf, Vertex{V4(right, top, back, 1), color});
        PushVertex(&lineBuf, Vertex{V4(right, bottom, back, 1), color});
    }

    void DrawLine(V4 start, V4 end, V4 color)
    {
        PushVertex(&lineBuf, Vertex{start, color});
        PushVertex(&lineBuf, Vertex{end, color});
    }

    void DrawSquare(V4 center, float size, V4 color)
    {
        PushVertex(&triBuf, Vertex{center + V3(-size / 2, 0, -size / 2), color});
        PushVertex(&triBuf, Vertex{center + V3(-size / 2, 0, size / 2), color});
        PushVertex(&triBuf, Vertex{center + V3(size / 2, 0, size / 2), color});

        PushVertex(&triBuf, Vertex{center + V3(-size / 2, 0, -size / 2), color});
        PushVertex(&triBuf, Vertex{center + V3(size / 2, 0, -size / 2), color});
        PushVertex(&triBuf, Vertex{center + V3(size / 2, 0, size / 2), color});
    }

    void DrawSphere(V4 center, float size, V4 color)
    {
    }

    void Render(M4 cameraVPMatrix)
    {
        // Rendering debug triangles
        /*if (triBuf.vbo != 0)
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
        triBuf.verticesAmount = 0;*/

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
