#pragma once

static void Print_f(const V4& v)
{
    std::cout << '(';
    for (size_t i = 0; i < 4; i++)
        std::cout << v.data[i] << (i == 3 ? ")\n" : ", ");
}

inline std::vector<V3> ClipPolygonAgainstPlane(const std::vector<V3>& vertices, const V3& planePoint, const V3& planeNormal)
{
    std::vector<V3> result;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        // Current and next vertex (wrapping around)
        V3 currVertex = vertices[i];
        V3 nextVertex = vertices[(i + 1) % vertices.size()];

        // Check distance of both points from the plane
        float currDist = Dot(currVertex - planePoint, planeNormal);
        float nextDist = Dot(nextVertex - planePoint, planeNormal);

        // If current vertex is inside the plane, keep it
        if (currDist >= 0)
        {
            result.push_back(currVertex);
        }

        // If edge crosses the plane, find intersection point and add it
        if (currDist * nextDist < 0) // Signs differ, there's an intersection
        {
            float t = currDist / (currDist - nextDist); // Interpolation factor
            V3 intersectionPoint = currVertex + t * (nextVertex - currVertex);
            result.push_back(intersectionPoint);
        }
    }

    return result;
}


inline std::vector<V3> ClipFaceAgainstFace(const Face& faceA, const Face& faceB)
{
    //printf("\n");
    //for (const V3& a : faceA.vertices)
    //{
    //    Print_f(a);
    //}
    //printf("\n");
    //Print_f(faceA.normal);
    //for (const V3& b: faceB.vertices)
    //{
    //    Print_f(b);
    //}
    //printf("\n");
    //Print_f(faceB.normal);
    //printf("\n");

    std::vector<V3> clippedVertices = faceA.vertices;

    for (size_t i = 0; i < faceB.vertices.size(); ++i)
    {
        // Get edge and the normal of faceB (edge from vertex i to vertex i+1, wrapping around)
        V3 edge = faceB.vertices[(i + 1) % faceB.vertices.size()] - faceB.vertices[i];
        V3 clipPlaneNormal = Cross(edge, faceB.normal);

        // Clip the vertices of faceA against this plane
        clippedVertices = ClipPolygonAgainstPlane(clippedVertices, faceB.vertices[i], clipPlaneNormal);

        //for each (V3 vert in clippedVertices) Print_f(findAverage(clippedVertices));
        //printf("%i\n", i);

        // If all vertices are outside the clipping plane, return an empty set (no collision)
        if (clippedVertices.empty())
        {
            return {};
        }
    }

    return clippedVertices; // Clipped region of faceA against faceB
}
