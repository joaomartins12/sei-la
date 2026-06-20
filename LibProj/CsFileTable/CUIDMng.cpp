
#include "stdafx.h"
#include "CuidMng.h"

#include <string>
#include <set>

void CsCuidMng::Delete()
{
	{
		CsCuid::LIST_IT it = m_List.begin();
		CsCuid::LIST_IT itEnd = m_List.end();
		for( ; it!=itEnd; ++it )
		{
			(*it)->Delete();
			delete *it;
		}
		m_List.clear();
	}	

	{
		CsCuid_Chat::LIST_IT it = m_ChatList.begin();
		CsCuid_Chat::LIST_IT itEnd = m_ChatList.end();
		for( ; it!=itEnd; ++it )
		{
			(*it)->Delete();
			delete *it;
		}
		m_ChatList.clear();
	}
	
}

bool CsCuidMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel_ID( "ID" );
		_LoadExcel_Chat( "Chatting" );
		break;
	case nsCsFileTable::FT_BIN:
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
	}
	return true;
}

bool CsCuidMng::CheckID( TCHAR const* sName )
{
	TCHAR* str = NULL;
	_tcscpy_s( m_CheckStr, 2048, sName );
	_tcslwr_s( m_CheckStr, _tcslen( sName ) + 1 );
	
	{		
		CsCuid::sINFO* pInfo;
		CsCuid::LIST_IT it = m_List.begin();
		CsCuid::LIST_IT itEnd = m_List.end();
		for( ; it!=itEnd; ++it )
		{
			pInfo = (*it)->GetInfo();

			switch( pInfo->s_MsgType )		
			{
			case CsCuid::CT_IN:
				{
					str = _tcsstr( m_CheckStr, pInfo->s_Name );
					if( str != NULL )
						return false;
				}
				break;
			case CsCuid::MT_ONLY:
				if( wcscmp( m_CheckStr, pInfo->s_Name ) == 0 )
				{
					return false;
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}

	{
		CsCuid_Chat::sINFO* pInfo;
		CsCuid_Chat::LIST_IT it = m_ChatList.begin();
		CsCuid_Chat::LIST_IT itEnd = m_ChatList.end();

		for( ; it!=itEnd; ++it )
		{
			pInfo = (*it)->GetInfo();

			str = _tcsstr( m_CheckStr, pInfo->s_Name );
			if( str != NULL )
				return false;			
		}
	}

	return true;
}

// [3/3/2016 hyun] 금지어 필터 수정
void CsCuidMng::CheckChat( TCHAR* const szOut )
{
	if(!szOut)
		return;

	// 원본 내용 복사본
	std::wstring inputString = szOut;
	if(inputString.empty())
		return;

	// 소문자 변경
	std::transform(inputString.begin(),inputString.end(),inputString.begin(),tolower);

	// 입력된 스트링중에 일치된 금지어 목록을 가져온다.
	std::vector<std::pair<int,int>> vecFindPos;
	GetBadWordPos(inputString,vecFindPos);
	
	// 원본 문자 다시 대입
	inputString.assign(szOut); 

	std::vector<std::pair<int,int>>::iterator it = vecFindPos.begin();
	for( ; it != vecFindPos.end(); ++it )
		inputString.replace( it->first, it->second, MakeReplaceStr(it->second));

// 	for(int i = 0; i < vecFindPos.size(); ++i) // 바뀐 단어 만큼(입력한 문자열중에 금지어가 몇개 포함되었나)
// 		inputString.replace(vecFindPos[i].first, vecFindPos[i].second, MakeReplaceStr(vecFindPos[i].second));

	// 복사
	wmemcpy_s(szOut, MESSAGE_COPY_SIZE, inputString.data(), inputString.length());
}
//@@ 여기까지

void CsCuidMng::_LoadExcel_ID( char* cSheetName )
{
	#define IL_COL_NAME					1
	#define IL_COL_TYPE					IL_COL_NAME + 1
	
	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_CUID(), cSheetName );
	assert_cs( bSuccess == true );
	// 1줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );		

	CsCuid::sINFO info;
	while( excel.MoveNext() == true )
	{
		//excel.GetData( IL_COL_NAME, cBuffer );
		//assert_csm2( strlen( cBuffer ) < CUIDNAME_LEN, "str=%s (%d)", cBuffer, strlen( cBuffer ) );

		//M2W( info.s_Name, cBuffer, CUIDNAME_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_Name, excel.GetData( IL_COL_NAME, cBuffer ), CUIDNAME_LEN );
			assert_csm2( strlen( cBuffer ) < CUIDNAME_LEN, "str=%s (%d)", cBuffer, strlen( cBuffer ) );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info.s_Name, CUIDNAME_LEN, excel.GetData( IL_COL_NAME, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		if( info.s_Name[ 0 ] == NULL )
			continue;

		info.s_MsgType = (CsCuid::eCUID_TYPE)atoi( excel.GetData( IL_COL_TYPE, cBuffer ) );
		assert_cs( info.s_MsgType != 0 );
		
		CsCuid* pCuid = csnew CsCuid;
		pCuid->Init( &info );

		m_List.push_back( pCuid );
	}
	excel.Close();
}

void CsCuidMng::_LoadExcel_Chat( char* cSheetName )
{

#define IL_COL_NAME					1

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_CUID(), cSheetName );
	assert_cs( bSuccess == true );
	// 1줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );		

	CsCuid_Chat::sINFO info;
	std::set<std::wstring> AddedWordList;

	while( excel.MoveNext() == true )
	{
		//excel.GetData( IL_COL_NAME, cBuffer );
		//assert_csm2( strlen( cBuffer ) < CUID_CHAT_LEN, "str=%s (%d)", cBuffer, strlen( cBuffer ) );
		//M2W( info.s_Name, cBuffer, CUID_CHAT_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_Name, excel.GetData( IL_COL_NAME, cBuffer ), CUID_CHAT_LEN );
			assert_csm2( strlen( cBuffer ) < CUID_CHAT_LEN, "str=%s (%d)", cBuffer, strlen( cBuffer ) );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info.s_Name, CUID_CHAT_LEN, excel.GetData( IL_COL_NAME, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		// [3/7/2016 hyun] 공백 제거
		Trim(info.s_Name,std::wstring(L" \t\n"));

		// [3/7/2016 hyun] 소문자 변경
		Lwr(info.s_Name);

		if( info.s_Name[ 0 ] == NULL)
			continue;

		// [3/7/2016 hyun] 조건 추가. 단어목록에 이미 있다면 추가하지 않는다.
		if(AddedWordList.find(std::wstring(info.s_Name)) != AddedWordList.end())
			continue;

		AddChatList(info);
		// 검사용 목록에 넣어둔다
		AddedWordList.insert(std::wstring(info.s_Name));
		// @@ 여기까지
	}
	// 정렬
	m_ChatList.sort(SortingByLength);

	excel.Close();
}


#define CUID_BIN		"Cuid.bin"

void CsCuidMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CUID_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	{
		int nCount = (int)m_List.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsCuid::LIST_IT it = m_List.begin();
		CsCuid::LIST_IT itEnd = m_List.end();
		for( ; it!=itEnd; ++it )
		{
			_tcslwr_s( (*it)->GetInfo()->s_Name, _tcslen( (*it)->GetInfo()->s_Name ) + 1 );
			fwrite( (*it)->GetInfo(), sizeof( CsCuid::sINFO ), 1, fp );
		}		
	}

	{
		int nCount = (int)m_ChatList.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsCuid_Chat::LIST_IT it = m_ChatList.begin();
		CsCuid_Chat::LIST_IT itEnd = m_ChatList.end();
		for( ; it!=itEnd; ++it )
		{
			_tcslwr_s( (*it)->GetInfo()->s_Name, _tcslen( (*it)->GetInfo()->s_Name ) + 1 );
			fwrite( (*it)->GetInfo(), sizeof( CsCuid_Chat::sINFO ), 1, fp );
		}		
	}
	
	fclose( fp );
}

bool CsCuidMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CUID_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsCuid::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsCuid::sINFO ), 1, fp );

			CsCuid* pObject = csnew CsCuid;
			pObject->Init( &info );
			m_List.push_back( pObject );
		}
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsCuid_Chat::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsCuid_Chat::sINFO ), 1, fp );

			CsCuid_Chat* pObject = csnew CsCuid_Chat;
			pObject->Init( &info );
			m_ChatList.push_back( pObject );
		}
	}

	fclose( fp );
	return true;
}

void CsCuidMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CUID_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsCuid::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsCuid::sINFO ) );

			CsCuid* pObject = csnew CsCuid;
			pObject->Init( &info );
			m_List.push_back( pObject );
		}
	}

	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsCuid_Chat::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsCuid_Chat::sINFO ) );

			CsCuid_Chat* pObject = csnew CsCuid_Chat;
			pObject->Init( &info );
			m_ChatList.push_back( pObject );
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );

}

bool CsCuidMng::SortingByLength( CsCuid_Chat* const leftWord, CsCuid_Chat* const rightWord )
{
	return std::wstring(leftWord->GetInfo()->s_Name).length() > std::wstring(rightWord->GetInfo()->s_Name).length() ;
}

std::wstring CsCuidMng::MakeReplaceStr( int iLength )
{
	std::wstring changeString;
	for(int j = 0; j < iLength; ++j) // 금지어 길이만큼 바꿀 스트링 설정
	{
		TCHAR tempChar;
#ifdef VERSION_TW
#elif VERSION_HK
		tempChar = L'*';
#else
		switch( rand()%7 )
		{
		case 0:		tempChar = L'!';		break;
		case 1:		tempChar = L'@';		break;
		case 2:		tempChar = L'#';		break;
		case 3:		tempChar = L'$';		break;
		case 4:		tempChar = L'%';		break;
		case 5:		tempChar = L'&';		break;
		case 6:		tempChar = L'*';		break;
		}
		changeString.push_back(tempChar);
#endif
	}
	return changeString;
}

