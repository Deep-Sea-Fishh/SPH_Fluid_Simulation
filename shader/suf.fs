#version 330 core
out vec4 FragColor;  
in vec3 Normal;

void main()
{
    vec3 norm=normalize(Normal);
    vec3 color=vec3(0.0,0.0,1.0);
    vec3 ambient = vec3(0.0,0.0,0.2);
    vec3 lightDir=vec3(1.0,-1.0,-1.0);
    float diff=max(dot(norm,lightDir),0.0);
    vec3 diffuse = diff*color;
    FragColor = vec4(diffuse+ambient, 1.0);
}