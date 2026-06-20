#include "stdafx.h"
#ifdef DIGIMON_INCUBATOR
#include "Incubator.h"

#pragma warning( disable : 4311 ) //포인터 잘림 경고 무시

#define IF_INCUBATOR_DELTA_Y		50

cDigimon_Incubator::cDigimon_Incubator()
{
	m_nSelectIdx = -1;
}
void cDigimon_Incubator::Destroy()
{
	//2017-01-11-nova 인큐베이터 창이 닫힐경우 팝업메시지도 같이 닫히도록 함
 	if( cMessageBox::IsMessageBox() )
 	{
 		cMessageBox::DelMsg( 30625, false );	//2017-01-23-nova
		cMessageBox::DelMsg( 30626, false );
		cMessageBox::DelMsg( 30629, false );	//2017-01-23-nova
 	}
	cBaseWindow::Delete();
}

void cDigimon_Incubator::DeleteResource()
{
	DeleteScript();
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		SAFE_NIDELETE( m_pBoostBtn[ i ] );
		SAFE_NIDELETE( m_pLockImg[ i ] );
		SAFE_NIDELETE( m_pLine[ i ] );
		//디보 정보 확인해서 촉진안한 디지몬 막혀있으면 풀어줘
		if( g_pDataMng->GetTacticsHouse()->_GetIncuSlot( i ) != 0 && m_pItem_Serv[ i ].GetType() == 0 )
		{
			g_pDataMng->GetTacticsHouse()->_SetIncuSlot( i, 0 );
			//아이템 락도 해제
			g_pDataMng->ItemUnlock( m_pItemData[ i ].s_nInvenSlot );
		}
	}
	SAFE_NIDELETE( m_pExplain );
	SAFE_NIDELETE( m_pScrollBar );
}

void cDigimon_Incubator::Create( int nValue /*= 0 */ )
{
	cBaseWindow::Init();
	CsPoint ptTacticsHouse = g_pGameIF->GetTacticsHouse()->GetRootClient();

	InitScript( "D_Incubator\\Incubator_BG.tga", ptTacticsHouse + CsPoint( 434, 0 ), CsPoint( 270, 300 ), true);
//	InitScript( "D_Incubator\\Incubator_BG.tga", CsPoint( 534, 200 ), CsPoint( 270, 300 ), true);
	AddTitle( _LAN("디지몬 인큐베이터") );

	//닫기버튼
	m_pCancelButton = AddButton( CsPoint( 247, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	CsPoint ptPos = CsPoint( 54, 49 );
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		int nDeltaY = i * IF_INCUBATOR_DELTA_Y;
		//디지몬, 아이템 아이콘
		m_pDigimon_IFIcon[ i ].Init( CsRect( ptPos + CsPoint( 0, nDeltaY ), CsSIZE( 32, 32 ) ) );
		m_pItem_IFIcon[ i ].Init( CsRect( ptPos + CsPoint( 86, nDeltaY ), CsSIZE( 32, 32 ) ) );

		//촉진 버튼
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_9);
		
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( _LAN( "성장 촉진" ) );
		m_pBoostBtn[ i ] = NiNew cButton;
		m_pBoostBtn[ i ]->Init( m_pRoot, ptPos + CsPoint( 128, 3 + nDeltaY ), CsPoint( 75, 26 ), cButton::IMAGE_NOR_1, &ti ,false );
// 		m_pBoostBtn[ i ]->Init( m_pRoot, ptPos + CsPoint( 128, 3 + nDeltaY ), CsPoint( 75, 26 ), cButton::IMAGE_NOR_1, _LAN( "성장 촉진" ) ,false );

		//자물쇠
		m_pLockImg[ i ] = NiNew cImage;
		m_pLockImg[ i ]->Init( m_pRoot, ptPos + CsPoint( -42, 1 + nDeltaY ), CsPoint( 30, 30 ), "D_Incubator\\Incubator_Lock.tga", false );
		m_pLockImg[ i ]->SetTexToken( CsPoint( 0, 30 ) );
		m_pLockImg[ i ]->SetState( cImage::NORMAL1 );	// 0 : 잠김, 1: 열림

		//디지몬 초상화
		m_pDigimonImg[ i ].Init();

		//연결선
		m_pLine[ i ] = NiNew cSprite;
		m_pLine[ i ]->Init( m_pRoot, ptPos + CsPoint( 36, 12 + ( i * IF_INCUBATOR_DELTA_Y ) ), CsPoint( 49, 10 ), "D_Incubator\\Incubator_SelectLine.tga", false );

		//등록 아이템 정보
		m_pItemData[ i ].s_nInvenSlot = -2;
	}

	//스크롤바
	m_pScrollBar = NiNew cScrollBar;
	m_pScrollBar->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( 238, 213 ), CsPoint( 16, 71 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 0, 200 ), CsSIZE( 270, 104 ) ), 4, false );		

	//설명 스트링
	m_pExplain = NiNew cStringList;

