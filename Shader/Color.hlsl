struct vertexInput
{
	float4 position		: POSITION0;
	float4 color			: COLOR0;
};

struct PixelInput
{
	float4 position		: SV_POSITION0;
	float4 color			: COLOR0;
};

cbuffer TransformBuffer : register(b0)
{ //cbuffer Constant Buffer 약자이다.
    /*
    register / (b0) 
    Gpu는 똑똑하지 않다, Cpu에서는 GPU공간에서 있는 트랜스폼 버퍼의 위치를
    정확하게 측정할 수 없다. 그래서 시피유가 트랜스폼 버퍼에 다이렉트로
    편하게 접근 가능하도록 등록을 해준다.
    b = 버퍼형자원이라고 알려주고,
    0 = 14개를 넣을 수 있고, 0 ~ 13번 까지이다.
    슬롯처럼 넣을 수 있는 공간이라 생각하자.
    */
    matrix world;
    matrix view;
    matrix proj;

    //버퍼 사용시 주의사항.
    /*
    상수 버퍼는 무조건 16바이트 형태로 만들어주어야 한다.
    4096 * 16바이트 이하인 크기만 들어갈 수 있다.
    */
};

PixelInput VS(vertexInput Input)
{
    PixelInput				pixelout;

    pixelout.position       = mul(Input.position, world);
    pixelout.position       = mul(pixelout.position, view);
    pixelout.position       = mul(pixelout.position, proj);
    //pixelout.position		= Input.position;
    pixelout.color			= Input.color;
    return					pixelout;
}

float4 PS(PixelInput input) : SV_Target
{
    /*
    SV_Target = System Value 현재 셋팅되어 있는 메인 랜더 타겟을 사용하라 
    //알려주어야 한다.
    */
    //버택스 쉐이더에서 넘긴 데이터를 
    //레스터 라이즈가 화면에 크기에 따라 늘려서 다시 원래 사이즈로 잡아준다.
    //픽셀 쉐이더에 호출이 될 수 있도록 . 자기영역 안에 있는 그려질 친구들에게
    //티켓을 준다. 보통 이걸 플래그 먼트로 불리기도 한다.
    //VS가 모든 정점 갯수만큼 돈다면 
    //PS는 RS가 지정한 픽셀 갯수만큼 호출이 된다.
    return input.color;
}





/*
float4
벡터와 같다, 플롯이 4개있다. float4 위치와, 컬러를 만들어줬다.
버택스가 들어가서 버택스 인풋, 픽셀에 들어갈 데이터라 픽셀 인풋이라 함.

POSITION 0 : 데이터의 출저와 역할에 대한 분명한 의미를 부여하기 위한 키워드,
0 은 시멘틱의 인덱스라 생각하면된다.

SV_ : System Value 더 중요한 데이터, 중요한 데이터는 반드시 SV를 붙인다.
*/
