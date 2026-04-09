#version 330 core

layout (location = 0) in vec4 vertex; // xy = position, zw = texture coordinate

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform vec4 texCoordsRect; // x, y, width, height in texture atlas

void main() {
    // Calculate actual texture coordinates from atlas rectangle
    TexCoords = texCoordsRect.xy + vec2(vertex.z, vertex.w) * texCoordsRect.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}