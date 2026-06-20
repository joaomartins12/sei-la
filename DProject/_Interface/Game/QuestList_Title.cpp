
#include "stdafx.h"
#include "QuestList.h"




//================================================================================================================
//
//		Create
//
//================================================================================================================
void cQuestList::_Create_QuestList()
{	
}

//================================================================================================================
//
//		Read
//
//================================================================================================================

void cQuestList::ResetQuestList( bool bKeepDiscript /*=false*/ )
{
	int nKeepValue2 = 0;
	if( bKeepDiscript == false )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT );
	}
	else
	{		
		if( m_pSelectString && g_pGameIF->GetQuestDiscript() )
		{
			nKeepValue2 = m_pSelectString->GetValue2();
			g_pGameIF->GetQuestDiscript()->SetQuest( nKeepValue2, true );
		}
	}

	m_QuestListStringMain.Delete();
	m_QuestListStringSub.Delete();

	cData_Quest* pDataQuest = g_pDataMng->GetQuest();
	nQuest::cInfo* pQuestInfo = pDataQuest->GetInfo();
	for( int i=0; i<nQuest::MaxExecute; ++i )
	{
		nQuest::sExeInfo ExeInfo = pQuestInfo->GetExeInfo( i );
		if( ExeInfo.m_nIDX == 0 )
			continue;

		AddQuest( i, ExeInfo.m_nIDX, true );
	}

	_CalScrollBarRange( true );
	_CalScrollBarRange( false );
	_CalQuestCount();

	// 선택된게 존재 한다면 유지 하자
	m_pSelectString = NULL;
	if( nKeepValue2 != 0 )
	{
		bool bFind = false;

		std::list< cString* >* pList = m_QuestListStringMain.GetList();
		std::list< cString* >::iterator it = pList->begin();
		std::list< cString* >::iterator itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			if( (*it)->GetValue2() == nKeepValue2 )
			{
				m_pSelectString = *it;
				bFind = true;
				break;
			}
		}

		if( bFind == false )
		{
			pList = m_QuestListStringSub.GetList();
			it = pList->begin();
			itEnd = pList->end();
			for( ; it!=itEnd; ++it )
			{
				if( (*it)->GetValue2() == nKeepValue2 )
				{
					m_pSelectString = *it;
					break;
				}
			}
		}		
	}
}

int cQuestList::_CreateQuestTab( TCHAR* szTab, bool bQTMain )
{
	char cTab[ QUEST_TITLE_LEN ] = {0, };
	W2M( cTab, szTab, QUEST_TITLE_LEN );
	int nTemp = 0;
	bool bExpand;
	if( bQTMain )
	{
		bExpand = m_ExpandStringMain.GetAt( cTab, nTemp );
	}
	else
	{
		bExpand = m_ExpandStringSub.GetAt( cTab, nTemp );
	}
	
	cString* pString = NiNew cString;

	pString->AddImage( m_pExpandImage, bExpand == true ? 1 : 0, CsPoint( 0, 0 ) );	
	pString->TailAddSizeX( 3 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10, NiColor( 247/255.0f, 211/255.0f, 153/255.0f ) );
	ti.SetText( szTab );
	pString->AddText( &ti );
	pString->SetValue1( bExpand == true ? IF_QL_TAB_EXPAND : IF_QL_TAB_UNEXPAND );
	pString->SetValue2( 0 );

	if( bQTMain == true )
	{
		m_QuestListStringMain.AddTail( pString );
		return m_QuestListStringMain.GetSize() - 1;
	}

	m_QuestListStringSub.AddTail( pString );
	return m_QuestListStringSub.GetSize() - 1;
}

