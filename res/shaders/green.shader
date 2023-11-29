#shader vertex
#version 330 core  

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
     
    void main() 
    { 
       gl_Position = aPos; 
       ourColor = aColor;
       TexCoord = aTexCoord;
    } ;

#shader fragment
#version 330 core  

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

     
    void main() 
    { 
      //  color = vec4(0.0,0.5,0.0,1.0);    //rgba
      FragColor = texture(ourTexture, TexCoord)*vec4(ourColor, 1.0);
    } ;


