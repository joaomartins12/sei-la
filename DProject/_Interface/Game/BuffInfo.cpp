
#include "stdafx.h"
#include "BuffInfo.h"

#define BUFF_IDX 0
#define DEBUFF_IDX 1

cBuffInfo::cBuffInfo() : m_pBGImage(NULL), m_pBuffData(NULL), m_AddSystempBuffPosY(0)
{

}

void cBuffInfo::Destroy()
{
	cBaseWindow::Delete();
	m_pBuffData = NULL;
}

void cBuffInfo::DeleteResource()
{
	DeleteScript();
	SAFE_NIDELETE( m_pBGImage );
	SAFE_NIDELETE( m_pTextTime );
	SAFE_NIDELETE( m_pPlusButton );
	SAFE_NIDELETE( m_pMinusButton );

	for( int i = 0 ; i < m_nStructCount; i++ )
	{
		SAFE_NIDELETE( m_sWin[ i ].s_pPopup );		
	}	
#ifdef GUILD_RENEWAL
	if( m_nType == IF_BUFF_SYSTEM )
		SAFE_NIDELETE( m_pGuildBuffIcon );
#endif
}

void cBuffInfo::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

#ifdef BUFFINFO_FIX
	CsPoint ptTargetWinPos;
	if( g_pGameIF )
	{
		cBaseWindow* pWin = g_pGameIF->_GetPointer( cBaseWindow::WT_TARGET_WINDOW, 0 );
		if( pWin )
			ptTargetWinPos = pWin->GetRootClient();
	}

	int nBuffWidth = ptTargetWinPos.x - 239;	//타겟 윈도우 시작점 - 버프윈도우 시작점 ( 실제 버프윈도우 사이즈 )
	m_nMAX_Col = nBuffWidth / 32;

	// 버프갯수 ( 버프리스트 + 덱 버프 )
	m_nBuffCnt = 0;
	m_ptResolution = CsPoint( g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight );
#else	//BUFFINFO_FIX
	m_nMAX_Col = g_nScreenWidth / 210;