//================================================================================================================
//
//		Add, Drop
//
//================================================================================================================
void cQuestList::AddQuest( int nQuestExeIndex, DWORD dwQuestFTID, bool bInitial /*=false*/ )
{
	CsQuest* pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( dwQuestFTID );

	cScrollBar* pScroll = NULL;
	cStringList* pStringList = NULL;
	bool bQTMain;
	if( pFTQuest->GetQuestType() == CsQuest::QT_MAIN )
	{
		bQTMain = true;
		pScroll = m_pScrollBarMain;
		pStringList = &m_QuestListStringMain;
	}
	else
	{
		bQTMain = false;
		pScroll = m_pScrollBarSub;
		pStringList = &m_QuestListStringSub;
	}

	// 퀘스트 탭을 찾는다.
	int nTab = pStringList->FindText( pFTQuest->m_szTitleTab );
	// 탭이 존재 하지 않는다면 탭 생성
	if( nTab == cStringList::INVALIDE_FIND )
	{
		nTab = _CreateQuestTab( pFTQuest->m_szTitleTab, bQTMain );
	}

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	// 타이틀 기입
	cString* pString = NiNew cString;
	bool bQuestHelperView = g_pResist->m_Character.s_bQuestHelperViewID[ nQuestExeIndex ];
	
	ti.s_eFontSize = CFont::FS_8;
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_LV" ).c_str() );
	pString->AddText( &ti, CsPoint( 0, 3 ));	
	
	ti.s_Color = g_pDataMng->GetQuest()->GetTitleColor( pFTQuest );	
	ti.s_eFontSize = CFont::FS_10;	
	ti.SetText( pFTQuest->GetLevel() );	
	pString->AddText( &ti );	
	
	CsPoint ptPos;
	int nBase;
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 20 - ptPos.x );	
	pString->AddCheckBox( "System\\Check.tga", bQuestHelperView, CsPoint( 0, 20 ), CsPoint( 15, 18 ), CsPoint( 0, -2 ) )->s_ptSize.x += 3;
	pString->AddImage( m_pProcessImage, g_pDataMng->GetQuest()->GetProcess( dwQuestFTID )->s_bCompleate ? 1 : 0, CsPoint( 0, -2 ) )->s_ptSize.x += 3;

	ti.s_Color = g_pDataMng->GetQuest()->GetTitleColor( pFTQuest );
	ti.SetText( pFTQuest->m_szTitleText );
	pString->AddText( &ti );
	pString->SetValue1( IF_QL_TITLE );
	pString->SetValue2( dwQuestFTID );
	pString->SetValue3( nQuestExeIndex );

	pStringList->AddIndexAfter( pString, nTab );
	
	if( bInitial == false )
	{
		_CalScrollBarRange( bQTMain );
		_CalQuestCount();
	}
}


//================================================================================================================
//
//		Update
//
//================================================================================================================
bool cQuestList::_Update_FM_QuestList()
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	cScrollBar* pScroll = NULL;
	cStringList* pStringList = NULL;
	CsPoint pos = CsPoint::ZERO;		// 로칼 계산이므로
	bool bQTMain;
	if( m_rcTitleRegionMain.PtInRect( ptMouseLocalPos ) )
	{
		pos = CsPoint( 22, 93 );
		bQTMain = true;
		pScroll = m_pScrollBarMain;
		pStringList = &m_QuestListStringMain;
	}
	else if( m_rcTitleRegionSub.PtInRect( ptMouseLocalPos ) )
	{
		pos = CsPoint( 22, 242 );
		bQTMain = false;
		pScroll = m_pScrollBarSub;
		pStringList = &m_QuestListStringSub;
	}
	else
	{
		return false;
	}

	int nRemainPos = pScroll->GetCurPosIndex();
	int nRender = pScroll->GetRenderCount();

	std::list< cString* >* pList = pStringList->GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();	
	while( it!=itEnd )
	{
		switch( (*it)->GetValue1() )
		{
			// 펼쳐진 탭이라면
		case IF_QL_TAB_EXPAND:
			{
				// 탭 체크
				switch( _Update_FM_QuestTab( pos, nRemainPos, nRender, it, ptMouseLocalPos ) )
				{
				case QL_END:
					return true;
				case QL_ACTION_CLICK:
					_QL_ClickTab( *it, false, bQTMain );
					return true;
				}

				// 하위 타이틀 체크
				switch( _Update_FM_QuestTitleExpand( pos, nRemainPos, nRender, it, itEnd, ptMouseLocalPos ) )
				{
				case QL_END:
					return true;
				case QL_ACTION_CLICK:
					_QL_ClickTitle( *it, bQTMain );
					return true;
				}
			}
			break;
			// 펼쳐지지 않은 탭이라면
		case IF_QL_TAB_UNEXPAND:
			{		 	
				// 탭 체크
				switch( _Update_FM_QuestTab( pos, nRemainPos, nRender, it, ptMouseLocalPos ) )
				{
				case QL_END:
					return true;
				case QL_ACTION_CLICK:
					_QL_ClickTab( *it, true, bQTMain );
					return true;
				}

				// 하위 타이틀 무시
				if( _Update_FM_QuestTitleUnExpand( it, itEnd ) == QL_END )
					return true;
			}
			break;
			// 타이틀이라면
		case IF_QL_TITLE:
			{
				// 하위 타이틀 체크
				switch( _Update_FM_QuestTitleExpand( pos, nRemainPos, nRender, it, itEnd, ptMouseLocalPos ) )
				{
				case QL_END:
					return true;
				case QL_ACTION_CLICK:
					_QL_ClickTitle( *it, bQTMain );
					return true;
				}
			}
			break;
		default:
			assert_cs( false );
		}
	}
	return true;
}

