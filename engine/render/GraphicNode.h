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
public:
	Actor* actor;
	GraphicNode(std::shared_ptr<MeshResource> geometry, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderResource> shader, Actor* actor);
	void DrawScene(M4& mvp, V4& rbga);

	std::shared_ptr<MeshResource> getMesh();
	std::shared_ptr<TextureResource> getTexture();
	std::shared_ptr<ShaderResource> getShader();

	void setMesh(std::shared_ptr<MeshResource> Mesh);
	void setTexture(std::shared_ptr<TextureResource> texture);
	void setShader(std::shared_ptr<ShaderResource> shader);	
};