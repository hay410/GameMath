#include"BasicShaderHeader.hlsli"

VSOutput main(float4 pos:POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output;
	//output.svpos = pos;
	output.svpos = mul(mat, pos);
	output.normal = normal;
	output.uv = uv;
	return output;
}

//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	//return pos + float4(-0.5f,+0.5f,0,0);
//	return pos * float4(0.5, 0.5, 1, 1);
//}