#endif	//BUFFINFO_FIX
	m_nType = nValue;
	// 버프 영역 설정
	switch( m_nType )
	{
	case IF_BUFF_MY_T:
		InitScript( NULL, CsPoint( 239, 26 ), CsPoint( g_nScreenWidth - 500, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_MY_D:
		InitScript( NULL, CsPoint( 239, 103 ), CsPoint( g_nScreenWidth - 500, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_SYSTEM:
		InitScript( NULL, CsPoint( 399, 583 ), CsPoint( 160, 40 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM );
		break;
	case IF_BUFF_TARGET:
#ifdef BUFFINFO_FIX
		m_nMAX_Col = 10;
		InitScript( NULL, CsPoint( 394, 88 ), CsPoint( g_nScreenWidth - 650, 70 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::TOP );
#else
		InitScript( NULL, CsPoint( 631, 35 ), CsPoint( g_nScreenWidth - 650, 70 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::CENTER );
#endif
		break;
#ifdef PARTY_BUFF_SHOW
	case IF_BUFF_PARTY1_T:
		InitScript( NULL, CsPoint( 204, 204 ), CsPoint( 160, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_PARTY1_D:
		InitScript( NULL, CsPoint( 204, 216+24 ), CsPoint( 160, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_PARTY2_T:
		InitScript( NULL, CsPoint( 204, 274 ), CsPoint( 160, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_PARTY2_D:
		InitScript( NULL, CsPoint( 204, 286+24 ), CsPoint( 160, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_PARTY3_T:
		InitScript( NULL, CsPoint( 204, 344 ), CsPoint( 160, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
	case IF_BUFF_PARTY3_D:
		InitScript( NULL, CsPoint( 204, 356+24 ), CsPoint( 160, 70 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
		break;
#endif
	default:
		assert_cs( false );
	}

	//윈도우 시작 위치 저장 - 위치 이동할때 써먹음
	m_ptRootPos = GetRootClient();

	// 기타 버프류 사용값 셋팅
	m_pBGImage = NiNew cImage;
	switch( m_nType )
	{
	case IF_BUFF_MY_T:	
	case IF_BUFF_MY_D:	
		m_pBGImage->Init( NULL, CsPoint::ZERO, CsPoint( 24, 24 ), "TargetWindow\\BuffInfo.tga", false );
		m_nDeltaX =	29;
		m_nDeltaY =	37;
		m_nStructCount = 2;
		break;
	case IF_BUFF_SYSTEM:
		m_pBGImage->Init( NULL, CsPoint::ZERO, CsPoint( 24, 24 ), "TargetWindow\\BuffInfo.tga", false );
		m_nDeltaX =	29;
		m_nDeltaY =	37;
		m_nStructCount = 1;
	
		if( g_pResist->m_CenterBar.s_bIsOpenQuick )//추가 퀵슬롯 오픈상태일 때
			SetAddSystempBuffPosY(44);
		else
			SetAddSystempBuffPosY(0);
	
#ifdef GUILD_RENEWAL
		// 적용중인 길드 스킬 버프로 표시
		m_pGuildBuffIcon = NiNew cImage;
		m_pGuildBuffIcon->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "MainBar\\Main_Guild_icon.tga", false );
#endif
		break;
#ifdef PARTY_BUFF_SHOW
	case IF_BUFF_PARTY1_T:		
	case IF_BUFF_PARTY1_D:
	case IF_BUFF_PARTY2_T:
	case IF_BUFF_PARTY2_D:
	case IF_BUFF_PARTY3_T:
	case IF_BUFF_PARTY3_D:
		m_pBGImage->Init( NULL, CsPoint::ZERO, CsPoint( 24, 24 ), "TargetWindow\\BuffInfo.tga", false );
		m_nDeltaX =	29;
		m_nDeltaY = 37;
		m_nStructCount = 2;
		break;
#endif

	default:
		m_pBGImage->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "TargetWindow\\BuffInfo.tga", false );		
		m_nDeltaX =	22;
		m_nDeltaY =	24;
		m_nStructCount = 2;
		break;

	}
	m_pBGImage->SetTexToken( CsPoint( 24, 0 ) );

	m_pPlusButton = NiNew cButton;	
	m_pPlusButton->Init( m_pRoot, CsPoint( m_nMAX_Col * 29, 5 ), CsPoint( 6, 18 ), "DigimonStatus\\Digi_Expen_Rbut.tga", false );
	m_pPlusButton->SetTexToken( CsPoint( 0, 32 ) );

	m_pMinusButton = NiNew cButton;	
	m_pMinusButton->Init( m_pRoot, CsPoint( m_nMAX_Col * 29, 5 ), CsPoint( 6, 18 ), "DigimonStatus\\Digi_Expen_Lbut.tga", false );
	m_pMinusButton->SetTexToken( CsPoint( 0, 32 ) );

	for( int i = 0 ; i < m_nStructCount; i++ )
	{
		m_sWin[ i ].s_pPopup = NiNew cPopUp;	
		m_sWin[ i ].s_pPopup->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), CsPoint( 16, 16 ), "Control_G\\Popup\\ON2Dlg", "tga", false );
		m_sWin[ i ].s_bRenderPopup = false;

		m_sWin[ i ].s_pCurButton = m_pPlusButton;
	}		

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9, FONT_WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );

	m_pTextTime = NiNew cText;
	m_pTextTime->Init( NULL, CsPoint::ZERO, &ti, false );	
}

void cBuffInfo::Update(float const& fDeltaTime)
{
	if( m_pBuffData == NULL )
		return;

	cBuffData::LIST_BUFF_IT it = m_pBuffData->GetBuff()->begin();
	cBuffData::LIST_BUFF_IT itEnd = m_pBuffData->GetBuff()->end();

	for( ; it != itEnd; ++it )
	{
		if( g_pCharMng->GetTamerUser()->IsBattle() == true && it->s_pBuffTable->GetInfo()->s_dwID == 40148 )	// 전투 상태로 변경되면 평환 선언 제거
		{
			CsC_AvObject* pTarget = g_pCharMng->GetTamerUser();
			( (CTamer*)pTarget)->GetBuff()->ReleaseBuff( 40148 );
			break;
		}

		if( it->s_pBuffTable->GetInfo()->s_dwID == 40148 && it->s_nLifeTime <= 0 )	// 시간 종료되면 평화 선언 Release
		{
			CsC_AvObject* pTarget = g_pCharMng->GetTamerUser();
			( (CTamer*)pTarget)->GetBuff()->ReleaseBuff( 40148 );
			break;
		}
	}

#ifdef BUFFINFO_FIX
	switch( m_nType )
	{
	case IF_BUFF_MY_T:	case IF_BUFF_MY_D:	case IF_BUFF_TARGET:
	#ifdef PARTY_BUFF_SHOW
		case IF_BUFF_PARTY1_T:		case IF_BUFF_PARTY1_D:
		case IF_BUFF_PARTY2_T:		case IF_BUFF_PARTY2_D:
		case IF_BUFF_PARTY3_T:		case IF_BUFF_PARTY3_D:
	#endif
		{
			int nUsingDeck = 0;
			int UseDeck = 0;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &UseDeck );

			if( UseDeck != INT_MIN && m_nType == IF_BUFF_MY_T )	// 덱 사용 중 && 테이머 버프 타입일 경우
					nUsingDeck = ENCY_MAX_OPTION;	//덱 효과는 최대 3개까지 가능하므로 ..

			// 버프랑 디버프 중에 개수가 더 큰 걸로, 저장해둔 버프 개수랑 비교
			// 다르면 업데이트
			int nBuffCnt = CsMax( m_pBuffData->GetDeBuff()->size(), m_pBuffData->GetBuff()->size() );
			if( m_nBuffCnt != nBuffCnt + nUsingDeck )
			{
				m_nBuffCnt = nBuffCnt + nUsingDeck;
				// 버프 윈도우 사이즈 수정
				m_pRoot->SetSize( CsPoint( m_nBuffCnt * 32 + 29, 70 ) );// 버프 수 * 사이즈 + 확장 버튼 사이즈
				
			}
			// 버프 윈도우 사이즈 셋팅
			CsPoint ptTargetWinPos;
			if( g_pGameIF )
			{
				cBaseWindow* pWin = g_pGameIF->_GetPointer( cBaseWindow::WT_TARGET_WINDOW, 0 );
				if( pWin )
					ptTargetWinPos = pWin->GetRootClient();
			}
			int nBuffWidth = ptTargetWinPos.x - 239;
			m_nMAX_Col = nBuffWidth / 32;

		}
		break;		
		default:		break;
	}

	// 해상도 얻어와서	// 해상도 저장
	m_ptResolution = CsPoint( g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight );

	m_ptRootPos = GetRootClient();	// 위치 저장(마우스업데이트용)

	//ResetDevice();
#endif	//BUFFINFO_FIX
}

cBaseWindow::eMU_TYPE
cBuffInfo::Update_ForMouse()
{
	if( m_nType == IF_BUFF_SYSTEM )	// 시스템버프( 퀵슬롯 위 )일 경우만 위치 이동
		m_pRoot->SetPos( m_ptRootPos - CsPoint( 0, -GetDeltaPosY() ) );

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW || NULL == m_pBuffData)
		return MUT_OUT_WINDOW;

	for( int i = 0 ; i < m_nStructCount; i++ )
	{	
		switch( m_sWin[ i ].s_pCurButton->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				m_sWin[ i ].s_bRenderPopup = !m_sWin[ i ].s_bRenderPopup;			

				if( m_sWin[ i ].s_bRenderPopup == true )
					m_sWin[ i ].s_pCurButton = m_pMinusButton;
				else
					m_sWin[ i ].s_pCurButton = m_pPlusButton;
			}			
			return muReturn;
		default:
			return muReturn;
		}
	}	

	CsPoint ptLocalMouse = MousePosToWindowPos( CURSOR_ST.GetPos() );
	int nIdx = 0;
	cBuffData::LIST_BUFF_IT it;
	cBuffData::LIST_BUFF_IT itEnd;

	switch( m_nType )
	{
	case IF_BUFF_SYSTEM:
		{
			it = m_pBuffData->GetSystemBuff()->begin();
			itEnd = m_pBuffData->GetSystemBuff()->end();
			for( ; it != itEnd; it++, nIdx++ )
			{	
				// 툴팁
				CsPoint pos = CsPoint( ( nIdx * m_nDeltaX ), GetDeltaPosY());

				CsRect rc( pos, CsSIZE( 24, 24 ) );

				if( rc.PtInRect( ptLocalMouse ) == TRUE )
				{
#ifdef VERSION_USA
					//버프툴팁 사이즈 수정
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 300, cTooltip::BUFFINFO,	m_nType, 0, 0, 0, &(*it) );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 240, cTooltip::BUFFINFO,	m_nType, 0, 0, 0, &(*it) );
#endif
					return muReturn;
				}
			}
#ifdef GUILD_RENEWAL
			bool isActiveSkill = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_ISACTIVESKILL, &isActiveSkill );
			// 적용 중인 길드 스킬 -> 시스템 버프 뒤에 버프형식으로 표현 - 툴팁
			if( isActiveSkill == true )
			{
				CsPoint pos = CsPoint( ( nIdx * m_nDeltaX ), GetDeltaPosY());

				CsRect rc( pos, CsSIZE( 32, 32 ) );
				if( rc.PtInRect( ptLocalMouse ) == TRUE )
				{
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 240, cTooltip::GUILD_BUFF, 0 );
					return muReturn;
				}
			}
#endif
		}
		break;
	default:
		{
			it = m_pBuffData->GetBuff()->begin();
			itEnd = m_pBuffData->GetBuff()->end();

			for( ; it != itEnd; it++, nIdx++ )
			{	
				if( nIdx == m_nMAX_Col )
					break;

				// 툴팁
				CsPoint pos = CsPoint( ( nIdx * m_nDeltaX ), 0 );
				CsRect rc( pos, CsSIZE( 24, 24 ) );

				if( rc.PtInRect( ptLocalMouse ) == TRUE )
				{
#ifdef VERSION_USA
					//버프툴팁 사이즈 수정
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 300, cTooltip::BUFFINFO,	m_nType, 0, 0, 0, &(*it) );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 240, cTooltip::BUFFINFO,	m_nType, 0, 0, 0, &(*it) );
#endif
					return muReturn;
				}
			}

#ifdef BUFFINFO_FIX
			int nEncyIdx = nIdx;
			int nAddIdx = 0;
			for( ; nEncyIdx < nIdx + ENCY_MAX_OPTION ; nEncyIdx++ )	// 한 덱에 최대 3가지 효과를 가질 수 있음
			{
				if( m_sWin[ BUFF_IDX ].s_bRenderPopup == true )	// 추가 버프창 렌더 중일 때만
				{
					if( nEncyIdx < m_nMAX_Col )	// 버프 윈도우 넓이를 벗어나지 않을 때
					{
						if( SetEncyTooltip( nEncyIdx, nEncyIdx - nIdx ) )	//덱 버프 툴팁
							return muReturn;
					}
					else	// 버프 윈도우 넓이를 벗어남, 추가 버프창 셋팅
					{
						//남은 버프 데이터
						for( ; it != itEnd; it++, nAddIdx++ )
						{
							// 툴팁
							CsPoint pos = GetRootClient() + CsPoint( (m_nMAX_Col * 32) + 7 , 2 ) + CsPoint( nAddIdx* m_nDeltaX , 0 ) ;
							CsRect rc( pos, CsSIZE( 24, 24 ) );
							CsPoint ptMousePos = CURSOR_ST.GetPos();

							if( rc.PtInRect( ptMousePos ) == TRUE )
							{
#ifdef VERSION_USA
								//버프툴팁 사이즈 수정
								TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( /*GetRootClient() + */pos , CsPoint( 24, 24 ), 300, cTooltip::BUFFINFO,	0, 0, 0, 0, &(*it) );
#else
								TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( /*GetRootClient() + */pos , CsPoint( 24, 24 ), 240, cTooltip::BUFFINFO,	0, 0, 0, 0, &(*it) );
#endif
								return muReturn;
							}
						}

						if( SetEncyTooltip( nEncyIdx - m_nMAX_Col + nAddIdx , nEncyIdx - nIdx, CsPoint( ( m_nMAX_Col * 32 ) + 7 , 0 ) ) )	//덱 버프 툴팁
							return muReturn;
					}
				}
				else
					if( nEncyIdx < m_nMAX_Col )	// 버프 윈도우 넓이를 벗어나지 않을 때
					{
						if( SetEncyTooltip( nEncyIdx, nEncyIdx - nIdx ) )	//덱 버프 툴팁
							return muReturn;
					}
			}
#endif	//BUFFINFO_FIX
			it = m_pBuffData->GetDeBuff()->begin();
			itEnd = m_pBuffData->GetDeBuff()->end();
			nIdx = 0;
			for( ; it != itEnd; it++, nIdx++ )
			{
				if( nIdx == m_nMAX_Col )
					break;

				// 툴팁
				CsPoint pos = CsPoint( ( nIdx * m_nDeltaX ), m_nDeltaY );
				CsRect rc( pos, CsSIZE( 24, 24 ) );

				if( rc.PtInRect( ptLocalMouse ) == TRUE )
				{
#ifdef VERSION_USA
					//버프툴팁 사이즈 수정
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 300, cTooltip::BUFFINFO,	 m_nType, 0, 0, 0, &(*it) );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos + GetRootClient(), CsPoint( 24, 24 ), 240, cTooltip::BUFFINFO,	 m_nType, 0, 0, 0, &(*it) );
#endif
					return muReturn;
				}
			}
#ifndef BUFFINFO_FIX
			int nUsingDeck = 0;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &nUsingDeck );

			if( nUsingDeck != INT_MIN && m_nType == IF_BUFF_MY_T)// || m_nType == IF_BUFF_TARGET) )	// 덱 사용 중 && 테이머 버프 타입일 경우
			{
				int nCurIdx = (int)m_pBuffData->GetBuff()->size();
				CsPoint ptCurPos;

				//적용중인 덱 옵션
				for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
				{
					std::pair<int,USHORT> sdfOpt;
					sdfOpt.first = i;
					GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );
					USHORT nOption = sdfOpt.second;

					if( nOption == 0 )
						continue;
					if( nCurIdx < m_nMAX_Col )
						ptCurPos = CsPoint( nCurIdx * m_nDeltaX, 0 );
					else
						ptCurPos = CsPoint( (nCurIdx-m_nMAX_Col) * m_nDeltaX, m_nDeltaY );
					CsRect rc( ptCurPos, CsSIZE( 24, 24 ) );

					if( rc.PtInRect( ptLocalMouse ) == TRUE )
					{
						//TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptCurPos + GetRootClient(), CsPoint( 24, 24 ), 240, cTooltip::DECKBUFF, nOpt, nVal, nIcon, nRemainTime, pGroupInfo );
						TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptCurPos + GetRootClient(), CsPoint( 24, 24 ), 240, cTooltip::DECKBUFF, i, 0, 0, 0, NULL );

						return muReturn;
					}
					nCurIdx++;
				}
			}
#endif	//BUFFINFO_FIX
		}
		break;
	}

	return MUT_OUT_WINDOW;
}

