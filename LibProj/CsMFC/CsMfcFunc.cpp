


#include "stdafx.h"
#include "CsMfcFunc.h"

int		nsCsMfcFunc::Control2Int( CWnd* pWnd )
{
	static TCHAR str[ 32 ] = {0, };
	pWnd->GetWindowText( str, 32 );
	return _ttoi( str );
}

float	nsCsMfcFunc::Control2Float( CWnd* pWnd )
{
	static TCHAR str[ 32 ] = {0, };
	pWnd->GetWindowText( str, 32 );
	return (float)_tstof( str );
}

void	nsCsMfcFunc::Int2Control( CWnd* pWnd, int nValue )
{
	static TCHAR str[ 32 ] = {0, };
	_stprintf_s( str, 32, L"%d", nValue );
	pWnd->SetWindowText( str );
}

void	nsCsMfcFunc::Float2Control( CWnd* pWnd, float fValue, int nDecimalPlace /*=2*/ )
{
	static TCHAR str[ 32 ] = {0, };
	switch( nDecimalPlace )
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

void	nsCsMfcFunc::ComboSelect( CComboBox* pCombo, int nValue )
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

void	nsCsMfcFunc::ComboSelect( CComboBox* pCombo, float fValue )
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

void	nsCsMfcFunc::ComboSelect_DWORD_Data( CComboBox* pCombo, DWORD dwSrcData )
{
	DWORD dwData;
	int nCount = pCombo->GetCount();
	for( int i=0; i<nCount; ++i )
	{
		pCombo->SetCurSel( i );
		dwData = (DWORD)pCombo->GetItemData( i );
		if( dwData == dwSrcData )
			return;
	}
	pCombo->SetCurSel( 0 );
}