cQuestList::eQL_UFM_STATE
cQuestList::_Update_FM_QuestTab( CsPoint& pos, int& nRemainPos, int& nRenderCount, std::list< cString* >::iterator& itTab, CsPoint ptLocal )
{
	if( nRemainPos > 0 )
	{
		--nRemainPos;
	}
	else
	{
		int nOrgY = pos.y;

		pos.y += (*itTab)->GetMaxSize().y;
		pos.y += IF_QUESTLIST_DELTA_Y;

		if( ( ptLocal.y > nOrgY )&&( ptLocal.y <= pos.y ) )
			return QL_ACTION_CLICK;

		if( ( --nRenderCount < 1 )||( pos.y > g_nScreenHeight ) )
			return QL_END;
	}
	++itTab;
	return QL_ACTION_NONE;
}

cQuestList::eQL_UFM_STATE
cQuestList::_Update_FM_QuestTitleExpand(	CsPoint& pos, int& nRemainPos, int& nRenderCount,
											std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd, CsPoint ptLocal )
{
	while( itTitle != itEnd )
	{
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
		{			
			return QL_ACTION_NONE;
		}

		if( nRemainPos > 0 )
		{
			--nRemainPos;
		}
		else
		{
			int nOrgY = pos.y;
			pos.y += (*itTitle)->GetMaxSize().y;
			pos.y += IF_QUESTLIST_DELTA_Y;

			if( ( ptLocal.y > nOrgY )&&( ptLocal.y <= pos.y ) )
				return QL_ACTION_CLICK;

			if( ( --nRenderCount < 1 )||( pos.y > g_nScreenHeight ) )
				return QL_END;
		}
		++itTitle;
	}
	return QL_END;
}

cQuestList::eQL_UFM_STATE
cQuestList::_Update_FM_QuestTitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd )
{
	// 탭 차일드 무시
	while( itTitle != itEnd )
	{		
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
		{
			return QL_ACTION_NONE;
		}
		++itTitle;
	}
	return QL_END;
}


//================================================================================================================
//
//		Render
//
//================================================================================================================
void cQuestList::_Render_QuestList( bool bQTMain )
{	
	CsPoint pos = GetRootClient();
	cScrollBar* pScroll = NULL;
	cStringList* pStringList = NULL;
	if( bQTMain == true )
	{
		pos += CsPoint( 22, 92 );
		pScroll = m_pScrollBarMain;
		pStringList = &m_QuestListStringMain;
	}
	else
	{
		pos += CsPoint( 22, 242 );
		pScroll = m_pScrollBarSub;
		pStringList = &m_QuestListStringSub;
	}

	int nRemainPos = pScroll->GetCurPosIndex();	
	int nRender = pScroll->GetRenderCount();

	std::list< cString* >* pList = pStringList->GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();
	while( it!=itEnd )
	{
		switch( (*it)->GetValue1() )
		{
			// 펼쳐진 탭이라면
		case IF_QL_TAB_EXPAND:
			{
				// 탭 그리고
				if( _Render_QuestTab( pos, nRemainPos, nRender, it ) == false )
					return;

				// 하위 타이틀 그리자
				if( _Render_QuestTitleExpand( pos, nRemainPos, nRender, it, itEnd ) == false )
					return;
			}
			break;
			// 펼쳐지지 않은 탭이라면
		case IF_QL_TAB_UNEXPAND:
			{
				// 탭그리고
				if( _Render_QuestTab( pos, nRemainPos, nRender, it ) == false )
					return;

				// 하위 타이틀 무시
				if( _Render_QuestTitleUnExpand( it, itEnd ) == false )
					return;
			}
			break;
			// 타이틀이라면
		case IF_QL_TITLE:
			{
				// 하위 타이틀 그리자
				if( _Render_QuestTitleExpand( pos, nRemainPos, nRender, it, itEnd ) == false )
					return;
			}
			break;
		default:
			assert_cs( false );
		}
	}
}

