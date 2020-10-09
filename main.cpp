// Chronos.cpp : Defines the entry point for the application.

#include "pch.h"
#include "Chronos.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize strings from string table
	const int max_loadstring = 100;
	WCHAR szTitle[max_loadstring];            // the title bar text
	WCHAR szClass[max_loadstring];            // the window class name
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, max_loadstring); // "Chronos"
	LoadStringW(hInstance, IDC_CHRONOS, szClass, max_loadstring); // "CHRONOS"

	// Perform application initialization:
	Chronos win(szClass);

	WndCreateArgs args;
	args.hInstance = hInstance;
	args.classStyle = CS_DBLCLKS;
	args.hbrBackground = CreateSolidBrush(RGB(0.2, 0.2, 0.2));
	//args.lpszMenuName = MAKEINTRESOURCE(IDC_PARTHENOS);
	args.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHRONOS));
	args.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	args.lpWindowName = szTitle;
	args.nWidth = 70;
	args.nHeight = 30;
	args.x = 1920 - args.nWidth;
	args.y = 1080 - args.nHeight;

	win.Register(args);
	if (!win.Create(args))
	{
		return -1;
	}
	ShowWindow(win.Window(), nCmdShow);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHRONOS));


	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
