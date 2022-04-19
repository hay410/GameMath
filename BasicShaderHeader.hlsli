cbuffer cbuff0 :register(b0)
{
	float4 color;
	matrix mat;
};

struct VSOutput
{
	float4 svpos :SV_POSITION;	//システム用頂点座標
	float3 normal :NORMAL;		//法線ベクトル
	float2 uv :TEXCOORD;		//uv値
};



//float4 main() :SV_TARGET
//{
//	return float4(1.0f,1.0f,1.0f,1.0f);
//}