// 	TCHAR szExplain[ IF_INCUBATOR_EXPLAIN_LEN ];
// 	cText::sTEXTINFO ti;
// 	ti.Init();
// 	ti.s_eFontSize = CFont::FS_10;
// 
// 	_stprintf_s( szExplain, IF_INCUBATOR_EXPLAIN_LEN, _LAN("성장 촉진을 위해서는 성장 촉진제가 필요합니다.\n") );
// 	_tcscat_s( szExplain, IF_INCUBATOR_EXPLAIN_LEN, _LAN("퀘스트, 루비 아이템의 경험치는 적용되지 않습니다.\n") );
// 	_tcscat_s( szExplain, IF_INCUBATOR_EXPLAIN_LEN, _LAN("사냥 1회당 최대로 획득할 수 있는 경험치는 30,000 입니다.") ); //Redmine #334 디지몬 보관소 인큐베이터 스트링 수정
// //	_tcscat_s( szExplain, IF_INCUBATOR_EXPLAIN_LEN, _LAN("1회 사냥 획득 경험치는 5000을 넘을 수 없습니다.") );

	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eFontSize = CFont::FS_10;
	std::wstring msg = UISTRING_TEXT("DIGIMON_INCUBATOR_MESSAGE");
	g_pStringAnalysis->Cut(m_pExplain, 210, msg.c_str(), &ti );

	m_pExplain->SetBuddy( m_pScrollBar );
	m_pScrollBar->SetRange( CsPoint( 0, m_pExplain->GetSize() ) );

}

void cDigimon_Incubator::Update(float const& fDeltaTime)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) == false )
	{
		Close(); //디보 안열려있으면 꺼줘
		return;
	}

	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		if( m_pItem_Serv[ i ].GetType() == 0 && m_pItemData[ i ].s_pItemInfo.GetType() != 0 )//촉진
			m_pBoostBtn[ i ]->SetEnable( true );
		else if( m_pItem_Serv[ i ].GetType() != 0 )//등록상태
			m_pBoostBtn[ i ]->SetEnable( true );
		else	//비어있는상태
			m_pBoostBtn[ i ]->SetEnable( false );
	}
	//버튼 텍스트 셋팅
	_SetBtnText();
	//윈도우 이동 업데이트
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE 
cDigimon_Incubator::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	//닫기버튼
	if( m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		if( CURSOR_ST.IsIcon() )
			CURSOR_ST.ReleaseIcon();
		Close();
		return muReturn;
	}

	//촉진버튼
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		if( m_pBoostBtn[ i ]->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			if( m_pItem_Serv[ i ].GetType() == 0 && m_pItemData[ i ].s_pItemInfo.GetType() != 0 )//촉진
			{
				int nIdx = g_pDataMng->GetTacticsHouse()->_GetIncuSlot( i ) - 1000;
				cData_PostLoad::sDATA* pDigimonInfo = g_pDataMng->GetTacticsHouse()->GetTactics( nIdx );

				if( pDigimonInfo->s_nLevel == Language::pCountry::DigimonLevelMax )
				{
					cPrintMsg::PrintMsg( 30627 ); // 최대 레벨을 달성한 디지몬은 성장 촉진을 할 수 없습니다.
					return muReturn;
				}
				else
				{
					m_nSelectIdx = i;		//선택한 인덱스 저장
					cPrintMsg::PrintMsg( 30625 );	// 디지몬 성장 촉진을 시작하시겠습니까?
					return muReturn;
				}
			}
			else //성장 촉진 멈추기
			{
				m_nSelectIdx = i;		//선택한 인덱스 저장
				cPrintMsg::PrintMsg( 30626 );	// 성장 촉진을 멈추시겠습니까?
				return muReturn;
			}
		}
	}

	//아이콘 업데이트
	if( _Update_ForMouse_Icon() == false )
	{	
		//아이콘에 안걸리면 마지막으로 윈도우 이동 체크
		if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( GetRoot()->GetSize().x, 30 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
			_UpdateMoveWindow_ForMouse();
	}

	// 스크롤바 업데이트
	m_pScrollBar->Update_ForMouse();

	return muReturn;
}

