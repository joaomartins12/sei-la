// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// MapTool.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"


CsGBTerrainMng	g_MapMng;

bool IsClientRect()
{
	CPoint pt;
	GetCursorPos( &pt );
	GET_MV_VIEW->ScreenToClient( &pt );

	CRect viewRect;
	GET_MV_VIEWFRAME->GetWindowRect( &viewRect );

	CRect rc = CRect( 0, 0, viewRect.Width(), viewRect.Height() );

	return ( PtInRect( &rc, pt ) == TRUE );
}
