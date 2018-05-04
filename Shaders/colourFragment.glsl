#version 330 core
 
in Vertex {
	vec4 colour;
} IN;
 
out vec4 color;

void main(void) {
  color = IN.colour;
}