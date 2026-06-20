
#include "stdafx.h"
#include "PrintMsg.h"


void cPrintMsg::PrintMsg( int nFTID, void* pValue1 /*=NULL*/, void* pValue2 /*=NULL*/, void* pValue3 /*=NULL*/, void* pValue4 /*=NULL*/, void* pValue5 /*=NULL*/ )
{
	// 파일 테이블에서 읽어오기
	CsTalk_Message* pFT = nsCsFileTable::g_pTalkMng->GetTalk_Message( nFTID );
	if( pFT == NULL )
		return;

	TCHAR szBody[ FT_TALK_MSG_BODY_LEN ] = {0, };
	_tcscpy_s( szBody, FT_TALK_MSG_BODY_LEN, pFT->GetInfo()->s_Message );
	cPrintMsg::_Parcing( szBody, FT_TALK_MSG_BODY_LEN, pValue1, pValue2, pValue3, pValue4, pValue5 );

	switch( nFTID )
	{
	case 12000:			// 공지
		if( g_pGameIF )
		{
			if( lstrlen(szBody) < 2)	// 공지사항 빈 문자가 올 경우에 따른 예외 처리.
				break;
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::NOTICE;
			CProtocol.m_wStr = szBody;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
		break;
	}

	switch( pFT->GetInfo()->s_MsgType )
	{
	case CsTalk_Message::MT_BOX:
		cMessageBox::InsertList( pFT->GetInfo()->s_TitleName, szBody, nFTID, pFT->GetInfo()->s_Type );
		break;
	case CsTalk_Message::MT_SYSTEM:
		g_pSystemMsg->AddSystemMessage( szBody );
		break;
	case CsTalk_Message::MT_CHAT:
		{
			ContentsStream kStream;
			if( pFT->GetInfo()->s_Type == 1 )	// 복수. 중복검사 하지 않는다
			{			
				wstring str = szBody;
				bool bParcing = true;	bool bCheckSameString = false;
				kStream << str << bParcing << bCheckSameString;
			}
			else if(pFT->GetInfo()->s_Type == 2) // 단수. 중복검사 한다
			{
				wstring str = szBody;
				bool bParcing = true;	bool bCheckSameString = true;
				kStream << str << bParcing << bCheckSameString;
			}

			// [4/14/2016 hyun] 아이템 획득 메세지 출력이라면
			if(nFTID == cPrintMsg::GAME_ITEM_CROP)
			{
				SAFE_POINTER_RET(pValue2); // 컬러값이 있어야만 한다
				NiColor itemStrColor = *(static_cast<NiColor*>(pValue2)); // 아이템의 스트링 컬러
				kStream << itemStrColor;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_ITEMCROP_PROCESS, &kStream);
			}
			else if(nFTID == cPrintMsg::GAME_ITEM_MANYCROP)
			{
				SAFE_POINTER_RET(pValue3); // 컬러값이 있어야만 한다
				NiColor itemStrColor = *(static_cast<NiColor*>(pValue3)); // 아이템의 스트링 컬러
				kStream << itemStrColor;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_ITEMCROP_PROCESS, &kStream);
			}
			else
			{
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream);
			}
			// @@ 여기까지
			break;
		}
	case CsTalk_Message::MT_NOTICE:
		cPrintMsg::PrintMsg( 12000, szBody );		// 공지
		break;
	case CsTalk_Message::MT_MONSTER_SKILL:
		g_pSkillMsg->AddSystemMessage( szBody );
		break;
	default:
		assert_cs( false );
	}

	if( pFT->GetInfo()->s_dwLinkID != 0 )
	{
		cPrintMsg::PrintMsg( (cPrintMsg::eTYPE)pFT->GetInfo()->s_dwLinkID, pValue1, pValue2, pValue3, pValue4 );
	}
}

