#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 aTexture;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 textureCoord;
void main()
{
    gl_Position = projection*view*model*vec4(vertexPosition,1.0);
    textureCoord = aTexture ;
}