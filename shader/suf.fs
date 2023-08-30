#version 330 core
out vec4 FragColor;  
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

void main()
{
    float ka=0.2,kd=0.7,ks=0.5;
    vec3 lightPos = vec3(-1.0,5.0,-1.0);
    vec3 lightColor=vec3(1.0,1.0,1.0);
    vec3 norm=normalize(Normal);
    vec3 waterColor=vec3(0.0,0.0,1.0);
    vec3 specColor=vec3(0.5,0.5,1.0);
    vec3 ambient = ka*vec3(1.0,1.0,1.0)*waterColor;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff=max(dot(norm,lightDir),0.0);
    vec3 diffuse = kd*diff*lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfV=normalize(viewDir+lightDir);

    float spec = pow(max(dot(norm, halfV), 0.0), 32);
    vec3 specular = ks * specColor * lightColor;

    FragColor = vec4(ambient+diffuse+specular, 1.0);
}