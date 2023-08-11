//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "imgui.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <chrono>
#ifdef __linux__
#include <unistd.h>
#endif
struct Actor;

using namespace Display;
namespace Example
{
	//------------------------------------------------------------------------------
	/**
	 */
	ExampleApp::ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	 */
	ExampleApp::~ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	 */

	const M4 quaternionToRotationMatrix(const Quaternion& q)
	{
		M4 rotationMatrix;

		float w = q.getW();
		float x = q.getX();
		float y = q.getY();
		float z = q.getZ();

		float ww = w * w;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;
		float xx = x * x;
		float xy = x * y;
		float xz = x * z;
		float yy = y * y;
		float yz = y * z;
		float zz = z * z;

		rotationMatrix[0][0] = 1 - 2 * (yy + zz);
		rotationMatrix[0][1] = 2 * (xy - wz);
		rotationMatrix[0][2] = 2 * (xz + wy);
		rotationMatrix[0][3] = 0.0;

		rotationMatrix[1][0] = 2 * (xy + wz);
		rotationMatrix[1][1] = 1 - 2 * (xx + zz);
		rotationMatrix[1][2] = 2 * (yz - wx);
		rotationMatrix[1][3] = 0.0;

		rotationMatrix[2][0] = 2 * (xz - wy);
		rotationMatrix[2][1] = 2 * (yz + wx);
		rotationMatrix[2][2] = 1 - 2 * (xx + yy);
		rotationMatrix[2][3] = 0.0;

		rotationMatrix[3][0] = 0.0;
		rotationMatrix[3][1] = 0.0;
		rotationMatrix[3][2] = 0.0;
		rotationMatrix[3][3] = 1.0;

		return rotationMatrix;
	}


