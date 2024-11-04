#version 430 core

out vec4 FragColor;

in vec2 texcoords;

// We use a 3D texture in 2D rendering so that we can
// use the same OpenCL kernels for 2D and 3D
layout (location = 0) uniform sampler3D mainTexture;

void main() {
  FragColor = vec4(texture(mainTexture, vec3(texcoords, 0)).rgb, 1.0);
}