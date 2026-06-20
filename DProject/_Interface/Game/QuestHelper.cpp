
#include "stdafx.h"
#include "QuestHelper.h"

cQuestHelper::cQuestHelper():m_bView(false),m_pScrollBar(NULL),m_pBGWindow(NULL)
,m_pHelperStateImage(NULL),m_pLineLeft(NULL),m_pLineRight(NULL)
{
}

cQuestHelper::~cQuestHelper()
{

}

void cQuestHelper::Destroy()
{
	cBaseWindow::Delete();	
}

void cQuestHelper::DeleteResource()
{
	DeleteScript();

	m_StringList.Delete();
	SAFE_NIDELETE( m_pBGWindow );
	SAFE_NIDELETE( m_pScrollBar );
	SAFE_NIDELETE( m_pHelperStateImage );
	SAFE_NIDELETE( m_pLineLeft );
	SAFE_NIDELETE( m_pLineRight );
}

void cQuestHelper::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint size( 220, 340 );
	InitScript( NULL, CsPoint( DEFAULT_SCREEN_WIDTH - size.x, 250 ), size, false, IFREGION_X::RIGHT, IFREGION_Y::TOP );

	m_pBGWindow = NiNew cPopUp;
	m_pBGWindow->Init( m_pRoot, CsPoint::ZERO, size, CsPoint( 3, 3 ), "Control_G\\Popup\\NDlg", "tga", false );

	m_pHelperStateImage = NiNew cImage;
	m_pHelperStateImage->Init( NULL, CsPoint::ZERO, CsPoint( 26, 15 ), "QuestList\\Quest_Check.tga", false );
	m_pHelperStateImage->SetTexToken( CsPoint( 0, 21 ) );

	// 스크롤바
	m_pScrollBar = NiNew cScrollBar;

#ifdef VERSION_USA
	m_pScrollBar->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( size.x - 15, 5 ), CsPoint( 10, size.y - 10 ), CsPoint( 10, 15 ), CsRect( CsPoint::ZERO, size ), 17, false );	
#else
	m_pScrollBar->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( size.x - 15, 5 ), CsPoint( 10, size.y - 10 ), CsPoint( 10, 15 ), CsRect( CsPoint::ZERO, size ), 18, false );	
#endif

	// 라인
	m_pLineLeft = NiNew cSprite;
	m_pLineLeft->Init( NULL, CsPoint::ZERO, CsPoint( 45, 1 ), "Control_G\\Popup\\N2Dlg_Line.tga", false );
	m_pLineLeft->SetAlpha( 0.7f );

	m_pLineRight = NiNew cSprite;
	m_pLineRight->Init( NULL, CsPoint::ZERO, CsPoint( 20, 1 ), "Control_G\\Popup\\N2Dlg_Line.tga", false );
	m_pLineRight->SetAlpha( 0.7f );

	m_bView = true;
}

bool cQuestHelper::IsShowWindow()
{
	if( m_StringList.GetSize() == 0 )
		return false;

	return cBaseWindow::IsShowWindow();
}

void cQuestHelper::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cQuestHelper::Update_ForMouse()
{
	if( !m_bView )
		return MUT_OUT_WINDOW;

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;
	 
	int nActive1;
	int nActive2;
	CsPoint ptPoint = CsPoint( 7, 5 ) + GetRootClient();			
	cString::sELEMENT* pActiveElement = NULL;
	cString::sBUTTON* pButtonInfo = (cString::sBUTTON*)m_StringList.Update_ForMouse_Control( nActive1, nActive2, ptPoint, 4, &pActiveElement );

	// 버튼일 경우에만
	if( ( pActiveElement != NULL )&&( pActiveElement->GetType() == cString::sELEMENT::BUTTON ) )
		return muReturn;


	return MUT_NONE;		//2016-02-11-nova MUT_OUT_WINDOW -> MUT_NONE
}