void CsCuidMng::GetBadWordPos(std::wstring const& InputString, std::vector<std::pair<int,int>>& rVecFindPos)
{
	rVecFindPos.clear();
	if(InputString.empty())
		return;

	CsCuid_Chat::LIST_CIT it = m_ChatList.begin();
	CsCuid_Chat::LIST_CIT itEnd = m_ChatList.end();
	std::wstring::size_type findPos = 0,NextPos = 0;
	for(;it!=itEnd;++it) // 테이블에 등록된 금지어를 순회
	{
		std::wstring badString = (*it)->GetName();
		while(1) // 한 금지어를 입력한 문자열내에서 다찾을 때까지
		{
			findPos = InputString.find(badString,NextPos);
			if(findPos != std::wstring::npos) // 찾았다면
			{
				rVecFindPos.push_back(std::make_pair(findPos,badString.length())); // 찾은 위치와 길이 목록에 추가
				NextPos += badString.length(); // 찾은 단어 다음 위치부터 찾기 위해
			}
			else 
				break;
		}
		// 금지어 하나의 검사를 끝냈다면 
		NextPos = 0;
	}
}

void CsCuidMng::AddChatList(CsCuid_Chat::sINFO& rInfo)
{
	CsCuid_Chat* p = csnew CsCuid_Chat;
	p->Init( &rInfo );
	m_ChatList.push_back( p );
}

void CsCuidMng::Lwr(TCHAR* const pText)
{
	std::wstring stext(pText);
	std::transform( stext.begin(), stext.end(), stext.begin(), tolower );

	stext.copy(pText, stext.length());
}

void CsCuidMng::Trim(TCHAR* const pText, const std::wstring& removeStr )
{
	std::wstring stext(pText);
	if(stext.empty() || removeStr.empty())
		return;

	std::wstring::size_type LeftPos = stext.find_first_not_of(removeStr);
	if(LeftPos != std::wstring::npos)
		stext = stext.substr(LeftPos, stext.length());

	std::wstring::size_type RightPos = stext.find_last_not_of(removeStr);
	if(RightPos != std::wstring::npos)
		stext = stext.substr(0, RightPos);

	stext.copy(pText, stext.length());
}