#include "config.h"
#include "render/MeshResource.h"
#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>

namespace Example
{
	std::pair<V3, V3> MeshResource::find_bounds()

	{
		// can't be less than the minimum value of 'undefined', so let's not, mate?
		min[0] = max[0] = positions[0][0];
		min[1] = max[1] = positions[0][1];
		min[2] = max[2] = positions[0][2];

		// I should actually do this for all 6 of them but.. nah

		for (size_t i = 1; i < positions.size(); i++)
		{
			if (positions[i][0] < min[0])
				min[0] = positions[i][0];

			if (positions[i][1] < min[1])
				min[1] = positions[i][1];

			if (positions[i][2] < min[2])
				min[2] = positions[i][2];
			
			if (positions[i][0] > max[0])
				max[0] = positions[i][0];

			if (positions[i][1] > max[1])
				max[1] = positions[i][1];

			if (positions[i][2] > max[2])
				max[2] = positions[i][2];
		}

		return { min, max };
	}

	struct Face
	{
		std::vector<V3> vertices;
		V3 normal;
	};

	
	static void LoadTeaPot(std::vector<Face>& _faces)
	{

		using namespace std;
		ifstream fs("textures/teapot.obj");
		string lineRemainder;

		if (!fs)
		{
			printf("file not found with path \"./%s\"\n", "textures/teapot.obj");
			return;
		}

		vector<V3> coords;
		vector<V3> normals;

		while (getline(fs, lineRemainder))
		{
			size_t pos = 0;
			string token;

			if ((pos = lineRemainder.find(" ")) != string::npos) {
				token = lineRemainder.substr(0, pos);
				lineRemainder.erase(0, pos + 1);
				if (token == "v")
				{
					size_t I = 0;
					V3 nextCoordinate;

					while ((pos = lineRemainder.find(" ")) != string::npos)
					{
						
						token = lineRemainder.substr(0, pos);
						lineRemainder.erase(0, pos + 1);
						nextCoordinate[I++] = stof(token);
					}
					nextCoordinate[I++] = stof(lineRemainder);
					coords.push_back(nextCoordinate);
				}

				else if (token == "f")
				{
					size_t I = 0;
					vector<string> args;

					while ((pos = lineRemainder.find(" ")) != string::npos)
					{					
						token = lineRemainder.substr(0, pos);
						lineRemainder.erase(0, pos + 1);
						args.push_back(token);
					}
					args.push_back(lineRemainder);
					//for (string s : args) cout << s << endl;

					size_t posi = 0;
					string tokenSmall;
					vector<unsigned> argi;


					for (auto arg : args)
					{
						argi.clear();
						while ((posi = arg.find("/")) != string::npos)
						{
							tokenSmall = arg.substr(0, posi);
							arg.erase(0, posi + 1);
							argi.push_back(stoi(tokenSmall));
						}
						argi.push_back(stoi(arg));
						std::vector<V3> temp;
						temp.push_back(coords[(argi[0]) - 1]);
						_faces.push_back({temp, Normalize(Cross(temp[1] - temp[0], temp[2] - temp[0]))});
					}
				}
			}
		}
		
		printf("loadedToBuffer %s\n", "teapot.obj");
	}
}