void cQuestHelper::Render()
{
	if( m_bView == false )
		return;

	if( m_StringList.GetSize() == 0 )
	{
		return;
	}
	
	m_pBGWindow->Render();

	RenderScript();

	if( m_pScrollBar->IsEnableScroll() == true )
		m_pScrollBar->Render();

	m_StringList.Render( GetRootClient() + CsPoint( 7, 5 ), 4 );
}

void cQuestHelper::ResetDevice()
{	
	ResetDeviceScript();
	m_pBGWindow->ResetDevice();
	m_pScrollBar->ResetDevice();
	m_StringList.ResetDevice();
}

bool cQuestHelper::IsInWindow( CsPoint pos )
{
	if( m_bView == false )
	{
		return false;
	}

	if( m_StringList.GetSize() == 0 )
	{
		return false;
	}

	CsSIZE size( 220, CsMin( 340, m_StringList.CalMaxSize( 2 ).y + 10 ) );
	CsRect rc( GetRootClient(), size );
	if( rc.PtInRect( pos ) == FALSE )
	{
		return false;
	}
	return cScriptUI::IsInWindow( pos );
}

void cQuestHelper::ResetHelperString()
{
	if( g_pResist == NULL )
		return;

	int nOldPos = m_pScrollBar->GetCurPosIndex();

	m_StringList.Delete();
	m_StringList.SetBuddy( m_pScrollBar );

	assert_csm( nQuest::MaxExecute == 20, _T( "갯수 변경시 Resist 에서 sCHAR::s_bQuestHelperViewID 갯수 변경도 있어야 한다." ) );
	_ResetHelperString( CsQuest::QT_MAIN );
	_ResetHelperString( CsQuest::QT_SUB );
	_ResetHelperString( CsQuest::QT_EVENTREPEAT );
	_ResetHelperString( CsQuest::QT_DAILY );
	_ResetHelperString( CsQuest::QT_REPEAT );
	_ResetHelperString( CsQuest::QT_TUTORIAL );
	_ResetHelperString( CsQuest::QT_JOINTPROGRESS );
	CsPoint size = CsPoint( 220, CsMin( 340, m_StringList.CalMaxSize( 4 ).y + 10 ) );
	m_pBGWindow->ReSize( size );
	m_pRoot->SetSize( size );
	m_pScrollBar->SetRange( CsPoint( 0, m_StringList.GetSize() ) );

	if( nOldPos > m_pScrollBar->GetMaxPosIndex() )
		nOldPos = m_pScrollBar->GetMaxPosIndex();

	m_pScrollBar->SetCurPosIndex( nOldPos );
}

