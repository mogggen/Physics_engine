#include "config.h"
#include "Debug.h"
#include "GL/glew.h"
#include "render/ShaderResource.h"

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

    // DebugRenderBuffer triBuf;
    DebugRenderBuffer lineBuf;

    inline void PushVertex(DebugRenderBuffer *buf, Vertex vert)
    {
        buf->vertices[buf->verticesAmount++] = vert;
    }

    // void DrawSquare(float size, V4 pos, V4 color)
    // {
    //     PushVertex(&triBuf, Vertex{pos + V4(-size / 2, 0, -size / 2), color});
    //     PushVertex(&triBuf, Vertex{pos + V4(-size / 2, 0, size / 2), color});
    //     PushVertex(&triBuf, Vertex{pos + V4(size / 2, 0, size / 2), color});

    //     PushVertex(&triBuf, Vertex{pos + V4(-size / 2, 0, -size / 2), color});
    //     PushVertex(&triBuf, Vertex{pos + V4(size / 2, 0, -size / 2), color});
    //     PushVertex(&triBuf, Vertex{pos + V4(size / 2, 0, size / 2), color});
    // }

    void DrawLine(V4 start, V4 end, V4 color)
    {
        PushVertex(&lineBuf, Vertex{start, color});
        PushVertex(&lineBuf, Vertex{end, color});
    }

    void Render(M4 cameraVPMatrix)
    {
        // Rendering debug triangles
        // if (triBuf.vbo == 0)
        // {
        //     triBuf.sr.LoadShader(triBuf.sr.vs, triBuf.sr.ps, "engine/render/DebugVShader.glsl", "engine/render/DebugPShader.glsl");
        //     glGenBuffers(1, &triBuf.vbo);
        //     glBindBuffer(GL_ARRAY_BUFFER, triBuf.vbo);
        //     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_SIZE, nullptr, GL_DYNAMIC_DRAW);
        // }
        // else
        // {
        //     glBindBuffer(GL_ARRAY_BUFFER, triBuf.vbo);
        // }
        // triBuf.sr.bindShaderResource();
        // triBuf.sr.setM4(cameraVPMatrix.data, "matrix");
        // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * triBuf.verticesAmount, triBuf.vertices);
        // glEnableVertexAttribArray(0);
        // glEnableVertexAttribArray(1);
        // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
        // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 4));
        // glDrawArrays(GL_TRIANGLES, 0, triBuf.verticesAmount);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // triBuf.verticesAmount = 0;

        // Rendering debug lines
        if (lineBuf.vbo == 0)
        {
            lineBuf.sr.LoadShader(lineBuf.sr.vs, lineBuf.sr.ps, "engine/render/DebugVShader.glsl", "engine/render/DebugPShader.glsl");
            glGenBuffers(1, &lineBuf.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, lineBuf.vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_SIZE, nullptr, GL_DYNAMIC_DRAW);
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, lineBuf.vbo);
        }
        lineBuf.sr.bindShaderResource();
        lineBuf.sr.setM4(cameraVPMatrix, "matrix");
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
