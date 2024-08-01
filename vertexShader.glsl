#version 460 core

layout (location = 0) in vec2 aPos;

uniform float offsetX;
uniform float offsetY;

void main() {
    gl_Position = vec4(aPos.x + offsetX, aPos.y + offsetY, 0.0, 1.0);
}