bool cPrintMsg::_ParcingToString( TCHAR* szOut, TCHAR* szIn, void* pValue1, void* pValue2, void* pValue3, void* pValue4, void* pValue5, int& nParcingIndex )
{
	void* pCurValue = NULL;
	switch( nParcingIndex )
	{
	case 0:		pCurValue = pValue1;		break;
	case 1:		pCurValue = pValue2;		break;
	case 2:		pCurValue = pValue3;		break;
	case 3:		pCurValue = pValue4;		break;
	case 4:		pCurValue = pValue5;		break;
	default:	assert_cs( false );
	}	 

	assert_cs( pCurValue != NULL );
	SAFE_POINTER_RETVAL( pCurValue, false );

	if( _tcscmp( szIn, _T( "[str]" ) ) == 0 )
	{
		_tcscpy_s( szOut, FT_TALK_MSG_BODY_LEN, (TCHAR*)pCurValue );		
		return true;
	}
	else if( _tcscmp( szIn, _T( "[commandnotice]" ) ) == 0 )
	{	
		_tcscpy_s( szOut, FT_TALK_MSG_BODY_LEN, (TCHAR*)pCurValue );
		cWindow::PlaySound( "System\\Notice.wav" );		
		return true;
	}	
	else if( _tcscmp( szIn, _T( "[money]" ) ) == 0 )
	{
		sMONEY* money = (sMONEY*)pCurValue;
		TCHAR sz[ 64 ];

		if( money->GetTera() != 0 )
		{
			_stprintf_s( sz, 32, _T( "%d[T] %d[M] %d[B]" ), money->GetTera(), money->GetMega(), money->GetBit() );
		}
		else if( money->GetMega() != 0 )
		{
			_stprintf_s( sz, 32, _T( "%d[M] %d[B]" ), money->GetMega(), money->GetBit() );
		}
		else
		{
			_stprintf_s( sz, 32, _T( "%d[B]" ), money->GetBit() );
		}
		_tcscpy_s( szOut, FT_TALK_MSG_BODY_LEN, sz );		
		return true;
	}	
	else if( _tcscmp( szIn, _T( "[int]" ) ) == 0 )
	{
		_stprintf_s( szOut, FT_TALK_MSG_BODY_LEN, _T( "%d" ), *(u4*)pCurValue );		
		return true;
	}	
	else if( _tcscmp( szIn, _T( "[rtime]" ) ) == 0 )
	{
		int nRemain = *(uint*)pCurValue;
		int nSec = nRemain;
		int nMin = nRemain / 60;
		int nHour = nMin / 60;
		int nDay = nHour / 24;
		if( nDay > 0 )
		{
			std::wstring wsDay = UISTRING_TEXT( "COMMON_TXT_DAY_HOUR_MIN" );
			DmCS::StringFn::Replace( wsDay, L"#Day#", nDay );
			DmCS::StringFn::Replace( wsDay, L"#Hour#", nHour%24 );
			DmCS::StringFn::Replace( wsDay, L"#Min#", nMin%60 );
			_stprintf_s( szOut, FT_TALK_MSG_BODY_LEN, wsDay.c_str() );
		}
		else if( nHour > 0 )
		{
			std::wstring wsHourMin = UISTRING_TEXT( "COMMON_TXT_HOUR_MIN" );
			DmCS::StringFn::Replace( wsHourMin, L"#Hour#", nHour%24 );
			DmCS::StringFn::Replace( wsHourMin, L"#Min#", nMin%60 );
			_stprintf_s( szOut, FT_TALK_MSG_BODY_LEN, wsHourMin.c_str() );
		}
		else if( nMin > 0 )
		{
			std::wstring wsMin;
			DmCS::StringFn::Format( wsMin, L"%d %s", nMin%60, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
			_stprintf_s( szOut, FT_TALK_MSG_BODY_LEN, wsMin.c_str() );
		}
		else if( nSec > 0 )
		{
			std::wstring wsSec;
			DmCS::StringFn::Format( wsSec, L"%d %s", nSec%60, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
			_stprintf_s( szOut, FT_TALK_MSG_BODY_LEN, wsSec.c_str() );
		}				
		return true;
	}

	return false;
}


TCHAR* cPrintMsg::_Parcing( TCHAR* szMsg, int nLen, void* pValue1, void* pValue2, void* pValue3, void* pValue4, void* pValue5 )
{
	size_t len = _tcslen( szMsg );
	assert_cs( (int)len < nLen );
	if( len == 0 )
		return szMsg;

	// 파싱해야 하는 메세지인지 검사
	if( szMsg[ len - 1 ] == L'%' )
	{
		int nParcingIndex = 0;

		len -= 1;
		szMsg[ len ] = NULL;

		TCHAR szSrcMsg[ FT_TALK_MSG_BODY_LEN ];
		int nSrcMsgIndex = 0;

		TCHAR szParcing[ 128 ];
		int nPacingIndex = 0;

		bool bFindToken  = false;
		for( uint i=0; i<len; ++i )
		{
			if( szMsg[ i ] == L'[' )
			{
				// 이미 분리 토큰자가 존재 한다면 앞에 있는 토큰자를 무시한다
				if( bFindToken == true )
				{
					szParcing[ nPacingIndex ] = NULL;
					szSrcMsg[ nSrcMsgIndex ] = NULL;
					_tcscat_s( szSrcMsg, FT_TALK_MSG_BODY_LEN, szParcing );
					nSrcMsgIndex += nPacingIndex;
					nPacingIndex = 0;
				}

				bFindToken = true;
			}

			if( bFindToken == true )
			{
				szParcing[ nPacingIndex ] = szMsg[ i ];
				++nPacingIndex;
			}
			else
			{
				szSrcMsg[ nSrcMsgIndex ] = szMsg[ i ];
				++nSrcMsgIndex;
			}

			if( szMsg[ i ] == L']' )
			{
				if( bFindToken == true )
				{
					// 파싱 목록을 찾았다면
					TCHAR szString[ FT_TALK_MSG_BODY_LEN ];
					szParcing[ nPacingIndex ] = NULL;
					szSrcMsg[ nSrcMsgIndex ] = NULL;
					if( _ParcingToString( szString, szParcing, pValue1, pValue2, pValue3, pValue4, pValue5, nParcingIndex ) == true )
					{						
						size_t stringLen = _tcslen( szString );					
						_tcscat_s( szSrcMsg, FT_TALK_MSG_BODY_LEN, szString );
						nSrcMsgIndex += stringLen;
						nPacingIndex = 0;
					}
					// 못찾았다면
					else
					{
						_tcscat_s( szSrcMsg, FT_TALK_MSG_BODY_LEN, szParcing );
						nSrcMsgIndex += nPacingIndex;
						nPacingIndex = 0;
					}
					++nParcingIndex;
				}
				bFindToken = false;
			}
		}

		szSrcMsg[ nSrcMsgIndex ] = NULL;
		_tcscpy_s( szMsg, nLen, szSrcMsg );
	}	

	return szMsg;
}

