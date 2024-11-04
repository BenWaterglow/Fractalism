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

layout (location = 2) uniform Material material;
layout (location = 4) uniform Light light;

layout (location = 8) uniform mat3 normalMatrix;
layout (location = 9) uniform vec3 eyePosition;

layout (location = 0) uniform sampler3D volume;

in vec3 worldspacePosition;

#define FLT_EPSILON      1.192092896e-07F
#define FLT_MIN_EXP      (-125)

const vec3 samples = textureSize(volume, 0);  // x == y == z should always be true.
const float samplesPerUnit = 2.0 * samples.x;
const vec3 cMax = vec3(0.5);
const vec3 cMin = -cMax;
const bool withinVolume = all(lessThan(abs(eyePosition), vec3(0.5 + FLT_EPSILON)));

void main() {
  vec3 start, end;
  // for detailed explanation of this algorithm, see /RayMarching.md
  if (withinVolume) {
    start = eyePosition - cMin;
    end = worldspacePosition - cMin;
  } else {
    vec3 d = worldspacePosition - eyePosition;
    vec3 lambdaMax = (cMax - eyePosition) / d;
    vec3 lambdaMin = (cMin - eyePosition) / d;

    ivec3 exp;
    frexp(lambdaMax, exp);
    vec3 epsilonMax = 3.0 * ldexp(vec3(FLT_EPSILON), max(exp - 1, ivec3(FLT_MIN_EXP)));
    frexp(lambdaMin, exp);
    vec3 epsilonMin = 3.0 * ldexp(vec3(FLT_EPSILON), max(exp - 1, ivec3(FLT_MIN_EXP)));

    lambdaMax = abs(lambdaMax / step(epsilonMax, lambdaMax - 1.0));
    lambdaMin = abs(lambdaMin / step(epsilonMin, lambdaMin - 1.0));

    float gamma = min(lambdaMax.x, min(lambdaMax.y, min(lambdaMax.z,
      min(lambdaMin.x, min(lambdaMin.y, lambdaMin.z)))));

    start = worldspacePosition - cMin;
    end = (eyePosition - cMin) + (gamma * d);
  }

  float stepSize = 1.0 / (length(end - start) * samplesPerUnit);
  vec3 dStartdx = dFdx(start), dStartdy = dFdy(start);
  vec3 dEnddx = dFdx(end), dEnddy = dFdy(end);
  vec4 totalColor = vec4(0.0);

  float t = 0.0;

  // Ray march until reaching the end of the volume, or color saturation
  while(t < 1.0 && totalColor.a < 1.0) {
    vec3 position = mix(start, end, t);
    vec3 dPositiondx = mix(dStartdx, dEnddx, t);
    vec3 dPositiondy = mix(dStartdy, dEnddy, t);

    vec4 currentColor = textureGrad(volume, position, dPositiondx, dPositiondy);
        
    float cos_theta;
    // Calculate cos(theta) between the light and the surface normal
    { 
#define get_alpha(x, y, z) textureOffset(volume, position, ivec3(x, y, z)).a
      vec3 normal = normalize(vec3(
        (get_alpha(-1, 0, 0) - get_alpha(1, 0, 0)),
        (get_alpha(0, -1, 0) - get_alpha(0, 1, 0)),
        (get_alpha(0, 0, -1) - get_alpha(0, 0, 1))));
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


