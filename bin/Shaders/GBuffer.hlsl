Texture2D txColor   : register(t0);
Texture2D txNormal  : register(t1);
Texture2D txRough   : register(t2);
Texture2D txMetal   : register(t3);
//Texture2D txReflect : register(t4);

SamplerState samPoint  : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso  : register(s2);

SamplerState samPointClamp : register(s3);
SamplerState samLinearClamp : register(s4);
SamplerState samAnisoClamp : register(s5);

SamplerState samPointMirror : register(s6);
SamplerState samLinearMirror : register(s7);
SamplerState samAnisoMirror : register(s8);


struct VertexInput {
  float3 position : POSITION0;
  float3 color    : COLOR0;
  float3 normal   : NORMAL0;
  float3 tangent  : TANGENT0;
  float2 texCoord : TEXCOORD0;
};

struct PixelInput {
  float4 position  : SV_POSITION;
  float3 color     : TEXCOORD0;
  float3 posWorld  : TEXCOORD1;
  float2 texCoord  : TEXCOORD2;
  float3x3 TBN     : TEXCOORD3;
};

struct ShadowPixel  {
  float4 position : SV_Position;
  //float3 posW     : TEXCOORD0;
};

struct GBuffer {
  float4 position : SV_Target0;   // x, y, z, metallic
  float4 normal   : SV_Target1;   // x, y, z, roughness
  float4 color    : SV_Target2;   // r, g, b, a
};

cbuffer MatrixCollection : register(b0) { //Registro de buffer 0
  float4x4 World;
  float4x4 View;
  float4x4 Projection;       
  float4x4 ViewProjection;   
  
  float4x4 lightView;
  float4x4 lightProjection;  //Mandar calculada la matriz de VP
  float4x4 lightViewProjection;
  
  float3   lightPosition;
  float    lightIntensity;
  
  float3 lightColor;
  float  lightRadius;
  
  float3 ViewPos;
  float time;
}

PixelInput gbuffer_vertex_main(VertexInput Input) {
  PixelInput output = (PixelInput) 0;
  
  output.position = float4(Input.position, 1.0f);
  output.position = mul(output.position, World);
  output.posWorld = output.position.xyz; // Guardar la posicion en el mundo
  output.position = mul(output.position, ViewProjection);
  //output.position = mul(output.position, Projection); //Ahora es posicion de clip
  
  float3 normal = normalize(mul(float4(Input.normal, 1.f), World).xyz);
  float3 tangent = normalize(mul(float4(Input.tangent, 1.f), World).xyz);
  float3 bitangent = -cross(normal, tangent); //para las normales invertidas
  //output.bitangent = cross(output.normal, output.tangent);
  
  output.TBN = float3x3(tangent, bitangent, normal); //Es el esapcio de tangentes
  
  output.color = Input.color;
  output.texCoord = Input.texCoord;

  return output;
}

GBuffer gbuffer_pixel_main(PixelInput Input) {
  GBuffer Output = (GBuffer) 0;
  
  float4 diffColor = txColor.Sample(samAniso, Input.texCoord);
  float4 normal   = txNormal.Sample(samAniso, Input.texCoord) * 2.f - 1.f;
  float roughness  = txRough.Sample(samAniso, Input.texCoord).r;
  float metallic   = txMetal.Sample(samAniso, Input.texCoord).r;
  
  normal.xyz = normalize(mul(normal.xyz, Input.TBN));
  
  Output.position = float4(Input.posWorld, metallic);
  //Output.normal = float4(normalize(normal.xyz * 0.5f + 0.5f), roughness);
  Output.normal = float4(normal.xyz * 0.5f + 0.5f, roughness);
  
  Output.color = diffColor;
  
  return Output;
}

ShadowPixel shadow_map_vertex_main(VertexInput Input) {
  ShadowPixel output = (ShadowPixel) 0;
  output.position = mul(float4(Input.position, 1.0f), World);
  output.position = mul(output.position, lightView);
  output.position = mul(output.position, lightProjection);
  return output;
}

void shadow_map_pixel_main(ShadowPixel Input) {
  
  //return float4(0.f, 0.f, 0.f, 1.f);
}