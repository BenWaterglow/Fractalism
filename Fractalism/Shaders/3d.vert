#version 430 core

layout (location = 1) uniform mat4 mvp;

layout (location = 0) in vec3 vertexPosition;
out vec3 worldspacePosition;

void main() {
  worldspacePosition = vertexPosition;
  gl_Position = mvp * vec4(vertexPosition, 1.0);
}
