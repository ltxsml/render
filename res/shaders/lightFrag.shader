#version 330 core

uniform vec3 lightColor;
out vec4 fragColor;
in vec2 textureCoord;
uniform sampler2D ourTexture;
uniform float matrixmove;

void main()
{
    fragColor = vec4(lightColor * texture(ourTexture,vec2(textureCoord.x,textureCoord.y+2*matrixmove)).rgb,1.0);
}
