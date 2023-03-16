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
   //// ���� ��Ŀ� ���ؼ��� ���� ���͸� ����մϴ�.
   pixelout.normal = mul(Input.normal, (float3x3) world);
   //// ���� ���͸� ����ȭ�մϴ�.
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
        ///////////////////���� ���� ���//////////////////////
        //float4 color;
        //float3 lightDir;
        //float lightIntensity;
        //// ����� ���� �� ������ ������ŵ�ϴ�.
        //lightDir = -lightDirection;
        //// �� �ȼ��� ���� ���� ����մϴ�.
        //lightIntensity = saturate(dot(input.normal, lightDir));
        //color = saturate(diffuseColor * lightIntensity);
        ///////////////////////////////////////////////////////
        //color = color * textureColor;

       //return color;

    return color;
}





/*
float4
���Ϳ� ����, �÷��� 4���ִ�. float4 ��ġ��, �÷��� ��������.
���ý��� ���� ���ý� ��ǲ, �ȼ��� �� �����Ͷ� �ȼ� ��ǲ�̶� ��.

POSITION 0 : �������� ������ ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����,
0 �� �ø�ƽ�� �ε����� �����ϸ�ȴ�.

SV_ : System Value �� �߿��� ������, �߿��� �����ʹ� �ݵ�� SV�� ���δ�.
*/