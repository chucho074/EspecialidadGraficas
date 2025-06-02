Texture2D txColor   : register(t0);
Texture2D txNormal  : register(t1);
Texture2D txRough   : register(t2);
Texture2D txReflect : register(t7);

SamplerState samPoint  : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso  : register(s2);

struct VertexInput {
  float3 position : POSITION0;
  float3 color    : COLOR0;
  float3 normal   : NORMAL0;
  float3 tangent  : TANGENT0;
  float2 texCoord : TEXCOORD0;
};

struct PixelInput {
  float4 position   : SV_POSITION;
  float3 color      : TEXCOORD0;
  float2 texCoord   : TEXCOORD1;
  float3 normal     : TEXCOORD2;
  float3 posWorld   : TEXCOORD3;
  float3 tangent    : TEXCOORD4;
  float3 bitangent  : TEXCOORD5;
};

cbuffer MatrixCollection : register(b0) { //Registro de buffer 0
  float4x4 World;
  float4x4 View;
  float4x4 Projection;
  float3 ViewDir; //ViewPos de Prince
  float time;
}

#define M_PI 3.14159265358979323846f
#define M_2PI M_PI * 2.0f

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

float3 FresnelSchlick(float F0, float cosTheta, float F90)
{
  return F0 + (F90 - F0) * pow(1.0f - cosTheta, 5.0f);
}

float3 FresnelSchlick(float F0, float cosTheta)
{
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

PixelInput vertex_main(VertexInput Input) {
  PixelInput output = (PixelInput)0;
  output.position = float4(Input.position, 1.0f);
  output.position = mul(output.position, World);
  output.posWorld = output.position.xyz; // Guardar la posición en el mundo
  output.position = mul(output.position, View);
  output.position = mul(output.position, Projection); //Ahora es posición de clip
  
  output.normal = normalize(mul(Input.normal, (float3x3)World).xyz);
  output.tangent = normalize(mul(Input.tangent, (float3x3)World).xyz);
  output.bitangent = -cross(output.normal, output.tangent); //para las normales invertidas
  //output.bitangent = cross(output.normal, output.tangent);
  
  output.color = Input.color;
  output.texCoord = Input.texCoord;

  return output;
}


float3
BRDF_Blinn_Phong(float3 normal, 
                 float3 lightDir,
                 float3 viewDir,
                 float3 reflectDir,
                 float3 diffuseColor,
                 float3 specularColor) {
  //Lambertian Diffuse
  float NdotL = max(dot(normal, lightDir), 0.0f);
  float3 diffuse = diffuseColor * NdotL;
  
  // Blinn-Phong Specular
  float NdotH = max(dot(normal, reflectDir), 0.0f);
  float NdotV = max(dot(normal, viewDir), 0.0f);
  float HdotV = max(dot(reflectDir, viewDir), 0.0f);
  
  float specular = pow(HdotV, 32.0f) * NdotH * NdotV;
  
  return diffuse + specular;
  
}

float3
BRDF_Cook_Torrance(float3 normal, 
                   float3 lightDir,
                   float3 viewDir,
                   float3 reflectDir,
                   float3 diffuseColor,
                   float3 specularColor,
                   float roughness) {
  //Lambertian Diffuse
  float NdotL = max(dot(normal, lightDir), 0.0f);
  float3 diffuse = diffuseColor * NdotL;
  
  //Cook-Torrance Specular
  float NdotV = max(dot(normal, viewDir), 0.0f);
  float NdotH = max(dot(normal, reflectDir), 0.0f);
  
  //specular = D * F * G / (4 * NdotL * NdotV)
  
  //return ndf_BlinnPhong(NdotH, 0.04f);
  float D = ndf_GGX(NdotH, roughness);
  //float3 F = FresnelSchlick(0.4f, NdotV);
  float3 F = FresnelSchlick(roughness, NdotV);
  float G = GeometrySmith(NdotL, NdotV, roughness);
  
  float3 specular = ((D * F * G) / (4.0f * NdotL * NdotV)) * specularColor;

  return diffuse + specular * NdotH * NdotV;
  
}

float4 pixel_main(PixelInput Input) : SV_Target {
  
  float4 diffcolor = txColor.Sample(samLinear, Input.texCoord);
  float4 normal = txNormal.Sample(samLinear, Input.texCoord) * 2.f - 1.f;
  float roughness = txRough.Sample(samLinear, Input.texCoord).r;
  
  float3x3 TBN = float3x3(Input.tangent, Input.bitangent, Input.normal); //Es el esapcio de tangentes
  normal.xyz = normalize(mul(normal.xyz, TBN));
  
  //incidencia de la luz //Light position
  float3 lightPos = float3(0.0f, 0.0f, 0.0f);
  
  //Directional Light
  float3 lightDir = normalize(lightPos - Input.posWorld);
  
  float3 finalColor = BRDF_Cook_Torrance(normal.xyz,
                                    lightDir,
                                    normalize(ViewDir - Input.posWorld),
                                    normalize(reflect(-lightDir, normal.xyz)),
                                    diffcolor.rgb,
                                    float3(1.f, 1.f, 1.f), 
                                    roughness);
  
  //Lo = kD + kS + kA
  return float4(finalColor, 1.0f);

}

float4 pixel_reflect_main(PixelInput Input) : SV_Target {
  float4 color = txColor.Sample(samLinear, Input.texCoord);
  
  float2 screenPos = Input.position.xy;
  float2 texSize;
  txReflect.GetDimensions(texSize.x, texSize.y);
  screenPos = screenPos / texSize;
  
  float4 colorReflect = txReflect.Sample(samLinear, screenPos);
  return saturate(color + (colorReflect * 0.35));
}