void cQuestHelper::_ResetHelperString( CsQuest::eQUEST_TYPE eType )
{
	nQuest::cInfo* pQuestInfo = g_pDataMng->GetQuest()->GetInfo();
	cText::sTEXTINFO tiTitle;
	tiTitle.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	cText::sTEXTINFO tiHelper;
	tiHelper.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	cStringList listProcess;
	cStringList TempStringlist;
	std::list< cString* >::iterator it;
	std::list< cString* >::iterator itEnd;
	TCHAR szHelper[ QUEST_HELPER_LEN ];

	bool bFirstQuest = true;

	bool* bQuestHelperView = g_pResist->m_Character.s_bQuestHelperViewID;

	for( int i=0; i<nQuest::MaxExecute; ++i )
	{
		nQuest::sExeInfo ExeInfo = pQuestInfo->GetExeInfo( i );
		if( ExeInfo.m_nIDX == 0 )
		{
			bQuestHelperView[ i ] = false;
			continue;
		}

		if( bQuestHelperView[ i ] == false )
			continue;

		CsQuest* pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( ExeInfo.m_nIDX );
		if( !pFTQuest->IsQuestType( eType ) )
			continue;

		// 해당 타입의 첫번째 퀘스트
		if( bFirstQuest == true )
		{
			bFirstQuest = false;
			_AddLine( eType );
		}

		bool bFirstList = true;		

		// 퀘스트 제목
		tiTitle.s_Color = g_pDataMng->GetQuest()->GetTitleColor( pFTQuest );
		g_pStringAnalysis->Cut_Parcing( &TempStringlist, 140, pFTQuest->m_szTitleText, &tiTitle );
		it	  = TempStringlist.GetList()->begin();
		itEnd = TempStringlist.GetList()->end();		

		bool bComplete = g_pDataMng->GetQuest()->GetProcess( ExeInfo.m_nIDX )->s_bCompleate;

		for( ; it!=itEnd; ++it )
		{
			if( bFirstList == true )
			{
				int nImageIndex = 0;//( pFTQuest->GetQuestType() == CsQuest::QT_MAIN ) ? 1 : 0;
				if( bComplete == true )
				{					
					nImageIndex += 1;//
				}

				(*it)->AddImage_Head( m_pHelperStateImage, nImageIndex, CsPoint( 0, -1 ) );				

				cButton* pBtn = NiNew cButton;
				pBtn->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "DigimonStatus\\Digi_Expen_Rbut.tga", false );
				pBtn->SetTexToken( CsPoint( 0, 32 ) );
				pBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cQuestHelper::OpenQuestDiscriptionWindow );
				pBtn->SetUserData( new QuestHelperButtonInfo( ExeInfo.m_nIDX ) );
				(*it)->AddButton_Head( pBtn, ExeInfo.m_nIDX, CsPoint( 0, 0 ), CsPoint( 16, 12 ) );

				bFirstList = false;
			}
			else
			{
				tiTitle.SetText( _T( "       " ) );
				(*it)->AddText_Head( &tiTitle );
			}

			if( bComplete == false )
			{				
				listProcess.AddTail( *it );
			}
			else
			{				
				m_StringList.AddTail( *it );
			}			
		}
		TempStringlist.GetList()->clear();

		// 퀘스트 헬퍼. 제목 밑에 내용
		if( g_pDataMng->GetQuest()->IsProcess( pFTQuest->GetUniqID() ) == true )
		{			
			CsQuestCompRequireGroup* pCompleteGroup = &g_pDataMng->GetQuest()->GetProcess( pFTQuest->GetUniqID() )->s_CurCompRequireGroup;
			SAFE_POINTER_RET(pCompleteGroup);

			cStringList TemphelperStringlist;
			g_pStringAnalysis->Quest_Parcing( szHelper, QUEST_HELPER_LEN, pFTQuest->m_szHelperText, pCompleteGroup );
			g_pStringAnalysis->Cut_Parcing( &TemphelperStringlist, 350, szHelper, &tiHelper );
			TemphelperStringlist.Delete();

			CsQuestCompRequire::LIST_IT iter = pCompleteGroup->GetList()->begin();
			CsQuestCompRequire::LIST_IT enditer = pCompleteGroup->GetList()->end();
			for( int i = 0; iter!=enditer; ++iter, ++i ) // 퀘스트 수행 목록
			{
				if( i < g_pStringAnalysis->m_vecStringTotalText.size() ) // 표시할 스트링이 있을때만 
				{
					cString* pButtonString = MakeFindWayButton( (*iter), g_pStringAnalysis->m_vecStringTotalText[i].c_str() );
					SAFE_POINTER_CON(pButtonString);

					if( bComplete ) // 완료 퀘스트라면
						m_StringList.AddTail( pButtonString );

					else	// 미완료된 퀘스트라면
						listProcess.AddTail( pButtonString ); // 임시 목록에 추가해 주고 
				}
			}

			if( pCompleteGroup->GetList()->empty() ) // 퀘스트 수행 목록이 없다면
			{
				if( !g_pStringAnalysis->m_vecStringTotalText.empty() ) // 표시할 스트링이 있을때만 
				{
					if( bComplete )	// 완료된 퀘스트라면
						m_StringList.AddTail( MakeFindWayButton( pFTQuest->GetQuestTargetValue1(), CsQuestCompRequire::NPCTALK,
							g_pStringAnalysis->m_vecStringTotalText[0].c_str(), FONT_GREEN ) );
					
					else	// 미완 퀘스트라면
						listProcess.AddTail( MakeFindWayButton( pFTQuest->GetQuestTargetValue1(), CsQuestCompRequire::NPCTALK,
							g_pStringAnalysis->m_vecStringTotalText[0].c_str(), FONT_WHITE ) ); // 임시 목록에 추가해 주고 
				}
			}
		}
		else
		{
			xstop(false, "퀘스트 목록 확인해 볼 것...");
			//#pragma todo( "서버가 정지 되면서 퀘스트 목록이 꼬인경우 - 완료에도 있으면서 진행상황에도 있을 - 예외처리" )

			net::game->SendQuestCancel( pFTQuest->GetUniqID() );
			pQuestInfo->Cancel( pFTQuest->GetUniqID() );

			// 수행지급 아이템 제거
			CsQuestApplyRequiteGroup* pApplyRequiteGroup = pFTQuest->GetApplyRequiteGroup();
			CsQuestApplyRequite* pApplyRequite;
			int nElementCount = pApplyRequiteGroup->GetListCount();
			for( int i=0; i<nElementCount; ++i )
			{
				pApplyRequite = pApplyRequiteGroup->Get( i );
				switch( pApplyRequite->GetType() )
				{
				case CsQuestApplyRequite::ITEM:
					{
						g_pDataMng->GetInven()->ItemRemove_NoServer( pApplyRequite->GetTypeID(), pApplyRequite->GetTypeCount(), false );
					}
					break;
				default:
					assert_cs( false );
				}
			}
		}
	}

	std::list< cString* >* pListProcess = listProcess.GetList();
	it = pListProcess->begin();
	itEnd = pListProcess->end();
	for( ; it!=itEnd; ++it )
	{
		m_StringList.AddTail( *it );
	}
	pListProcess->clear();
}