bool cDigimon_Incubator::_Update_ForMouse_Icon() //아이콘 업데이트
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		//디지몬 아이콘
		if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient() + m_pDigimon_IFIcon[ i ].GetPos() , CsSIZE( 32, 32) ) ) != CURSOR::BUTTON_OUTWINDOW 
			&& m_pDigimonImg[ i ].s_bData )
		{
			m_nSelectIdx = i;		//선택한 인덱스 저장
			switch( CURSOR_ST.GetButtonState() )
			{
			case CURSOR::LBUTTON_DOWN://좌클릭
				{
					//디지몬 아이콘
					if( m_pDigimon_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) )
					{
// 						if( 	m_pItemData[ i ].s_nInvenSlot == -1)		//이미 촉진 중인 놈이면 패스
// 							return true;
						//촉진 중인 디지몬도 해제 가능하게 수정
						if( CURSOR_ST.IsIcon() )
							return true;

						// 디지몬 아이콘 클릭
						if( ( m_pDigimon_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK ) && m_pDigimon_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) )
						{
							CURSOR_ST.SetIcon( CURSOR_ICON::CI_INCUBATOR, 0, i, &m_pDigimon_IFIcon[ i ] );
							CURSOR_ST.SetIconSubInfo( (int)m_pDigimonImg[ i ].s_pDigimonImage , 0, 0 );
							return true;
						}
					}
				}
				break;
			case CURSOR::RBUTTON_UP://우클릭
				{
					if( m_pItemData[ i ].s_nInvenSlot != -1 ) // 서버 등록 전일때만
					{
						//디지몬 해제
						_RecvExtraction(i);
					}
					else //서버 등록한 놈인데 뺄지 물어보자
					{
						m_nIconIdx = -1;
 						cPrintMsg::PrintMsg( 30629 );	// 성장 촉진 중인 디지몬을 인큐베이터에서 꺼내겠습니까
					}
					return true;
				}
			default: //툴팁
				{
					int nIdx = g_pDataMng->GetTacticsHouse()->_GetIncuSlot( i ) - 1000;
					cData_PostLoad::sDATA* pData = g_pDataMng->GetTacticsHouse()->GetTactics( nIdx );
					TCHAR szName[ 64 ];
					TCHAR szLevel[ 32 ];
					_stprintf_s( szName, 64, _LAN( "디지몬이름 : %s \n" ), pData->s_szName );
					_stprintf_s( szLevel, 32, _LAN( "디지몬레벨 : %d" ), (int)pData->s_nLevel );
					_tcscat_s( szName, 64, szLevel );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, szName );
				}
				break;
			}
			m_pDigimon_IFIcon[ i ].ReleaseState();
		}

		//아이템 아이콘
		if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient() + m_pItem_IFIcon[ i ].GetPos() , CsSIZE( 32, 32) ) ) != CURSOR::BUTTON_OUTWINDOW 
			&& m_pItemData[ i ].s_pItemInfo.GetType() != 0 )
		{
			switch( CURSOR_ST.GetButtonState() )
			{
			case CURSOR::LBUTTON_DOWN://좌클릭
				{
					//아이템 아이콘
					if( m_pItem_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) )
					{
						if( m_pItemData[ i ].s_nInvenSlot == -1 )		//이미 촉진 중인 놈이면 패스
							continue;

						if( CURSOR_ST.IsIcon() )
							return true;

						// 아이템 아이콘 클릭
						else if( ( m_pItem_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK ) && m_pItem_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) )
						{
							CURSOR_ST.SetIcon( CURSOR_ICON::CI_INCUBATOR, 1, i, &m_pItem_IFIcon[ i ] );
							CURSOR_ST.SetIconSubInfo( (int)m_pItemData[ i ].s_pItemInfo.GetType() , 1, 0 );
							break;
						}
					}
					else
						m_pItem_IFIcon[ i ].ReleaseState();
				}
				break;
			case CURSOR::RBUTTON_UP://우클릭
				{
					if( m_pItemData[ i ].s_nInvenSlot != -1 ) // 서버 등록 전일때만
					{
						// 인큐베이터에서 아이템 제거
						g_pDataMng->ItemUnlock( m_pItemData[ i ].s_nInvenSlot ); //인벤 락 해제

						m_pItemData[ i ].s_nInvenSlot = -2;	//비어있는 상태로 
						m_pItemData[ i ].s_pItemInfo.m_nType = 0;	
					}
					else
					{
					}
					return true;
				}
			default: //툴팁
				{
					if( m_pItemData[ i ].s_nInvenSlot >= 0 )	// 촉진 전 등록만 되있는 아이템
						TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos() + CsPoint( 20, 20 ) , CsPoint::ZERO, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_pItemData[ i ].s_pItemInfo.GetType(), cBaseWindow::WT_INVENTORY, 0, 0, &m_pItemData[ i ].s_pItemInfo );
					else if( m_pItemData[ i ].s_nInvenSlot == -1 )// 촉진 중인 아이템
					{
						CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( m_pItemData[ i ].s_pItemInfo.GetType() )->GetInfo();

						TCHAR szExpApply[ 32 ];
						float fVal = (float)pFTItemInfo->s_nTypeValue * 0.01f;//만분율이라 0.01 곱함

						_stprintf_s( szExpApply, 32, _LAN( "적용 경험치 : %.2f%%" ), fVal );
// 						TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, szExpApply );
					}
				}
				break;
			}
			m_pItem_IFIcon[ i ].ReleaseState();
		}
	}

	return false;
}

