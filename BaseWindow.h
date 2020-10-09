#pragma once

/* Borderless Window code taken from https://github.com/melak47/BorderlessWindow */

#include "pch.h"

#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")


namespace
{
	bool maximized(HWND hwnd) 
	{
		WINDOWPLACEMENT placement;
		if (!::GetWindowPlacement(hwnd, &placement))
			return false;
		return placement.showCmd == SW_MAXIMIZE;
	}

	/* Adjust client rect to not spill over monitor edges when maximized.
	 * rect(in/out): in: proposed window rect, out: calculated client rect
	 * Does nothing if the window is not maximized.
	 */
	auto adjust_maximized_client_rect(HWND hwnd, RECT& rect) -> void {
		if (!maximized(hwnd)) return;

		auto monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
		if (!monitor) return;

		MONITORINFO monitor_info{};
		monitor_info.cbSize = sizeof(monitor_info);
		if (!::GetMonitorInfoW(monitor, &monitor_info)) return;

		// when maximized, make the client area fill just the monitor (without task bar) rect,
		// not the whole window rect which extends beyond the monitor.
		rect = monitor_info.rcWork;
	}
}


// Argument struct to pass to BaseWindow::Register/Create
struct WndCreateArgs
{
	HINSTANCE hInstance;

	// WNDCLASSEX only

	UINT classStyle = CS_HREDRAW | CS_VREDRAW;
	HCURSOR hCursor = 0;
	//HBRUSH hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	HBRUSH hbrBackground = (HBRUSH)0;
	LPCWSTR lpszMenuName = 0;
	HICON hIcon = 0;
	HICON hIconSm = 0;

	// CreateWindowEx only

	PCWSTR lpWindowName = L"BaseWindow";
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	DWORD dwExStyle = 0;
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int nWidth = CW_USEDEFAULT;
	int nHeight = CW_USEDEFAULT;
	HWND hWndParent = 0;
	HMENU hMenu = 0;
};


template <class DERIVED_TYPE>
class BaseWindow
{
public:

	// Statics
	static DWORD const style = WS_POPUP | WS_VISIBLE;
	static DWORD const exStyle = WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;

	// Constructors
	BaseWindow() : m_wsClassName(L"BASE WINDOW"), m_hwnd(NULL), m_hInstance(NULL) { }
	BaseWindow(PCWSTR pClassName) : m_wsClassName(pClassName), m_hwnd(NULL), m_hInstance(NULL) { }

	// Creation Functions
	BOOL Register(WndCreateArgs& args)
	{
		WNDCLASSEXW wcex{};

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = args.classStyle;
		wcex.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = args.hInstance;
		wcex.hCursor = args.hCursor;
		wcex.hbrBackground = args.hbrBackground;
		wcex.lpszClassName = ClassName();
		wcex.lpszMenuName = args.lpszMenuName;
		wcex.hIcon = args.hIcon;
		wcex.hIconSm = args.hIconSm;

		ATOM atom = RegisterClassExW(&wcex);

		return (atom ? TRUE : FALSE);
	}

	BOOL Create(WndCreateArgs& args) 
	{
		args.dwStyle = style;
		args.dwExStyle = exStyle;

		m_hInstance = args.hInstance;
		m_hwnd = CreateWindowEx(
			args.dwExStyle, ClassName(), args.lpWindowName, args.dwStyle, args.x, args.y,
			args.nWidth, args.nHeight, args.hWndParent, args.hMenu, args.hInstance, this
		);

		return (m_hwnd ? TRUE : FALSE);
	}

	// Main Callback
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DERIVED_TYPE* pThis = NULL;
		if (uMsg == WM_NCCREATE)
		{
			// Set user data pointer to self (`this` is passed as the application data)
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->m_hwnd = hwnd;
		}
		else
		{
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (pThis)
		{
			// Either return in case, or on break, return derivedClass->HandleMessage()
			switch (uMsg)
			{
			case WM_NCHITTEST:
			{
				// When we have no border or title bar, we need to perform our own hit testing to allow resizing and moving.
				return pThis->hit_test(POINT{
					GET_X_LPARAM(lParam),
					GET_Y_LPARAM(lParam)
				});
			}
			default:
				break;
			}
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	// Getters/Setters
	HWND Window() const { return m_hwnd; }
	PCWSTR ClassName() const { return m_wsClassName.c_str(); }


protected:

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	std::wstring m_wsClassName;
	HWND m_hwnd;
	HINSTANCE m_hInstance;

private:

	auto hit_test(POINT cursor) const -> LRESULT {
		// identify borders and corners to allow resizing the window.
		// Note: On Windows 10, windows behave differently and
		// allow resizing outside the visible window frame.
		// This implementation does not replicate that behavior.

		bool borderless_resize = false;  // should the window allow resizing by dragging the borders while borderless
		bool borderless_drag = false; // should the window allow moving my dragging the client area

		const POINT border{
			::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
			::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
		};
		RECT window;
		if (!::GetWindowRect(this->m_hwnd, &window)) {
			return HTNOWHERE;
		}

		const auto drag = borderless_drag ? HTCAPTION : HTCLIENT;

		enum region_mask {
			client = 0b0000,
			left = 0b0001,
			right = 0b0010,
			top = 0b0100,
			bottom = 0b1000,
		};

		const auto result =
			left * (cursor.x < (window.left + border.x)) |
			right * (cursor.x >= (window.right - border.x)) |
			top * (cursor.y < (window.top + border.y)) |
			bottom * (cursor.y >= (window.bottom - border.y));

		switch (result) {
		case left: return borderless_resize ? HTLEFT : drag;
		case right: return borderless_resize ? HTRIGHT : drag;
		case top: return borderless_resize ? HTTOP : drag;
		case bottom: return borderless_resize ? HTBOTTOM : drag;
		case top | left: return borderless_resize ? HTTOPLEFT : drag;
		case top | right: return borderless_resize ? HTTOPRIGHT : drag;
		case bottom | left: return borderless_resize ? HTBOTTOMLEFT : drag;
		case bottom | right: return borderless_resize ? HTBOTTOMRIGHT : drag;
		case client: return drag;
		default: return HTNOWHERE;
		}
	}

};