void cQuestHelper::_AddLine( CsQuest::eQUEST_TYPE eType )
{
	cString* pString = NiNew cString;
	pString->AddSprite( m_pLineLeft, CsPoint( 0, 6 ) )->s_ptSize.x += 9;	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 247/255.0f, 211/255.0f, 153/255.0f ) );
	switch( eType )
	{
	case CsQuest::QT_MAIN:
		ti.SetText( UISTRING_TEXT( "QUESTHELPER_MAIN_QUEST" ).c_str() );
		break;

	case CsQuest::QT_SUB:
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_SUBQUEST" ).c_str() );
		break;

	case CsQuest::QT_EVENTREPEAT:
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_EVENT_QUEST" ).c_str() );
		break;

	case CsQuest::QT_REPEAT:
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_REPEATABLE_QUEST" ).c_str() );
		break;

	case CsQuest::QT_DAILY:
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_DAILY_QUEST" ).c_str() );
		break;
// 		
// 	case CsQuest::QT_TIME:
// 		ti.SetText( _LAN( "시간제한 퀘스트" ) );
// 		break;
// 		
	case CsQuest::QT_JOINTPROGRESS:
		ti.SetText( UISTRING_TEXT( "QUEST_JOGRESS_QUEST" ).c_str() );
		break;
#ifdef SDM_TAMER_XGUAGE_20180628
	case CsQuest::QT_XANTI_JOINTPROGRESS:
		ti.SetText( UISTRING_TEXT( "QUEST_XJOGRESS_QUEST" ).c_str() );
		break;
#endif
	case CsQuest::QT_TUTORIAL:
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_TUTORIAL_QUEST" ).c_str() );
		break;
	default:
		assert_cs( false );
	}
	pString->AddText( &ti )->s_ptSize.x += 9;
	pString->CalMaxSize();
	pString->AddSprite( m_pLineRight, CsPoint( 0, 6 ), CsPoint( 220 - pString->GetMaxSize().x - 30, 1 ) );

	m_StringList.AddTail( pString );
}

