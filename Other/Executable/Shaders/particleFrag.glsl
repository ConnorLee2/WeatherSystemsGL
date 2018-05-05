#version 330 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 color;

void main(void)	{
	color = IN.colour * texture(diffuseTex, IN.texCoord);
}