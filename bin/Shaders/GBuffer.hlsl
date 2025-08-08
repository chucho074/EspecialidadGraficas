Texture2D txColor   : register(t0);
Texture2D txNormal  : register(t1);
Texture2D txRough   : register(t2);
Texture2D txMetal   : register(t3);
Texture2D txReflect : register(t4);

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
  float4 position  : SV_POSITION;
  float3 color     : TEXCOORD0;
  float2 texCoord  : TEXCOORD1;
  float3 normal    : TEXCOORD2;
  float3 posWorld  : TEXCOORD3;
  float3 tangent   : TEXCOORD4;
  float3 bitangent : TEXCOORD5;
};

struct ShadowPixel  {
  float4 position : SV_Position;
  float3 posW     : TEXCOORD0;
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
  
  float4x4 lightView;
  float4x4 lightProjection;
  
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
  output.posWorld = output.position.xyz; // Guardar la posición en el mundo
  output.position = mul(output.position, View);
  output.position = mul(output.position, Projection); //Ahora es posición de clip
  
  output.normal = normalize(mul(Input.normal, (float3x3) World).xyz);
  output.tangent = normalize(mul(Input.tangent, (float3x3) World).xyz);
  output.bitangent = -cross(output.normal, output.tangent); //para las normales invertidas
  //output.bitangent = cross(output.normal, output.tangent);
  
  output.color = Input.color;
  output.texCoord = Input.texCoord;

  return output;
}

GBuffer gbuffer_pixel_main(PixelInput Input) {
  GBuffer Output = (GBuffer) 0;
  
  float4 diffColor = txColor.Sample(samLinear, Input.texCoord);
  float4 normal = txNormal.Sample(samLinear, Input.texCoord) * 2.f - 1.f;
  //normal.rgb = normalize(normal.rgb);
  float roughness = txRough.Sample(samLinear, Input.texCoord).r;
  float metallic = txMetal.Sample(samLinear, Input.texCoord).r;
  
  float3x3 TBN = float3x3(Input.tangent, Input.bitangent, Input.normal); //Es el esapcio de tangentes
  normal.xyz = normalize(mul(normal.xyz, TBN));
  
  Output.position = float4(Input.posWorld, metallic);
  Output.normal = float4(normalize(normal.xyz * 0.5f + 0.5f), roughness);
  
  
  //Output.position = float4(Input.posWorld, 1.f);
  //Output.normal = float4(normal.xyz * 0.5f + 0.5f, 1.f);
  //Output.normal = float4(Input.normal.xyz * 0.5f + 0.5f, 0.04f);
  Output.color = diffColor;
  
  return Output;
}

ShadowPixel shadow_map_vertex_main(VertexInput Input) {
  ShadowPixel output = (ShadowPixel) 0;
  float4 position = float4(Input.position, 1.0f);
  output.posW = position.xyz;
  position = mul(position, World);
  position = mul(position, lightView);
  position = mul(position, lightProjection);
  output.position = position;
  return output;
}

float4 shadow_map_pixel_main(ShadowPixel Input) : SV_Target0 {
  
  return float4(0.f, 0.f, 0.f, 1.f);
}