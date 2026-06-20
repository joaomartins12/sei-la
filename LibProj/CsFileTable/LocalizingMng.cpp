
#include "stdafx.h"
#include "LocalizingMng.h"
#include <ShellAPI.h>


void CsLocalizingMng::Delete()
{
	CsLocalizing::LIST_IT it = m_List.begin();
	CsLocalizing::LIST_IT itEnd = m_List.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete *it;
	}
	m_List.clear();
}

void CsLocalizingMng::Init()
{
	_LoadExcel();
}

void CsLocalizingMng::_LoadExcel()
{
	_LoadExcel( "Word" );
	_LoadExcel( "String" );
}

void CsLocalizingMng::_LoadExcel( char* cSheet )
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Localizing(), cSheet );
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	

	TCHAR szKor[ CEXCLE_CELL_LEN ];
	TCHAR szEng[ CEXCLE_CELL_LEN ];
	wchar_t szThai[ CEXCLE_CELL_LEN ];

	while( excel.MoveNext() == true )
	{
		// ID			
		if( atoi( excel.GetData( 0, cBuffer ) ) == 0 )
			continue;

		// str
		M2W( szKor, excel.GetData( 1, cBuffer ), CEXCLE_CELL_LEN );
		M2W( szEng, excel.GetData( 2, cBuffer ), CEXCLE_CELL_LEN );
		wcscpy_s( szThai, CEXCLE_CELL_LEN, excel.GetData( 3, wBuffer ) );

		CsLocalizing* pData = csnew CsLocalizing;
		pData->Init( szKor, szEng, szThai);

		m_List.push_back( pData );
	}
}
