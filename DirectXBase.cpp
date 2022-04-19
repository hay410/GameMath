#include "DirectXBase.h"

HRESULT DirectXBase::result{};
WindowsAPI DirectXBase::Window{};
ComPtr<ID3D12Device> DirectXBase::dev{};
ComPtr<IDXGIFactory6> DirectXBase::dxgiFactory{};
vector<IDXGIAdapter1*> DirectXBase::adapters{};
IDXGIAdapter1* DirectXBase::tmpAdapter{};
vector<D3D_FEATURE_LEVEL> DirectXBase::levels{};
D3D_FEATURE_LEVEL DirectXBase::featureLevel{};
ComPtr<IDXGISwapChain4> DirectXBase::swapchain{};
ComPtr<ID3D12CommandAllocator> DirectXBase::cmdAllocator{};
ComPtr<ID3D12GraphicsCommandList> DirectXBase::cmdList{};
ComPtr<ID3D12CommandQueue> DirectXBase::cmdQueue{};
ComPtr<ID3D12DescriptorHeap> DirectXBase::rtvHeaps{};
ComPtr<ID3D12Resource> DirectXBase::depthBuffer{};
ComPtr<ID3D12DescriptorHeap> DirectXBase::dsvHeap{};
vector<ID3D12Resource*> DirectXBase::backBuffers{};
D3D12_DESCRIPTOR_HEAP_DESC DirectXBase::heapDesc{};
ComPtr<ID3D12Fence> DirectXBase::fence{};
UINT64 DirectXBase::fenceVal{};
IDirectInput8* DirectXBase::dinput{};
IDirectInputDevice8* DirectXBase::devkeybord{};
IDirectInputDevice8* DirectXBase::devmouse{};

DirectXBase::DirectXBase()
{
}

void DirectXBase::Resize()
{
	backBuffers.resize(2);
}

void DirectXBase::Init()
{
	Window.Init();
	//デバックレイヤーの有効化
#ifdef _DEBUG

	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}

#endif

	//DirectX初期化処理　ここから

	Resize();

	//DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) !=
		DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(tmpAdapter); //動的配列に追加する
	}

	for (int i = 0; i < adapters.size(); ++i)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc); //アダプターの情報を取得

		//ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description; //アダプター名
		// Intel UHD Graphics (オンボードグラフィック) を回避
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //採用
			break;
		}
	}

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	featureLevel;

	for (int i = 0; i < _countof(levels); ++i)
	{
		//採用したアダプターでデバイス生成
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	//コマンドアロケータを生成
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));

	//コマンドリストを生成
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));

	//標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	//各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//色情報の書式
	swapchainDesc.SampleDesc.Count = 1;								//マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;				//バックバッファ用
	swapchainDesc.BufferCount = 2;									//バッファ数を2つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		//フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1のComPtrを用意
	ComPtr<IDXGISwapChain1> swapchain1;
	//スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		Window.hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);

	swapchain1.As(&swapchain);

	//各種設定をしてデスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//レンダーターゲットビュー
	heapDesc.NumDescriptors = 2;						//裏表の2つ
	dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeaps));

	//裏表2つ分について
	for (int i = 0; i < 2; ++i)
	{
		//スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		//デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
		//レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i],
			nullptr,
			handle
		);
	}

	//リソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		Window.width,
		Window.height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//リソース生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	//アップロード不可
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,					//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;		//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;		//デプスステンシルビュー
	dsvHeap = nullptr;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;		//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//フェンスの生成
	fence = nullptr;
	fenceVal = 0;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInputオブジェクトの生成
	result = DirectInput8Create(
		Window.w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//入力データの形式セット
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);

	//排他制御レベルのセット
	result = devkeybord->SetCooperativeLevel(
		Window.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//マウスデバイスの生成
	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);

	//入力データの形式セット
	result = devmouse->SetDataFormat(&c_dfDIMouse);

	//排他制御レベルのセット
	result = devmouse->SetCooperativeLevel(
		Window.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//DirectX初期化処理　ここまで
}

void DirectXBase::BeforeDrawing()
{
	//メッセージがある？
	if (PeekMessage(&Window.msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Window.msg); //キー入力メッセージの処理
		DispatchMessage(&Window.msg); //プロシージャにメッセージを送る
	}

	//×ボタンで終了メッセージが来たらゲームループを抜ける
	if (Window.msg.message == WM_QUIT)
	{
		exit(true);
	}



	//バックバッファの番号を取得(2つなので0番か1番)
	UINT bbIndex =
		swapchain->GetCurrentBackBufferIndex();

	//1.リソースバッファで書き込み可能に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex],
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//2.描画先指定
	//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//3.画面クリア
	//float clearColor[] = { 0.25f,0.35f,0.5f,0.0f };	//青っぽい色
	float clearColor[] = { 0,0,0,0 };
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, Window.width, Window.height));
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, Window.width, Window.height));
}

void DirectXBase::AfterDrawing()
{
	UINT bbIndex =
		swapchain->GetCurrentBackBufferIndex();
	//5.リソースバリアを戻す
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//命令のクローズ
	cmdList->Close();
	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	//コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	//コマンドリストの実行完了を待つ
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	cmdAllocator->Reset();					//キューをクリア
	cmdList->Reset(cmdAllocator.Get(), nullptr);	//再びコマンドリストを貯める準備

	//バッファのフリップ(裏表の入れ替え)
	swapchain->Present(1, 0);

	//DirectX毎フレーム処理　ここまで
}

void DirectXBase::zBufferClear()
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
