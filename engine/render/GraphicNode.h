#pragma once
#include <memory>
#include "render/MeshResource.h"
#include "render/TextureResource.h"
#include "render/ShaderResource.h"
#include "render/Actor.h"


class GraphicNode
{
protected:
	float mass;
	V3 center_of_mass;
	std::shared_ptr<MeshResource> Mesh;
	std::shared_ptr<TextureResource> Texture;
	std::shared_ptr<ShaderResource> Shader;
public:
	std::shared_ptr<Actor> actor;
	GraphicNode(
		std::shared_ptr<MeshResource> mesh,
		std::shared_ptr<TextureResource> texture,
		std::shared_ptr<ShaderResource> shader,
		std::shared_ptr<Actor> actor);
	void DrawScene(const M4& mvp, const V4& rbga);

	std::shared_ptr<MeshResource> getMesh() { return Mesh; }
	std::shared_ptr<TextureResource> getTexture() { return Texture; }
	std::shared_ptr<ShaderResource> getShader() { return Shader; }
	std::shared_ptr<Actor> getActor() { return actor; }

	void setMesh(std::shared_ptr<MeshResource> mesh) { Mesh = mesh; }
	void setTexture(std::shared_ptr<TextureResource> texture) { Texture = texture; }
	void setShader(std::shared_ptr<ShaderResource> shader) { Shader = shader; }
	void setActor(std::shared_ptr<Actor> _actor) { _actor = actor; }
};