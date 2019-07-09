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

uniform sampler2D texGrass;
uniform sampler2D texRock;
uniform sampler2D texSnow;


uniform vec3 viewPos;

//Venant du vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 texPos;


//Couleur de sortie du pixel
out vec4 FragColor;

void main(){

    vec3 colorGrass = texture(texGrass, texPos).rgb;

    /*vec3 colorAmbient = material.ambient;
    vec3 colorDiffuse = material.diffuse;
    vec3 colorSpecular = material.specular;*/

    vec3 colorAmbient = vec3(0.95, 0.96, 0.93);
    vec3 colorDiffuse = vec3(0.95, 0.96, 0.93);
    vec3 colorSpecular = vec3(0.95, 0.96, 0.93);

    if(FragPos.y < 15 ){
        colorAmbient = vec3(1, 0.84, 0.51);
        colorDiffuse = colorAmbient;
        colorSpecular = colorAmbient;
    }
    /*else if(FragPos.y <= 50){
        colorAmbient = texture(texGrass, texPos).rgb;
        colorDiffuse = colorAmbient;
        colorSpecular = colorAmbient;
    }
    else{
        colorAmbient = texture(texRock, texPos).rgb;
        colorDiffuse = colorAmbient;
        colorSpecular = colorAmbient;
    }*/



    /*vec3 colorAmbient = colorGrass;
    vec3 colorDiffuse = colorGrass;
    vec3 colorSpecular = colorGrass + 0.1;*/

    /*
    if(FragPos.y > 4.0){

        float ratio = (FragPos.y - 4)/10.0;

        ratio = min(ratio, 1.0);

        colorAmbient = mix(colorAmbient, texture(texRock, texPos).rgb, ratio);
        colorDiffuse = colorAmbient;
        colorSpecular = mix(colorGrass, vec3(0.0, 0.0, 0.0), ratio);
    }
    if(FragPos.y > 20.0){

        float ratio = (FragPos.y - 20.0)/13.0;

        ratio = min(ratio, 1.0);

        colorAmbient = mix(colorAmbient, texture(texSnow, texPos).rgb, ratio);
        colorDiffuse = colorAmbient;
        colorSpecular = colorAmbient;
    }*/

    // ambient
    vec3 ambient = light.ambient * colorAmbient;
    // diffuse

    //Ici c'est la normal normalement..
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(-light.direction);
    //vec3 lightDir = normalize(light.position - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * colorDiffuse;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * colorSpecular;


    //Attenuation
    /*float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));*/

    //ambient  *= attenuation;
    //diffuse  *= attenuation;
    //specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(Normal, 1.0);

    //FragColor = vec4(1, 1, 1, 1.0);


}