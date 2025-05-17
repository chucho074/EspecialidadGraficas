Texture2D txColor : register(t0);
Texture2D txNormal : register(t1);
Texture2D txReflect : register(t7);

SamplerState samPoint : register(s0);
SamplerState samLinear : register(s1);
SamplerState samAniso : register(s2);

struct VertexInput {
  float3 position : POSITION0;
  float3 color    : COLOR0;
  float3 normal   : NORMAL0;
  float3 tangent  : TANGENT0;
  float2 texCoord : TEXCOORD0;
};

struct PixelInput {
  float4 position : SV_POSITION;
  float3 color    : TEXCOORD0;
  float2 texCoord : TEXCOORD1;
  float3 normal   : TEXCOORD2;
  float3 posWorld : TEXCOORD3;
  float3 tangent  : TEXCOORD4;
  float3 bitangent  : TEXCOORD5;
};

cbuffer MatrixCollection : register(b0) { //Registro de buffer 0
  float4x4 World;
  float4x4 View;
  float4x4 Projection;
  float3 ViewDir;
  float time;
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

float4 pixel_main(PixelInput Input) : SV_Target {
  
  //float4 normal = txNormal.Sample(samLinear, Input.texCoord); //Esta en espacio de tangentes
  float4 normal = txNormal.Sample(samLinear, Input.texCoord) * 2.f - 1.f;
  
  float3x3 TBN = float3x3(Input.tangent, Input.bitangent, Input.normal); //Es el esapcio de tangentes
  normal.xyz = normalize(mul(normal.xyz, TBN));
  
  //incidencia de la luz
  //Light position
  float3 lightPos = float3(-20.0f, 2.0f, 2.0f);
  
  
  //Directional Light
  //float3 lightDir = normalize(lightPos - Input.position.xyz);
  
  //Point light
  //float3 lightDir = normalize(lightPos - Input.posWorld);
  
  //Spot light
  float3 lightDir = normalize(lightPos - Input.posWorld);
  float spotDir = normalize(float3(1.0, 0, 0)); //Direccion de la luz
  
  float spotAngle = 45.0 * (3.14159265383f / 180.0); //El angulo de apertura de la luz
  
  //con lo de time
  //float spotAngle = (25.0 + (25 * sin(time*2.f))) * (3.14159265383f / 180.0); //El angulo de apertura de la luz
  spotAngle = cos(spotAngle); 

  //Lamber factor
  float DiffuseIncidence = dot(lightDir, normal);
  float IncidenceToLight = dot(-spotDir, lightDir);
  
  //Specular
  //Compute light reflection
  float3 ReflectVector = reflect(lightDir, normal); //Direccion de la luz que va a entrar a la superficie y la normal
  float3 myViewDir = -ViewDir;
  
  float3 H = normalize(lightDir + myViewDir); //H = half
  
  //float SpecularIncidence = max(0.0f, dot(myViewDir, ReflectVector));
  float SpecularIncidence = max(0.0f, dot(myViewDir, H));
  SpecularIncidence = pow(SpecularIncidence, 3); //Aqui iria el valor de especularidad que se le desea dar al material
  
  
  if (IncidenceToLight < spotAngle) {
    DiffuseIncidence = 0.0f;
  }
  
  //Probar que salgan las normales
  //return float4(Input.normal, 1.0f);

  float4 color = txColor.Sample(samLinear, Input.texCoord);
  

  
  color.rgb *= SpecularIncidence;
  color.rgb *= DiffuseIncidence;
  
  
  //Returns the normal 
  //return float4(normal.xyz, 1);
  //return float4(Input.normal.xyz, 1.f);
 
  //return the tangents
  //return float4(Input.tangent, 1.0f);
  
  //Color difuso
  //Light Output = kDiffuse + kSpecular + kAmbient
  //return color;
  return color + SpecularIncidence;
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