void cBuffInfo::OnRButtonUp( CsPoint pos )
{
	if( m_pBuffData == NULL )
		return;

	cBuffData::LIST_BUFF_IT it;
	cBuffData::LIST_BUFF_IT itEnd;

	int AddSysBuffY = 0;

	switch( m_nType )
	{
	case IF_BUFF_MY_T:	
	case IF_BUFF_MY_D:
		{
			it = m_pBuffData->GetBuff()->begin();
			itEnd = m_pBuffData->GetBuff()->end();	
		}
		break;
	case IF_BUFF_SYSTEM:
		{
			it = m_pBuffData->GetSystemBuff()->begin();
			itEnd = m_pBuffData->GetSystemBuff()->end();
			AddSysBuffY = GetDeltaPosY();
		}
		break;
	default:
		return;
	}	

	CsPoint ptLocalMouse = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int nIdx = 0; it != itEnd; it++, nIdx++ )
	{
		CsBuff::sINFO* pInfo = it->s_pBuffTable->GetInfo();
		if( pInfo->s_bDelete == false || pInfo->s_dwSkillCode == 0 )
			continue;

		CsPoint pos;
		if( nIdx < m_nMAX_Col )
			pos = CsPoint( ( nIdx * m_nDeltaX ), AddSysBuffY );
		else if( m_sWin[ BUFF_IDX ].s_bRenderPopup )
			pos = CsPoint( (m_nMAX_Col * 32) + 7 , 2 ) + CsPoint( (nIdx - m_nMAX_Col)* m_nDeltaX , AddSysBuffY ) ;
		else
			break;

		CsRect rc( pos, CsSIZE( 24, 24 ) );
		if( !rc.PtInRect( ptLocalMouse ) )	// 버프 삭제
			continue;
		cPrintMsg::PrintMsg( 30451 );
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1( m_pBuffData->GetObject()->GetLeafRTTI() );
		pMBox->SetValue2( pInfo->s_dwID );	//m_pBuffData->ReleaseBuff(pInfo->s_dwID);		
	//	pMBox->SetCharValue1( pInfo->s_szName );
		break;
	}
}

