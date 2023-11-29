#version 330 core

in vec2 thCoord;
uniform sampler2D th;
out vec4 thColor;

void main()
{
    thColor = vec4(texture(th,thCoord));
}