#version 430 core

out vec4 FragColor;

in vec3 ioFragPos;
in vec3 ioNormal;
in vec2 ioUV;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform vec3 uObjectColor;

uniform sampler2D uTexture;

void main()
{
   // ambient
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * uLightColor;

   // diffuse
   vec3 norm = normalize(ioNormal);
   vec3 lightDir = normalize(uLightPos - ioFragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * uLightColor;

   // specular
   float specularStrength = 0.5;
   vec3 viewDir = normalize(uViewPos - ioFragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
   vec3 specular = specularStrength * spec * uLightColor;

   vec3 result = (ambient + diffuse + specular) * uObjectColor;

   vec4 texColor = texture(uTexture, ioUV) * vec4(result, 1.0);
   if (texColor.a < 0.1)
   {
      discard;
   }

   FragColor = texColor;
}