void cBuffInfo::Render()
{
	//RenderScript();
	if( m_pBuffData == NULL )
		return;

	bool bTextRender = false;
	CsPoint ptSize;
 
	switch( m_nType )
	{
	case IF_BUFF_MY_T:	
	case IF_BUFF_MY_D:	
	case IF_BUFF_SYSTEM:	
	case IF_BUFF_PARTY1_T:
	case IF_BUFF_PARTY1_D:
	case IF_BUFF_PARTY2_T:
	case IF_BUFF_PARTY2_D:
	case IF_BUFF_PARTY3_T:
	case IF_BUFF_PARTY3_D:
		bTextRender = true;
		ptSize = CsPoint( 20, 20 );
		break;
	default:
		ptSize = CsPoint( 15, 15 );
		break;
	}

	int nIdx = 0;
	cBuffData::LIST_BUFF_IT it;		
	cBuffData::LIST_BUFF_IT itEnd; 

	switch( m_nType )
	{
	case IF_BUFF_SYSTEM:
		{
			it = m_pBuffData->GetSystemBuff()->begin();
			itEnd = m_pBuffData->GetSystemBuff()->end();

			for( ; it != itEnd; it++, nIdx++ )
			{
				UINT nRemain = it->s_nLifeTime;
				UINT nSec = nRemain;
				UINT nMin = nRemain / 60;
				UINT nHour = nMin / 60;
				UINT nDay = nHour / 24;		

				if( bTextRender == true && nHour == 0 ) 
				{
					if( nMin > 0 )
					{
						std::wstring wsMin;
						DmCS::StringFn::Format( wsMin, L"%d %s", nMin, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
						m_pTextTime->SetText( wsMin.c_str() );
					}
					else
					{
						std::wstring wsSec;
						DmCS::StringFn::Format( wsSec, L"%d %s", nSec, UISTRING_TEXT( "COMMON_TXT_SHORT_SEC" ).c_str() );
						m_pTextTime->SetText( wsSec.c_str() );
					}
					m_pTextTime->Render( GetRootClient() + CsPoint( 11 + (nIdx * m_nDeltaX), GetDeltaPosY() + 24 ) );
				}
				else if( bTextRender == true && nHour >= 1)
				{
					std::wstring wsHour;
					DmCS::StringFn::Format( wsHour, L"%d %s", nHour, UISTRING_TEXT( "COMMON_TXT_SHORT_HOUR" ).c_str() );
					m_pTextTime->SetText( wsHour.c_str() );
					m_pTextTime->Render( GetRootClient() + CsPoint( 11 + (nIdx * m_nDeltaX), GetDeltaPosY() + 24 ) );
				}

				m_pBGImage->Render( GetRootClient() + CsPoint( nIdx * m_nDeltaX, GetDeltaPosY() ), 2 );		

				g_pIconMng->RenderIcon( ICONITEM::SKILL2, GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), GetDeltaPosY() + 2 ), ptSize, it->s_pBuffTable->GetInfo()->s_nBuffIcon % 1000 );		
				g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), GetDeltaPosY()+ 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );
			}
