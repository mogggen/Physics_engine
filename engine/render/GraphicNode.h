#pragma once
#include <memory>
#include "render/MeshResource.h"
#include "render/TextureResource.h"
#include "render/ShaderResource.h"
#include "core/Actor.h"


class GraphicNode
{
	std::shared_ptr<MeshResource> Mesh;
	std::shared_ptr<TextureResource> Texture;
	std::shared_ptr<ShaderResource> Shader;
	std::shared_ptr<Actor> actor;
public:
	GraphicNode(std::shared_ptr<MeshResource> geometry, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderResource> shader, std::shared_ptr<Actor> actor);
	void DrawScene(M4& mvp, V4& rbga);

	std::shared_ptr<MeshResource> getMesh();
	std::shared_ptr<TextureResource> getTexture();
	std::shared_ptr<ShaderResource> getShader();
	std::shared_ptr<Actor> getActor();

	void setMesh(std::shared_ptr<MeshResource> Mesh);
	void setTexture(std::shared_ptr<TextureResource> texture);
	void setShader(std::shared_ptr<ShaderResource> shader);
	void setActor(std::shared_ptr<Actor> actor);
};