
#pragma once

#include <math.h>

int		_Control2Int( CWnd* pWnd );
float	_Control2float( CWnd* pWnd );
void	_Int2Control( CWnd* pWnd, int nValue );
void	_Float2Control( CWnd* pWnd, float fValue );
void	_ComboSelect( CComboBox* pCombo, int nValue );
void	_ComboSelect( CComboBox* pCombo, float fValue );
void	_ComboSelect_DWORD_Data( CComboBox* pCombo, DWORD dwSrcData );