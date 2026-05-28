#version 330 core

//out color in screen
out vec4 FragColor;

//from Vertex_shader 
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

//my texture picture

uniform sampler2D wallTexture;  //sampler 0: wall
uniform sampler2D floorTexture;  //sampler 0: floor
uniform sampler2D doorTexture; //sampler 0: door

bool isWindow = (FragPos.y > 0.01 && FragPos.y < 2.79);

void main()
{

//texture
  
    //Sample the color of the current pixel from the picture
    
    vec4 texColor;

    //height judement

    vec3 currentMaterialColor;
    
    if (TexCoords.x >= 10.0){

        texColor = texture(doorTexture, TexCoords - vec2(10.0, 0.0)); // - offset to revert UV coordinate
    } else if (TexCoords.x < 0.0){
        
        //the color for windows
        texColor = vec4(0.6, 0.8, 0.9, 0.2);
    } else if (FragPos.y < 0.0) {
  
        texColor = texture(floorTexture, TexCoords); 
    } else {

        texColor = texture(wallTexture, TexCoords);  
    }

    //get basic color

    vec3 baseColor = texColor.rgb;

//Lighting model
    //1. Ambient

    float ambientStrength = 0.2;

    vec3 ambient = ambientStrength * lightColor;

    //Screen Space Derivatives 
    //Geometric Normal Reconstruction
    //Flat Shading

    //dFdx: partial derivative of F with respect to x 

    vec3 xTangent = dFdx(FragPos);

    //dFdy: partial derivative of F with respect to y
    
    vec3 yTangent = dFdy(FragPos);

    //cross product

    vec3 realNormal = normalize(cross(xTangent, yTangent));

    //2. Diffuse

    vec3 norm = realNormal;
    vec3 lightDir = normalize(lightPos - FragPos);

    //dot, if value < 0, pass
    float diff = max(dot(norm, lightDir), 0.0);

    //diffuse = Coefficients * lighting colors
    vec3 diffuse = diff * lightColor;

    //4.Specular

    float specularStrength = 0.5;
    
    //view
    vec3 viewDir = normalize(viewPos - FragPos);
    
    //the path of reflect

    vec3 reflectDir = reflect(-lightDir, norm);

    //V and R Alignment degree
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    //4. mix
    vec3 result = (ambient + diffuse + specular) * baseColor;

    //vec4 (red, green, black, alpha)
    FragColor = vec4(result, texColor.a);

}