#ifdef GUILD_RENEWAL
			bool isActiveSkill = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_ISACTIVESKILL, &isActiveSkill );
			// 적용 중인 길드 스킬 -> 시스템 버프 뒤에 버프형식으로 표현 - 버프아이콘 랜더
			if( isActiveSkill )
			{
				CsPoint pos = GetRootClient() + CsPoint( ( nIdx * m_nDeltaX ), GetDeltaPosY());
				m_pGuildBuffIcon->Render(pos);
			}
#endif
		}
		break;
	default:
		{
			it = m_pBuffData->GetBuff()->begin();
			itEnd = m_pBuffData->GetBuff()->end();

			for( ; it != itEnd; it++, nIdx++ )
			{
				// Cap only applies to IF_BUFF_TARGET — the small buff strip drawn on
				// a targeted entity, which has limited horizontal space (computed
				// from the target window's position in Update()).  Player's own buff
				// bars (IF_BUFF_MY_T, IF_BUFF_MY_D) and party variants must render
				// every active buff regardless of where the target window sits;
				// originally a single cap was shared, which made own buffs vanish
				// the moment the player happened to have anything targeted.
				if( m_nType == IF_BUFF_TARGET && nIdx == m_nMAX_Col )
					break;

				UINT nRemain = it->s_nLifeTime;
				UINT nSec = nRemain;
				UINT nMin = nRemain / 60;
				UINT nHour = nMin / 60;
				UINT nDay = nHour / 24;

				if( bTextRender == true && nHour == 0 ) 
				{
					if( nMin > 0 )
					{
						std::wstring wsMin;
						DmCS::StringFn::Format( wsMin, L"%d %s", nMin, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
						m_pTextTime->SetText( wsMin.c_str() );
						//m_pTextTime->Render( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), 24 ) );
					}
					else
					{
						std::wstring wsSec;
						DmCS::StringFn::Format( wsSec, L"%d %s", nSec, UISTRING_TEXT( "COMMON_TXT_SHORT_SEC" ).c_str() );
						m_pTextTime->SetText( wsSec.c_str() );
						//m_pTextTime->Render( GetRootClient() + CsPoint( 11 + (nIdx * m_nDeltaX), 24 ), DT_CENTER );
					}
					m_pTextTime->Render( GetRootClient() + CsPoint( 11 + (nIdx * m_nDeltaX), 24 ) );
				}

				m_pBGImage->Render( GetRootClient() + CsPoint( nIdx * m_nDeltaX, 0 ), 0 );		

				int nStack = 0;
#ifdef MONSTER_SKILL_GROWTH
				nStack = it->m_nStack;
#endif
				ICONITEM::eTYPE type = ICONITEM::SKILL4;
				USHORT	usBuffIcon = it->s_pBuffTable->GetInfo()->s_nBuffIcon;
				if( usBuffIcon >= 4000 )		type = ICONITEM::SKILL4;
				else if( usBuffIcon >= 3000 )	type = ICONITEM::SKILL3;
				else if( usBuffIcon >= 2000 )	type = ICONITEM::SKILL2;
				else							type = ICONITEM::SKILL1;

				g_pIconMng->RenderIcon( type, GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), 2 ), ptSize, it->s_pBuffTable->GetInfo()->s_nBuffIcon % 1000, nStack  );

				// 디지몬의 전체 hp에서 일정 %이하일때 발동되니 %이상일때는 mask 렌더
				if( it->s_pBuffTable != 0 && it->s_pBuffTable->GetInfo()->s_dwSkillCode != 0)
				{
					CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->s_pBuffTable->GetInfo()->s_dwSkillCode );

					float fHpRate = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetHPRate();

					switch( pSkill->GetInfo()->s_Apply[0].s_nA )
					{
					case APPLY_HPDMG/*42*/:		// 내 디지몬 HP가 B% 이하 일때 스킬 데미지 C% 증가
					case APPLY_HPDEF/*44*/:		// 내 디지몬 HP가 B% 이하 일때 받는 데미지 C% 감소
					case APPLY_INSURANCE/*47*/:	// 내 디지몬 HP가 B% 이하 일때 HP C% 증가
						if( fHpRate > (float)( pSkill->GetInfo()->s_Apply[0].s_nB) / 100.f )
							g_pIconMng->RenderMask( ICONMASK::IF_ICON_REAL_RED, GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), 2 ), ptSize );
						break;
						// 					default:
						// 						assert_cs( false );
					}
				}



				// 타겟 윈도우가 아닐 때(My테이머 / My디지몬)에만 타임마스크 렌더
#ifdef TARGETUI_TIMEMASK_DELETE
				if( m_nType != IF_BUFF_TARGET )
#endif
				{
					if( it->s_nTimeIverse < 60.0f )  //60초 미만 버프 마스크 랜더 시 이상 관련
						g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), 2 ), (float)(ptSize.y / 2), (float)nSec, (float)it->s_nTimeIverse );
					else
						g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );
				}
			}	

