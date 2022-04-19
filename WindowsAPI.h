#pragma once
#include<Windows.h>
#include"Input.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

class WindowsAPI
{
public:
	const int width = 1280;
	const int height = 720;
	HWND hwnd;
	RECT wrc;
	WNDCLASSEX w{};
	MSG msg;
	void Init();	
};