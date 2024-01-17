#pragma once
#include <memory>
#include "render/MeshResource.h"
#include "render/TextureResource.h"
#include "render/ShaderResource.h"
#include "render/Actor.h"


class Object3D
{
protected:
	float mass;
	V3 center_of_mass;
	std::shared_ptr<MeshResource> Mesh;
	std::shared_ptr<TextureResource> Texture;
	std::shared_ptr<ShaderResource> Shader;
	std::shared_ptr<RigidBody> actor;
public:
	Object3D();
	Object3D(
		std::shared_ptr<MeshResource> mesh,
		std::shared_ptr<TextureResource> texture,
		std::shared_ptr<ShaderResource> shader,
		std::shared_ptr<RigidBody> actor);
	void DrawScene(const M4& mvp, const V4& rbga);

	std::shared_ptr<MeshResource> getMesh() { return Mesh; }
	std::shared_ptr<TextureResource> getTexture() { return Texture; }
	std::shared_ptr<ShaderResource> getShader() { return Shader; }
	std::shared_ptr<RigidBody> getActor() { return actor; }

	void setMesh(std::shared_ptr<MeshResource> mesh) { Mesh = mesh; }
	void setTexture(std::shared_ptr<TextureResource> texture) { Texture = texture; }
	void setShader(std::shared_ptr<ShaderResource> shader) { Shader = shader; }
	void setActor(std::shared_ptr<RigidBody> _actor) { _actor = actor; }
};