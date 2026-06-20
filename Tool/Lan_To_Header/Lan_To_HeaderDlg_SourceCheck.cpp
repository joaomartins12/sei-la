

#include "stdafx.h"
#include "Lan_To_HeaderDlg.h"


void CLan_To_HeaderDlg::__SourceCheck()
{
	CString strDrt( m_Resist.s_szWorkDir );	
	__FindCheckSubDir( strDrt, m_CheckFileArray );

	// 리스트 복사
	FILE* fpWrite = NULL;
	TCHAR	szTitle[ MAX_PATH ];
	_tfopen_s( &fpWrite, L"ToolData\\Lan_To_Header.txt", L"wt, ccs=UNICODE" );
	_stprintf_s( szTitle, MAX_PATH, L"\n------ [ 엑셀에 중복 셋팅 ] ------\n" );
	fwrite( szTitle, sizeof( TCHAR ), _tcslen( szTitle ) , fpWrite );
	bool bRewrite = false;

	CsLocalizing::MAP mapLocal;
	CsLocalizing::MAP mapLocalOrg;
	CsLocalizing::LIST* pList = nsCsFileTable::g_pLocalizingMng->GetList();
	CsLocalizing::LIST_IT it = pList->begin();
	CsLocalizing::LIST_IT itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		CsLocalizing* pData = csnew CsLocalizing;
		pData->Init( (TCHAR*)(*it)->GetInfo()->s_szKor.data(), (TCHAR*)(*it)->GetInfo()->s_szEng.data(), (wchar_t*)(*it)->GetInfo()->s_szThai.data() );
		DWORD nHash = (DWORD)CsFileHash::GetHashCode( (TCHAR*)pData->GetInfo()->s_szKor.data() );
		if( mapLocalOrg.find( nHash ) == mapLocalOrg.end() )
		{
			mapLocal[ nHash ] = pData;
			mapLocalOrg[ nHash ] = pData;
		}
		else
		{
			TCHAR szMessage[ 2048 ];
			_stprintf_s( szMessage, 2048, L"%s : %s\n", (TCHAR*)(*it)->GetInfo()->s_szKor.data(), mapLocalOrg[ nHash ]->GetInfo()->s_szKor.data() );
			fwrite( szMessage, sizeof( TCHAR ), _tcslen( szMessage ) , fpWrite );
			bRewrite = true;
		}
	}

	_stprintf_s( szTitle, MAX_PATH, L"\n\n------ [ 엑셀에 없음 ] ------\n" );
	fwrite( szTitle, sizeof( TCHAR ), _tcslen( szTitle ) , fpWrite );

	for( int i = 0; i < m_CheckFileArray.GetSize(); i++ )
	{
		__Serch_SourceExist( fpWrite, &mapLocalOrg, &mapLocal, &m_CheckFileArray[ i ] );
	}

	_stprintf_s( szTitle, MAX_PATH, L"\n\n------ [ 엑셀상에만 존재 ] ------\n" );
	fwrite( szTitle, sizeof( TCHAR ), _tcslen( szTitle ) , fpWrite );

	CsLocalizing::MAP_IT itLocal = mapLocal.begin();
	CsLocalizing::MAP_IT itLocalEnd = mapLocal.end();
	for( ; itLocal != itLocalEnd; ++itLocal )
	{
		_stprintf_s( szTitle, MAX_PATH, L"%s\n", itLocal->second->GetInfo()->s_szKor.data() );
		fwrite( szTitle, sizeof( TCHAR ), _tcslen( szTitle ) , fpWrite );
	}
	mapLocal.clear();

	itLocal = mapLocalOrg.begin();
	itLocalEnd = mapLocalOrg.end();
	for( ; itLocal != itLocalEnd; ++itLocal )
	{
		SAFE_DELETE( itLocal->second );		
	}
	mapLocalOrg.clear();


	fclose( fpWrite );	
	ShellExecute( NULL, _T( "open" ), _T( "notepad.exe" ) , _T( "ToolData\\Lan_To_Header.txt" ), NULL, SW_SHOW );
}

void CLan_To_HeaderDlg::__Serch_SourceExist( FILE* fp, CsLocalizing::MAP* pMapOrg, CsLocalizing::MAP* pMap, CString* pFile )
{	
	sWRITE_INFO wi;
	wi.s_pWriteFile	= fp;
	wi.s_nLine		= 1;
	wi.s_szFileName	= pFile->GetBuffer( pFile->GetLength() );

	char cLine[ 2048 ];	

	FILE* fpFile = NULL;	
	_tfopen_s( &fpFile, wi.s_szFileName, L"rt" );	

	while( feof( fpFile ) == 0 )
	{
		fgets( cLine, 2048, fpFile );
		char* pSerch = strstr( cLine, "_LAN(" );
		if( pSerch != NULL )
		{
			__Serch_SourceExist_Line( &wi, pMapOrg, pMap, cLine );
		}
		++wi.s_nLine;
	}

	fclose( fpFile );	
}

