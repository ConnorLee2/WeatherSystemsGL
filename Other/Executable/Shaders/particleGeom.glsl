#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float size;

in Vertex {
	vec4 colour;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
} OUT;

void main() {
	for (int i = 0; i < gl_in.length(); ++i) {
		vec4 pos = gl_in[i].gl_Position;
		OUT.colour = IN[i].colour;
	
		gl_Position = pos + (vec4(0.0, 1.0, 0.0, 0.0) * size);
		OUT.texCoord = vec2(0.0, 1.0);
		EmitVertex();
	
		gl_Position = pos + (vec4(0.0, 0.0, 0.0, 0.0) * size);
		OUT.texCoord = vec2(0.0, 0.0);
		EmitVertex();
	
		gl_Position = pos + (vec4(1.0, 1.0, 0.0, 0.0) * size);
		OUT.texCoord = vec2(1.0, 1.0);
		EmitVertex();
	
		gl_Position = pos + (vec4(1.0, 0.0, 0.0, 0.0) * size);
		OUT.texCoord = vec2(1.0, 0.0);
		EmitVertex();
	
		EndPrimitive();
	}
}