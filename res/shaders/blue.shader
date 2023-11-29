#shader vertex
#version 330 core  
     
    layout(location=0) in vec3 position; 
    layout(location=1) in vec4 aColor;
    out vec4 ourColor;
    void main() 
    { 
       gl_Position = vec4(position,1.0f); 
       ourColor = aColor;
    } ;

#shader fragment
    #version 330 core  
     
    layout(location=0) out vec4 color; 
   //  uniform vec4 ourColor;
   in vec4 ourColor;
     
    void main() 
    { 
       color = ourColor;    //rgba
    } ;
