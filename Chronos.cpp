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
	HR(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
	m_d2.CreateLifetimeResources(m_hwnd);
	return 0;
}


LRESULT Chronos::OnPaint()
{
	m_d2.CreateGraphicsResources(m_hwnd);
	OutputDebugString(L"Paint\n");

	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	RECT rc;
	GetClientRect(m_hwnd, &rc);
	D2D1_RECT_F updateRect = DPIScale::PixelsToDips(ps.rcPaint);

	m_d2.dc()->BeginDraw();
	m_d2.dc()->Clear();

	m_d2.brush()->SetColor(D2D1::ColorF(0.6f, 0.0f, 0.7f, 0.5f));
	m_d2.dc()->FillEllipse({ {50, 50}, 50, 50 }, m_d2.brush());
	HR(m_d2.dc()->EndDraw());

	HR(m_d2.swapChain()->Present(1, 0)); 

	EndPaint(m_hwnd, &ps);
	return 0;
}