int cDigimon_Incubator::_GetClickIdx()
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		//디지몬 아이콘
		if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient() + m_pDigimon_IFIcon[ i ].GetPos() , CsSIZE( 32, 32) ) ) != CURSOR::BUTTON_OUTWINDOW )
			return i;
	}
	return -1;//디지몬 아이콘 위치 아님
}

bool cDigimon_Incubator::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	m_nIconIdx = nIconCount;	// 커서 인덱스 저장
	//아이콘 드래그 다운
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INCUBATOR:	//인큐 -> 인큐
		{
			m_nSelectIdx = _GetClickIdx();
		}
		break;
	case CURSOR_ICON::CI_INVEN:	// 인벤 -> 인큐
		{
			//	nIconSlot = 인벤슬롯
			cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nIconSlot );
			CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();

			if( pFTItemInfo->s_nType_L == nItem::Incubater )	// 촉진제 타입일때만
				_SetResist( nIconSlot );	// 인큐베이터 등록
			else
				cPrintMsg::PrintMsg( 11014 );	// 해당 슬롯에 등록할 수 없습니다	

			CURSOR_ST.ReleaseIcon();
			return true;
		}
	case CURSOR_ICON::CI_TACTICSHOUSE:	//디보 -> 인큐
		{
			if( nIconSlot == 0 ) // 소지 용병 디지몬
			{
				cPrintMsg::PrintMsg( 11014 );	// 해당 슬롯에 등록할 수 없습니다	
			}
			else if( nIconSlot == 1 )
			{
				// 파트너디지몬(시작 디지몬) 일 경우 안됨
// 				int nDigimonID = g_pDataMng->GetTacticsHouse()->GetTactics( nIconCount )->s_dwBaseDigimonID;
// 				if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( nDigimonID ) == true )
// 				{
// 					cPrintMsg::PrintMsg( 30162 );
// 					return true;
// 				}

				//현재 마우스 위치로 인큐베이터 인덱스 찾음
				int nIncuIdx_onMouse = _GetClickIdx();
				if( nIncuIdx_onMouse == -1 )
				{
					//인큐베이터 슬롯 위치 아님
				}
				else if( m_pItemData[ nIncuIdx_onMouse ].s_nInvenSlot == -1 )	// 촉진 중인 슬롯이라면
				{
					m_nSelectIdx = nIncuIdx_onMouse;		//선택한 인덱스 저장
					if( m_pDigimonImg[ nIncuIdx_onMouse ].s_bData == false )	//	디지몬은 비어있는 상태
						cPrintMsg::PrintMsg( 30630 );	// 해당 디지몬 보관소의 디지몬을 촉진 중인 인큐베이터에 등록 하시겠습니까
					else
						cPrintMsg::PrintMsg( 30628 );	// 성장 촉진 중인 디지몬을 해당 디지몬 보관소의 디지몬과 교체하시겠습니까
				}
				else //비어있는 슬롯이라면
				{
					m_pDigimonImg[ nIncuIdx_onMouse ].s_bData = true;	//데이터 셋팅
					m_pDigimonImg[ nIncuIdx_onMouse ].s_pDigimonImage = g_pGameIF->GetTacticsHouse()->_GetHouseInfo( nIconCount ).s_pCharImage; // nIconCount =디보 슬롯
					g_pDataMng->GetTacticsHouse()->_SetIncuSlot( nIncuIdx_onMouse, nIconCount+1000 );	//인큐베이터 등록 상태 저장
				}

			}
			CURSOR_ST.ReleaseIcon();

			return true;
		}
	}

	return false;
}

