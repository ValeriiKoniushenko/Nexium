// Shader: color
#version 460 core

out vec4 FragColor;

in vec3 ioFragPos;
in vec3 ioNormal;
in vec2 ioUV;

uniform float uAmbientStrength = 1.;
uniform float uSpecularStrength = 0.5;
uniform float uMinLightStrength = 0.2;
uniform float uSpecularPow = 32;
uniform vec3 uLightColor = vec3(1, 1, 1);
uniform vec3 uLightPos;
uniform vec3 uViewPos;

uniform sampler2D uTexture;

void main()
{
   // ambient
   vec3 ambient = uAmbientStrength * uLightColor;

   // diffuse
   vec3 norm = normalize(ioNormal);
   vec3 lightDir = normalize(uLightPos - ioFragPos);
   vec3 viewDir = normalize(uViewPos - ioFragPos);
   float diffuse = max(dot(norm, lightDir), uMinLightStrength);

   // specular
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), uSpecularPow);
   float specular = uSpecularStrength * spec;

   vec3 result = ambient * diffuse + specular;

   vec4 texColor = texture(uTexture, ioUV) * vec4(result, 1.0);
   if (texColor.a < 0.1)
   {
      discard;
   }

   FragColor = texColor;
}