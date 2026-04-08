#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 Color;

uniform sampler2D sprite;

void main() {
    Color = texture(sprite, TexCoords) * ParticleColor;
}