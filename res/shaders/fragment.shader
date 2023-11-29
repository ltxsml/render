#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float kejiandu;

void main()
{
    FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), kejiandu);
    
}