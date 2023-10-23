#include "config.h"
#include "render/GraphicNode.h"


GraphicNode::GraphicNode(
	std::shared_ptr<MeshResource> mesh,
	std::shared_ptr<TextureResource> texture,
	std::shared_ptr<ShaderResource> shader,
	std::shared_ptr<Actor> _actor) :
	Mesh(mesh),
	Texture(texture),
	Shader(shader),
	actor(_actor)
{
	
}

void GraphicNode::DrawScene(const M4& mvp, const V4& rgba)
{
	Texture->BindTexture();

	glUseProgram(this->Shader->program);

	//Set matrix
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "m4Pos"), 1, GL_TRUE, (float*)&mvp);

	//set colorVector
	glUniform4fv(glGetUniformLocation(Shader->program, "colorVector"), 1, (float*)&rgba);

	Mesh->render();
}