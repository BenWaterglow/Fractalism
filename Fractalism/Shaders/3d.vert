#version 430 core

layout (location = 0) in vec3 aPos;

layout (location = 1) uniform mat4 mvp;

out vec3 worldspacePosition;

void main() {
  worldspacePosition = aPos;
  gl_Position = mvp * vec4(aPos, 1.0);
}
