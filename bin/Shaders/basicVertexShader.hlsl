struct VertexInput {
  float3 position : POSITION0;
  float3 color : COLOR0;
};

struct PixelInput {
  float4 position : SV_POSITION;
  float3 color : COLOR0;
};

PixelInput vertex_main(VertexInput Input) {
  PixelInput output = (PixelInput)0;
  output.position = float4(Input.position, 1);
  output.color = Input.color;
  return output;
}

float4 pixel_main(PixelInput Input) : SV_Target {
  return float4(Input.color, 1.f);
}