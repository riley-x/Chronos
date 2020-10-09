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

private:

	D2Objects m_d2;


	LRESULT OnCreate();
	LRESULT OnPaint();
	LRESULT OnSize();
};