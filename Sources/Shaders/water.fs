#version 330 core

struct Material {
    //Couleur des reflet sur chaque type de reflets
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //bonus
    //sampler2D emission;

    //Brillance
    float shininess;
};
uniform Material material;

struct Light {
    //Position de la lumiere
    vec3 position;
    vec3 direction;

    //Intensité de la lumière sur chaque type de reflet
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //Attenuation
    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

/*uniform sampler2D texGrass;
uniform sampler2D texRock;
uniform sampler2D texSnow;*/


uniform vec3 viewPos;

//Venant du vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 texPos;


//Couleur de sortie du pixel
out vec4 FragColor;

void main(){


    // ambient
    vec3 ambient = light.ambient * material.ambient;
    // diffuse

    //Ici c'est la normal normalement..
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(-light.direction);
    //vec3 lightDir = normalize(light.position - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;


    //Attenuation
    /*float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));*/

    //ambient  *= attenuation;
    //diffuse  *= attenuation;
    //specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 0.5);
    //FragColor = vec4(Normal, 1.0);

    //FragColor = vec4(1, 1, 1, 1.0);


}