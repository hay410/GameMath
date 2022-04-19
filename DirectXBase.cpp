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
	//�f�o�b�N���C���[�̗L����
#ifdef _DEBUG

	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}

#endif

	//DirectX�����������@��������

	Resize();

	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) !=
		DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(tmpAdapter); //���I�z��ɒǉ�����
	}

	for (int i = 0; i < adapters.size(); ++i)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc); //�A�_�v�^�[�̏����擾

		//�\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description; //�A�_�v�^�[��
		// Intel UHD Graphics (�I���{�[�h�O���t�B�b�N) �����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //�̗p
			break;
		}
	}

	//�Ή����x���̔z��
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
		//�̗p�����A�_�v�^�[�Ńf�o�C�X����
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

	//�R�}���h�A���P�[�^�𐶐�
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));

	//�R�}���h���X�g�𐶐�
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));

	//�W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	//�e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//�F���̏���
	swapchainDesc.SampleDesc.Count = 1;								//�}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;				//�o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2;									//�o�b�t�@����2�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		//�t���b�v��͔j��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1��ComPtr��p��
	ComPtr<IDXGISwapChain1> swapchain1;
	//�X���b�v�`�F�[���̐���
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		Window.hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);

	swapchain1.As(&swapchain);

	//�e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//�����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = 2;						//���\��2��
	dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeaps));

	//���\2���ɂ���
	for (int i = 0; i < 2; ++i)
	{
		//�X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
		//�����_�[�^�[�Q�b�g�r���[�̐���
		dev->CreateRenderTargetView(
			backBuffers[i],
			nullptr,
			handle
		);
	}

	//���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		Window.width,
		Window.height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//���\�[�X����
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	//�A�b�v���[�h�s��
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,					//�[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);

	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;		//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;		//�f�v�X�X�e���V���r���[
	dsvHeap = nullptr;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;		//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//�t�F���X�̐���
	fence = nullptr;
	fenceVal = 0;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInput�I�u�W�F�N�g�̐���
	result = DirectInput8Create(
		Window.w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//�L�[�{�[�h�f�o�C�X�̐���
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//���̓f�[�^�̌`���Z�b�g
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);

	//�r�����䃌�x���̃Z�b�g
	result = devkeybord->SetCooperativeLevel(
		Window.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//�}�E�X�f�o�C�X�̐���
	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);

	//���̓f�[�^�̌`���Z�b�g
	result = devmouse->SetDataFormat(&c_dfDIMouse);

	//�r�����䃌�x���̃Z�b�g
	result = devmouse->SetCooperativeLevel(
		Window.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//DirectX�����������@�����܂�
}

void DirectXBase::BeforeDrawing()
{
	//���b�Z�[�W������H
	if (PeekMessage(&Window.msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Window.msg); //�L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&Window.msg); //�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
	}

	//�~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
	if (Window.msg.message == WM_QUIT)
	{
		exit(true);
	}



	//�o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	UINT bbIndex =
		swapchain->GetCurrentBackBufferIndex();

	//1.���\�[�X�o�b�t�@�ŏ������݉\�ɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex],
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//2.�`���w��
	//�����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//3.��ʃN���A
	//float clearColor[] = { 0.25f,0.35f,0.5f,0.0f };	//���ۂ��F
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
	//5.���\�[�X�o���A��߂�
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//���߂̃N���[�Y
	cmdList->Close();
	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	//�R�}���h���X�g�̔z��
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	//�R�}���h���X�g�̎��s������҂�
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	cmdAllocator->Reset();					//�L���[���N���A
	cmdList->Reset(cmdAllocator.Get(), nullptr);	//�ĂуR�}���h���X�g�𒙂߂鏀��

	//�o�b�t�@�̃t���b�v(���\�̓���ւ�)
	swapchain->Present(1, 0);

	//DirectX���t���[�������@�����܂�
}

void DirectXBase::zBufferClear()
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