void cDigimon_Incubator::ResetDevice()
{
	ResetDeviceScript();
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		m_pBoostBtn[ i ]->ResetDevice();
		m_pLockImg[ i ]->ResetDevice();
		m_pLine[ i ]->ResetDevice();
	}
	m_pExplain->ResetDevice();
	m_pScrollBar->ResetDevice();
}

void cDigimon_Incubator::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	ResetDevice();
}

void cDigimon_Incubator::Render()
{
	RenderScript();
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		// 아이템 아이콘
		if( m_pItemData[ i ].s_pItemInfo.GetType() != 0 ) // 등록되어 있을 때만
		{
			g_pIconMng->RenderItem( &m_pItemData[ i ].s_pItemInfo, m_pItem_IFIcon[ i ].GetPos() + GetRootClient(), false );
			m_pLine[ i ]->Render();		// 연결선
		}
		// 디지몬 아이콘
		if( m_pDigimonImg[ i ].s_bData )	// 등록되어 있을 때만
		{
			if( m_pDigimonImg[ i ].s_pDigimonImage )
				m_pDigimonImg[ i ].s_pDigimonImage->Render_S( m_pDigimon_IFIcon[ i ].GetPos() + GetRootClient(), CsPoint( 32, 32 ) );
		}

		m_pBoostBtn[ i ]->Render();	// 촉진버튼
		m_pLockImg[ i ]->Render();	// 잠금이미지
	}
	m_pExplain->Render( GetRootClient() + CsPoint( 20, 215 ), 4 );
	m_pScrollBar->Render();
}

