#pragma once

#include "pch.h"

inline void OutputMessage(const std::wstring format, ...)
{
	wchar_t msg[1024];
	va_list args;
	va_start(args, format);
	vswprintf_s(msg, format.c_str(), args);
	va_end(args);

	OutputDebugString(msg);
}
inline void OutputHRerr(HRESULT hr, const std::wstring& msg)
{
	_com_error err(hr);
	OutputMessage(msg + L": (0x%lx) %s\n", hr, err.ErrorMessage());
}

inline void HR(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_error err(hr);
		std::stringstream ss;
		ss << std::hex << std::showbase << hr;
		ss << " " << err.ErrorMessage();
		throw std::runtime_error(ss.str());
	}
}