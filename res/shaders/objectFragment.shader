#version 330 core
struct Material{
    sampler2D diffuse; //������
    sampler2D specular;//�������
    sampler2D emission;  //�Է���
    float shininess;  //�����
    
};
//̫�������ͣ�ǿ�Ȳ������˥��
struct ParallelLight{
    vec3 ambient;  //����������ɫ
    vec3 diffuse;  //���������
    vec3 specular; //���淴�����
    vec3 direction; //���䷽��
};           
//���Դ����ͨ��
struct PointLight{
    vec3 postion;   //λ��
    //˥��ϵ��
    float constant;  
    float linear;
    float quadratic;
    //����ɫ
    vec3 ambient;  //����������ɫ
    vec3 diffuse;  //���������
    vec3 specular; //���淴�����
};

struct SpotLight{
    vec3 postion;
    vec3 direction;
    vec3 ambient;  //����������ɫ
    vec3 diffuse;  //���������
    vec3 specular; //���淴�����
    float cutOff;  //�۹�뾶
    float outerCutoff;  //��뾶
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
in vec3 fraPos;  //��������ϵ�������
in vec2 TexCoords;

//lightDir �Ǵ�����ָ���Դ�ĵ�λ����

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
    // vec3 ambient = material.ambient * light.ambient; //��������

    // vec3 emission = texture(material.emission,vec2(ourTexture.x,ourTexture.y+2*matrixmove)).rgb;
    FragColor = vec4(result, 1.0);
}


vec3 caParalleLight(ParallelLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // �������
    float diff = max(dot(normal, lightDir), 0.0);
    // ����ⷴ���
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
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

    vec3 reflectDir = reflect( -light.direction,normal);   //reflect����Ҫ���һ�������Ǵӹ�Դָ��Ƭ��λ�õ�����
    float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);

    //���嵽��Դ�ķ���
    vec3 dir = normalize(light.postion - fraPos);

    float beta = dot(lightDir,dir);
    float epsilon = light.cutOff - light.outerCutOff ;
    float intensity = clamp((beta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords))*attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords))*attenuation;
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords)) *attenuation;

    return ambient +(diffuse + specular) *intensity;
}
