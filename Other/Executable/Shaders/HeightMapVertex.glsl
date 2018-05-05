#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out Vertex {
  vec3 normal;
  vec2 texCoord;
  vec3 position;
} OUT;

void main(void) {
  mat4 mvp = projMatrix * viewMatrix * modelMatrix;
  gl_Position = mvp * vec4(position, 1.0);
  OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
  
  mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
  OUT.normal = normalize(normalMatrix * normalize(normal));
  OUT.position = position;
}