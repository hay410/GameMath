#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>
#include "WindowsAPI.h"
#include <string>
#include <DirectXMath.h>
#include "Singleton.h"


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace std;
using namespace Microsoft::WRL;

class DirectXBase :public Singleton<DirectXBase>
{
public:
	friend Singleton<DirectXBase>;

	static HRESULT result;
	static 	WindowsAPI Window;
	static 	ComPtr<ID3D12Device> dev;
	static 	ComPtr<IDXGIFactory6> dxgiFactory;
	static vector<IDXGIAdapter1*> adapters;
	static IDXGIAdapter1* tmpAdapter;
	static vector<D3D_FEATURE_LEVEL>levels;
	static D3D_FEATURE_LEVEL featureLevel;
	static ComPtr<IDXGISwapChain4> swapchain;
	static ComPtr<ID3D12CommandAllocator> cmdAllocator;
	static ComPtr<ID3D12GraphicsCommandList> cmdList;
	static ComPtr<ID3D12CommandQueue> cmdQueue;
	static ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	static vector<ID3D12Resource*> backBuffers;
	static D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	static D3D12_RESOURCE_BARRIER barrierDesc;
	static ComPtr<ID3D12Fence> fence;
	static UINT64 fenceVal;
	static ComPtr<ID3D12Resource> depthBuffer;
	static ComPtr<ID3D12DescriptorHeap> dsvHeap;
	static IDirectInput8* dinput;					//DirectInputオブジェクト
	static IDirectInputDevice8* devkeybord;			//キーボードオブジェクト
	static IDirectInputDevice8* devmouse;			//マウスオブジェクト

	DirectXBase();
	friend Singleton<DirectXBase>;

	void Resize();
	void Init();
	void BeforeDrawing();
	void AfterDrawing();
	void zBufferClear();
};