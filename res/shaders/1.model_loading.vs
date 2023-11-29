#version 330 core
layout ( location = 0)  in vec3 position;
layout ( location = 1)  in vec3 aNormal;
layout ( location = 2)  in vec2 aCoords;
layout ( location = 3)  in vec3 aTangent;
layout ( location = 4)  in vec3 aBitangent;
layout ( location = 5)  in vec4 ids;
layout ( location = 6)  in vec3 aWeights;

out vec2 texCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 faxianjuzhen;
out vec3 Normal;
out vec3 fragPos;

void main()
{   
    gl_Position = projection*view*model*vec4(position,1.0);
    Normal = faxianjuzhen * aNormal;
    fragPos = vec3(model * vec4(position, 1.0));
    texCoords = aCoords;
}