bool cDigimon_Incubator::OnEscapeKey()
{
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		if( m_pDigimonImg[ i ].s_pDigimonImage != m_pDigimonImg[ i ].s_pImage_Serv )
		{
			//촉진 안하고 인큐에 넣기만 한 상태에서 종료될 때 - 디지몬
			g_pDataMng->GetTacticsHouse()->_SetIncuSlot( i, 0 );

			//촉진 안하고 인큐에 넣기만 한 상태에서 종료될 때 - 아이템
			if( m_pItemData[ i ].s_pItemInfo.GetType() != m_pItem_Serv[ i ].GetType() )
				g_pDataMng->ItemUnlock( m_pItemData[ i ].s_nInvenSlot );
		}
	}

	if( CURSOR_ST.IsIcon() )
		CURSOR_ST.ReleaseIcon();
	Close();
	return true;
}

void cDigimon_Incubator::_SetBtnText()
{
	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		if( m_pItem_Serv[ i ].GetType() != 0 )//등록상태
		{
			TCHAR cTime[ 32 ];

			int nRemainTime = (int)( m_pItem_Serv[ i ].m_nEndTime - _TIME_TS );
			if( nRemainTime < 1 )//시간 다되면
			{
				_RecvExtraction( i );
				_stprintf_s( cTime, 32, _LAN( "%d 초" ), 0 );

				m_pBoostBtn[ i ]->SetText( cTime );
				continue;
			}
			
			int nSec = nRemainTime;
			int nMin = nRemainTime / 60;
			int nHour = nMin / 60;
			int nDay = nHour / 24;
			
			if( nDay > 0 )
				_stprintf_s( cTime, 32, _LAN( "%d 일 %d 시간" ), nDay, nHour%24 );
			else if( nHour > 0 )
				_stprintf_s( cTime, 32, _LAN( " %d 시간 %d 분 " ), nHour%24, nMin%60 );
			else if( nMin > 0 )
				_stprintf_s( cTime, 32, _LAN( "%d 분" ), nMin%60 );
			else if( nSec > 0 )
				_stprintf_s( cTime, 32, _LAN( "%d 초" ), nSec%60 );
			m_pBoostBtn[ i ]->SetText( cTime );
		}
		else	//촉진 or 비어있는상태
			m_pBoostBtn[ i ]->SetText( _LAN( "성장 촉진") );
	}
}

void cDigimon_Incubator::_SetResist( int nInvenIndex )
{
	// - 아이템 등록
	//등록한 아이템 인벤에서 락
	g_pDataMng->ItemLock( nInvenIndex );

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nInvenIndex );

	for( int i = 0 ; i < IF_INCUBATOR_MAX_DIGIMON_COUNT ; i++ )
	{
		if( m_pDigimonImg[ i ].s_bData && m_pItemData[ i ].s_pItemInfo.GetType() == 0 )//디지몬 등록되고 아이템 등록 안된놈 있으면 등록
		{
			m_pItemData[ i ].s_pItemInfo.m_nType = pInfo->GetType();
			m_pItemData[ i ].s_nInvenSlot = nInvenIndex;
			return;
		}
		if( i+1 == IF_INCUBATOR_MAX_DIGIMON_COUNT )	// 빈 슬롯 못찾으면 락걸었던거 풀어줘
			g_pDataMng->ItemUnlock( nInvenIndex );
	}
}

