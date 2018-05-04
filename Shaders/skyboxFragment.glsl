#version 330 core

uniform samplerCube cubeTex;
uniform samplerCube cubeTex2;
uniform float time;


in Vertex {
	vec3 normal;
} IN;

out vec4 color;

void main(void) {
	vec4 morningTex = texture(cubeTex, normalize(IN.normal));
	vec4 nightTex = texture(cubeTex2, normalize(IN.normal));
	color = mix (morningTex, nightTex, time);
}