	const const V3 ray_intersection(
		Ray& r,
		M4 WorldSpaceTransform,
		std::vector<V3>& i_worldSpace_coords,
		std::vector<unsigned>& i_meshModel_indices,
		std::vector<V3>* normals = nullptr)
	{
		V3 closest_point = NAN_V3;
		bool isUndef = true;
		//use the world space coordinates for the entire model
		for (size_t i = 0; i < i_meshModel_indices.size(); i += 3)
		{
			if (i_worldSpace_coords.size() - 1 < i_meshModel_indices[i + 2]) break;
			Plane p = Plane(NAN_V3, NAN_V3);
			//V3 a = i_worldSpace_coords[i_meshModel_indices[i + 0]];
			//V3 b = i_worldSpace_coords[i_meshModel_indices[i + 1]];
			//V3 c = i_worldSpace_coords[i_meshModel_indices[i + 2]];

			V4 a4 = V4(Transpose(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 0]], 1));
			V4 b4 = V4(Transpose(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 1]], 1));
			V4 c4 = V4(Transpose(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 2]], 1));

			V3 a = V3(a4.x, a4.y, a4.z);
			V3 b = V3(b4.x, b4.y, b4.z);
			V3 c = V3(c4.x, c4.y, c4.z);

			//create a plane for each of the faces check for intersections
			if (false && normals != nullptr) // always out of range, probably because the parser is broken
			{
				//if the model has normals use them instead
				p.point = a;
				p.normal = (*normals)[i_meshModel_indices[i]];
			}
			else
			{
				//calculate the normals and use them to create a plane
				p.point = a;
				p.normal = Cross(b - a, c - a);
			}

			// point in triangle with cross product
			float epsilon = 1e-5;
			V3 currentIntersect = r.intersect(p, epsilon);

			// TODO: undefined check
			if (epsilon == 1337) continue;
			isUndef = false;
			
			if (point_in_triangle_3D(currentIntersect, a, b, c))
			{
				if (isUndef)
				{
					V3 curr_dist = currentIntersect - r.origin;
					V3 best_dist = closest_point - r.origin;

					// only update if it is the closest point on the mesh.
					if (curr_dist.Length2() < best_dist.Length2())
					{
						closest_point = currentIntersect;
					}
				}
				else
				{
					closest_point = currentIntersect;
				}
			}
		}

		return closest_point;
	}

	void Print(M4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4 v = m[i];
			std::cout << '(';
			for (size_t i = 0; i < 4; i++)
				std::cout << v.data[i] << (i == 3 ? ")\n" : ", ");
		}
	}

	bool
	ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;

		// assign ExampleApp variables
		w = a = s = d = q = e = false;
		window->GetSize(width, height);

		window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
									{
			//deltatime
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			case GLFW_KEY_W: w = action; break;
			case GLFW_KEY_S: s = action; break;
			case GLFW_KEY_A: a = action; break;
			case GLFW_KEY_D: d = action; break;

			case GLFW_KEY_Q: q = action; break;
			case GLFW_KEY_E: e = action; break;
			} });

		window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
									  { isPressed = button == GLFW_MOUSE_BUTTON_1 && action; });

		window->SetMouseMoveFunction([this](float64 x, float64 y)
									 {
			if (isPressed)
			{
				senseX = prevX + (0.002 * (x - width / 2));
				senseY = prevY + (0.002 * (y - height / 2));
				cubeProjectionViewTransform = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
				fireHydrantProjectionViewTransform = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
			} });

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// MeshResource
			std::vector<unsigned> fireIndices;
			std::vector<V3> fireCoords;
			std::vector<V2> fireTexels;
			std::vector<V3> fireNormals;
			std::vector<Vertex> fireVertices;
			fireHydrantMesh = MeshResource::LoadObj("textures/cube.obj", fireIndices, fireCoords, fireTexels, fireNormals, fireVertices);
			fireHydrantMesh->indicesAmount = fireIndices;
			fireHydrantMesh->positions = fireCoords;
			fireHydrantMesh->texels = fireTexels;
			fireHydrantMesh->normals = fireNormals;
			fireHydrantMesh->vertices = fireVertices;
			// fireHydrantMesh->positions = MeshResource::LoadVerticesFromFile("textures/fireHydrant.obj");
			assert(fireCoords.size() == fireHydrantMesh->positions.size());

			// TextureResource
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			fireHydrantTexture->LoadFromFile();

			// shaderResource
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->LoadShader(fireHydrantScript->vs, fireHydrantScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// Actor
			Actor temp;
			Actor *fireHydrantActor = &temp;

			// GraphicNode
			fireHydrant = std::make_shared<GraphicNode>(fireHydrantMesh, fireHydrantTexture, fireHydrantScript, fireHydrantActor);

			all_loaded.push_back(fireHydrant);

			// MeshResource
			std::vector<unsigned> cubeIndices;
			std::vector<V3> cubeCoords;
			std::vector<V2> cubeTexels;
			std::vector<V3> cubeNormals;
			std::vector<Vertex> cubeVertices;
			cubeMesh = MeshResource::LoadObj("textures/cube.obj", cubeIndices, cubeCoords, cubeTexels, cubeNormals, cubeVertices);
			cubeMesh->indicesAmount = cubeIndices;
			cubeMesh->positions = cubeCoords;
			cubeMesh->texels = cubeTexels;
			cubeMesh->normals = cubeNormals;
			cubeMesh->vertices = cubeVertices;
			// cubeMesh->positions = MeshResource::LoadVerticesFromFile("textures/cube.obj");
			
			cubeTexture = std::make_shared<TextureResource>("textures/red.png");

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/psNoTexture.glsl");
			// note: bindTexture() still requires a texture, but just won't use it

			// Actor
			Actor temp2;
			Actor *cubeActor = &temp2;

			// GraphicNode
			cube = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);

			all_loaded.push_back(cube);

			// MeshResource
			std::vector<unsigned> quadIndices;
			std::vector<V3> quadCoords;
			std::vector<V2> quadTexels;
			std::vector<V3> quadNormals;
			std::vector<Vertex> quadVertices;
			quadMesh = MeshResource::LoadObj("textures/quad.obj", quadIndices, quadCoords, quadTexels, quadNormals, quadVertices);
			quadMesh->indicesAmount = quadIndices;
			quadMesh->positions = quadCoords;
			quadMesh->texels = quadTexels;
			quadMesh->normals = quadNormals;
			quadMesh->vertices = quadVertices;
			// quadMesh->positions = MeshResource::LoadVerticesFromFile("textures/quad.obj");
			
			// Actor
			Actor temp3;
			Actor *quadActor = &temp3;

			// GraphicNode
			quad = std::make_shared<GraphicNode>(quadMesh, cubeTexture, cubeScript, quadActor);

			all_loaded.push_back(quad);

			this->window->SetUiRender([this]()
									  { this->RenderUI(); });
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	 */
	

	V3 mesh_intersection_test(Ray& ray)
	{
		V4 identity[4] = {
			V4(1, 0, 0, 0),
			V4(0, 1, 0, 0),
			V4(0, 0, 1, 0),
			V4(0, 0, 0, 1)
		};

		M4 emptyMatrix = M4(identity);

		//Print(emptyMatrix);

		std::vector<V3> vertexPositions;

		V3 bottomLeft = V3(-1, -1, 5);
		V3 topMiddle = V3(0, 1, 5);
		V3 bottomRight = V3(1, -1, 5);
		
		//V3 bottomLeft = V3();
		//TODO: CONTINUE CODING HERE, ADD ANOTHER FACE TO MAKE SURE DEPTH TEST WORKS
		vertexPositions.push_back(bottomLeft);
		vertexPositions.push_back(topMiddle);
		vertexPositions.push_back(bottomRight);

		std::vector<uint32> indices;

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		std::vector<V3>* normals = nullptr;

		V3 hit = ray_intersection(ray, emptyMatrix, vertexPositions, indices, normals); // resulting hit should be 0, 0, 1
		return hit;
		std::cout << "x: " << hit[0] << ", y: " << hit[1] << ", z: " << hit[2] << std::endl;
	}

	void
	ExampleApp::Run()
	{
		AABB aabb1 = { V3(12, 63, 52), V3(150, 200, 139) };
		AABB aabb2 = { V3(63, 65, 95), V3(187, 173, 183) };
		AABB aabb3 = { V3(23, 93, 24), V3(139, 161, 160) };
		AABB aabb4 = { V3(53, 16, 70), V3(182, 118, 136)};

		std::vector<AABB> aabbs =
		{
			aabb1,
			aabb2,
			aabb3,
			aabb4
		};

		aabbPlaneSweep(aabbs);
		exit(0);


		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// deltatime
		float dt;
		Ray ray(V3(FLT_MAX, FLT_MAX, FLT_MAX), V3(FLT_MAX, FLT_MAX, FLT_MAX));

		// gravity
		const float g = -9.806e-3f;
#define GRAVITY V3(0, g, 0)

		Camera cam(90, (float)width / height, 0.01f, 1000.0f);
		// cam.setPos(V4(0, 4, 3));
		cam.setRot(V4(0, 1, 0), M_PI);

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .8f;

		// set identies
		fireHydrantWorldSpaceTransform = Translate(V4(0, 0, 15));
		fireHydrantProjectionViewTransform = Translate(V4());
		fireHydrantMesh->findbounds();
		

		cubeWorldSpaceTransform = Translate(V4(0, 0, -5));
		cubeProjectionViewTransform = Translate(V4());

		M4 quadWorldSpaceTransform[100];
		M4 quadProjectionViewTransform[100];
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				quadProjectionViewTransform[i * 10 + j] = Translate(V4());
				quadWorldSpaceTransform[i * 10 + j] = Translate(V4(i * 2, j * 2, 0));
			}			
		}

		plane = new Plane(V3(0, 0, -0), V3(0, 0, 1));
		while (this->window->IsOpen())
		{
			//--------------------ImGui section--------------------

			auto start = std::chrono::high_resolution_clock::now();

			//--------------------math section--------------------
			cam.setPos(cam.getPos() + Normalize(V4((d - a), (q - e), (w - s))) * -camSpeed);
			V3 rayOrigin = cam.getPos() * 1.f;

			// std::cout << "frame " << frameIndex << std::endl;
			
			Debug::DrawBB(*fireHydrant->getMesh(), V4(0, 1, 1, 1), fireHydrantWorldSpaceTransform);
			Debug::DrawAABB(*fireHydrant->getMesh(), V4(1, 0, 0, 1), fireHydrantWorldSpaceTransform);
			
			// effect of gravity
			//fireHydrant->actor->apply_force(GRAVITY, dt);

			//fireHydrant world space
			fireHydrantWorldSpaceTransform = Rotation(V4(0, 0, 1), -0.012f) * Rotation(V4(0, 1, 0), 0.004f) * fireHydrantWorldSpaceTransform
			
			//* Translate(fireHydrant->actor->velocity)
				;

			//fireHydrant view space
			fireHydrantProjectionViewTransform = cam.pv() * fireHydrantWorldSpaceTransform/* * Scalar(V4(.1, .1, .1))*/;

			// cube world space
			cubeWorldSpaceTransform = cubeWorldSpaceTransform
										//* Translate(V4(0, 0, cos(frameIndex / 20.f)))
				;

			// // cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

			/*for (size_t i = 0; i < countLines; i++)
			{
				Debug::DrawLine(start[i], dirSize[i], 1.f, V4(1, 1, 1, 1));
			}*/

			// TODO: turn this into a function
			Debug::DrawBB(*fireHydrant->getMesh(), V4(0, 1, 1, 1), fireHydrantWorldSpaceTransform);
			Debug::DrawAABB(*fireHydrant->getMesh(), V4(1, 0, 0, 1), fireHydrantWorldSpaceTransform);


			if (isPressed)
			{
				glfwGetCursorPos(this->window->GetHandle(), &mouseDirX, &mouseDirY);
				// shot a ray


				V4 normalizedDeviceCoordinates(mouseDirX / width * 2 - 1, 1 - mouseDirY / height * 2, 1, 1);
				V4 mousePickingWorldSpace = Inverse(cam.pv()) * normalizedDeviceCoordinates;
				ray = Ray(rayOrigin, (mousePickingWorldSpace - rayOrigin).toV3());

				//resultingHit=mesh_intersection_test(ray);
				
				if (true)// || isnan(resultingHit.data))
				{
					Debug::DrawLine(V4(resultingHit - V3(0, 3, 0), 1), V4(resultingHit - V3(0, 0, 0), 1), V4(1, 0, 0, 1));
					Debug::DrawSquare(V4(resultingHit, 1));
				}
				resultingHit = ray_intersection(ray, fireHydrantWorldSpaceTransform, fireHydrantMesh->positions, fireHydrantMesh->indicesAmount, &(fireHydrantMesh)->normals);
			}

			cubeWorldSpaceTransform = Translate(V4(resultingHit, 1));

			//TODO: here
			std::vector<std::pair<size_t, size_t>> in = aabbPlaneSweep(aabbs);
			for (size_t i = 0; i < in.size(); i++)
			{
				std::shared_ptr<GraphicNode>& ith = all_loaded[in[i].first];
				std::shared_ptr<GraphicNode>& jth = all_loaded[in[i].second];
				
				std::vector<V3> i_vertices = ith->getMesh()->positions;
				apply_worldspace(i_vertices, ith->actor->get_world_space_transform());

				std::vector<V3> j_vertices = jth->getMesh()->positions;
				apply_worldspace(j_vertices, jth->actor->get_world_space_transform());
				std::vector<V3> simplex_placeholder;
				if (gjk(simplex_placeholder, i_vertices, j_vertices))
				{
					V3 normal;
					float depth;
					std::vector<V3> suppe = epa(normal, depth, simplex_placeholder,
						i_vertices, j_vertices);
					V3 p = get_collision_point_in_model_space(suppe, normal, depth);
					// draw a line along the normal where the collision happened (allegedly)
					Debug::DrawLine(V4(p, 1), V4(p + normal * depth, 1), V4(0, 1, 0, 1));

					// handle COllision responses here
				}
			}

			// TODO create an impulse
			(*fireHydrant->actor).apply_linear_impulse(ray, (fireHydrantProjectionViewTransform * V4(fireHydrantMesh->center_of_mass, 1)).toV3(), resultingHit);
			(*fireHydrant->actor).update(dt);
			for (size_t i = 0; i < 100; i++)
			{
				quadProjectionViewTransform[i] = cam.pv() * quadWorldSpaceTransform[i];
			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			fireHydrantScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(fireHydrantScript, cam.getPos());
			fireHydrant->DrawScene(fireHydrantProjectionViewTransform, fireHydrantColor); // cause

			light.bindLight(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor); // cause

			for (int i = 0; i < 100; i++)
			{
				if (true || plane->pointIsOnPlane(quadWorldSpaceTransform[i].toV3(), .0000001))
				{
					//cube->DrawScene(quadProjectionViewTransform[i], fireHydrantColor);
				}
			}

			// usleep(10000);
			this->window->Update();
			frameIndex++;

			if (showDebugRender)
			{
				Debug::Render(cam.pv());
			}
			this->window->SwapBuffers();
			
			auto stop = std::chrono::high_resolution_clock::now();
			using ms = std::chrono::duration<float, std::milli>;
			dt = std::chrono::duration_cast<ms>(stop - start).count();
#ifdef __linux__
			duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
#endif
		}
	}

	void ExampleApp::RenderUI()
	{
		bool show = true;
		ImGui::Begin("Mega Cringe", &show, ImGuiWindowFlags_NoSavedSettings);
		float cube[3];
		for (int i = 0; i < 3; i++)
		{
			cube[i] = cubeWorldSpaceTransform[i][3];
		}
		ImGui::Text("cube: %.3f\t%.3f\t%.3f", cube[0], cube[1], cube[2]);

		ImGui::Checkbox("Debug Mode: ", &showDebugRender);
		ImGui::SliderFloat("x", &x, -5.f, 5.f);
		ImGui::SliderFloat("y", &y, -5.f, 5.f);
		ImGui::SliderFloat("z", &z, -5.f, 5.f);

		// plane->normal = V3(x, y, z);
		ImGui::Text("resultingHit: %.3f\t%.3f\t%.3f", resultingHit.data[0], resultingHit.data[1], resultingHit.data[2]);

		ImGui::Text("frames: %d %.0f", frameIndex, 1e6f / float(duration));

		ImGui::End();
	}

} // namespace Example