bool cQuestList::_Render_QuestTab( CsPoint& pos, int& nRemainPos, int& nRenderCount, std::list< cString* >::iterator& itTab )
{
	// 탭 그리자
	if( nRemainPos > 0 )
	{
		--nRemainPos;
	}
	else
	{
		// 마우스온 랜더링
		if( m_pMouseOnString == (*itTab) )
		{
			m_pOnMouseMask->Render( pos + CsPoint( 2, -4 ) );
			m_pMouseOnString = NULL;
		}

		pos.y = (*itTab)->Render( pos );
		pos.y += IF_QUESTLIST_DELTA_Y;

		if( ( --nRenderCount < 1 )||( pos.y > g_nScreenHeight ) )
			return false;
	}

	++itTab;
	return true;
}

bool cQuestList::_Render_QuestTitleExpand(	CsPoint& pos, int& nRemainPos, int& nRenderCount,
											std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd )
{
	// 탭 차일드 그리자
	while( itTitle != itEnd )
	{		
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
		{			
			return true;
		}

		if( nRemainPos > 0 )
		{
			--nRemainPos;
		}
		else
		{
			// 셀렉트 마크 랜더링
			if( m_pSelectString == (*itTitle) )
			{
				m_pSelMask->Render( pos + CsPoint( 2, -4 ) );
			}

			// 마우스온 랜더링
			if( m_pMouseOnString == (*itTitle) )
			{
				m_pOnMouseMask->Render( pos + CsPoint( 2, -4 ) );
				m_pMouseOnString = NULL;
			}

			pos.y = (*itTitle)->Render( pos );
			pos.y += IF_QUESTLIST_DELTA_Y;

			if( ( --nRenderCount < 1 )||( pos.y > g_nScreenHeight ) )
				return false;
		}
		++itTitle;
	}
	return false;
}

bool cQuestList::_Render_QuestTitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd )
{
	// 탭 차일드 무시
	while( itTitle != itEnd )
	{		
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
		{
			return true;
		}
		++itTitle;
	}
	return false;
}

//================================================================================================================
//
//		동작
//
//================================================================================================================
void cQuestList::_QL_ClickTab( cString* pString, bool bToExpand, bool bQTMain )
{
	// 마우스온
	m_pMouseOnString = pString;

	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
	{
		cString::sIMAGE* pExpand = (cString::sIMAGE*)pString->GetElement( 0 );
		cString::sTEXT* pTab = (cString::sTEXT*)pString->GetElement( 1 );
		assert_cs( pExpand->GetType() == cString::sELEMENT::IMAGE );
		assert_cs( pTab->GetType() == cString::sELEMENT::TEXT );

		char cTab[ QUEST_TITLE_LEN ] = {0, };
		W2M( cTab, pTab->s_Text.GetTextInfo()->GetText(), QUEST_TITLE_LEN );

		// Expand 로 교체
		if( bToExpand == true )
		{
			pExpand->s_nImageIndex = 1;
			pString->SetValue1( IF_QL_TAB_EXPAND );

			int nTemp = 0;

			if( bQTMain )
			{
				assert_cs( m_ExpandStringMain.GetAt( cTab, nTemp ) == false );
				m_ExpandStringMain.SetAt( cTab, nTemp );
			}
			else
			{
				assert_cs( m_ExpandStringSub.GetAt( cTab, nTemp ) == false );
				m_ExpandStringSub.SetAt( cTab, nTemp );
			}
		}
		else
		{
			pExpand->s_nImageIndex = 0;
			pString->SetValue1( IF_QL_TAB_UNEXPAND );

			int nTemp = 0;
			if( bQTMain )
			{
				assert_cs( m_ExpandStringMain.GetAt( cTab, nTemp ) == true );
				m_ExpandStringMain.RemoveAt( cTab );
			}
			else
			{
				assert_cs( m_ExpandStringSub.GetAt( cTab, nTemp ) == true );
				m_ExpandStringSub.RemoveAt( cTab );
			}
		}

		pString->CalMaxSize();
		_CalScrollBarRange( bQTMain );
	}
}

