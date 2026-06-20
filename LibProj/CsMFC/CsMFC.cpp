
#include "stdafx.h"
#include "CsMFC.h"

void CsMFC::Init()
{
	ResetScreen();
}

void CsMFC::ResetScreen()
{
	CsSpinCtrl::m_nDispHeight = GetSystemMetrics(SM_CYSCREEN);
}