#define M_PI 3.14159265358979323846f
#define M_2PI M_PI * 2.0f
#define GAMMA 2.2f // 1.8 - 2.4

Texture2D gbuffer_Position : register(t0);
Texture2D gbuffer_Normal : register(t1);
Texture2D gbuffer_Color : register(t2);
Texture2D gbuffer_AO : register(t3);
Texture2D shadowMap : register(t4);

SamplerState samPoint : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso : register(s2);

SamplerState samPointClamp : register(s3);
SamplerState samLinearClamp : register(s4);
SamplerState samAnisoClamp : register(s5);

SamplerState samPointMirror : register(s6);
SamplerState samLinearMirror : register(s7);
SamplerState samAnisoMirror : register(s8);

struct PixelInput {
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD0;
};

cbuffer MatrixCollection : register(b0) { //Registro de buffer 0
  float4x4 World;
  float4x4 View;
  float4x4 Projection;
  
  float4x4 lightView;
  float4x4 lightProjection;
  
  float3 lightPosition;
  float  lightIntensity;
  
  float3 lightColor;
  float  lightRadius;
  
  float3 ViewPos;
  float time;
}

PixelInput vertex_main(uint vertexId : SV_VertexID) {
  PixelInput output = (PixelInput) 0;
  
  float2 positions[3] = {
    float2(-1.0f, -1.0f),
    float2(3.0f, -1.0f),
    float2(-1.0f, 3.0f)
  };
  
  output.position = float4(positions[vertexId], 0.f, 1.f);
  output.texCoord = (positions[vertexId] + float2(1.f, 1.f)) * 0.5f;
  output.texCoord.y = 1.f - output.texCoord.y;

  return output;
}


//Normal Distribution functions
float ndf_BlinnPhong(float NdotH, float alpha) {
  float a2 = alpha * alpha;
  float n = (2.0f / a2) - 1.0f;
  return (n + 1.0f) / M_2PI * pow(NdotH, n);

}

float ndf_Beckmann(float NdotH, float alpha) {
  float a2 = alpha * alpha;
  float cosThetaHSqr = NdotH * NdotH;
  
  float tanThetaHSqr = (1.0f - cosThetaHSqr) / cosThetaHSqr;
  float e = exp(-tanThetaHSqr / a2);
  
  return e / M_PI * a2 * cosThetaHSqr * NdotH;
}

float ndf_OrenNayar(float NdotL, float NdotV, float alpha) {
  float alpha2 = alpha * alpha;
  float NdotL2 = NdotL * NdotL;
  float NdotV2 = NdotV * NdotV;
  
  float a = max(NdotL, NdotV);
  float b = min(NdotL, NdotV);
  
  return (1.0f / M_PI) * (1.0f - (a * b) / (alpha2 + 0.33f)) * (1.0f + (b / a));
}

float ndf_GGX(float NdotH, float alpha) {
  float a2 = alpha * alpha;
  float cosThetaHSqr = NdotH * NdotH;
  
  float tanThetaHSqr = (1.0f - cosThetaHSqr) / cosThetaHSqr;
  float e = exp(-tanThetaHSqr / a2);
  
  return e / M_PI * a2 * cosThetaHSqr * NdotH;
}

float3 FresnelSchlick(float3 F0, float cosTheta, float F90) {
  return F0 + (F90 - F0) * pow(1.0f - cosTheta, 5.0f);
}

float3 FresnelSchlick(float3 F0, float cosTheta) {
  return F0 + (1.f - F0) * pow(1.0f - cosTheta, 5.0f);
}

//Geometric Distribution function Smith Schlick
float GeometrySchlickGGX(float NdotX, float alpha) {
  float k = alpha / 2.0f;
  return NdotX / (NdotX * (1.0f - k) + k);
}

float GeometrySmith(float NdotL, float NdotV, float alpha) {
  return GeometrySchlickGGX(NdotL, alpha)
         * GeometrySchlickGGX(NdotV, alpha);
}

