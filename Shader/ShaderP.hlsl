struct vertexInput
{
    float4 position : POSITION0;
};

struct PixelInput
{
    float4 position : SV_POSITION0;
};

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
};

PixelInput VS(vertexInput Input)
{
    PixelInput pixelout;
    
    Input.position.w = 1;
    
    pixelout.position = mul(Input.position, world);
    pixelout.position = mul(pixelout.position, view);
    pixelout.position = mul(pixelout.position, proj);
    
    return pixelout;
}

float4 PS(PixelInput input) : SV_Target
{
    return float4(1, 1, 1, 1);
}


/*
float4
���Ϳ� ����, �÷��� 4���ִ�. float4 ��ġ��, �÷��� ��������.
���ý��� ���� ���ý� ��ǲ, �ȼ��� �� �����Ͷ� �ȼ� ��ǲ�̶� ��.

POSITION 0 : �������� ������ ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����,
0 �� �ø�ƽ�� �ε����� �����ϸ�ȴ�.

SV_ : System Value �� �߿��� ������, �߿��� �����ʹ� �ݵ�� SV�� ���δ�.
*/