#ifdef BUFFINFO_FIX// BUFFINFO_FIX (버프 윈도우 수정)
			if( nIdx < m_nMAX_Col /*&& m_nType == IF_BUFF_MY_T*/ ) // 버프 윈도우 넓이를 벗어나지 않을 때
			{
				RenderEncyBuff( nIdx, ptSize );
			}
			else //if( nIdx >= m_nMAX_Col )	// 버프 윈도우 넓이를 벗어남, 추가 버프창 셋팅
			{
				// 덱 효과 사용 중인지 체크
				int nUsingDeck = 0;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &nUsingDeck );

				bool bUse = false;

				if( nUsingDeck != INT_MIN && m_nType == IF_BUFF_MY_T)// || m_nType == IF_BUFF_TARGET) )	// 덱 사용 중 && 테이머 버프 타입일 경우
					bUse = true;

				//덱 효과 사용 중이거나 버프가 남아있으면
				if( (int)m_pBuffData->GetBuff()->size() > m_nMAX_Col || bUse )
				{
					//남아있는 버프 마저 셋팅해주자
					nIdx = 0;

					// 추가 버프창 열기/닫기버튼
					m_sWin[ BUFF_IDX ].s_pCurButton->SetPos( CsPoint( m_nMAX_Col * 29, 5 ) );
					m_sWin[ BUFF_IDX ].s_pCurButton->Render();

					// 추가 버프창 열려있을때
					if( m_sWin[ BUFF_IDX ].s_bRenderPopup == true )
					{
						// 추가 버프창 배경, 덱버프 발동 중인지 체크
						if( bUse )
						{
							int nOptionCnt = 0;
							for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
							{
								std::pair<int,USHORT> sdfCon;
								sdfCon.first = i;
								GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_CONDITION, &sdfCon );
								USHORT nCondition = sdfCon.second;

								if( nCondition != 0 )
									nOptionCnt++;
							}

							m_sWin[ BUFF_IDX ].s_pPopup->ReSize( CsPoint( (m_pBuffData->GetBuff()->size() + nOptionCnt - m_nMAX_Col) * m_nDeltaX, 33 ) + CsPoint( 5, 0 ) );
						}
						else
							m_sWin[ BUFF_IDX ].s_pPopup->ReSize( CsPoint( (m_pBuffData->GetBuff()->size() - m_nMAX_Col) * m_nDeltaX, 33 ) + CsPoint( 5, 0 ) );

						int nEndPosX = m_nMAX_Col * 32; // 보여지고 있는 (추가 버프창에 속하지 않은) 버프아이콘의 끝

						m_sWin[ BUFF_IDX ].s_pPopup->Render( GetRootClient() + CsPoint( nEndPosX, -4 ) );


						//남은 버프 데이터
						for( ; it != itEnd; it++, nIdx++ )
						{
							UINT nRemain = it->s_nLifeTime;
							UINT nSec = nRemain;
							UINT nMin = nRemain / 60;
							UINT nHour = nMin / 60;
							UINT nDay = nHour / 24;

							m_pBGImage->Render( GetRootClient() + CsPoint( nIdx * m_nDeltaX + /*167*/nEndPosX + 5, 0 ), 0 );					

							int nStack = 0;
#ifdef MONSTER_SKILL_GROWTH
							nStack = it->m_nStack;	//	중첩 표현
#endif

							ICONITEM::eTYPE type = ICONITEM::SKILL2;
							USHORT	usBuffIcon = it->s_pBuffTable->GetInfo()->s_nBuffIcon;

							if( usBuffIcon >= 4000 )		type = ICONITEM::SKILL4;
							else if( usBuffIcon >= 3000 )	type = ICONITEM::SKILL3;
							else if( usBuffIcon >= 2000 )	type = ICONITEM::SKILL2;
							else							type = ICONITEM::SKILL1;

							g_pIconMng->RenderIcon( type, GetRootClient() + CsPoint( /*169*/nEndPosX + 7 + (nIdx * m_nDeltaX), 2 ), ptSize, it->s_pBuffTable->GetInfo()->s_nBuffIcon % 1000, nStack );

							g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( /*169*/nEndPosX + 7 + (nIdx * m_nDeltaX), 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );
						}
						//덱버프 발동 중일 때 덱버프아이콘
						RenderEncyBuff( nIdx, ptSize, CsPoint( /*169*/nEndPosX + 5, 0 ) );
					}
				}
			}
#else	//BUFFINFO_FIX
			if( (int)m_pBuffData->GetBuff()->size() > m_nMAX_Col )
			{
				nIdx = 0;
				m_sWin[ BUFF_IDX ].s_pCurButton->Render();

				if( m_sWin[ BUFF_IDX ].s_bRenderPopup == true )
				{
					m_sWin[ BUFF_IDX ].s_pPopup->ReSize( CsPoint( (m_pBuffData->GetBuff()->size() - m_nMAX_Col) * 32, 34 ) );
					m_sWin[ BUFF_IDX ].s_pPopup->Render( GetRootClient() + CsPoint( 162, -4 ) );

					for( ; it != itEnd; it++, nIdx++ )
					{
						UINT nRemain = it->s_nLifeTime;
						UINT nSec = nRemain;
						UINT nMin = nRemain / 60;
						UINT nHour = nMin / 60;
						UINT nDay = nHour / 24;

						m_pBGImage->Render( GetRootClient() + CsPoint( nIdx * m_nDeltaX + 167, 0 ), 0 );					

						int nStack = 0;
#ifdef MONSTER_SKILL_GROWTH
						nStack = it->m_nStack;	//	중첩 표현
#endif
						g_pIconMng->RenderIcon( ICONITEM::SKILL2, GetRootClient() + CsPoint( 169 + (nIdx * m_nDeltaX), 2 ), ptSize, it->s_pBuffTable->GetInfo()->s_nBuffIcon % 1000, nStack );

						g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 169 + (nIdx * m_nDeltaX), 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );
					}
				}
			}
#endif	//BUFFINFO_FIX
			nIdx = 0;
			it = m_pBuffData->GetDeBuff()->begin();
			itEnd = m_pBuffData->GetDeBuff()->end();

			for( ; it != itEnd; it++, nIdx++ )
			{
				if( nIdx == m_nMAX_Col )
					break;

				UINT nRemain = it->s_nLifeTime;
				UINT nSec = nRemain;
				UINT nMin = nRemain / 60;
				UINT nHour = nMin / 60;
				UINT nDay = nHour / 24;

				if( bTextRender == true && nHour == 0 ) 
				{
					if( nMin > 0 )
					{
						std::wstring wsMin;
						DmCS::StringFn::Format( wsMin, L"%d %s", nMin, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
						m_pTextTime->SetText( wsMin.c_str() );
						//m_pTextTime->Render( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), m_nDeltaY + 24 ) );
					}
					else
					{
						std::wstring wsSec;
						DmCS::StringFn::Format( wsSec, L"%d %s", nSec, UISTRING_TEXT( "COMMON_TXT_SHORT_SEC" ).c_str() );
						m_pTextTime->SetText( wsSec.c_str() );
						//m_pTextTime->Render( GetRootClient() + CsPoint( 11 + (nIdx * m_nDeltaX), m_nDeltaY + 24 ), DT_CENTER );
					}
					m_pTextTime->Render( GetRootClient() + CsPoint( 11 + (nIdx * m_nDeltaX), m_nDeltaY + 24 ) );
				}

				m_pBGImage->Render( GetRootClient() + CsPoint( nIdx * m_nDeltaX, m_nDeltaY ), 1 );

				g_pIconMng->RenderIcon( ICONITEM::SKILL2, GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), m_nDeltaY + 2 ), ptSize, it->s_pBuffTable->GetInfo()->s_nBuffIcon % 1000 );		
				//g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), m_nDeltaY + 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );

