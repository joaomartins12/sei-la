
#include "stdafx.h"
#include "_DlgHeader.h"

int		_Control2Int( CWnd* pWnd )
{
	static TCHAR str[ 32 ] = {0, };
	pWnd->GetWindowText( str, 32 );
	return _ttoi( str );
}

float	_Control2float( CWnd* pWnd )
{
	static TCHAR str[ 32 ] = {0, };
	pWnd->GetWindowText( str, 32 );
	return (float)_tstof( str );
}

void	_Int2Control( CWnd* pWnd, int nValue )
{
	static TCHAR str[ 32 ] = {0, };
	_stprintf_s( str, 32, L"%d", nValue );
	pWnd->SetWindowText( str );
}

void	_Float2Control( CWnd* pWnd, float fValue )
{
	static TCHAR str[ 32 ] = {0, };
	_stprintf_s( str, 32, L"%.2f", fValue );
	pWnd->SetWindowText( str );
}

void	_Float2Control( CWnd* pWnd, float fValue, int nCommaPos )
{
	static TCHAR str[ 32 ] = {0, };
	switch( nCommaPos )
	{
	case 1:		_stprintf_s( str, 32, L"%.1f", fValue );	break;
	case 2:		_stprintf_s( str, 32, L"%.2f", fValue );	break;
	case 3:		_stprintf_s( str, 32, L"%.3f", fValue );	break;
	case 4:		_stprintf_s( str, 32, L"%.4f", fValue );	break;
	case 5:		_stprintf_s( str, 32, L"%.5f", fValue );	break;
	default:	assert_cs( false );
	}
	
	pWnd->SetWindowText( str );
}

void	_ComboSelect( CComboBox* pCombo, int nValue )
{
	static TCHAR str[ 32 ] = {0, };
	int nCount = pCombo->GetCount();
	for( int i=0; i<nCount; ++i )
	{
		pCombo->SetCurSel( i );
		pCombo->GetWindowText( str, 32 );
		if( nValue == _ttoi( str ) )
			return;
	}
	pCombo->SetCurSel( 0 );
}

void	_ComboSelect( CComboBox* pCombo, float fValue )
{
	static TCHAR str[ 32 ] = {0, };
	int nCount = pCombo->GetCount();
	for( int i=0; i<nCount; ++i )
	{
		pCombo->SetCurSel( i );
		pCombo->GetWindowText( str, 32 );
		if( abs( _tstof( str ) - fValue ) < 0.000005f )
			return;
	}
	pCombo->SetCurSel( 0 );
}

