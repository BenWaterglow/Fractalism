#version 430 core

layout (location = 0) in vec2 aPos;

out vec2 texcoords;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    texcoords = 0.5 * aPos + vec2(0.5);
}