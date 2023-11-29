#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 faxianjuzhen;
out vec3 Normal;
out vec3 fraPos;
out vec2 TexCoords;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = faxianjuzhen * aNormal;
    fraPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexture;
}