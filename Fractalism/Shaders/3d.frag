#version 430 core

out vec4 FragColor;

struct Material {
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (location = 0) uniform sampler3D volume;
layout (location = 2) uniform Material material;
layout (location = 4) uniform Light light;
layout (location = 8) uniform mat3 normalMatrix;
layout (location = 9) uniform vec3 eyePosition;

in vec3 worldspacePosition;

#define FLT_EPSILON      1.192092896e-07F
#define FLT_MIN_EXP      (-125)

const float samplesPerUnit = length(textureSize(volume, 0));
const vec3 cMax = vec3(0.5);
const vec3 cMin = -cMax;
const bool withinVolume = all(lessThan(abs(eyePosition), vec3(0.5)));

void main() {
  vec3 start, end;
  // for detailed explanation of this algorithm, see /RayMarching.md
  if (withinVolume) {
    start = clamp(eyePosition - cMin, 0.0, 1.0);
    end = clamp(worldspacePosition - cMin, 0.0, 1.0);
  } else {
    vec3 direction = worldspacePosition - eyePosition;
    vec3 lambdaMax = ((cMax - eyePosition) / direction);
    vec3 lambdaMin = ((cMin - eyePosition) / direction);

    ivec3 exp;
    frexp(lambdaMax, exp);
    vec3 epsilonMax = 4.0 * ldexp(vec3(FLT_EPSILON), max(exp - 1, ivec3(FLT_MIN_EXP)));
    frexp(lambdaMin, exp);
    vec3 epsilonMin = 4.0 * ldexp(vec3(FLT_EPSILON), max(exp - 1, ivec3(FLT_MIN_EXP)));

    lambdaMax = abs(lambdaMax / step(epsilonMax, lambdaMax - 1.0));
    lambdaMin = abs(lambdaMin / step(epsilonMin, lambdaMin - 1.0));

    float gamma = min(lambdaMax.x, min(lambdaMax.y, min(lambdaMax.z,
      min(lambdaMin.x, min(lambdaMin.y, lambdaMin.z)))));

    start = clamp(worldspacePosition - cMin, 0.0, 1.0);
    end = clamp((eyePosition - cMin) + (gamma * direction), 0.0, 1.0);
  }
  float stepSize = 1.0 / (distance(start, end) * samplesPerUnit);
  vec4 totalColor = vec4(0.0);

  float t = 0.0;
  // Ray march until reaching the end of the volume, or color saturation
  while (t < 1.0 && totalColor.a < 1.0) {
    vec3 position = mix(start, end, t);

    vec4 currentColor = texture(volume, position);
        
    float cos_theta;
    // Calculate cos(theta) between the light and the surface normal
    {
#define get_alpha(x, y, z) textureOffset(volume, position, ivec3(x, y, z)).a
      vec3 normal = vec3(
        (get_alpha(-1, 0, 0) - get_alpha(1, 0, 0)),
        (get_alpha(0, -1, 0) - get_alpha(0, 1, 0)),
        (get_alpha(0, 0, -1) - get_alpha(0, 0, 1)));
      cos_theta = max(
        dot(
          normalize(normalMatrix * normal),
          normalize(light.direction - position)),
        0.0);
    }

    vec3 ambient = light.ambient * currentColor.rgb;
    vec3 diffuse = light.diffuse * cos_theta * currentColor.rgb;
    vec3 specular = light.specular * pow(cos_theta, material.shininess) * material.specular;
        
    // Alpha-blending
    float alphaBlend = (1.0f - totalColor.a) * (currentColor.a);
    totalColor.rgb += (diffuse + specular + ambient) * alphaBlend;
    totalColor.a += alphaBlend;
        
    t += stepSize;
  }
    
  FragColor = totalColor;
}


