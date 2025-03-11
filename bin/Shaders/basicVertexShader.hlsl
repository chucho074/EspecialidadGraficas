struct VertexInput {
  float3 position : POSITION0;
  float3 color : COLOR0;
};

struct PixelInput {
  float4 position : SV_POSITION;
  float3 color : COLOR0;
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
  return output;
}

float4 pixel_main(PixelInput Input) : SV_Target {
  return float4(Input.color, 1.f);
}