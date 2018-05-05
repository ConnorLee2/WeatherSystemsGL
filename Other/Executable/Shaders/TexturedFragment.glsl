#version 330
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 color;

void main(void){
	color = texture(diffuseTex, IN.texCoord);
}