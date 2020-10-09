#include "pch.h"
#include "D2Objects.h"
#include "DPIScale.h"
#include "errors.h"


void D2Objects::CreateLifetimeResources(HWND window)
{
	m_hwndParent = window;

	// Initialize DPI settings
	DPIScale::Initialize(window);

	// Create the DX11 API device object
	HR(D3D11CreateDevice(
		nullptr,    // Adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,    // Module
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, // Highest available feature level
		D3D11_SDK_VERSION,
		&direct3dDevice, // returns the Direct3D device created
		nullptr,    // returns feature level of device created
		nullptr  // returns the device immediate context ID3D11DeviceContext
	));

	// Obtain the underlying DXGI device of the Direct3D11 device.
	HR(direct3dDevice.As(&dxgiDevice));

	HR(CreateDXGIFactory2(
		DXGI_CREATE_FACTORY_DEBUG,
		__uuidof(dxFactory),
		reinterpret_cast<void**>(dxFactory.GetAddressOf())
	));

	// Set the description for the swap chain.
	RECT rect = {};
	GetClientRect(window, &rect);
	DXGI_SWAP_CHAIN_DESC1 description = {};
	description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount = 2;
	description.SampleDesc.Count = 1;
	description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	description.Width = rect.right - rect.left;
	description.Height = rect.bottom - rect.top;

	// Create the swap chain, targeting composition instead of a hwnd
	HR(dxFactory->CreateSwapChainForComposition(
		dxgiDevice.Get(),
		&description,
		nullptr, // Don’t restrict
		m_swapChain.GetAddressOf()
	));

	// Create a single-threaded Direct2D factory with debugging information
	D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
	HR(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		d2Factory.GetAddressOf()
	));
	
	// Create the Direct2D device that links back to the Direct3D device
	HR(d2Factory->CreateDevice(
		dxgiDevice.Get(),
		d2Device.GetAddressOf()
	));

	// Create the Direct2D device context that is the actual render target and exposes drawing commands
	HR(d2Device->CreateDeviceContext(
		D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		deviceContext.GetAddressOf()
	));

	// Set DPI
	deviceContext->SetDpi(DPIScale::DPIX(), DPIScale::DPIY());

	// Create the DirectComposition device using the original Direct3D device's DXGI interface.
	HR(DCompositionCreateDevice(
		dxgiDevice.Get(),
		__uuidof(dcompDevice),
		reinterpret_cast<void**>(dcompDevice.GetAddressOf())
	));

	// Create a target for the window
	HR(dcompDevice->CreateTargetForHwnd(
		window,
		true, // Top most
		target.GetAddressOf()
	));

	// Get the DirectComposition visual for the target, and set its contents to our swap chain
	HR(dcompDevice->CreateVisual(visual.GetAddressOf()));
	HR(visual->SetContent(m_swapChain.Get()));

	// Set our only visual as the root of the tree of visuals
	HR(target->SetRoot(visual.Get()));

	// Commit the changes to the device
	HR(dcompDevice->Commit());

	// Create brushes, styles, etc.
	CreateDrawingResources();
}


void D2Objects::CreateDrawingResources()
{
	// Create main brush
	HR(deviceContext->CreateSolidColorBrush(
		D2D1::ColorF(1, 1, 1, 1),
		solidBrush.GetAddressOf()
	));

	// Create dashed stroke style
	D2D1_STROKE_STYLE_PROPERTIES dashed = D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_FLAT,		// The start cap.
		D2D1_CAP_STYLE_FLAT,		// The end cap.
		D2D1_CAP_STYLE_FLAT,		// The dash cap.
		D2D1_LINE_JOIN_MITER,		// The line join.
		10.0f,						// The miter limit.
		D2D1_DASH_STYLE_DASH,		// The dash style.
		0.0f						// The dash offset.
	);
	HR(d2Factory->CreateStrokeStyle(dashed, NULL, 0, pDashedStyle.GetAddressOf()));
	

	// Create fixed transform style
	D2D1_STROKE_STYLE_PROPERTIES1 fixed = D2D1::StrokeStyleProperties1(
		D2D1_CAP_STYLE_FLAT,				// The start cap.
		D2D1_CAP_STYLE_FLAT,				// The end cap.
		D2D1_CAP_STYLE_FLAT,				// The dash cap.
		D2D1_LINE_JOIN_MITER,				// The line join.
		10.0f,								// The miter limit.
		D2D1_DASH_STYLE_SOLID,				// The dash style.
		0.0f,								// The dash offset.
		D2D1_STROKE_TRANSFORM_TYPE_FIXED	// The transform type.
	);
	HR(d2Factory->CreateStrokeStyle(fixed, NULL, 0, pFixedTransformStyle.GetAddressOf()));

	// Create hairline style
	D2D1_STROKE_STYLE_PROPERTIES1 hairline = D2D1::StrokeStyleProperties1(
		D2D1_CAP_STYLE_FLAT,				// The start cap.
		D2D1_CAP_STYLE_FLAT,				// The end cap.
		D2D1_CAP_STYLE_FLAT,				// The dash cap.
		D2D1_LINE_JOIN_MITER,				// The line join.
		10.0f,								// The miter limit.
		D2D1_DASH_STYLE_SOLID,				// The dash style.
		0.0f,								// The dash offset.
		D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE	// The transform type.
	);
	HR(d2Factory->CreateStrokeStyle(hairline, NULL, 0, pHairlineStyle.GetAddressOf()));
	
	// Create a DirectWrite factory
	HR(DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pDWriteFactory),
			reinterpret_cast<IUnknown**>(pDWriteFactory.GetAddressOf())
	));

	// Create text formats
	for (int i = 0; i < nFormats; i++)
	{
		HR(pDWriteFactory->CreateTextFormat(
			L"Segoe UI",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			FontSize(static_cast<D2Objects::Formats>(i)),
			L"", //locale
			pTextFormats[i].GetAddressOf()
		));

		// Set default alignment
		if (i == Segoe10)
		{
			pTextFormats[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			pTextFormats[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		}
		else if (i == Segoe18)
		{
			pTextFormats[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextFormats[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
		else
		{
			pTextFormats[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			pTextFormats[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}
}


// Resizes m_swapChain, recreates surface and bitmap
void D2Objects::CreateGraphicsResources(HWND hwnd)
{
	// I.e. DiscardGraphicsResources() was called
	if (bitmap == NULL)
	{
		// Get client rect
		RECT rect;
		GetClientRect(hwnd, &rect);

		// Resize the buffers
		HR(m_swapChain->ResizeBuffers(
			0, // Keep same number of buffers 
			rect.right - rect.left, // Width
			rect.bottom - rect.top, // Height
			DXGI_FORMAT_UNKNOWN, // Keep same format
			0 // Flags
		));

		// Retrieve the swap chain's back buffer
		HR(m_swapChain->GetBuffer(
			0, // index
			__uuidof(surface),
			reinterpret_cast<void**>(surface.GetAddressOf())
		));

		// Create a Direct2D bitmap that points to the swap chain surface
		D2D1_BITMAP_PROPERTIES1 properties = {};
		properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		properties.dpiX = DPIScale::DPIX();
		properties.dpiY = DPIScale::DPIY();
		HR(deviceContext->CreateBitmapFromDxgiSurface(
			surface.Get(),
			properties,
			bitmap.GetAddressOf()
		));

		// Point the device context to the bitmap for rendering
		deviceContext->SetTarget(bitmap.Get());
	}
}


void D2Objects::DiscardGraphicsResources()
{
	deviceContext->SetTarget(NULL);
	bitmap.Reset();
	surface.Reset();
}

