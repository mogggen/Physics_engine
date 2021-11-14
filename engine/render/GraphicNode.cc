#include "config.h"
#include "render/GraphicNode.h"


GraphicNode::GraphicNode(std::shared_ptr<MeshResource> mesh, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderResource> shader, std::shared_ptr<Actor> actor) : Mesh(mesh), Texture(texture), Shader(shader), actor(actor)
{
	
}

void GraphicNode::DrawScene(M4& mvp, V4& rgba)
{
	Texture->BindTexture();

	glUseProgram(this->Shader->program);

	//Set matrix
	glUniformMatrix4fv(glGetUniformLocation(Shader->program, "m4Pos"), 1, GL_TRUE, (float*)&mvp);

	//set colorVector
	glUniform4fv(glGetUniformLocation(Shader->program, "colorVector"), 1, (float*)&rgba);

	Mesh->render();
}

std::shared_ptr<MeshResource> GraphicNode::getMesh() { return Mesh; }
std::shared_ptr<TextureResource> GraphicNode::getTexture() { return Texture; }
std::shared_ptr<ShaderResource> GraphicNode::getShader() { return Shader; }
std::shared_ptr<Actor> GraphicNode::getActor() { return actor; }

void GraphicNode::setMesh(std::shared_ptr<MeshResource> Mesh) { Mesh = Mesh; }
void GraphicNode::setTexture(std::shared_ptr<TextureResource> texture) { Texture = texture; }
void GraphicNode::setShader(std::shared_ptr<ShaderResource> shader) { Shader = shader; }
void GraphicNode::setActor(std::shared_ptr<Actor> actor) { this->actor = actor; }