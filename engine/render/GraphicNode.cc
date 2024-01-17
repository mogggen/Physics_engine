#include "config.h"
#include "render/GraphicNode.h"


Object3D::Object3D(){};

Object3D::Object3D(
	std::shared_ptr<MeshResource> mesh,
	std::shared_ptr<TextureResource> texture,
	std::shared_ptr<ShaderResource> shader,
	std::shared_ptr<RigidBody> _actor) :
	Mesh(mesh),
	Texture(texture),
	Shader(shader),
	actor(_actor)
{
	
}

void Object3D::DrawScene(const M4& mvp, const V4& rgba)
{
	Texture->BindTexture();

	glUseProgram(this->Shader->program);

	//Set matrix
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "m4Pos"), 1, GL_TRUE, (float*)&mvp);

	//set colorVector
	glUniform4fv(glGetUniformLocation(Shader->program, "colorVector"), 1, (float*)&rgba);

	Mesh->render();
}