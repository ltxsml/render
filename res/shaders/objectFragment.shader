#version 330 core
struct Material{
    sampler2D diffuse; //漫反射
    sampler2D specular;//镜面光照
    sampler2D emission;  //自发光
    float shininess;  //反光度
    
};
//太阳的类型，强度不随距离衰减
struct ParallelLight{
    vec3 ambient;  //环境光照颜色
    vec3 diffuse;  //漫反射光照
    vec3 specular; //镜面反射光照
    vec3 direction; //入射方向
};           
//点光源，普通灯
struct PointLight{
    vec3 postion;   //位置
    //衰减系数
    float constant;  
    float linear;
    float quadratic;
    //光颜色
    vec3 ambient;  //环境光照颜色
    vec3 diffuse;  //漫反射光照
    vec3 specular; //镜面反射光照
};

struct SpotLight{
    vec3 postion;
    vec3 direction;
    vec3 ambient;  //环境光照颜色
    vec3 diffuse;  //漫反射光照
    vec3 specular; //镜面反射光照
    float cutOff;  //聚光半径
    float outerCutoff;  //外半径
    float constant;  
    float linear;
    float quadratic;

};

vec3 caParalleLight(ParallelLight light,vec3 normal,vec3 viewDir);
vec3 caPointLight  (PointLight light, vec3 normal, vec3 viewDir, vec3 fraPos);
vec3 caSpotLight   (SpotLight light, vec3 normal, vec3 viewDir, vec3 fraPos);


out vec4 FragColor;
// uniform float matrixlight;
// uniform float matrixmove;

uniform Material material;
uniform ParallelLight dLight;
uniform PointLight poLight[4];
uniform SpotLight sLight;
uniform vec3 viewPos;
in vec3 Normal;
in vec3 fraPos;  //世界坐标系里的坐标
in vec2 TexCoords;

//lightDir 是从物体指向光源的单位向量

void main()
{  
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-fraPos);
    
    vec3 result = caParalleLight(dLight,norm ,viewDir);

    for(int i = 0;i<4;++i)
    {
        result += caPointLight(poLight[i],norm,viewDir,fraPos);
    }

    // result += caSpotLight (sLight,norm, viewDir, fraPos);
    // float ambientStrength = 0.1;
    // vec3 ambient = material.ambient * light.ambient; //环境光照

    // vec3 emission = texture(material.emission,vec2(ourTexture.x,ourTexture.y+2*matrixmove)).rgb;
    FragColor = vec4(result, 1.0);
}


vec3 caParalleLight(ParallelLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射角
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光反射角
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 caPointLight  (PointLight light, vec3 normal, vec3 viewDir, vec3 fraPos){

    vec3 lightDir = normalize(light.postion - fraPos);
    float diff = max(dot(lightDir,normal),0);
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(reflectDir,viewDir),0),material.shininess);

    float distance = length(light.postion - fraPos);
    float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*(distance*distance));

    vec3 ambient = light.ambient* vec3(texture(material.diffuse, TexCoords)) * attenuation;
    vec3 diffuse = light.diffuse *vec3(texture(material.diffuse,TexCoords)) * attenuation*diff;
    vec3 specular = light.specular * vec3(texture(material.specular,TexCoords)) * attenuation * spec;

    return ambient +diffuse + specular;
}

vec3 caSpotLight   (SpotLight light, vec3 normal, vec3 viewDir, vec3 fraPos)
{
    vec3 lightDir = normalize(-light.direction);  

    float distance = length (light.postion - fraPos);
    float attenuation = 1.0/light.constant+ light.linear*distance +(distance *distance) *quadratic;

    float diff = max(dot(normal,lightDir),0.0);

    vec3 reflectDir = reflect( -light.direction,normal);   //reflect函数要求第一个向量是从光源指向片段位置的向量
    float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);

    //物体到光源的方向
    vec3 dir = normalize(light.postion - fraPos);

    float beta = dot(lightDir,dir);
    float epsilon = light.cutOff - light.outerCutOff ;
    float intensity = clamp((beta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords))*attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords))*attenuation;
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords)) *attenuation;

    return ambient +(diffuse + specular) *intensity;
}