void cQuestList::_QL_ClickTitle( cString* pString, bool bQTMain )
{
	// 마우스 온
	m_pMouseOnString = pString;	

	if( _QL_String_Update_Control( bQTMain ) == false )
	{
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
		{
			m_pSelectString = pString;

			cQuestDiscript* pIF = (cQuestDiscript*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT );
			pIF->SetQuest( pString->GetValue2(), 0 );
		}
	}	
}

bool cQuestList::_QL_String_Update_Control( bool bQTMain )
{
	cScrollBar* pScroll = NULL;
	cStringList* pStringList = NULL;
	CsPoint pos = GetRootClient();
	if( bQTMain )
	{
		pos += CsPoint( 22, 93 );
		pScroll = m_pScrollBarMain;
		pStringList = &m_QuestListStringMain;

	}
	else
	{
		pos += CsPoint( 22, 242 );
		pScroll = m_pScrollBarSub;
		pStringList = &m_QuestListStringSub;
	}

	int nRemainPos = pScroll->GetCurPosIndex();
	int nRender = pScroll->GetRenderCount();
	std::list< cString* >* pList = pStringList->GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();	
	while( it!=itEnd )
	{
		switch( (*it)->GetValue1() )
		{
			// 펼쳐진 탭이라면
		case IF_QL_TAB_EXPAND:
			{
				// 탭 체크
				if( _QL_String_Update_Control_Tab( pos, nRemainPos, nRender, it ) == false )
				{
					return false;
				}

				// 하위 타이틀 체크
				if( _QL_String_Update_Control_TitleExpand( pos, nRemainPos, nRender, it, itEnd ) == true )
				{
					return true;
				}
			}
			break;
			// 펼쳐지지 않은 탭이라면
		case IF_QL_TAB_UNEXPAND:
			{		 	
				// 탭 체크
				if( _QL_String_Update_Control_Tab( pos, nRemainPos, nRender, it ) == false )
				{
					return false;
				}

				// 하위 타이틀 무시
				_QL_String_Update_Control_TitleUnExpand( it, itEnd );				
			}
			break;
			// 타이틀이라면
		case IF_QL_TITLE:
			{
				// 하위 타이틀 체크
				if( _QL_String_Update_Control_TitleExpand( pos, nRemainPos, nRender, it, itEnd ) == true )
				{
					return true;
				}				
			}
			break;
		default:
			assert_cs( false );
		}
	}
	return false;
}

bool cQuestList::_QL_String_Update_Control_Tab( CsPoint& pos, int& nRemainPos, int& nRenderCount, std::list< cString* >::iterator& itTab )
{
	if( nRemainPos > 0 )
	{
		--nRemainPos;
	}
	else
	{
		pos.y += (*itTab)->GetMaxSize().y;
		pos.y += IF_QUESTLIST_DELTA_Y;

		if( ( --nRenderCount < 1 )||( pos.y > g_nScreenHeight ) )
			return false;
	}
	++itTab;
	return true;
}

bool cQuestList::_QL_String_Update_Control_TitleExpand(	CsPoint& pos, int& nRemainPos, int& nRenderCount,
														std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd )
{
	bool bReturn = false;
	while( itTitle != itEnd )
	{
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
		{			
			return bReturn;
		}

		if( nRemainPos > 0 )
		{
			--nRemainPos;
		}
		else
		{
			cString::sELEMENT* pElement = (*itTitle)->Update_ForMouse_Control( pos );
			pos.y += (*itTitle)->GetMaxSize().y;
			pos.y += IF_QUESTLIST_DELTA_Y;
			
			if( pElement != NULL )
			{
				int nExeInfoArrayIndex = (*itTitle)->GetValue3();
				assert_cs( nExeInfoArrayIndex < nQuest::MaxExecute );
				g_pResist->m_Character.s_bQuestHelperViewID[ nExeInfoArrayIndex ] = ( (cString::sCHECKBOX*)pElement )->s_pCheckBox->IsCheck();
				if( g_pGameIF->GetQuestList() )
				{
					g_pGameIF->GetQuestHelper()->ResetHelperString();
				}
				bReturn = true;
			}
		}
		++itTitle;
	}
	return bReturn;
}

