Texture2D txColor : register(t0);
Texture2D txReflect : register(t1);

SamplerState samPoint : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso : register(s2);

struct VertexInput {
  float3 position : POSITION0;
  float3 color : COLOR0;
  float2 texCoord : TEXCOORD0;
};

struct PixelInput {
  float4 position : SV_POSITION;
  float3 color : COLOR0;
  float2 texCoord : TEXCOORD1;
};

cbuffer MatrixCollection : register(b0) { //Registro de buffer 0
  float4x4 World;
  float4x4 View;
  float4x4 Projection;
}

PixelInput vertex_main(VertexInput Input) {
  PixelInput output = (PixelInput)0;
  output.position = float4(Input.position, 1);
  output.position = mul(output.position, World);
  output.position = mul(output.position, View);
  output.position = mul(output.position, Projection);
  
  output.color = Input.color;
  output.texCoord = Input.texCoord;

  return output;
}

float4 pixel_main(PixelInput Input) : SV_Target {
  float4 color = txColor.Sample(samLinear, Input.texCoord);
  return color;
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