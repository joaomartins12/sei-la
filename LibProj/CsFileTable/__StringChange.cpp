

#include "stdafx.h"
#include "__StringChange.h"


void __CsStringChange::Delete()
{
	// 메모리 제거
	std::list< sSTRING_CHANGE* >::iterator it = m_List.begin();
	std::list< sSTRING_CHANGE* >::iterator itEnd = m_List.end();
	for( ; it!=itEnd; ++it )
		delete *it;
	m_List.clear();
}

void __CsStringChange::Load( nsCsFileTable::eLANGUAGE language )
{
	// 스킬명만 변경 된다. 
	// 스킬테이블만 먼저 바꾸어 주자

	char cExcel[ MAX_FILENAME ] = "ToolData\\DM_StringTable.xls";
	char cSheet[ MAX_FILENAME ] = "Skill";

	assert_cs( m_List.empty() == true );
	_LoadStringChange( cExcel, cSheet );
}

void __CsStringChange::_LoadStringChange( char* cExcel, char* cSheet )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cExcel );
		return;
	}

#define IL_COL_ID					0
#define IL_COL_SRC					1
#define IL_COL_DEST					2

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, cSheet );
	if( bSuccess == false )
	{
		CsMessageBoxA( MB_OK, "%s 시트 네임이 잘못되었습니다.", cSheet );
		return;
	}
	// 1줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	

	while( excel.MoveNext() == true )
	{
		// ID
		if( atoi( excel.GetData( IL_COL_ID, cBuffer ) ) == 0 )
			continue;

		sSTRING_CHANGE* pData = csnew sSTRING_CHANGE;

		// Src
		M2W( pData->s_Src, excel.GetData( IL_COL_SRC, cBuffer ), STRING_CHANGE_LEN );

		// Dest
		M2W( pData->s_Dest, excel.GetData( IL_COL_DEST, cBuffer ), STRING_CHANGE_LEN );

		m_List.push_back( pData );
	}
	excel.Close();
}

void __CsStringChange::Change( TCHAR* sz, int nLen )
{
	assert_cs( nLen < 1024 );
	TCHAR szSwap[ 1024 ];

	std::list< sSTRING_CHANGE* >::iterator it = m_List.begin();
	std::list< sSTRING_CHANGE* >::iterator itEnd = m_List.end();

	sSTRING_CHANGE* pData;

	for( ; it!=itEnd; ++it )
	{
		pData = *it;
		TCHAR* pFind = _tcsstr( sz, pData->s_Src );
		if( pFind != NULL )
		{
			int nTotalLen = (int)_tcslen( sz );
			int nFindLen = (int)_tcslen( pFind );
			int nSrcLen = (int)_tcslen( pData->s_Src );
			int nFindPosLen = nTotalLen - nFindLen;

			assert_cs( nTotalLen >= nFindLen );

			_tcsncpy_s( szSwap, 1024, sz, nFindPosLen );
			_tcscat_s( szSwap, 1024, pData->s_Dest );
			_tcscat_s( szSwap, 1024, &sz[ nFindPosLen + nSrcLen ] );

			_tcscpy_s( sz, nLen, szSwap );
		}
	}
}

TCHAR* __CsStringChange::NewLineCheck( TCHAR* sz )
{
	size_t len = _tcslen( sz );
	if( len == 0 )
		return sz;

	for( UINT i=0; i<len; ++i )
	{
		if( _tcsncmp( &sz[ i ], _T( "\\n" ), 2 ) == 0 )
		{
			sz[ i ] = L'\n';
			for( size_t n=i+1; n<len-1; ++n )
			{
				sz[ n ] = sz[ n + 1 ];
			}
			sz[ len-1 ] = NULL;
			--len;
		}		
	}
	return sz;
}