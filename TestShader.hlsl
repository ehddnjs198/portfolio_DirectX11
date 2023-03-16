struct vertexInput
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    uint4 boneids : BONEIDS0;
    float4 weights : WEIGHTS0;
};
//버텍스인풋에 텍스쳐 번호 주고 콘스탄트버퍼로 텍스쳐갯수 줘야할듯
struct PixelInput
{
    float4 position : SV_POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer BoneBuffer : register(b1)
{
    matrix bones[100];
}
//cbuffer LightBuffer : register(b0)
//{
//    float4 diffuseColor; //16
//    float3 lightDirection; //12
//    float padding;
//};

PixelInput VS(vertexInput Input)
{
    PixelInput pixelout;

	Input.position.w = 1.0f;
    
   matrix BoneTransform = bones[Input.boneids[0]] * Input.weights[0];
   BoneTransform += bones[Input.boneids[1]] * Input.weights[1];
   BoneTransform += bones[Input.boneids[2]] * Input.weights[2];
   BoneTransform += bones[Input.boneids[3]] * Input.weights[3];


    pixelout.position = mul(Input.position, BoneTransform);

    pixelout.position = mul(pixelout.position, world);
    pixelout.position = mul(pixelout.position, view);
    pixelout.position = mul(pixelout.position, proj);
    
   //// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
   pixelout.normal = mul(Input.normal, (float3x3) world);
   //// 법선 벡터를 정규화합니다.
   pixelout.normal = normalize(pixelout.normal);
    //
   pixelout.uv = Input.uv;
    

    return pixelout;
}

Texture2D source_texture : register(t0);
SamplerState samp : register(s0);

float4 PS(PixelInput input) : SV_Target
{
    
    float4 color = source_texture.Sample(samp, input.uv);
	
    return color;
}

float4 PS1(PixelInput input) : SV_Target
{

	float4 color = source_texture.Sample(samp, input.uv);
	
	return float4(0, 1, 1, 1);
}