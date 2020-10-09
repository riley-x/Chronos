#pragma once

#include "pch.h"
#include "Resource.h"
#include "BaseWindow.h"
#include "D2Objects.h"

class Chronos : public BaseWindow<Chronos>
{
public:

	Chronos(PCWSTR szClassName);
	~Chronos();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	enum Timers { sec1 = 100, nTimers };

private:

	D2Objects m_d2;

	int m_minute = 0;
	int m_second = 0;

	LRESULT OnCreate();
	LRESULT OnPaint();
	LRESULT OnSize();
	LRESULT OnLButtonDown(D2D1_POINT_2F point, WPARAM wParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(POINT point);

	std::wstring time() const;
	void ResetTimer();
};