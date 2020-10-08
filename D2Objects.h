#pragma once

#include "pch.h"
#include "Resource.h"


class D2Objects
{
public:
	HWND					hwndParent = NULL;

	////////////////////////////////////////////////////////
	// Data

	// Direct Write
	static const int nFormats = 4;
	enum Formats { Segoe10, Segoe12, Segoe14, Segoe18 };

	////////////////////////////////////////////////////////
	// Pointers

	// Rendering 
	ID2D1Factory1* pFactory = NULL;
	ID3D11Device1* pDirect3DDevice = NULL;
	ID3D11DeviceContext1* pDirect3DContext = NULL;
	ID2D1Device* pDirect2DDevice = NULL;
	ID2D1DeviceContext* pD2DContext = NULL; // inherits from ID2D1RenderTarget (?); use similarly
	IDXGISwapChain1* pDXGISwapChain = NULL; // stores buffers that the d2dcontext targets
	ID2D1Bitmap1* pDirect2DBackBuffer = NULL; // actual bitmap that the d2dcontext targets

	// Drawing
	ID2D1StrokeStyle* pDashedStyle = NULL;
	ID2D1StrokeStyle1* pFixedTransformStyle = NULL;
	ID2D1StrokeStyle1* pHairlineStyle = NULL;
	ID2D1SolidColorBrush* pBrush = NULL;

	// Direct Write pointers
	IDWriteFactory1* pDWriteFactory = NULL;
	IDWriteTextFormat* pTextFormats[nFormats] = {};


	////////////////////////////////////////////////////////
	// Functions

	HRESULT CreateLifetimeResources(HWND hwnd);
	HRESULT	CreateGraphicsResources(HWND hwnd);
	void	DiscardLifetimeResources();
	void	DiscardGraphicsResources();
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