float3 LambertianDiffuse(float3 normal, float3 lightDir, float3 diffuseColor) {
  //Lambertian Diffuse
  float NdotL = max(dot(normal, lightDir), 0.0f);
  //float NdotL = dot(normal, lightDir);

  
  return diffuseColor * NdotL;
  return NdotL;
  return normal;
}

float3
BRDF_Blinn_Phong(float3 normal,
                 float3 lightDir,
                 float3 viewDir,
                 float3 reflectDir,
                 float3 diffuseColor,
                 float3 specularColor) {
  float3 diffuse = LambertianDiffuse(normal, lightDir, diffuseColor);
  
  // Blinn-Phong Specular
  float NdotH = max(dot(normal, reflectDir), 0.0f);
  float NdotV = max(dot(normal, viewDir), 0.0f);
  float HdotV = max(dot(reflectDir, viewDir), 0.0f);
  
  float specular = pow(HdotV, 32.0f); // El 32 es el specular power
  
  return diffuse;
  return diffuse + specular * specularColor;
}

float3
BRDF_Cook_Torrance(float3 normal,
                   float3 lightDir,
                   float3 viewDir,
                   float3 reflectDir,
                   float3 diffuseColor,
                   float3 specularColor,
                   float roughness) {
  
  float3 diffuse = LambertianDiffuse(normal, lightDir, diffuseColor);
  
  //Lambertian Diffuse
  float NdotL = max(dot(normal, lightDir), 0.0f);
  
  //Cook-Torrance Specular
  //float NdotV = max(dot(normal, viewDir), 0.0f);
  //float NdotH = max(dot(normal, reflectDir), 0.0f);
  
  float NdotV = dot(normal, viewDir);
  float NdotH = dot(normal, reflectDir);
  
  //specular = D * F * G / (4 * NdotL * NdotV)
  float D = ndf_GGX(NdotH, roughness);
  float3 F = FresnelSchlick(specularColor, NdotV);
  float G = GeometrySmith(NdotL, NdotV, roughness);
  
  //float3 specular = (D * F * G) / (4.0f * NdotL * NdotV);
  float3 specular = (D * F * G) / min(0.00001, (NdotL * NdotV * 4.0f));
  //float3 specular = min(0.00001, (D * F * G) / (NdotL * NdotV * 4.0f));

  return diffuse + specular;
  
}

float4 GetPosition(float2 uv) {
  return gbuffer_Position.Sample(samPoint, uv);
}

float4 GetNormal(float2 uv) {
  float4 normal = gbuffer_Normal.Sample(samPoint, uv);
  normal.xyz = normal.xyz * 2.f - 1.f;
  return normal;
}

float2 GetRandom(float2 uv) {
  float noiseX = frac(sin(dot(uv.xy, float2(15.8989f, 76.132f) * 1.f)) * 43758.5453f);
  float noiseY = frac(sin(dot(uv.xy, float2(11.9898f, 62.233f) * 2.f)) * 34748.5453f);
  float noiseZ = frac(sin(dot(uv.xy, float2(13.3232f, 63.123f) * 3.f)) * 59998.5736f);
  
  return normalize(float3(noiseX, noiseY, noiseZ));
}

