#pragma once

#include "pch.h"

class DPIScale
{
	static float scaleX;
	static float scaleY;
	static float dpiX;
	static float dpiY;

	// Half / full pixel in DIPs
	static float halfPX;
	static float halfPY;
	static float fullPX;
	static float fullPY;

public:

	static inline void Initialize(HWND hwnd)
	{
		dpiX = static_cast<float>(GetDpiForWindow(hwnd));
		//dpiX = 96;
		dpiY = dpiX;
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;

		halfPX = PixelsToDipsX(0.5);
		halfPY = PixelsToDipsY(0.5);
		fullPX = PixelsToDipsX(1);
		fullPY = PixelsToDipsY(1);
	}

	static inline float DPIX() { return dpiX; }
	static inline float DPIY() { return dpiY; }

	static inline D2D1_POINT_2F PixelsToDips(POINT p)
	{
		return D2D1::Point2F(static_cast<float>(p.x) / scaleX, static_cast<float>(p.y) / scaleY);
	}

	static inline D2D1_RECT_F PixelsToDips(RECT rc)
	{
		return D2D1::RectF(
			PixelsToDipsX(rc.left),
			PixelsToDipsY(rc.top),
			PixelsToDipsX(rc.right),
			PixelsToDipsY(rc.bottom)
		);
	}

	template <typename T>
	static inline float PixelsToDipsX(T x)
	{
		return static_cast<float>(x) / scaleX;
	}

	template <typename T>
	static inline float PixelsToDipsY(T y)
	{
		return static_cast<float>(y) / scaleY;
	}

	static inline POINT DipsToPixels(D2D1_POINT_2F point)
	{
		POINT out;
		out.x = DipsToPixelsX(point.x);
		out.y = DipsToPixelsY(point.y);
		return out;
	}

	static inline RECT DipsToPixels(D2D1_RECT_F rect)
	{
		RECT out;
		out.left = DipsToPixelsX(rect.left);
		out.top = DipsToPixelsY(rect.top);
		out.right = DipsToPixelsX(rect.right);
		out.bottom = DipsToPixelsY(rect.bottom);
		return out;
	}

	template <typename T>
	static inline int DipsToPixelsX(T x)
	{
		return static_cast<int>(static_cast<float>(x) * scaleX);
	}

	template <typename T>
	static inline int DipsToPixelsY(T y)
	{
		return static_cast<int>(static_cast<float>(y) * scaleY);
	}


	static inline float SnapToPixelX(float x)
	{
		return roundf(x * scaleX) / scaleX;
	}

	static inline float SnapToPixelY(float y)
	{
		return roundf(y * scaleY) / scaleY;
	}

	static inline D2D1_RECT_F SnapToPixel(D2D1_RECT_F rect, bool inwards = true)
	{
		rect.left = SnapToPixelX(rect.left) - (1.0f - 2.0f * inwards) * halfPX;
		rect.top = SnapToPixelY(rect.top) - (1.0f - 2.0f * inwards) * halfPY;
		rect.right = SnapToPixelX(rect.right) + (1.0f - 2.0f * inwards) * halfPX;
		rect.bottom = SnapToPixelY(rect.bottom) + (1.0f - 2.0f * inwards) * halfPY;
		return rect;
	}

	// NOTE: when drawing lines, need to align to 0.5 boundaries. Use hp() below.
	// Alternatively, use D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE in future
	static inline float hpx() { return halfPX; }
	static inline float hpy() { return halfPY; }
	static inline float px() { return fullPX; }
	static inline float py() { return fullPY; }
};