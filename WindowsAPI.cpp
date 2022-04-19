#include"WindowsAPI.h"


LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY: //�E�B���h�E���j�����ꂽ
		PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); //�W���̏������s��
}

void WindowsAPI::Init()
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; //�E�B���h�E�v���V�\�W����ݒ�
	w.lpszClassName = L"DirectXGame"; //�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr); //�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //�J�[�\���w��

	//�E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&w);

	//�E�B���h�E�T�C�Y{ x���W y���W ���� �c�� }
	wrc = { 0,0,width,height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); //�����ŃT�C�Y�␳

	//�E�B���h�E�I�u�W�F�N�g�̐���
		hwnd = CreateWindow(w.lpszClassName, //�N���X��
		L"�]���ۑ�A",						  //�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,				  //�W���ȃE�B���h�E�T�C�Y
		CW_USEDEFAULT,						  //�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,						  //�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,				  //�E�B���h�E����
		wrc.bottom - wrc.top,				  //�E�B���h�E�c��
		nullptr,							  //�e�E�B���h�E�n���h��
		nullptr,							  //���j���[�n���h��
		w.hInstance,						  //�Ăяo���A�v���P�[�V�����n���h��
		nullptr);							  //�I�v�V����

			//�E�B���h�E�\��
		ShowWindow(hwnd, SW_SHOW);
}