#ifdef TARGETUI_TIMEMASK_DELETE
				if( m_nType != IF_BUFF_TARGET )
#endif
				{

					if( it->s_nTimeIverse < 60.0f )  //60초 미만 버프 마스크 랜더 시 이상 관련
						g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), m_nDeltaY + 2 ), (float)(ptSize.y / 2), (float)nSec, (float)it->s_nTimeIverse );

					else
						g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nIdx * m_nDeltaX), m_nDeltaY + 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );
				}
			}

			if( (int)m_pBuffData->GetDeBuff()->size() > m_nMAX_Col )
			{
				m_sWin[ DEBUFF_IDX ].s_pCurButton->Render();			

				if( m_sWin[ DEBUFF_IDX ].s_bRenderPopup == true )
				{
					m_sWin[ DEBUFF_IDX ].s_pPopup->ReSize( CsPoint( (m_pBuffData->GetBuff()->size() - m_nMAX_Col) * 32, 34 ) );
					m_sWin[ DEBUFF_IDX ].s_pPopup->Render( GetRootClient() + CsPoint( 162, m_nDeltaY - 4 ) );

					for( ; it != itEnd; it++, nIdx++ )
					{
						UINT nRemain = it->s_nLifeTime;
						UINT nSec = nRemain;
						UINT nMin = nRemain / 60;
						UINT nHour = nMin / 60;
						UINT nDay = nHour / 24;

						m_pBGImage->Render( GetRootClient() + CsPoint( nIdx * m_nDeltaX + 167, m_nDeltaY ), 0 );					

						g_pIconMng->RenderIcon( ICONITEM::SKILL2, GetRootClient() + CsPoint( 169 + (nIdx * m_nDeltaX), m_nDeltaY + 2 ), ptSize, it->s_pBuffTable->GetInfo()->s_nBuffIcon % 1000 );		
						g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 169 + (nIdx * m_nDeltaX), m_nDeltaY + 2 ), (float)(ptSize.y / 2), (float)nSec, 60.0f );
					}
				}
			}

#ifndef BUFFINFO_FIX
			//덱 적용 효과 버프
			int nUsingDeck = 0;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &nUsingDeck );

			if( nUsingDeck != INT_MIN && m_nType == IF_BUFF_MY_T )	// 덱 사용 중 && 테이머 버프 타입일 경우
			{
				int nCurPos = (int)m_pBuffData->GetBuff()->size();

				//적용중인 덱 옵션
				for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
				{
					std::pair<int,USHORT> sdfOpt;
					sdfOpt.first = i;
					GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );
					USHORT nOption = sdfOpt.second;

					if( nOption == 0 )
						continue;

					int nIcon = 0;

					CsEncy_OptData* pData = nsCsFileTable::g_pDigimonMng->GetOptData(nOption);

					if( pData == NULL )
						continue;

					nIcon = pData->GetInfo()->s_nIcon;


					if( nCurPos < m_nMAX_Col )
					{
						m_pBGImage->Render( GetRootClient() + CsPoint( nCurPos * m_nDeltaX, 0 ), 3 );		

						g_pIconMng->RenderIcon( ICONITEM::CARD, 
							GetRootClient() + CsPoint( 2 + (nCurPos * m_nDeltaX), 2 ), 
							ptSize, nIcon % 1000 );		
					}
					else
					{
						m_pBGImage->Render( GetRootClient() + CsPoint( (nCurPos-m_nMAX_Col) * m_nDeltaX, m_nDeltaY ), 3 );		

						g_pIconMng->RenderIcon( ICONITEM::CARD, 
							GetRootClient() + CsPoint( 2 + ( (nCurPos-m_nMAX_Col) * m_nDeltaX), m_nDeltaY + 2 ), 
							ptSize, nIcon % 1000 );		
					}


					//옵션 중 발동형 버프
					cBuffData::LIST_ENCY_BUFF_IT itEncy = m_pBuffData->_GetEncyBuff()->begin();
					cBuffData::LIST_ENCY_BUFF_IT itEncyEnd = m_pBuffData->_GetEncyBuff()->end();
					for( ; itEncy != itEncyEnd; ++itEncy )
					{
						if( itEncy->s_nOptIdx != i )
							continue;
						if( itEncy->s_nEndTime < _TIME_TS )
							continue;

						std::pair<int,u4> sdfTime;
						sdfTime.first = itEncy->s_nOptIdx;
						GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_TIME, &sdfTime );

						u4 nRemainTime = itEncy->s_nEndTime - _TIME_TS;
						u4 nTime = sdfTime.second;

						if( nTime == 0 )
							return;
						if( nCurPos < m_nMAX_Col )
						{
							g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nCurPos * m_nDeltaX), 2 ), 
								(float)(ptSize.y / 2), (float)nRemainTime, (float)nTime );
							break;
						}
						else
						{
							g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + ( (nCurPos-m_nMAX_Col) * m_nDeltaX), m_nDeltaY + 2 ), 
								(float)(ptSize.y / 2), (float)nRemainTime, (float)nTime );
							break;
						}
					}

					nCurPos++;
				}
			}
#endif	// BUFFINFO_FIX
		}
		break;
	}	
}