float4
pixel_main(PixelInput input) : SV_Target {
  //Position
  float4 gBuffer0 = GetPosition(input.texCoord);
  float3 position = gBuffer0.xyz;
  float  metallic = gBuffer0.a;
  //Normals
  float4 gBuffer1 = GetNormal(input.texCoord);
  float3 normal = gBuffer1.xyz;
  float  roughness = gBuffer1.a;
  //Albedo
  float4 gBuffer2 = gbuffer_Color.Sample(samPoint, input.texCoord);
  float3 color = gBuffer2.rgb;
  float  stencil = gBuffer2.a;
  
  clip(stencil < 1.f ? -1 : 1); // Stencil
  
  //AO
  //float4 ao = gbuffer_AO.Sample(samPoint, input.texCoord);
  //normal = normal * 0.5f + 0.5f;
  
  //Light position
  //float3 lightPos = float3(65, 35, 5000);
  
  //Shadows
  float4 lightVP = mul(float4(position, 1.f), lightView);
  lightVP = mul(lightVP, lightProjection);
  lightVP /= lightVP.w;
  lightVP.xy = lightVP.xy * 0.5f + 0.5f; //Convert to NDC
  
  lightVP.y = 1.f - lightVP.y;
  /////////////////////////////////////////////////////////////////////////////
  
  //Rotate light position by time
  //float cosTime = cos(time);
  //float sinTime = sin(time);

  //float3x3 rotationMatrix = float3x3(cosTime, 0.f, sinTime,
  //                                   0.f, 1.f, 0.f,
  //                                   -sinTime, 0.f, cosTime);

  //float3 rotatedLightPos = mul(lightPos, rotationMatrix);
  float3 rotatedLightPos = lightPosition;
  //float3 rotatedLightPos = float3(-600, 5, 0);
  
  //Directional Light
  float3 lightDir = rotatedLightPos - position;
  
  //Testing
  //Sacar la magnitud
  float distance = length(lightDir); 
  lightDir /= distance;
  
  //float attenuation = saturate(1.0 - distance / lightRadius);
  //float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
  
  float d = distance / lightRadius; // normalizar por radio
  float attenuation = 1.0f;
  //float attenuation = 1.0 / (1.0 + 0.1 * d + 0.01 * d * d);
  //attenuation *= saturate(1.0 - d); // corta fuera del radio
  
  float3 specularColor = lerp(0.04f, color, metallic);
  
  float3 viewDir = normalize(ViewPos - position);
  float3 halfVec = normalize(lightDir + viewDir);
  float3 reflectDir = reflect(-lightDir, normal);
  
  float3 colorFinal = BRDF_Cook_Torrance(normal,
                                         lightDir,
                                         viewDir,
                                         reflectDir,
                                         color,
                                         specularColor,
                                         roughness);
  
  /*
  float3 colorFinal = BRDF_Blinn_Phong(normal,
                                       lightDir,
                                       viewDir,
                                       halfVec,
                                       //normalize(reflect(-lightDir, normal.xyz)),
                                       color,
                                       specularColor);
  */
  
  //float4 shadowSample = shadowMap.Sample(samPoint, lightVP.xy);
  //float shadowDepth = shadowSample.x;
  //float lightDepth = lightVP.z - 0.005; //BIAS HERE
  //float shadowFactor = 0.f;
  
  //if (lightDepth > shadowDepth) {
  //  shadowFactor = 0.f;
  //}
  //else {
  //  shadowFactor = 1.f;
  //}
  
  //if (lightVP.x < 0.01f || lightVP.x > 0.99f ||
  //    lightVP.y < 0.01f || lightVP.y > 0.99f) {
  //  shadowFactor = 1.f;
  //}
  
  //colorFinal *= lightColor * lightIntensity * attenuation;
  
  //return float4(color, 1.f);
  
  //return float4(colorFinal, 1.f);
  //return float4(pow(colorFinal, 1.f / GAMMA), 1.f);
  return float4(pow(colorFinal, 1.f / GAMMA), 1.f);
  //return float4(pow(colorFinal * ao.xyz, 1.f / GAMMA), 1.f);
}

float
DoAmbienOcclussion(in float2 tcoord, in float2 uv, in float3 p, in float3 ncoord) {
  float AOScale = 1.0f;
  float AOIntensity = 2.0f;

  float3 diff = GetPosition(tcoord + uv).xyz - p;
  float3 v = normalize(diff);
  float d = length(diff) * AOScale;
  return max(0.0f, dot(ncoord, v)) * (1.f / (1.f + d)) * AOIntensity;
}

float4 ao_main(PixelInput input) : SV_Target {
  return float4(1.f, 1.f, 1.f, 1.f);
}