void CLan_To_HeaderDlg::__Serch_SourceExist_Line( sWRITE_INFO* wi, CsLocalizing::MAP* pMapOrg, CsLocalizing::MAP* pMap, char* cLine )
{
	assert_cs( strlen( cLine ) < 2000 );
	
	TCHAR szToken[ 2048 ] = { 0, };
	TCHAR szLocal[ 2048 ] = { 0, };	
	TCHAR szOrgLocal[ 2048 ] = { 0, };	
	M2W( szOrgLocal, cLine, 2048 );

	TCHAR* p = _tcsstr( szOrgLocal, L"_LAN(" );

	while( p != NULL )
	{
		bool bStart = false;				
		int nIdx = 0;
		int nTokenIdx = 0;
		if( p == NULL )
			break;

		_tcscpy_s( szLocal, 2048, p );

		while( 1 )
		{
			TCHAR szWord = szLocal[ nIdx ];
			if( szWord == NULL )
			{
				nIdx = -1;
				break;
			}

			if( szWord == L'"') 
			{
				if( bStart == true )					
					break;	
			}

			if( bStart == true )
			{						
				szToken[ nTokenIdx ] = szWord;						
				nTokenIdx++;
			}	

			if( szWord == L'"') 
			{				
				bStart = true;						
			}
			nIdx++;							
		}

		if( nIdx == -1 )
			break;

		CsLocalizing::MAP_IT itFind = pMap->find( (DWORD)CsFileHash::GetHashCode( szToken ) );
		// 소스 상에는 존재 하나 엑셀상에는 존재 하지 않는다면 문제
		if( itFind == pMap->end() )
		{
			if( pMapOrg->find( (DWORD)CsFileHash::GetHashCode( szToken ) ) == pMapOrg->end() )
			{
				TCHAR szMessage[ 2048 ];
				_stprintf_s( szMessage, 2048, L"%s - Line : %d     내용 : %s\n", _tcsrchr( wi->s_szFileName, L'\\' ), wi->s_nLine, szToken );
				fwrite( szMessage, sizeof( TCHAR ), _tcslen( szMessage ) , wi->s_pWriteFile );
			}
		}
		else
		{
			pMap->erase( itFind );
		}

		memset( szToken, 0, sizeof( szToken ) );
		p = _tcsstr( &szLocal[ 1 ], L"_LAN(" );
	}	
}

//void CLan_To_HeaderDlg::__SetCheckDir()
//{
//	BROWSEINFOW bi;
//	memset( &bi, 0, sizeof( BROWSEINFOW ) );
//	bi.ulFlags = BIF_NEWDIALOGSTYLE;
//
//	LPITEMIDLIST pIDL = ::SHBrowseForFolder( &bi );
//	SHGetPathFromIDList( pIDL, m_CheckDir );
//}

void CLan_To_HeaderDlg::__FindCheckSubDir(CString strDir, CStringArray &FileArray )
{
	strDir += "\\*.*";	

	CFileFind ff;
	BOOL bFound = ff.FindFile(strDir);	

	while(bFound)
	{
		bFound = ff.FindNextFile();

		if(ff.IsDots()) continue;
		if(ff.IsDirectory())		
		{
			if( ff.GetFilePath().Find( _T( ".svn") ) != -1 )
				continue;
			if( ff.GetFilePath().Find( _T( "Debug") ) != -1 )
				continue;
			if( ff.GetFilePath().Find( _T( "Release") ) != -1 )
				continue;
			if( ff.GetFilePath().Find( _T( "Give") ) != -1 )
				continue;
			__FindCheckSubDir( ff.GetFilePath(), FileArray );					
		}
		else	
		{
			if( ff.GetFilePath().Find( _T("_Lan_") ) != -1 )
				continue;

			if( ff.GetFilePath().Find( _T(".cpp") ) != -1 )				
			{			
				FileArray.Add( ff.GetFilePath() );				
			}
			if( ff.GetFilePath().Find( _T(".h") ) != -1 )				
			{			
				FileArray.Add( ff.GetFilePath() );				
			}
		}
	}
}