void cDigimon_Incubator::_RecvFacilitation( u1 nSlot, u4 nRemainTime ) //촉진
{
	// 디지몬 등록상태 셋팅
	m_pDigimonImg[ nSlot ].s_bData = true;
	m_pDigimonImg[ nSlot ].s_pImage_Serv = m_pDigimonImg[ nSlot ].s_pDigimonImage;

	// 자물쇠 이미지 잠김 상태로 셋팅
	m_pLockImg[ nSlot ]->SetState( cImage::NORMAL0 );

	// 서버 등록 아이템 셋팅
	m_pItem_Serv[ nSlot ].m_nType = g_pGameIF->GetIncubator()->m_pItemData[ nSlot ].s_pItemInfo.GetType();
	m_pItem_Serv[ nSlot ].m_nEndTime = nRemainTime;

	//인벤 락해제
	g_pDataMng->ItemUnlock( m_pItemData[ nSlot ].s_nInvenSlot );
	//아이템 감소
	g_pDataMng->GetInven()->GetData( m_pItemData[ nSlot ].s_nInvenSlot )->DecreaseCount( 1 );
	//서버 등록상태로 바꿈
	m_pItemData[ nSlot ].s_nInvenSlot = -1;
	//데이터 수신중 메세지
	cMessageBox::DelMsg( 10019, false );
}

void cDigimon_Incubator::_RecvExtraction( u4 nSlot )	//꺼내기
{
	//지금 등록되어있는 상태라면 인벤 락 해제
	if( m_pItemData[ nSlot ].s_nInvenSlot >= 0 ) 
		g_pDataMng->ItemUnlock( m_pItemData[ nSlot ].s_nInvenSlot );

	// 아이템 비어있는 상태로
	m_pItemData[ nSlot ].s_nInvenSlot = -2;
	m_pItemData[ nSlot ].s_pItemInfo.m_nType = 0;

	// 서버 등록 아이템 없애줘
	m_pItem_Serv[ nSlot ].m_nType = 0;
	m_pItem_Serv[ nSlot ].m_nEndTime = 0;

	// 디지몬 등록 상태 비어있는 상태로
	m_pDigimonImg[ nSlot ].s_bData = false;
	m_pDigimonImg[ nSlot ].s_pImage_Serv = NULL;
	m_pDigimonImg[ nSlot ].s_pDigimonImage = NULL;

	//자물쇠 풀린상태로
	m_pLockImg[ nSlot ]->SetState( cImage::NORMAL1 );

	//디보 잠금 해제
	g_pDataMng->GetTacticsHouse()->_SetIncuSlot( (int)nSlot, 0 );

	//데이터 수신중 메세지
	cMessageBox::DelMsg( 10019, false );
}

void cDigimon_Incubator::_RecvDigimonChange( int nIncuSlot, int nHouseSlot )
{
	if( nHouseSlot == 0 )//디지몬만 뺀 경우
	{
		m_pDigimonImg[ nIncuSlot ].s_bData = false;
		m_pDigimonImg[ nIncuSlot ].s_pDigimonImage = NULL;
		if( g_pGameIF->GetIncubator()->m_pLockImg[ nIncuSlot ]->GetState() == cImage::NORMAL0 )//잠금이미지  잠긴 상태라면
			g_pGameIF->GetIncubator()->m_pLockImg[ nIncuSlot ]->SetState( cImage::NORMAL1 );//열어줌
	}
	else//디보의 디지몬이랑 교체한 경우
	{
		m_pDigimonImg[ nIncuSlot ].s_bData = true;
		m_pDigimonImg[ nIncuSlot ].s_pDigimonImage = g_pGameIF->GetTacticsHouse()->_GetHouseInfo( nHouseSlot - 1000 ).s_pCharImage; // nIconCount =디보 슬롯
		if( g_pGameIF->GetIncubator()->m_pLockImg[ nIncuSlot ]->GetState() == cImage::NORMAL1 )//잠금이미지  열린 상태라면
			g_pGameIF->GetIncubator()->m_pLockImg[ nIncuSlot ]->SetState( cImage::NORMAL0 );//잠궈줌
	}

	g_pDataMng->GetTacticsHouse()->_SetIncuSlot( nIncuSlot, nHouseSlot );	//인큐베이터 등록 상태 저장

	//데이터 수신중 메세지
	cMessageBox::DelMsg( 10019, false );
}

#pragma warning( default : 4311 )
#endif