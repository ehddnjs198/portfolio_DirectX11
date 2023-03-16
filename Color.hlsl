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
{ 
 
    matrix world;
    matrix view;
    matrix proj;
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
    
    return input.color;
}