void cBuffInfo::ResetDevice()
{
	switch( m_nType )
	{
	case IF_BUFF_MY_T:
	case IF_BUFF_MY_D:
		SetPosSize( m_ptRootClient, CsPoint( g_nScreenWidth - 500, 70) );
		break;
	case IF_BUFF_TARGET:
#ifdef MONSTER_SKILL_GROWTH
		SetPosSize( CsPoint( 394, 88 ), CsPoint( g_nScreenWidth - 650, 70) );
#else
		SetPosSize( CsPoint( 631, 35 ), CsPoint( g_nScreenWidth - 650, 70) );
#endif	
		break;
	}
	ResetDeviceScript();
	
	if(m_pBGImage)
		m_pBGImage->ResetDevice();
	if(m_pTextTime)
		m_pTextTime->ResetDevice();

	if(m_pPlusButton)
		m_pPlusButton->ResetDevice();
	if(m_pMinusButton)
		m_pMinusButton->ResetDevice();

	for( int i = 0 ; i < m_nStructCount; i++ )
	{
		if(m_sWin[ i ].s_pPopup)
			m_sWin[ i ].s_pPopup->ResetDevice();		
	}	
#ifndef BUFFINFO_FIX
	m_nMAX_Col = g_nScreenWidth / 210;
#endif
#ifdef GUILD_RENEWAL
	if( m_nType == IF_BUFF_SYSTEM )
	{
		if(m_pGuildBuffIcon)
			m_pGuildBuffIcon->ResetDevice();
	}
#endif
}


#ifdef BUFFINFO_FIX

void cBuffInfo::RenderEncyBuff( u2 nIdx, CsPoint ptIconSize, CsPoint ptDeltaPos /*= CsPoint::ZERO */ )
{
	//덱 적용 효과 버프
	int nUsingDeck = 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &nUsingDeck );

#ifdef PARTY_BUFF_SHOW	// || m_nType == IF_BUFF_PARTY1_T || m_nType == IF_BUFF_PARTY2_T || m_nType == IF_BUFF_PARTY3_T) )
	if( nUsingDeck != INT_MIN && ( m_nType == IF_BUFF_MY_T || GetBuffUser() == RTTI_TAMER_USER ) )		
#else
	if( nUsingDeck != INT_MIN && m_nType == IF_BUFF_MY_T)	// 덱 사용 중 && 테이머 버프 타입일 경우
#endif
	{
		int nCurPos = nIdx;

		//적용중인 덱 옵션

		GS2C_RECV_ENCYCLOPEDIA_GETDECKIDX recv;
		recv.iDeckIdx = nUsingDeck;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_USE_GETDECK, &recv );

		for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
		{
			std::pair<int,USHORT> sdfOpt;
			sdfOpt.first = i;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );

			USHORT nOption = sdfOpt.second;

			if( nOption == 0 )
				continue;

			int nIcon = 0;

			CsEncy_OptData* pData = nsCsFileTable::g_pDigimonMng->GetOptData(nOption);

			if( pData == NULL )
				continue;

			nIcon = pData->GetInfo()->s_nIcon;

			m_pBGImage->Render( GetRootClient() + CsPoint( nCurPos * m_nDeltaX, 0 ) + ptDeltaPos, 3 );		

			g_pIconMng->RenderIcon( ICONITEM::CARD, 
				GetRootClient() + CsPoint( 2 + (nCurPos * m_nDeltaX), 2 ) + ptDeltaPos, 
				ptIconSize, nIcon % 1000 );		

			//옵션 중 발동형 버프
			cBuffData::LIST_ENCY_BUFF_IT itEncy = m_pBuffData->_GetEncyBuff()->begin();
			cBuffData::LIST_ENCY_BUFF_IT itEncyEnd = m_pBuffData->_GetEncyBuff()->end();
			for( ; itEncy != itEncyEnd; ++itEncy )
			{
				if( itEncy->s_nOptIdx != i )
					continue;
				if( itEncy->s_nEndTime < _TIME_TS )
					continue;

				std::pair<int,u4> sdfTime;
				sdfTime.first = itEncy->s_nOptIdx;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_TIME, &sdfTime );

				u4 nRemainTime = itEncy->s_nEndTime - _TIME_TS;
				u4 nTime = sdfTime.second;

				if( nTime == 0 )
					return;

				g_pIconMng->RenderTimeInverse( GetRootClient() + CsPoint( 2 + (nCurPos * m_nDeltaX), 2 ) + ptDeltaPos, 
					(float)(ptIconSize.y / 2), (float)nRemainTime, (float)nTime );
				break;

			}
			nCurPos++;
		}
	}
}

bool cBuffInfo::SetEncyTooltip( u2 nPosIdx, u2 nApplyIdx, CsPoint ptDeltaPos /*= CsPoint::ZERO */ )
{
	int nUsingDeck = 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &nUsingDeck );

	if( nUsingDeck != INT_MIN && m_nType == IF_BUFF_MY_T )	// 덱 사용 중 && 테이머 버프 타입일 경우
	{
		std::pair<int,USHORT> sdfOpt;
		sdfOpt.first = nApplyIdx;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );

		USHORT nOption = sdfOpt.second;

		if( nOption == 0 )
			return false;

		CsPoint ptPos = GetRootClient() + CsPoint( nPosIdx * m_nDeltaX, 0 ) + ptDeltaPos;

		CsRect rc( ptPos, CsSIZE( 24, 24 ) );
		CsPoint ptMousPos = CURSOR_ST.GetPos();

		if( rc.PtInRect( ptMousPos ) == TRUE )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptPos, CsPoint( 24, 24 ), 240, cTooltip::DECKBUFF, nApplyIdx, 0, 0, 0, NULL );

			return true;
		}
	}
	return false;
}
#endif	//BUFFINFO_FIX

void cBuffInfo::SetBuffUser(USHORT	ctype)
{
	m_Buff_Client_Type = ctype;
}
USHORT	cBuffInfo::GetBuffUser() const
{
	return m_Buff_Client_Type;
}

void cBuffInfo::SetAddSystempBuffPosY(int y)
{
	m_AddSystempBuffPosY = y;
}

void cBuffInfo::SetBuffData( cBuffData* pData )
{ 
	m_pBuffData = pData; 

	if( m_pBuffData )
		SetShowWindow( true );
	else
		SetShowWindow( false );
}

void cBuffInfo::ReleaseData()
{ 
	m_pBuffData = NULL; 
	SetShowWindow( false );
}

int cBuffInfo::GetDeltaPosY(void)
{
	int DeltaY = 0;
	DeltaY = - m_AddSystempBuffPosY;
#ifdef SDM_TAMER_XGUAGE_20180628
	if( g_pDataMng->GetTEquip()->IsExistItem(nTamer::eParts::XAI) == true)
		DeltaY -= 18;
#endif
	return DeltaY;
}