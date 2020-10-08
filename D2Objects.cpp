#include "pch.h"
#include "D2Objects.h"
#include "DPIScale.h"

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

struct ComException
{
	HRESULT result;
	ComException(HRESULT const value) :
		result(value)
	{}
};
void HR(HRESULT const result)
{
	if (S_OK != result)
	{
		throw ComException(result);
	}
}


void D2Objects::CreateLifetimeResources(HWND window)
{
	m_hwndParent = window;

	HR(D3D11CreateDevice(nullptr,    // Adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,    // Module
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, // Highest available feature level
		D3D11_SDK_VERSION,
		&direct3dDevice,
		nullptr,    // Actual feature level
		nullptr));  // Device context

	HR(direct3dDevice.As(&dxgiDevice));

	HR(CreateDXGIFactory2(
		DXGI_CREATE_FACTORY_DEBUG,
		__uuidof(dxFactory),
		reinterpret_cast<void**>(dxFactory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 description = {};
	description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount = 2;
	description.SampleDesc.Count = 1;
	description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

	RECT rect = {};
	GetClientRect(window, &rect);
	description.Width = rect.right - rect.left;
	description.Height = rect.bottom - rect.top;

	HR(dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(),
		&description,
		nullptr, // Don’t restrict
		swapChain.GetAddressOf()));

	// Create a single-threaded Direct2D factory with debugging information
	D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		d2Factory.GetAddressOf()));
	// Create the Direct2D device that links back to the Direct3D device
	HR(d2Factory->CreateDevice(dxgiDevice.Get(),
		d2Device.GetAddressOf()));
	// Create the Direct2D device context that is the actual render target
	// and exposes drawing commands
	HR(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		deviceContext.GetAddressOf()));
	// Retrieve the swap chain's back buffer
	HR(swapChain->GetBuffer(
		0, // index
		__uuidof(surface),
		reinterpret_cast<void**>(surface.GetAddressOf())));
	// Create a Direct2D bitmap that points to the swap chain surface
	D2D1_BITMAP_PROPERTIES1 properties = {};
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET |
		D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	HR(deviceContext->CreateBitmapFromDxgiSurface(surface.Get(),
		properties,
		bitmap.GetAddressOf()));
	// Point the device context to the bitmap for rendering
	deviceContext->SetTarget(bitmap.Get());

	HR(DCompositionCreateDevice(
		dxgiDevice.Get(),
		__uuidof(dcompDevice),
		reinterpret_cast<void**>(dcompDevice.GetAddressOf())));


	HR(dcompDevice->CreateTargetForHwnd(window,
		true, // Top most
		target.GetAddressOf()));

	HR(dcompDevice->CreateVisual(visual.GetAddressOf()));
	HR(visual->SetContent(swapChain.Get()));
	HR(target->SetRoot(visual.Get()));
	HR(dcompDevice->Commit());

	D2D1_COLOR_F const brushColor = D2D1::ColorF(0.18f,  // red
		0.55f,  // green
		0.34f,  // blue
		0.75f); // alpha
	HR(deviceContext->CreateSolidColorBrush(brushColor,
		solidBrush.GetAddressOf()));
}


