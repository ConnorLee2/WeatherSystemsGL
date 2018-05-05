#version 330
uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;
uniform float blend;

in Vertex {
	vec3 normal;
	vec2 texCoord;
	vec3 position;
} IN;

out vec4 color;

void main(void){
	float slope = 1.0f - IN.normal.y;
	float blendAmount;
	vec4 terrain = texture (diffuseTex, IN.texCoord);
	vec4 two = texture (diffuseTex2, IN.texCoord);
	
	if (slope < 0.22) {
		blendAmount = slope / 0.22f;
		vec3 texColor = mix (two.rgb, terrain.rgb, blendAmount);
		color.rgb = mix(terrain.rgb, texColor, blend);
	}
	else {
		color.rgb = terrain.rgb;
	}
}