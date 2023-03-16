struct vertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXTURECOOD0;
};

struct PixelInput
{
    float4 position : SV_POSITION0;
    float2 uv : TEXTURECOOD0;
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

    pixelout.position = mul(Input.position, world);
    pixelout.position = mul(pixelout.position, view);
    pixelout.position = mul(pixelout.position, proj);
    
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
