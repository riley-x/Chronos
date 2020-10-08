#include "pch.h"
#include "Chronos.h"
#include "errors.h"
#include "DPIScale.h"

Chronos::Chronos(PCWSTR szClassName) :
	BaseWindow(szClassName)
{
}

Chronos::~Chronos()
{
}

LRESULT Chronos::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return OnCreate();
	//case WM_COMMAND: // Command items from application menu; accelerators
	//{
	//	int wmId = LOWORD(wParam);
	//	switch (wmId)
	//	{
	//	case IDM_ABOUT:
	//		DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, About);
	//		break;
	//	case IDA_COPY:
	//		OnCopy();
	//		break;
	//	default:
	//		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	//	}
	//	return 0;
	//}
	//case WM_CLOSE: {
	//	if (MessageBox(m_hwnd, L"Really quit?", L"Parthenos", MB_OKCANCEL) == IDOK)
	//	{
	//		::DestroyWindow(m_hwnd);
	//	}
	//	return 0;
	//}
	//case WM_GETMINMAXINFO:
	//{
	//	LPMINMAXINFO lpMMI = reinterpret_cast<LPMINMAXINFO>(lParam);
	//	lpMMI->ptMinTrackSize.x = 600;
	//	lpMMI->ptMinTrackSize.y = 600;
	//	return 0;
	//}
	case WM_DESTROY:
		m_d2.DiscardGraphicsResources();
		m_d2.DiscardLifetimeResources();
		CoUninitialize();
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		return OnPaint();
	//case WM_SIZE:
	//	return OnSize(wParam);
	//case WM_MOUSEMOVE:
	//	return OnMouseMove(
	//		POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) },
	//		wParam
	//	);
	//case WM_MOUSELEAVE:
	//{
	//	// This message seems to be sent when screen turns off, even when m_bMouseTracking == false.
	//	// OnMouseMove() begins tracking, so don't call it or else infinite loop.
	//	D2D1_POINT_2F dipCursor = DPIScale::PixelsToDips(POINT{ -1, -1 });
	//	if (m_mouseCaptured)
	//	{
	//		m_mouseCaptured->OnMouseMove(dipCursor, wParam, false);
	//	}
	//	else
	//	{
	//		bool handeled = false;
	//		for (auto item : m_activeItems)
	//		{
	//			handeled = item->OnMouseMove(dipCursor, wParam, handeled) || handeled;
	//		}
	//	}
	//	m_mouseTrack.Reset(m_hwnd);
	//	return 0;
	//}
	//case WM_MOUSEHOVER:
	//	// TODO: Handle the mouse-hover message.
	//	m_mouseTrack.Reset(m_hwnd);
	//	return 0;
	//case WM_MOUSEWHEEL:
	//	return OnMouseWheel(
	//		POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) },
	//		wParam
	//	);
	//case WM_SETCURSOR:
	//	if (LOWORD(lParam) == HTCLIENT)
	//	{
	//		// Handled in WM_MOUSEMOVE
	//		return TRUE;
	//	}
	//	break;
	//case WM_LBUTTONDOWN:
	//	return OnLButtonDown(
	//		POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) },
	//		wParam
	//	);
	//case WM_LBUTTONDBLCLK:
	//	return OnLButtonDblclk(
	//		POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) },
	//		wParam
	//	);
	//case WM_LBUTTONUP:
	//	return OnLButtonUp(
	//		POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) },
	//		wParam
	//	);
	//case WM_CHAR:
	//	return OnChar(static_cast<wchar_t>(wParam), lParam);
	//case WM_KEYDOWN:
	//	if (OnKeyDown(wParam, lParam)) return 0;
	//	break; // pass on to DefWindowProc to process WM_CHAR messages...?
	//case WM_TIMER:
	//	return OnTimer(wParam, lParam);
	//case WM_APP:
	//	ProcessCTPMessages();
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}



LRESULT Chronos::OnCreate()
{
	SetLayeredWindowAttributes(m_hwnd, 0, 0, LWA_ALPHA);
	SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY); // Set black to be full transparent
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		hr = m_d2.CreateLifetimeResources(m_hwnd);
		m_d2.hwndParent = m_hwnd;
	}
	if (FAILED(hr))
	{
		throw Error(L"OnCreate failed!");
		return -1;  // Fail CreateWindowEx.
	}

	
	return 0;
}


LRESULT Chronos::OnPaint()
{
	RECT rc;
	BOOL bErr = GetClientRect(m_hwnd, &rc);
	if (bErr == 0) OutputError(L"GetClientRect failed");

	HRESULT hr = m_d2.CreateGraphicsResources(m_hwnd);
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		D2D1_RECT_F dipRect = DPIScale::PixelsToDips(ps.rcPaint);
		m_d2.pD2DContext->BeginDraw();

		m_d2.pD2DContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));

		m_d2.pBrush->SetColor(D2D1::ColorF(0.8f, 0.8f, 0.8f, 1.0f));
		OutputDebugString(L"HIHI\n\n");
		m_d2.pD2DContext->DrawText(
			L"Hello World!",
			static_cast<UINT32>(12),
			m_d2.pTextFormats[m_d2.Segoe12],
			dipRect,
			m_d2.pBrush
		);


		hr = m_d2.pD2DContext->EndDraw();

		if (SUCCEEDED(hr))
		{
			// Present
			DXGI_PRESENT_PARAMETERS parameters = { 0 };
			parameters.DirtyRectsCount = 0;
			parameters.pDirtyRects = nullptr;
			parameters.pScrollRect = nullptr;
			parameters.pScrollOffset = nullptr;

			hr = m_d2.pDXGISwapChain->Present1(1, 0, &parameters);
		}

		EndPaint(m_hwnd, &ps);
	}
	if (FAILED(hr)) OutputHRerr(hr, L"OnPaint failed");

	return 0;
}