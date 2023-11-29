#pragma once
#include "MathLibrary.h"
#include "Vertex.h"
#include <vector>

namespace Example
{
	struct Face
	{
		std::vector<V3> vertices;
		V3 normal;
	};

	struct MeshResource
	{
		std::vector<V3> positions;

		V3 center_of_mass;
		V3 min, max;
		
		std::pair<V3, V3> find_bounds();
		static void LoadTeaPot(std::vector<Face>&);
	};
}