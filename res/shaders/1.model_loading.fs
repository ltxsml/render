#version 330 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
};

uniform Material material;
uniform vec3 viewPos;
in vec3 Normal;
in vec3 fragPos;  //��������ϵ�������
out vec4 fragColor;
in vec2 texCoords;

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

struct ParallelLight{
    vec3 ambient;  //����������ɫ
    vec3 diffuse;  //���������
    vec3 specular; //���淴�����
    vec3 direction; //���䷽��
};   

vec4 caPointLight  (PointLight light, vec3 normal, vec3 viewDir, vec3 fraPos);
vec4 caParalleLight(ParallelLight light,vec3 normal,vec3 viewDir);



uniform PointLight PLight;
uniform ParallelLight dirLight;

void main()
{       
        vec3 norm = normalize(Normal);
        vec3 viewDir =  normalize(viewPos - fragPos);
        vec4 result = 3 * caPointLight(PLight,norm,viewDir,fragPos);
        result += caParalleLight(dirLight, norm , viewDir);
        fragColor = result;

}

vec4 caPointLight  (PointLight light, vec3 normal, vec3 viewDir, vec3 fraPos){

    vec3 lightDir = normalize(light.postion - fraPos);
    float diff = max(dot(lightDir,normal),0);
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(reflectDir,viewDir),0),32);

    float distance = length(light.postion - fraPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic*(distance*distance));

    vec4 ambient = vec4(light.ambient * attenuation,1.0) ;
    ambient *= texture(material.texture_diffuse1, texCoords).rgba;

    vec4 diffuse = vec4(light.diffuse * attenuation * diff,1.0) ;
    diffuse *= texture(material.texture_diffuse1,texCoords).rgba;

    vec4 specular = vec4(light.specular  * attenuation * spec,1.0) ;
    specular *= texture(material.texture_specular1,texCoords).rgba;

    return ambient+diffuse + specular;
}


vec4 caParalleLight(ParallelLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // �������
    float diff = max(dot(normal, lightDir), 0.0);
    // ����ⷴ���
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // �ϲ����
    vec4 ambient = vec4(light.ambient,1.0) ;
    ambient *= texture(material.texture_diffuse1, texCoords).rgba;

    vec4 diffuse = vec4(light.diffuse * diff,1.0) ;
    diffuse *= texture(material.texture_diffuse1,texCoords).rgba;

    vec4 specular = vec4(light.specular * spec,1.0) ;
    specular *= texture(material.texture_specular1,texCoords).rgba;
    return (ambient + diffuse + specular);
}