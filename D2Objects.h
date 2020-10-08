#pragma once

#include "pch.h"
#include "Resource.h"


class D2Objects
{

public:
	enum Formats { Segoe10, Segoe12, Segoe14, Segoe18, nFormats };

	void CreateLifetimeResources(HWND hwnd);
	void CreateGraphicsResources(HWND hwnd);
	void DiscardGraphicsResources();

	ID2D1DeviceContext* dc() const { return deviceContext.Get(); }
	ID2D1SolidColorBrush* brush() const { return solidBrush.Get(); }
	IDXGISwapChain1* swapChain() const { return m_swapChain.Get(); }

private:

	HWND m_hwndParent = nullptr;

	// Boilerplate
	ComPtr<ID3D11Device> direct3dDevice;
	ComPtr<IDXGIDevice> dxgiDevice;
	ComPtr<IDXGIFactory2> dxFactory;
	ComPtr<ID2D1Factory2> d2Factory;
	ComPtr<ID2D1Device1> d2Device;
	ComPtr<IDXGISurface2> surface;
	ComPtr<ID2D1Bitmap1> bitmap;
	ComPtr<IDCompositionDevice> dcompDevice;
	ComPtr<IDCompositionTarget> target;
	ComPtr<IDCompositionVisual> visual;
	ComPtr<IDWriteFactory1> pDWriteFactory;

	// User
	ComPtr<IDXGISwapChain1> m_swapChain;
	ComPtr<ID2D1DeviceContext> deviceContext;
	ComPtr<ID2D1SolidColorBrush> solidBrush;

	// Styles
	ComPtr<ID2D1StrokeStyle> pDashedStyle;
	ComPtr<ID2D1StrokeStyle1> pFixedTransformStyle;
	ComPtr<ID2D1StrokeStyle1> pHairlineStyle;
	ComPtr<IDWriteTextFormat> pTextFormats[nFormats];


	////////////////////////////////////////////////////////
	// Functions

	void CreateDrawingResources();

};


inline float FontSize(D2Objects::Formats font)
{
	switch (font)
	{
	case D2Objects::Segoe10:
		return 10.0f;
	case D2Objects::Segoe12:
		return 12.0f;
	case D2Objects::Segoe14:
		return 14.0f;
	case D2Objects::Segoe18:
		return 18.0f;
	default:
		OutputDebugString(L"FontSize font not recognized\n");
		return 0.0f;
	}
}