void cQuestList::_QL_String_Update_Control_TitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd )
{
	// 탭 차일드 무시
	while( itTitle != itEnd )
	{		
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
		{
			return;
		}
		++itTitle;
	}
}

//================================================================================================================
//
//		QuestCount
//
//================================================================================================================
void cQuestList::_CalQuestCount()
{
	SAFE_POINTER_RET( m_pCurQuestCountText );
	SAFE_POINTER_RET( g_pDataMng );	
	cData_Quest* pQuestData = g_pDataMng->GetQuest();
	SAFE_POINTER_RET( pQuestData );
	nQuest::cInfo* pInfo = pQuestData->GetInfo();
	SAFE_POINTER_RET( pInfo );

	std::wstring wsQuestCount = UISTRING_TEXT( "QUESTLIST_QUESTCOUNT_TEXT" ).c_str();
	DmCS::StringFn::ReplaceAll( wsQuestCount, L"#Current#", pInfo->GetExecuteCount() );
	DmCS::StringFn::ReplaceAll( wsQuestCount, L"#Max#", nQuest::MaxExecute );
	m_pCurQuestCountText->SetText( wsQuestCount.c_str() );
}

//================================================================================================================
//
//		ScrollBar
//
//================================================================================================================

void cQuestList::_CalScrollBarRange( bool bQTMain, cString* pEnsureString /*=NULL*/ )
{
	cScrollBar* pScroll = NULL;
	cStringList* pStringList = NULL;
	if( bQTMain == true )
	{
		pScroll = m_pScrollBarMain;
		pStringList = &m_QuestListStringMain;
	}
	else
	{
		pScroll = m_pScrollBarSub;
		pStringList = &m_QuestListStringSub;
	}

	std::list< cString* >* pList = pStringList->GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();

	CsPoint ptRange = CsPoint::ZERO;
	int nScrollPos = -1;
	while( it!=itEnd )
	{
		switch( (*it)->GetValue1() )
		{
			// 펼쳐진 탭이라면
		case IF_QL_TAB_EXPAND:
			{
				_CalScrollBarRange_Tab( ptRange, it );
				int sp = _CalScrollBarRange_TitleExpand( ptRange, it, itEnd, pEnsureString );
				if( sp != -1 )
				{
					nScrollPos = sp;
				}
			}
			break;
			// 펼쳐지지 않은 탭이라면
		case IF_QL_TAB_UNEXPAND:
			{
				_CalScrollBarRange_Tab( ptRange, it );
				_CalScrollBarRange_TitleUnExpand( it, itEnd );
			}
			break;
		default:
			assert_cs( false );
		}
	}

	++ptRange.y;
	pScroll->SetRange( ptRange );

	if( nScrollPos >= 0 )
	{
		if( ( nScrollPos < pScroll->GetCurPosIndex() )||( nScrollPos > pScroll->GetCurPosIndex() + pScroll->GetRenderCount() ) )
		{
			pScroll->SetCurPosIndex( nScrollPos );
		}
	}	
}

void cQuestList::_CalScrollBarRange_Tab( CsPoint& ptRange, std::list< cString* >::iterator& itTab )
{
	++itTab;
	++ptRange.y;
}

int cQuestList::_CalScrollBarRange_TitleExpand( CsPoint& ptRange, std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd, cString* pEnsureString )
{
	int nReturn = -1;
	while( itTitle != itEnd )
	{		
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
			return nReturn;

		if( *itTitle == pEnsureString )
		{
			nReturn = ptRange.y;
		}			

		++itTitle;
		++ptRange.y;
	}
	return nReturn;
}

void cQuestList::_CalScrollBarRange_TitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd )
{
	while( itTitle != itEnd )
	{		
		if( (*itTitle)->GetValue1() != IF_QL_TITLE )
			return;
		++itTitle;
	}
}