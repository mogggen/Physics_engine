
namespace Example
{
	class Vertex
	{
		vec3 Pos;
		vec4 Rgba;
		vec2 Texel;
		vec3 Normal;

		Vertex()
		{
			Pos = new vec3();
			Rgba = new vec4(new vec3(), 1);
			Texel = new vec2();
			Normal = new vec3();
		}

		Vertex(vec3 pos, vec3 rgb)
		{
			Pos = pos;
			Rgba = new vec4(rgb, 1);
			Texel = new vec2();
			Normal = new vec3();

		}

		Vertex(vec3 pos, vec4 rgba, vec2 texel = new vec2(), vec3 normal = new vec3())
		{
			Pos = pos;
			Rgba = rgba;
			Texel = texel;
			Normal = normal;

		}
	}
}