HRESULT D2Objects::CreateLifetimeResources(HWND hwnd)
{
	// Initialize DPI settings
	DPIScale::Initialize(hwnd);

	// Create a Direct2D factory
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1),
		NULL,
		reinterpret_cast<void**>(&pFactory)
	);


	// This flag adds support for surfaces with a different color channel ordering than the API default.
	// You need it for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// This array defines the set of DirectX hardware feature levels this app supports.
	// The ordering is important and you should preserve it.
	// Don't forget to declare your app's minimum required feature level in its
	// description.  All apps are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	D3D_FEATURE_LEVEL returnedFeatureLevel;
	if (SUCCEEDED(hr))
		hr = D3D11CreateDevice(
			nullptr,                    // specify null to use the default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,              // optionally set debug and Direct2D compatibility flags
			featureLevels,              // list of feature levels this app can support
			ARRAYSIZE(featureLevels),   // number of possible feature levels
			D3D11_SDK_VERSION,
			&device,                    // returns the Direct3D device created
			&returnedFeatureLevel,      // returns feature level of device created
			&context                    // returns the device immediate context
		);

	// Get underlying interfaces for D3D device
	if (SUCCEEDED(hr))
		hr = device->QueryInterface(__uuidof(ID3D11Device1), (void**)&pDirect3DDevice);
	if (SUCCEEDED(hr))
		hr = context->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&pDirect3DContext);

	// Obtain the underlying DXGI device of the Direct3D11 device.
	ComPtr<IDXGIDevice> dxgiDevice;
	if (SUCCEEDED(hr))
		hr = device.As(&dxgiDevice);

	// Obtain the Direct2D device for 2-D rendering.
	if (SUCCEEDED(hr))
		hr = pFactory->CreateDevice(dxgiDevice.Get(), &pDirect2DDevice);

	// Get Direct2D device's corresponding device context object.
	if (SUCCEEDED(hr))
		hr = pDirect2DDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&pD2DContext
		);

	// Set the DPI
	if (SUCCEEDED(hr))
		pD2DContext->SetDpi(DPIScale::DPIX(), DPIScale::DPIY());

	// Identify the physical adapter (GPU or card) this device is runs on.
	ComPtr<IDXGIAdapter> dxgiAdapter;
	if (SUCCEEDED(hr))
		hr = dxgiDevice->GetAdapter(&dxgiAdapter);

	// Get the factory object that created the DXGI device.
	ComPtr<IDXGIFactory2> dxgiFactory;
	if (SUCCEEDED(hr))
		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

	// Allocate a descriptor.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = 0; // use automatic sizing
	swapChainDesc.Height = 0;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1; // don't use multi-sampling
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1; // Only render one frame at a time, so only need one buffer
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // see https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/dxgi-flip-model
	swapChainDesc.Flags = 0;
	//swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

	// Create DXGI swap chain targeting a window handle
	if (SUCCEEDED(hr))
		hr = dxgiFactory->CreateSwapChainForHwnd(pDirect3DDevice, hwnd, &swapChainDesc, nullptr, nullptr, &pDXGISwapChain);

	// Create dashed stroke style
	if (SUCCEEDED(hr))
	{
		D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties = D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,		// The start cap.
			D2D1_CAP_STYLE_FLAT,		// The end cap.
			D2D1_CAP_STYLE_FLAT,		// The dash cap.
			D2D1_LINE_JOIN_MITER,		// The line join.
			10.0f,						// The miter limit.
			D2D1_DASH_STYLE_DASH,		// The dash style.
			0.0f						// The dash offset.
		);

		hr = pFactory->CreateStrokeStyle(strokeStyleProperties, NULL, 0, &pDashedStyle);
	}
	// Create fixed transform style
	if (SUCCEEDED(hr))
	{
		D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1(
			D2D1_CAP_STYLE_FLAT,				// The start cap.
			D2D1_CAP_STYLE_FLAT,				// The end cap.
			D2D1_CAP_STYLE_FLAT,				// The dash cap.
			D2D1_LINE_JOIN_MITER,				// The line join.
			10.0f,								// The miter limit.
			D2D1_DASH_STYLE_SOLID,				// The dash style.
			0.0f,								// The dash offset.
			D2D1_STROKE_TRANSFORM_TYPE_FIXED	// The transform type.
		);

		hr = pFactory->CreateStrokeStyle(strokeStyleProperties, NULL, 0, &pFixedTransformStyle);
	}
	// Create hairline style
	if (SUCCEEDED(hr))
	{
		D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1(
			D2D1_CAP_STYLE_FLAT,				// The start cap.
			D2D1_CAP_STYLE_FLAT,				// The end cap.
			D2D1_CAP_STYLE_FLAT,				// The dash cap.
			D2D1_LINE_JOIN_MITER,				// The line join.
			10.0f,								// The miter limit.
			D2D1_DASH_STYLE_SOLID,				// The dash style.
			0.0f,								// The dash offset.
			D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE	// The transform type.
		);

		hr = pFactory->CreateStrokeStyle(strokeStyleProperties, NULL, 0, &pHairlineStyle);
	}
	// Create a DirectWrite factory
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);
	}
	// Create text formats
	for (int i = 0; i < nFormats; i++)
	{
		IDWriteTextFormat** ppTextFormat = &pTextFormats[i];
		if (SUCCEEDED(hr))
		{
			hr = pDWriteFactory->CreateTextFormat(
				L"Segoe UI",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				FontSize(static_cast<D2Objects::Formats>(i)),
				L"", //locale
				ppTextFormat
			);
		}
		// Set default alignment
		if (SUCCEEDED(hr))
		{
			if (i == Segoe10)
			{
				(*ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				(*ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			}
			else if (i == Segoe18)
			{
				(*ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				(*ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
			else
			{
				(*ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				(*ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
	}
	
	// Create main brush
	if (SUCCEEDED(hr))
	{
		const D2D1_COLOR_F color = D2D1::ColorF(0.15f, 0.15f, 0.16f);
		hr = pD2DContext->CreateSolidColorBrush(color, &pBrush);
	}

	D2D1_PIXEL_FORMAT format = pD2DContext->GetPixelFormat();

	return hr;
}

// Creates pDXGISwapChain, pDirect2DBackBuffer
HRESULT D2Objects::CreateGraphicsResources(HWND hwnd)
{
	HRESULT hr = S_OK;
	if (pDirect2DBackBuffer == NULL)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		D2D1_RECT_F dipRect = DPIScale::PixelsToDips(rc);

		// Resize the buffers (automatically fits client window)
		hr = pDXGISwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		// Now we set up the Direct2D render target bitmap linked to the swapchain. 
		// Whenever we render to this bitmap, it is directly rendered to the 
		// swap chain associated with the window.
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			DPIScale::DPIX(),
			DPIScale::DPIY()
		);

		// Direct2D needs the dxgi version of the backbuffer surface pointer.
		ComPtr<IDXGISurface> dxgiBackBuffer;
		if (SUCCEEDED(hr))
			hr = pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

		// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
		if (SUCCEEDED(hr))
			hr = pD2DContext->CreateBitmapFromDxgiSurface(
				dxgiBackBuffer.Get(),
				&bitmapProperties,
				&pDirect2DBackBuffer
			);

		// Now we can set the Direct2D render target.
		if (SUCCEEDED(hr))
			pD2DContext->SetTarget(pDirect2DBackBuffer);
	}

	return hr;
}



void D2Objects::DiscardLifetimeResources()
{
	SafeRelease(&pDXGISwapChain);
	SafeRelease(&pD2DContext);
	SafeRelease(&pDirect2DDevice);
	SafeRelease(&pDirect3DContext);
	SafeRelease(&pDirect3DDevice);
	SafeRelease(&pFactory);

	SafeRelease(&pDashedStyle);
	SafeRelease(&pFixedTransformStyle);
	SafeRelease(&pHairlineStyle);

	SafeRelease(&pDWriteFactory);
	for (int i = 0; i < nFormats; i++)
	{
		SafeRelease(&pTextFormats[i]);
	}

	SafeRelease(&pBrush);
}

void D2Objects::DiscardGraphicsResources()
{
	pD2DContext->SetTarget(NULL);
	SafeRelease(&pDirect2DBackBuffer);
}

