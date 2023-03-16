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
{ //cbuffer Constant Buffer �����̴�.
    /*
    register / (b0) 
    Gpu�� �ȶ����� �ʴ�, Cpu������ GPU�������� �ִ� Ʈ������ ������ ��ġ��
    ��Ȯ�ϰ� ������ �� ����. �׷��� �������� Ʈ������ ���ۿ� ���̷�Ʈ��
    ���ϰ� ���� �����ϵ��� ����� ���ش�.
    b = �������ڿ��̶�� �˷��ְ�,
    0 = 14���� ���� �� �ְ�, 0 ~ 13�� �����̴�.
    ����ó�� ���� �� �ִ� �����̶� ��������.
    */
    matrix world;
    matrix view;
    matrix proj;

    //���� ���� ���ǻ���.
    /*
    ��� ���۴� ������ 16����Ʈ ���·� ������־�� �Ѵ�.
    4096 * 16����Ʈ ������ ũ�⸸ �� �� �ִ�.
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
    SV_Target = System Value ���� ���õǾ� �ִ� ���� ���� Ÿ���� ����϶� 
    //�˷��־�� �Ѵ�.
    */
    //���ý� ���̴����� �ѱ� �����͸� 
    //������ ����� ȭ�鿡 ũ�⿡ ���� �÷��� �ٽ� ���� ������� ����ش�.
    //�ȼ� ���̴��� ȣ���� �� �� �ֵ��� . �ڱ⿵�� �ȿ� �ִ� �׷��� ģ���鿡��
    //Ƽ���� �ش�. ���� �̰� �÷��� ��Ʈ�� �Ҹ��⵵ �Ѵ�.
    //VS�� ��� ���� ������ŭ ���ٸ� 
    //PS�� RS�� ������ �ȼ� ������ŭ ȣ���� �ȴ�.
    return input.color;
}





/*
float4
���Ϳ� ����, �÷��� 4���ִ�. float4 ��ġ��, �÷��� ��������.
���ý��� ���� ���ý� ��ǲ, �ȼ��� �� �����Ͷ� �ȼ� ��ǲ�̶� ��.

POSITION 0 : �������� ������ ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����,
0 �� �ø�ƽ�� �ε����� �����ϸ�ȴ�.

SV_ : System Value �� �߿��� ������, �߿��� �����ʹ� �ݵ�� SV�� ���δ�.
*/
