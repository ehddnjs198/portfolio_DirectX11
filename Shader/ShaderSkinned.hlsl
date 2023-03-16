struct vertexInput
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    uint4 boneids : BONEIDS0;
    float4 weights : WEIGHTS0;
};

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
    //matrix bones[100];
};

cbuffer BoneTransformBuffer : register(b1)
{
	matrix bones[100];
};

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor; //16
    float3 lightDirection; //12
    float padding;
};

PixelInput VS(vertexInput Input)
{
    PixelInput pixelout;

	Input.position.w = 1.0f;

    matrix BoneTransform = bones[Input.boneids[0]] * Input.weights[0];
    BoneTransform += bones[Input.boneids[1]] * Input.weights[1];
    BoneTransform += bones[Input.boneids[2]] * Input.weights[2];
    BoneTransform += bones[Input.boneids[3]] * Input.weights[3];

    //float4 PosL = mul(float4(Input.position.x, Input.position.y, Input.position.z, 1.0), BoneTransform);
    //pixelout.position = PosL; //mul(PosL, world * view * proj);

    pixelout.position = mul(Input.position, BoneTransform);
    
    pixelout.position = mul(pixelout.position, world);
    pixelout.position = mul(pixelout.position, view);
    pixelout.position = mul(pixelout.position, proj);

    //pixelout.position = mul(pixelout.position, BoneTransform); //mul(PosL, world * view * proj);
    //pixelout.position = float4(2, 2, 0, 1);

    //pixelout.position = Input.position;
    //
    //pixelout.position = mul(Input.position, world);
    //pixelout.position = mul(pixelout.position, view);
    //pixelout.position = mul(pixelout.position, proj);
    ////
   //// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
   pixelout.normal = mul(Input.normal, (float3x3) world);
   //// 법선 벡터를 정규화합니다.
   pixelout.normal = normalize(pixelout.normal);
    //
   pixelout.uv = Input.uv;
    /////////////////////////////////////////////////////////////////////////////////////////////
    //matrix BoneTransform = bones[Input.boneids[0]] * Input.weights[0];
    //BoneTransform += bones[Input.boneids[1]] * Input.weights[1];
    //BoneTransform += bones[Input.boneids[2]] * Input.weights[2];
    //BoneTransform += bones[Input.boneids[3]] * Input.weights[3];

    //
    //float4 PosL = mul(pixelout.position, BoneTransform);
    //pixelout.position = mul(Input.position, BoneTransform);
    //pixelout.position += BoneTransform;

    //
    //float4 gl_Position = mul(PosL, (world * view * proj));
    //float4 NormalL = mul( float4(Input.normal.x, Input.normal.y, Input.normal.z, 0.0), BoneTransform);
    //
    //pixelout.normal = mul(NormalL, world);
    //pixelout.position = mul(PosL, world);

    return pixelout;
}

Texture2D source_texture : register(t0);
SamplerState samp : register(s0);

float4 PS(PixelInput input) : SV_Target
{
	float4 color = source_texture.Sample(samp, input.uv);
	//float4 color = source_texture.Sample(samp, float2(input.uv.x, 1 - input.uv.y));
	/*float4 color = 0.0f;
    if(input.uv.x < 1.0f)
       color = source_texture0.Sample(samp, input.uv);
    else
       color = source_texture1.Sample(samp, float2(input.uv.x - 1.0f, input.uv.y));*/
       //float4 color = float4(0, 0, 0, 1);

        //float4 textureColor = source_texture0.Sample(samp, input.uv);
        ////float4 textureColor = float4(0.2f,0.2f,0.2f,1);
        ///////////////////빛에 대한 계산//////////////////////
        //float4 color;
        //float3 lightDir;
        //float lightIntensity;
        //// 계산을 위해 빛 방향을 반전시킵니다.
        //lightDir = -lightDirection;
        //// 이 픽셀의 빛의 양을 계산합니다.
        //lightIntensity = saturate(dot(input.normal, lightDir));
        //color = saturate(diffuseColor * lightIntensity);
        ///////////////////////////////////////////////////////
        //color = color * textureColor;

       //return color;

    return color;
}





/*
float4
벡터와 같다, 플롯이 4개있다. float4 위치와, 컬러를 만들어줬다.
버택스가 들어가서 버택스 인풋, 픽셀에 들어갈 데이터라 픽셀 인풋이라 함.

POSITION 0 : 데이터의 출저와 역할에 대한 분명한 의미를 부여하기 위한 키워드,
0 은 시멘틱의 인덱스라 생각하면된다.

SV_ : System Value 더 중요한 데이터, 중요한 데이터는 반드시 SV를 붙인다.
*/