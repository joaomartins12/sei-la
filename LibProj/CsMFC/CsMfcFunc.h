

#pragma once 

#include <Math.h>

namespace nsCsMfcFunc
{
	int		Control2Int( CWnd* pWnd );
	float	Control2Float( CWnd* pWnd );
	void	Int2Control( CWnd* pWnd, int nValue );
	void	Float2Control( CWnd* pWnd, float fValue, int nDecimalPlace = 2 );

	void	ComboSelect( CComboBox* pCombo, int nValue );
	void	ComboSelect( CComboBox* pCombo, float fValue );
	void	ComboSelect_DWORD_Data( CComboBox* pCombo, DWORD dwSrcData );
}