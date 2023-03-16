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
벡터와 같다, 플롯이 4개있다. float4 위치와, 컬러를 만들어줬다.
버택스가 들어가서 버택스 인풋, 픽셀에 들어갈 데이터라 픽셀 인풋이라 함.

POSITION 0 : 데이터의 출저와 역할에 대한 분명한 의미를 부여하기 위한 키워드,
0 은 시멘틱의 인덱스라 생각하면된다.

SV_ : System Value 더 중요한 데이터, 중요한 데이터는 반드시 SV를 붙인다.
*/
