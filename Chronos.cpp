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
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED || wParam == SIZE_MAXIMIZED)
			break;
		else
			return OnSize();

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
	case WM_LBUTTONDOWN:
		return OnLButtonDown(
			DPIScale::PixelsToDips(POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }),
			wParam
		);
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
	case WM_TIMER:
		return OnTimer(wParam, lParam);
	case WM_CONTEXTMENU:
		return OnContextMenu(POINT{ GET_X_LPARAM(lParam) , GET_Y_LPARAM(lParam) });
	//case WM_APP:
	//	ProcessCTPMessages();
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}



LRESULT Chronos::OnCreate()
{
	HR(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
	m_d2.CreateLifetimeResources(m_hwnd);

	SetTimer(m_hwnd,             // handle to main window 
		 	 sec1,				 // timer identifier 
			 1000,               // 1-second interval 
			 (TIMERPROC)NULL);   // no timer callback 

	return 0;
}


LRESULT Chronos::OnPaint()
{
	m_d2.CreateGraphicsResources(m_hwnd);

	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	RECT rc;
	GetClientRect(m_hwnd, &rc);
	D2D1_RECT_F clientRect = DPIScale::PixelsToDips(rc);

	m_d2.dc()->BeginDraw();
	m_d2.dc()->Clear();

	//m_d2.brush()->SetColor(D2D1::ColorF(0xEFD3B5, 0.7));
	//m_d2.dc()->FillRectangle(clientRect, m_d2.brush());

	std::wstring s = time();
	m_d2.brush()->SetColor(D2D1::ColorF(0.9f, 0.5f, 1.0f, 1.0f));
	m_d2.dc()->DrawText(
		s.c_str(),
		static_cast<UINT32>(s.size()),
		m_d2.textFormat(D2Objects::Segoe18),
		clientRect,
		m_d2.brush()
	);



	//float w2 = (clientRect.right - clientRect.left) / 2.0f;
	//float h2 = (clientRect.bottom - clientRect.top) / 2.0f;
	//m_d2.dc()->FillEllipse({ {w2, h2}, w2, h2 }, m_d2.brush());



	HR(m_d2.dc()->EndDraw());
	HR(m_d2.swapChain()->Present(1, 0)); 

	EndPaint(m_hwnd, &ps);
	return 0;
}

LRESULT Chronos::OnSize()
{
	if (m_d2.dc() != NULL)
	{
		m_d2.DiscardGraphicsResources();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	return 0;
}



LRESULT Chronos::OnLButtonDown(D2D1_POINT_2F point, WPARAM wParam)
{
	ResetTimer();
	return 0;
}


LRESULT Chronos::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case sec1:
		if (++m_second >= 60)
		{
			m_second = 0;
			m_minute++;
		}
		InvalidateRect(m_hwnd, NULL, FALSE);
		break;
	}
	return 0;
}

LRESULT Chronos::OnContextMenu(POINT point)
{
	return 0;
}

std::wstring Chronos::time() const
{
	std::wstringstream ss;
	ss << std::setfill(L'0') << std::setw(2) << m_minute << L":" << std::setw(2) << m_second;
	return ss.str();
}

void Chronos::ResetTimer()
{
	SetTimer(m_hwnd,             // handle to main window 
		 	 sec1,				 // timer identifier 
			 1000,               // 1-second interval 
			 (TIMERPROC)NULL);   // no timer callback 
	m_minute = 0;
	m_second = 0;
	InvalidateRect(m_hwnd, NULL, FALSE);
}