cString* cQuestHelper::MakeFindWayButton( int nTargetID, CsQuestCompRequire::eTYPE eQuestCompleteType, const TCHAR* pButtonText, NiColor color )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	CsPoint ptPos1;
	int nBase1;
	cText::GetStringSize( &ti, ptPos1, nBase1, pButtonText );
	ti.SetText( pButtonText );

	cString* pDestString = NiNew cString;
	cQuestFindWay FindWayInfo;
	if( FindWayInfo.Initialize(nTargetID , eQuestCompleteType) ) // 길찾기 정보 초기화 성공
	{
		ti.s_Color = color;

		cButton* pDestButton = NiNew cButton;
		pDestButton->Init( NULL, CsPoint::ZERO, ptPos1, cButton::IMAGE_ALPHA_1, &ti, false, CsPoint( 0, -1 ) ); 			
		pDestButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cQuestHelper::ProcessFindWay );
		pDestButton->SetUserData( new cQuestFindWay(FindWayInfo) );
		pDestString->AddButton( pDestButton, 0, CsPoint::ZERO, ptPos1 );
	}
	else // 실패
	{
		ti.s_Color = color;
		pDestString->AddText( &ti );
	}
	return pDestString;
}

cString* cQuestHelper::MakeFindWayButton( CsQuestCompRequire* pCompleteInfo, const TCHAR* pButtonText )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	CsPoint ptPos1;
	int nBase1;
	cText::GetStringSize( &ti, ptPos1, nBase1, pButtonText );
	ti.SetText( pButtonText );

	cString* pDestString = NiNew cString;

	ti.s_Color = FONT_WHITE;
	if( pCompleteInfo->IsCompleat() )
		ti.s_Color = FONT_GREEN;

	cQuestFindWay FindWayInfo;
	if( FindWayInfo.Initialize(pCompleteInfo->GetTypeID(), pCompleteInfo->GetType(), pCompleteInfo->GetTargetID()) ) // 길찾기 정보 초기화 성공
	{			
		cButton* pDestButton = NiNew cButton;
		pDestButton->Init( NULL, CsPoint::ZERO, ptPos1, cButton::IMAGE_ALPHA_1, &ti, false, CsPoint( 0, -1 ) ); 			
		pDestButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cQuestHelper::ProcessFindWay );
		pDestButton->SetUserData( new cQuestFindWay(FindWayInfo) );
		pDestString->AddButton( pDestButton, 0, CsPoint::ZERO, ptPos1 );
	}
	else // 실패
	{
		pDestString->AddText( &ti );
	}
	return pDestString;
}

void cQuestHelper::ProcessFindWay( void* pSender, void* pData )
{
	SAFE_POINTER_RET(pSender);
	cButton* pButton = static_cast<cButton*>(pSender);
	cQuestFindWay findWayInfo = *static_cast<cQuestFindWay*>(pButton->GetUserData());

	if( g_pHelpArrow->SetArrowPoint( findWayInfo.m_nDestTableID, findWayInfo.m_nDestMapID, findWayInfo.m_nDestType, const_cast<TCHAR*>(findWayInfo.m_strDestName.c_str()) ))
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_ZONEMAP );
	}
}

void cQuestHelper::OpenQuestDiscriptionWindow( void* pSender, void* pData )
{
	SAFE_POINTER_RET(pSender);
	cButton* pButton = static_cast<cButton*>(pSender);
	QuestHelperButtonInfo ButtonInfo = *static_cast<QuestHelperButtonInfo*>(pButton->GetUserData());

	// 이미 열려져 있다면
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_LIST ) )
	{
		// 같은퀘스트 라면 창 닫아준다.
		if( g_pGameIF->GetQuestList()->GetSelectQuestID() == ButtonInfo.nQuestIndex )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_LIST );
		// 다른 퀘스트라면 해당 퀘스트 선택
		else
			g_pGameIF->GetQuestList()->InitSelectQuest( ButtonInfo.nQuestIndex );
	}
	// 닫혀져 있다면 열어주자
	else
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_LIST, 0, ButtonInfo.nQuestIndex  );
	}	
}