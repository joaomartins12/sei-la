#include "stdafx.h"
#include "Encyclopedia.h"

cEncyclopedia::cEncyclopedia() : m_bIsMouseOn(false)
{
	m_pEncyListBox			= NULL;
	m_pCancelButton			= NULL;
	m_pStatisticsBtn		= NULL;
	m_pRadioBtn				= NULL;

	m_pTooltipBG			= NULL;		
	m_szName				= NULL;	
	m_szLevel				= NULL;	
	m_szScale				= NULL;	

	for( int i = 0 ; i < IF_ENCY_MAX_ENCHANT_COUNT ; i++ )
		m_pEnchant[ i ] = NULL;

	for( int i = 0 ; i < IF_ENCY_VIEW_ENCHANT_COUNT ; i++ )
		m_szEnchantVal[ i ] = NULL;
	
	m_szExplain = NULL;	

	m_nGroupCount = 0;
	m_nCurTab = -1;
	m_nTooltipDigimonID = 0;
	iScrollPos = 0;
}

cEncyclopedia::~cEncyclopedia()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

bool cEncyclopedia::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::ENCYCLOPEDIA_REFRESHLIST, this );
	GetSystem()->Register( SystemType::ENCYCLOPEDIA_USEDECK, this );

	return true;
}

void cEncyclopedia::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::ENCYCLOPEDIA_REFRESHLIST:	
		{
			if( IsShowWindow() )
				_RefreshList();
		}
		break;

	case SystemType::ENCYCLOPEDIA_USEDECK:	
		{
			iScrollPos = m_pScrollBar->GetCurPosIndex();
			_SetTabList(m_nCurTab);
			m_pScrollBar->SetCurPosIndex(iScrollPos);
		}
		break;
	}
}

void cEncyclopedia::Destroy()
{
	m_pCurTabList_map.clear();
	cBaseWindow::Delete();	
}

void cEncyclopedia::DeleteResource()
{
	//툴팁
	NISAFE_DELETE( m_pTooltipBG );
	NISAFE_DELETE( m_szName );
	NISAFE_DELETE( m_szLevel );
	NISAFE_DELETE( m_szScale );

	for( int i = 0 ; i < IF_ENCY_MAX_ENCHANT_COUNT ; i++ )
	{
		NISAFE_DELETE( m_pEnchant[ i ] );
	}

	for( int i = 0 ; i < IF_ENCY_VIEW_ENCHANT_COUNT ; i++ )
	{
		NISAFE_DELETE( m_szEnchantVal[ i ] );
	}
	NISAFE_DELETE( m_szExplain );

	DeleteScript();
}

void cEncyclopedia::Create( int nValue /*= 0 */ )
{
	cBaseWindow::Init();

	int nPosX = ( g_nScreenWidth/2 ) - ( 383/2 );
	int nPosY = ( g_nScreenHeight/2 ) - ( 492/2 );

	InitScript( "Encyclopedia\\Encyclopedia_BG.tga", CsPoint( nPosX, nPosY ) , CsPoint( 383, 492 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );

	m_pCancelButton = AddButton( CsPoint( 359, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	 
	m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cEncyclopedia::PressCancelButton);

	// 통계창 버튼
	m_pStatisticsBtn = AddButton( CsPoint( 305, 453 ), CsPoint( 45, 29 ), CsPoint( 0, 29 ), "Encyclopedia\\Encyclopedia_StatBtn.tga" );
	m_pStatisticsBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cEncyclopedia::PressStatisticsButton);
	m_pStatisticsBtn->SetTexToken( CsPoint( 0, 29 ) );	

	// 가나다 순 나누기 텝
	CsPoint ptRadioPos = CsPoint( 8, 42 );
	std::map<std::wstring,std::wstring> const& mapGroupStr = GetSystem()->GetGroupString();
	std::map<std::wstring,std::wstring>::const_iterator it = mapGroupStr.begin();

	for( int n = 0; it != mapGroupStr.end(); ++it, ++n )
	{		
		TCHAR* pString = const_cast<TCHAR*>( it->second.c_str() );
		if( n == 0 )
			m_pRadioBtn = AddRadioButton(ptRadioPos, CsPoint( 51, 31 ), cCheckBox::IMAGE_RADIO_ENCYCLOPEDIA, pString);
		else
			m_pRadioBtn->AddNode( ptRadioPos+CsPoint( 0, 33 +( 33 * (n - 1)) ), CsPoint( 51, 31 ), cCheckBox::IMAGE_RADIO_ENCYCLOPEDIA, pString );
	}

	m_nGroupCount = (int)mapGroupStr.size();

	//그룹 목록
	m_pRadioBtn->AddNode( ptRadioPos+CsPoint( 2, 411 ), CsPoint( 45, 29 ), cCheckBox::IMAGE_RADIO_ENCY_DECK, _T( " " ) );
	m_pRadioBtn->SetCheckIndex( 0 );

	m_pScrollBar = NiNew cScrollBar;	
	m_pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint::ZERO, CsPoint( 13, 410 ), CsPoint( 13, 13 ), CsRect( CsPoint::ZERO, CsPoint::ZERO ), 5, false );

	m_pEncyListBox = NiNew cListBox;
	m_pEncyListBox->Init( m_pRoot, CsPoint(63, 35), CsPoint( 311, 410 ), NULL, false );		
	m_pEncyListBox->SetScrollBar(m_pScrollBar);
	m_pEncyListBox->SetScrollBarRenderCount(2);
	AddChildControl( m_pEncyListBox );	

	/************************************************************************/
	/*							도감 UI - 툴팁								*/
	/************************************************************************/
	//배경
	m_pTooltipBG = NiNew cSprite;
	m_pTooltipBG->Init( NULL, CsPoint::ZERO, CsPoint( 228, 250 ), "Encyclopedia\\Encyclopedia_Tooltip.tga", false );
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem );
	ti.s_eTextAlign = DT_CENTER;
	ti.s_eFontSize = CFont::FS_10;

	//이름
	m_szName = NiNew cText;
	m_szName->Init( NULL,CsPoint::ZERO, &ti, false );
	//레벨
	m_szLevel = NiNew cText;
	m_szLevel->Init( NULL,CsPoint::ZERO, &ti, false );
	//스케일
	m_szScale = NiNew cText;
	m_szScale->Init( NULL,CsPoint::ZERO, &ti, false );

	// 강화수치 텍스트
	for( int i = 0 ; i < IF_ENCY_VIEW_ENCHANT_COUNT ; i++ )
	{
		m_szEnchantVal[ i ] = NiNew cText;
		m_szEnchantVal[ i ]->Init( NULL,CsPoint::ZERO, &ti, false );
	}

	//설명
	m_szExplain = NiNew cStringList;

	// 공격력
	m_pEnchant[ 0 ] = NiNew cSprite;
	m_pEnchant[ 0 ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 16 ), "Encyclopedia\\Encyclopedia_Tooltip_AT.tga", false );
	// 크리티컬
	m_pEnchant[ 1 ] = NiNew cSprite;
	m_pEnchant[ 1 ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 16 ), "Encyclopedia\\Encyclopedia_Tooltip_CT.tga", false );
	// 회피
	m_pEnchant[ 2 ] = NiNew cSprite;
	m_pEnchant[ 2 ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 16 ), "Encyclopedia\\Encyclopedia_Tooltip_EV.tga", false );
	// 블럭
	m_pEnchant[ 3 ] = NiNew cSprite;
	m_pEnchant[ 3 ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 16 ), "Encyclopedia\\Encyclopedia_Tooltip_BL.tga", false );
	// HP
	m_pEnchant[ 4 ] = NiNew cSprite;
	m_pEnchant[ 4 ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 16 ), "Encyclopedia\\Encyclopedia_Tooltip_HP.tga", false );

	// 툴팁 대기 시간
	m_DelayTime.SetEndTime( 0.4f );		

	_SetTabList( 0 );
}

void cEncyclopedia::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cEncyclopedia::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( muReturn == MUT_OUT_WINDOW )
	{
		m_bIsMouseOn = false;
		return muReturn;
	}

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 305, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
	{
		_UpdateMoveWindow_ForMouse();
	}
	
	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )		// 캔슬 버튼
	{
		return muReturn;
	}
	
	if(m_pStatisticsBtn && m_pStatisticsBtn->Update_ForMouse() == cButton::ACTION_CLICK )	//통계버튼
	{
		return muReturn;
	}

	if( m_pRadioBtn->Update_ForMouse() == cRadioButton::ACTION_CLICK )
	{
		cPrintMsg::PrintMsg( 10019 );
		_SetTabList( m_pRadioBtn->GetCheckIndex() );
		cMessageBox::DelMsg( 10019, false );
	}

	if( m_pEncyListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{	
		cListBoxItem const* pOverItem = m_pEncyListBox->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverItem, muReturn);
		SAFE_POINTER_RETVAL(g_pGameIF, muReturn);

		if(!pOverItem->GetScrollVisible())
		{
			return muReturn;
		}

		sDIGIMON_ID* pInfo = static_cast<sDIGIMON_ID*>(pOverItem->GetUserData());

		if( m_pRadioBtn->GetCheckIndex() < m_nGroupCount )// 그룹(7)이 아닐 때만
		{
			_SetTooltip( pInfo->iDigimonId );	
		}
		else
		{
			bIsTooltip = false;
			_Updata_ForMouse_Group( pInfo->iCount );

			if(bIsTooltip)
				_SetGroupTooltip( pInfo->iDigimonId );	
		}

		return muReturn;
	}

	return muReturn;
}

void cEncyclopedia::Render()
{
	RenderScript();

	if( m_bIsMouseOn )		//툴팁
	{
		CsPoint ptMousePos = CURSOR_ST.GetPos();

		if( m_pRadioBtn->GetCheckIndex() < m_nGroupCount )// 그룹(7)이 아닐 때만
			_RenderTooltip(ptMousePos);
	}

	EndRenderScript();
}

void cEncyclopedia::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cEncyclopedia::ResetDevice()
{
	ResetDeviceScript();
}

bool cEncyclopedia::OnEscapeKey()
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCYCLOPEDIA_STATS ) == false )		//통계창 켜져 있을 땐 통계창 먼저 끔
	{
		Close();
		return true;
	}
	return false;
}

void cEncyclopedia::_RenderTooltip( CsPoint ptMousePos )
{
	if( m_DelayTime.IsCoolTime() == true )
		return;

	//툴팁 배경
	m_pTooltipBG->Render( ptMousePos );

	//계열체 이름
	m_szName->Render( ptMousePos + CsPoint( 115, 11 ), DT_CENTER );

	//레벨,스케일
	m_szLevel->Render( ptMousePos + CsPoint( 86, 38 ), DT_CENTER );
	m_szScale->Render( ptMousePos + CsPoint( 86, 62 ), DT_CENTER );

	//강화종류
	int nFirst  = GetSystem()->GetEnchant_Order( 0, m_pEnchantVal[0], m_pEnchantVal[1], m_pEnchantVal[2], m_pEnchantVal[3], m_pEnchantVal[4] );
	int nSecond = GetSystem()->GetEnchant_Order( 1, m_pEnchantVal[0], m_pEnchantVal[1], m_pEnchantVal[2], m_pEnchantVal[3], m_pEnchantVal[4] );

	//강화 값이 0이면 안보여줌
	if( m_pEnchantVal[ nFirst ] != 0 )
		m_pEnchant[ nFirst ]->Render( ptMousePos + CsPoint( 136, 37 ) );

	if( m_pEnchantVal[ nSecond ] != 0 )
		m_pEnchant[ nSecond ]->Render( ptMousePos + CsPoint( 136, 61 ) );

	//강화 수치
	m_szEnchantVal[ 0 ]->Render( ptMousePos + CsPoint( 185, 38 ), DT_CENTER );//185, 38
	m_szEnchantVal[ 1 ]->Render( ptMousePos + CsPoint( 185, 62 ), DT_CENTER );//185, 62

	//계열체 설명
	m_szExplain->Render( ptMousePos + CsPoint( 10, 83 ), 2 );

	m_bIsMouseOn = false;
}

void cEncyclopedia::_SetTabList( int nCurRadioIdx )
{
	m_pCurTabList_map.clear();	

	if( nCurRadioIdx < m_nGroupCount )	// 그룹 탭이 아닐 경우
	{
		EncyclopediaContents::MAP_CIT it    = GetSystem()->GetEncyMapData().begin();
		EncyclopediaContents::MAP_CIT itEnd = GetSystem()->GetEncyMapData().end();

		for( ; it != itEnd ; it++ )
		{
			if( it->second->s_nGroup == nCurRadioIdx		// 현재 선택된 텝과 일치하는 그룹이어야 해
				&& nsCsFileTable::g_pDigimonMng->_IsExceptionDigimon( it->first ) == false )	// 제외 디지몬인지 확인
			{
				EncyclopediaContents::sEVOL_INFO* pInfo = csnew EncyclopediaContents::sEVOL_INFO;
				memcpy( pInfo, it->second, sizeof(EncyclopediaContents::sEVOL_INFO) );

				for( int i = 0 ; i < nLimit::EvoUnit ; i++ ) // 디지몬 진화슬롯은 1번부터~
				{
					if( pInfo->s_sInfo[ i ].s_nDigimonID == -1 || pInfo->s_sInfo[ i ].s_eImgState == EncyclopediaContents::sINFO::S_NONE )	// 3
						continue;						
				}						

				// 키값은 정렬번호로.. 가나다순이니까ㅋ
				m_pCurTabList_map[ pInfo->s_nOrder ] = pInfo;					
			}
		}

		// 마지막 3개중 2칸짜리(3칸짜리) 확인
		// 여러칸이면 늘어난 칸만큼 스크롤바 사이즈 늘려줌
		EncyclopediaContents::MAP_IT itCurList = m_pCurTabList_map.begin();
		EncyclopediaContents::MAP_IT itCurListEnd = m_pCurTabList_map.end();

		for( int i = 0 ; i < 5 ; i++ )
		{
			itCurListEnd--;

			if( itCurListEnd->second == NULL )
				break;
			
			int nCnt = itCurListEnd->second->s_nCount;// 몇줄짜리인지 판단하기위함

			if( itCurListEnd == itCurList )	// 거슬러 올라가다 처음까지 갔으면 break
				break;
		}

		SetTabListData();		
	}
	else // 그룹 탭이면
	{
		EncyclopediaContents::MAP_GROUP_CIT itGroup    = GetSystem()->GetGroupMapData().begin();
		EncyclopediaContents::MAP_GROUP_CIT itGroupEnd = GetSystem()->GetGroupMapData().end();

		for( ; itGroup != itGroupEnd ; ++itGroup )
		{
			EncyclopediaContents::MAP_DATA_CIT itList	 = itGroup->second->s_pListData.begin();
			EncyclopediaContents::MAP_DATA_CIT itListEnd = itGroup->second->s_pListData.end();

			itGroup->second->s_bGroupOpen = true;

			for( ; itList != itListEnd ; ++itList )
			{
				int nDigimonID = (*itList)->s_dwBaseDigimonID;
				EncyclopediaContents::MAP_CIT itInfo = GetSystem()->GetEncyMapData().find( nDigimonID );
				for( int i = 0;i < nLimit::EvoUnit ; i++)
				{
					if( itInfo->second->s_sInfo[i].s_nDigimonID != (*itList)->s_dwDestDigimonID )
						continue;
					if( itInfo->second->s_sInfo[i].s_eImgState == EncyclopediaContents::sINFO::S_OPEN )
						(*itList)->s_bIsOpen = true;
					else
						itGroup->second->s_bGroupOpen = false;

					EncyclopediaContents::MAP_DATA_CIT itTemp	 = itGroup->second->s_pListData.begin();
					EncyclopediaContents::MAP_DATA_CIT itTempEnd = itGroup->second->s_pListData.end();

					for( ; itTemp != itTempEnd ; ++itTemp )
					{
						if( (*itList)->s_dwDestDigimonID == (*itTemp)->s_dwDestDigimonID 
							&& (*itTemp)->s_bIsRender == false 
							&& (*itList)->s_bIsOpen == true )
						{
							EncyclopediaContents::MAP_CIT it = GetSystem()->GetEncyMapData().find( (*itTemp)->s_dwBaseDigimonID );

							if( it->second->s_sInfo[i].s_eImgState == EncyclopediaContents::sINFO::S_CLOSE )
								(*itList)->s_bIsOpen = false;
						}
					}
				}
			}
		}

		// 마지막 3개중 2칸짜리(3칸짜리) 확인
		// 여러칸이면 늘어난 칸만큼 스크롤바 사이즈 늘려줌
		itGroup = GetSystem()->GetGroupMapData().begin();
		itGroupEnd = GetSystem()->GetGroupMapData().end();

		for( int i = 0 ; i < 5 ; i++ )
		{
			itGroupEnd--;

			if( itGroupEnd->second == NULL )
				break;

			int nCnt = itGroupEnd->second->s_cRanderCnt;// 몇줄짜리인지 판단하기위함

			if( itGroupEnd == itGroup )	// 거슬러 올라가다 처음까지 갔으면 break
				break;
		}

		SetTabGroupListData();
	}

	//Refresh가 아닌 경우에만 스크롤바 설정
	if( m_nCurTab != nCurRadioIdx )
	{
		m_nCurTab = nCurRadioIdx;
	}
	else	//Refresh인 경우 툴팁이랑 통계 있을 때
	{
		if( m_nTooltipDigimonID != 0 )
		{
			_SetTooltip( 0 );//툴팁 새로고침
			_SetGroupTooltip( 0 );//툴팁 새로고침
		}
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCYCLOPEDIA_STATS ) )
			g_pGameIF->GetEncyclopedia_Statistics()->_SetText_Stat();//통계 새로고침
	}	
}

void cEncyclopedia::_RefreshList() 
{ 
	_SetTabList( m_pRadioBtn->GetCheckIndex() ); 
}

void cEncyclopedia::SetTabListData()
{
	m_pEncyListBox->RemoveAllItem();

	EncyclopediaContents::MAP_IT it = m_pCurTabList_map.begin();
	EncyclopediaContents::MAP_IT itEnd = m_pCurTabList_map.end();

	//cSprite* IconBG = NiNew cSprite;
	//IconBG->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "DigimonStatus\\Evol\\IconBG.tga", false );

	CsPoint ptDigimonPos = CsPoint( 10, 29 ) ;

	for( ; it != itEnd ; it++ )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cListBoxItem * addItem  = NiNew cListBoxItem;
		SAFE_POINTER_BEK( addItem );

		cSprite* DigimonBG = NiNew cSprite;
		SAFE_POINTER_BEK( DigimonBG );

		if( it->second->s_nCount > 10 )	// 3줄
		{
			if( it->second->s_bIsAllOpen )	// 모두 오픈 상태일 땐 
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 165 ), "Encyclopedia\\Encyclopedia_DigimonBG_XL.tga", false );
			}
			else
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 165 ), "Encyclopedia\\Encyclopedia_DigimonBG_XL_Close.tga", false );
			}

		}
		else if( it->second->s_nCount > 5 )	// 2줄
		{
			if( it->second->s_bIsAllOpen )	// 모두 오픈 상태일 땐 
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 118 ), "Encyclopedia\\Encyclopedia_DigimonBG_L.tga", false );
			}
			else							// 하나라도 잠겨있으면
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 118 ), "Encyclopedia\\Encyclopedia_DigimonBG_L_Close.tga", false );
			}
		}
		else
		{
			if( it->second->s_bIsAllOpen )	// 모두 오픈 상태일 땐 
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 59 ), "Encyclopedia\\Encyclopedia_DigimonBG_S.tga", false );
			}
			else							// 하나라도 잠겨있으면
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 59 ), "Encyclopedia\\Encyclopedia_DigimonBG_S_Close.tga", false );
			}
		}

		cString::sSPRITE* sSprite = pItem->AddSprite(DigimonBG, ptDigimonPos);
		SAFE_POINTER_RET(sSprite);
		sSprite->SetAutoPointerDelete(true);

		for( int j = 0 ; j < nLimit::EvoUnit ; j++ )
		{
			if( it->second->s_sInfo[ j ].s_nDigimonID == -1 
				|| it->second->s_sInfo[ j ].s_eImgState == EncyclopediaContents::sINFO::S_NONE )
				continue;

			CsPoint ptPos = it->second->s_sInfo[ j ].s_rcIcon.GetPos();
		
// 			if( it->second->s_nCount > 5 )	// 2줄
// 			{
// 				ptPos = ptPos + CsPoint( 0, 4 );
// 				if( j > 10 )
// 					ptPos = ptPos + CsPoint( 0, -14 );
// 				else if( j > 5 )
// 					ptPos = ptPos + CsPoint( 0, -7 );
// 			}
			cSprite* IconBG = NiNew cSprite;
			IconBG->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "DigimonStatus\\Evol\\IconBG.tga", false );
			cString::sSPRITE* sSprite = pItem->AddSprite(IconBG, ptDigimonPos + ptPos);
			SAFE_POINTER_RET(sSprite);
			sSprite->SetAutoPointerDelete(true);

			cImage*	IconIMG = NiNew cImage;
			IconIMG->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), it->second->s_sInfo[ j ].s_ImgFileName.c_str(), false, false );
			IconIMG->SetTexToken( CsPoint( 32, 0 ) );

			cString::sIMAGE* pImage = pItem->AddImage(IconIMG, (cImage::eSTATE)(it->second->s_sInfo[j].s_eImgState) , ptDigimonPos + ptPos + CsPoint( 6, 6 ));
			pImage->SetAutoPointerDelete(true);
		}

		cButton* GetItemBtn = NiNew cButton;
		GetItemBtn->Init( NULL, CsPoint::ZERO, CsPoint( 181 , 21 ), cButton::IMAGE_ENCYCLOPEDIA_GET, it->second->s_cDigimonName.c_str(), false );

		if( it->second->s_bIsAllOpen == true && it->second->s_bIsReward == false )
		{
			GetItemBtn->SetEnable( true );
		}
		else
		{
			GetItemBtn->SetEnable( false );
		}

		GetItemBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cEncyclopedia::PressGroupListBtn);
		GetItemBtn->SetUserData( new sDIGIMON_ID( it->second->s_sInfo[ 1 ].s_nDigimonID ) ); 
		pItem->AddButton( GetItemBtn, 0, ptDigimonPos + CsPoint( 90, -19 ), CsPoint::ZERO, CsPoint(90,5) );


		//별 종류,수 체크
		if( it->second->s_nLevel != 0 )	//아직 부화 안한 디지몬은 별 랜더 안함
		{
			int nVal_Scale = GetSystem()->GetScaleVal( it->second->s_nSize );

			int nStarIdx = nVal_Scale + ( it->second->s_nEnchant * 5 );

			std::string fileName;
			DmCS::StringFn::FormatA( fileName, "Encyclopedia\\Star\\Star%02d.tga", nStarIdx+1 );

			int nStarCnt = nStarIdx / 5;
			CsPoint ptStarSize= CsPoint( 13 + nStarCnt*8, 14 );

			cSprite* m_pStarImg = NiNew cSprite;
			m_pStarImg->Init( NULL, CsPoint::ZERO , ptStarSize, fileName.c_str(), false );
			cString::sSPRITE* sSprite = pItem->AddSprite(m_pStarImg, ptDigimonPos + CsPoint( 6, -16 ));
			SAFE_POINTER_RET(sSprite);
			sSprite->SetAutoPointerDelete(true);
		}

		addItem->Delete();
		addItem->SetUserData(new sDIGIMON_ID( it->second->s_sInfo[ 1 ].s_nDigimonID ));
		addItem->SetItem( pItem );
		m_pEncyListBox->AddItemMultiHeight( addItem, 8 );	
	}

	//SetTabListDataRenderCount();	
}

// void cEncyclopedia::SetTabListDataRenderCount()
// {
// 	EncyclopediaContents::MAP_CIT_R it	  = m_pCurTabList_map.rbegin();
// 	EncyclopediaContents::MAP_CIT_R itEnd = m_pCurTabList_map.rend();
// 
// 	int nRenderCnt = 0;
// 	int count = 0;
// 
// 	for( ; it != itEnd ; it++ )
// 	{
// 		if( it->second->s_nCount > 10 )	
// 		{
// 			nRenderCnt = nRenderCnt + 3;
// 		}
// 		else if( it->second->s_nCount > 5 )
// 		{
// 			nRenderCnt = nRenderCnt + 2;
// 		}
// 		else
// 		{
// 			nRenderCnt = nRenderCnt + 1;
// 		}
// 
// 		count++;
// 
// 		if(nRenderCnt >= 5)
// 			break;
// 	}
//	m_pEncyListBox->SetScrollBarRenderCount(count);
// }

void cEncyclopedia::SetTabGroupListData()
{
	m_pEncyListBox->RemoveAllItem();
	m_IconList.clear();

	EncyclopediaContents::MAP_GROUP_CIT itGroup    = GetSystem()->GetGroupMapData().begin();
	EncyclopediaContents::MAP_GROUP_CIT itGroupEnd = GetSystem()->GetGroupMapData().end();

	CsPoint ptDigimonPos = CsPoint( 10, 29 );

	int count = 0;	

	for( ; itGroup != itGroupEnd ; itGroup++ )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cListBoxItem * addItem  = NiNew cListBoxItem;
		SAFE_POINTER_BEK( addItem );

		cSprite* DigimonBG = NiNew cSprite;

		int nListSize = itGroup->second->s_cRanderCnt;

		NiColor fColor = ENCY_ENABLE_COLOR;
		float fAlpha = ENCY_ENABLE_ALPHA;

		if( itGroup->second->s_nGroupIdx != GetSystem()->GetUseDeck() && GetSystem()->GetUseDeck() != INT_MIN )
		{
			fColor = ENCY_DISABLE_COLOR;
			fAlpha = ENCY_DISABLE_ALPHA;
		}

		if( nListSize > 10 )		// 3줄
		{
			if( itGroup->second->s_bGroupOpen )	// 모두 오픈 상태일 땐 
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 118 ), "Encyclopedia\\Encyclopedia_DigimonBG_XL.tga", false );
				DigimonBG->SetColor( fColor );
			}
			else								// 하나라도 잠겨있으면
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 118 ), "Encyclopedia\\Encyclopedia_DigimonBG_XL_Close.tga", false );
			}

		}
		else if( nListSize > 5 )	// 2줄
		{
			if( itGroup->second->s_bGroupOpen )	// 모두 오픈 상태일 땐 
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 118 ), "Encyclopedia\\Encyclopedia_DigimonBG_L.tga", false );
				DigimonBG->SetColor( fColor );
			}
			else								// 하나라도 잠겨있으면
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 118 ), "Encyclopedia\\Encyclopedia_DigimonBG_L_Close.tga", false );
			}
		}
		else
		{
			if( itGroup->second->s_bGroupOpen )	// 모두 오픈 상태일 땐 
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 59 ), "Encyclopedia\\Encyclopedia_DigimonBG_S.tga", false );
				DigimonBG->SetColor( fColor );
			}
			else								// 하나라도 잠겨있으면
			{
				DigimonBG->Init( NULL, CsPoint::ZERO, CsPoint( 272, 59 ), "Encyclopedia\\Encyclopedia_DigimonBG_S_Close.tga", false );
			}
		}

		DigimonBG->SetAlpha( fAlpha );
		cString::sSPRITE* sSprite = pItem->AddSprite(DigimonBG, ptDigimonPos);
		SAFE_POINTER_RET(sSprite);
		sSprite->SetAutoPointerDelete(true);

		EncyclopediaContents::MAP_DATA_CIT itList	 = itGroup->second->s_pListData.begin();
		EncyclopediaContents::MAP_DATA_CIT itListEnd = itGroup->second->s_pListData.end();
		int nListIdx = 0;
		for( ; itList != itListEnd ; ++itList )
		{
			if( (*itList)->s_bIsRender == false )
				continue;

			CsPoint ptPos;
			if( nListIdx < 5 )
				ptPos = CsPoint( 10 + ( nListIdx * 52), 9 );
			else if( nListIdx < 10 )	// 6번째 디지몬부터 다음줄
				ptPos = CsPoint( 10 + ( ( nListIdx - 5 ) * 52 ), 66 );
			else						// 11번째 디지몬부터 다음줄
				ptPos = CsPoint( 10 + ( ( nListIdx - 10 ) * 52 ), 123 );

			if( nListSize > 5 )	// 2줄인 경우 위치 조금씩 변경
			{
				ptPos = ptPos + CsPoint( 0, 4 );
				if( nListIdx > 4 )
					ptPos = ptPos + CsPoint( 0, -7 );
			}

			cSprite* IconBG = NiNew cSprite;
			IconBG->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "DigimonStatus\\Evol\\IconBG.tga", false );
			IconBG->SetColor( fColor );
			IconBG->SetAlpha( fAlpha );
 
 			sICON_INFO* iconInfo = new sICON_INFO();
			iconInfo->s_Sprite = pItem->AddSprite(IconBG, ptDigimonPos + ptPos);
			iconInfo->s_Sprite->SetAutoPointerDelete(true);

			iconInfo->s_dwBaseDigimonID = (*itList)->s_dwBaseDigimonID;
			iconInfo->s_dwDestDigimonID = (*itList)->s_dwDestDigimonID;
			iconInfo->s_count = count;
			iconInfo->s_nGroupIdx = itGroup->second->s_nGroupIdx;
			m_IconList.push_back(iconInfo);

			cImage*	IconIMG = NiNew cImage;
			IconIMG->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), (*itList)->s_ImgFileName.c_str(), false, false );
			IconIMG->SetTexToken( CsPoint( 32, 0 ) );
			IconIMG->SetColor( fColor );
			IconIMG->SetAlpha( fAlpha );

			if( (*itList)->s_bIsOpen )
			{
				cString::sIMAGE* pImage = pItem->AddImage(IconIMG, (cImage::eSTATE)(EncyclopediaContents::sINFO::S_OPEN),  ptDigimonPos + ptPos + CsPoint( 6, 6 ));
				pImage->SetAutoPointerDelete(true);
			}

			else
			{
				cString::sIMAGE* pImage = pItem->AddImage(IconIMG, (cImage::eSTATE)(EncyclopediaContents::sINFO::S_CLOSE), ptDigimonPos + ptPos + CsPoint( 6, 6 ));
				pImage->SetAutoPointerDelete(true);
			}

			nListIdx++;
		}

		cButton* GetItemBtn = NiNew cButton;
		GetItemBtn->Init( NULL, CsPoint::ZERO, CsPoint( 181 , 21 ), cButton::IMAGE_ENCYCLOPEDIA_GET, itGroup->second->s_GroupName.c_str(), false );
		GetItemBtn->SetAlpha( fAlpha );

		// 효과 적용 버튼 상태
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )										// 전투상태일 때는 무조건 비활성화
			GetItemBtn->SetEnable( false );
		else if( itGroup->second->s_bGroupOpen == true && GetSystem()->GetUseDeck() == INT_MIN )	// 해당 덱 완성 이면서 // 덱 효과 사용중이 아닐 때				  					
			GetItemBtn->SetEnable( true );
		else if( itGroup->second->s_nGroupIdx == GetSystem()->GetUseDeck() )						// 해당 덱 효과를 적용 중일 때
			GetItemBtn->SetEnable( true );
		else
			GetItemBtn->SetEnable( false );

		GetItemBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cEncyclopedia::PressGroupListBtn);
		GetItemBtn->SetUserData( new sDIGIMON_ID( itGroup->second->s_nGroupIdx ) ); 
		pItem->AddButton( GetItemBtn, 0, ptDigimonPos + CsPoint( 90, -19 ), CsPoint::ZERO, CsPoint(90,5) );

		addItem->Delete();
		addItem->SetUserData(new sDIGIMON_ID( itGroup->second->s_nGroupIdx, count ));
		addItem->SetItem( pItem );
		m_pEncyListBox->AddItemMultiHeight( addItem, 8 );	

		count++;
	}

	SetTabGroupListDataRenderCount();
}

void cEncyclopedia::SetTabGroupListDataRenderCount()
{
	EncyclopediaContents::MAP_GROUP_CIT_R itGroup	 = GetSystem()->GetGroupMapData().rbegin();
	EncyclopediaContents::MAP_GROUP_CIT_R itGroupEnd = GetSystem()->GetGroupMapData().rend();

	int nRenderCnt = 0;
	int count = 0;

	for( ; itGroup != itGroupEnd ; itGroup++ )
	{
		if( itGroup->second->s_cRanderCnt > 10 )	
		{
			nRenderCnt = nRenderCnt + 3;
		}
		else if( itGroup->second->s_cRanderCnt > 5 )
		{
			nRenderCnt = nRenderCnt + 2;
		}
		else
		{
			nRenderCnt = nRenderCnt + 1;
		}

		count++;

		if(nRenderCnt >= 5)
			break;
	}
	m_pEncyListBox->SetScrollBarRenderCount(count);
}

void cEncyclopedia::_SetTooltip( int nDigimonID )
{
	m_bIsMouseOn = true;

	if( m_nTooltipDigimonID == nDigimonID )	//툴팁 표시중인 디지몬과 같은놈이면 ㅌㅌ, 최초 한번만 셋 하기위함
		return;
	else if( nDigimonID != 0 )// 0 : Refresh인 경우
	{
		m_nTooltipDigimonID = nDigimonID;
	}

	m_DelayTime.Start();

	EncyclopediaContents::sEVOL_INFO* pDigimonData = GetSystem()->GetEncyData(m_nTooltipDigimonID);
	
	//레벨, 스케일
	if( pDigimonData->s_nLevel == 0 )	// 아직 부화 안한 디지몬이라 정보 없음
	{
		m_szLevel->SetText( _T( "-" ) );
		m_szScale->SetText( _T( "-" ) );
	}
	else
	{
		m_szLevel->SetText( pDigimonData->s_nLevel );
		m_szScale->SetText( pDigimonData->s_nSize );
	}

	//강화수치
	m_pEnchantVal.clear();

	int nIdx = 0;
	m_pEnchantVal[  nIdx++ ] = pDigimonData->s_nEnchant_AT;
	m_pEnchantVal[  nIdx++ ] = pDigimonData->s_nEnchant_CT;
	m_pEnchantVal[  nIdx++ ] = pDigimonData->s_nEnchant_EV;
	m_pEnchantVal[  nIdx++ ] = pDigimonData->s_nEnchant_BL;
	m_pEnchantVal[  nIdx ] = pDigimonData->s_nEnchant_HP;

	//높은 두 수치
	int nFirst  = GetSystem()->GetEnchant_Order( 0, m_pEnchantVal[0], m_pEnchantVal[1], m_pEnchantVal[2], m_pEnchantVal[3], m_pEnchantVal[4] );
	int nSecond = GetSystem()->GetEnchant_Order( 1, m_pEnchantVal[0], m_pEnchantVal[1], m_pEnchantVal[2], m_pEnchantVal[3], m_pEnchantVal[4] );

	//가장 높은 강화수치
	std::map <int, int>::iterator it = m_pEnchantVal.find( nFirst );
	if( it->second == 0 )//강화값이 0일 경우
		m_szEnchantVal[ 0 ]->SetText( _T( "-" ) );
	else
		m_szEnchantVal[ 0 ]->SetText( it->second );

	//두번째 높은 강화수치
	it = m_pEnchantVal.find( nSecond );
	if( it->second == 0 )//강화값이 0일 경우
		m_szEnchantVal[ 1 ]->SetText( _T( "-" ) );
	else
		m_szEnchantVal[ 1 ]->SetText( it->second );

	//계열체 이름
	m_szName->SetText( nsCsFileTable::g_pTacticsMng->GetTacticsExplain( m_nTooltipDigimonID )->GetInfo()->s_szTacticsName );

	cString* pString = NiNew cString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9 );

	//계열체 설명
	m_szExplain->Delete();
	TCHAR szExplain[ 512 ];
	_tcscpy( szExplain, nsCsFileTable::g_pTacticsMng->GetTacticsExplain( m_nTooltipDigimonID )->GetInfo()->s_szTacticsExplain );
	g_pStringAnalysis->Cut( m_szExplain, 205, szExplain, &ti );
}

void cEncyclopedia::_Updata_ForMouse_Group( int count )
{
	std::list<sICON_INFO*>::const_iterator it1		 = m_IconList.begin();
	std::list<sICON_INFO*>::const_iterator itEnd1	 = m_IconList.end();

	for( ; it1 != itEnd1 ; ++it1 )
	{
		sICON_INFO* pIcon = (*it1);

		if(count != pIcon->s_count)
		{
			continue;
		}

		bIsTooltip = true;
				
		if( CURSOR_ST.CheckClickBox( CsRect(pIcon->s_Sprite->s_RenderPos, pIcon->s_Sprite->s_RenderPos + pIcon->s_Sprite->s_pSprite->GetSize()) ) != CURSOR::BUTTON_OUTWINDOW )
		{
			CsDigimon::sINFO* pDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pIcon->s_dwDestDigimonID )->GetInfo();
			TCHAR szTooltip[128]={0,};
			TCHAR szName[64]={0,};
			//TCHAR szEvoType[64]={0,};

			//계열체 이름
			_tcscpy_s( szTooltip, nsCsFileTable::g_pTacticsMng->GetTacticsExplain( pIcon->s_dwBaseDigimonID )->GetInfo()->s_szTacticsName );

			EncyclopediaContents::MAP_GROUP_CIT itGroup    = GetSystem()->GetGroupMapData().begin();
			EncyclopediaContents::MAP_GROUP_CIT itGroupEnd = GetSystem()->GetGroupMapData().end();

			int count = 0;
			for( ; itGroup != itGroupEnd ; ++itGroup )
			{			
				if(pIcon->s_count == count)
				{
					EncyclopediaContents::MAP_DATA_CIT itList	 = itGroup->second->s_pListData.begin();
					EncyclopediaContents::MAP_DATA_CIT itListEnd = itGroup->second->s_pListData.end();

					int nCnt = 0;//두개 이상이면 다음줄로.. 너무 길어지면 안되니까

					for( ; itList != itListEnd ; ++itList)
					{
						if( pIcon->s_dwDestDigimonID == (*itList)->s_dwDestDigimonID	&& pIcon->s_dwBaseDigimonID != (*itList)->s_dwBaseDigimonID	)
						{
							if( nCnt > 2 )
								_tcscat_s( szTooltip, 128, _T("\n") );

							_tcscat_s( szTooltip, 128, _T(" & ") );
							_tcscat_s( szTooltip, 128, nsCsFileTable::g_pTacticsMng->GetTacticsExplain( (*itList)->s_dwBaseDigimonID )->GetInfo()->s_szTacticsName );
							nCnt++;
						}
					}
					break;
				}
				count++;
			}

			//디지몬 이름
			_tcscpy_s( szName, pDigimonInfo->s_szName );

			nsCsDigimonTable::eEVOLUTION_TYPE eEvoType = nsCsFileTable::g_pDigimonMng->GetDigimon( pIcon->s_dwDestDigimonID )->GetInfo()->s_eEvolutionType;

			std::wstring strEvolType;
			if( nsCsFileTable::g_pBaseMng )
				strEvolType = nsCsFileTable::g_pBaseMng->GetEvoTypeName( (int)eEvoType );

			std::wstring evoName;
			DmCS::StringFn::Format( evoName, L"( %s )", strEvolType.c_str() );

			_tcscat_s( szTooltip, 128, _T("\n") );
			_tcscat_s( szTooltip, 128, szName );
			_tcscat_s( szTooltip, 128, evoName.c_str() );

			bIsTooltip = false;
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 20 ) , CsPoint::ZERO, szTooltip );
			break;					
		}
	}	
}

void cEncyclopedia::_SetGroupTooltip( int nGroupIdx )
{
	if( nGroupIdx != 0 )
	{
		CsPoint ptCurPos = CURSOR_ST.GetPos() + CsPoint(20,20);
#if ( defined VERSION_USA || defined VERSION_TH )
		u4 nMaxSizeX = 350;
#else
		u4 nMaxSizeX = 240;
#endif
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptCurPos, CsPoint( 24, 24 ), nMaxSizeX, cTooltip::DECKLIST, nGroupIdx, 0, 0, 0, NULL );

	}
}

void cEncyclopedia::PressCancelButton( void* pSender, void* pData )
{
	Close();
}

void cEncyclopedia::PressStatisticsButton( void* pSender, void* pData )
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCYCLOPEDIA_STATS ) == false )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_ENCYCLOPEDIA_STATS );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ENCYCLOPEDIA_STATS );
}

void cEncyclopedia::PressGroupListBtn( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( GetSystem() );
	cButton* pButton = static_cast<cButton*>(pSender);
	sDIGIMON_ID* pDigimon = dynamic_cast<sDIGIMON_ID*>(pButton->GetUserData());
	SAFE_POINTER_RET( pDigimon );

	
	if( m_pRadioBtn->GetCheckIndex() < m_nGroupCount )	// 그룹 탭이 아닐 경우
	{
		EncyclopediaContents::MAP_CIT it = GetSystem()->GetEncyMapData().begin();
		EncyclopediaContents::MAP_CIT itEnd = GetSystem()->GetEncyMapData().end();

		for( ; it != itEnd ; it++ )
		{
			if( it->second->s_sInfo[ 1 ].s_nDigimonID == pDigimon->iDigimonId )
			{
				//획득 가능한 상태인지 인벤 검사
				if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )	// 빈칸 하나이상은 꼭 있어야함 무슨 아이템을 얻을지 모르니까
				{
					cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
				}
				else
				{
					// 아이템 보상을 이미 받은 경우
					if( it->second->s_bIsReward )
						continue;

					//아이템 획득 요청, 메세지 띄울듯?
					//뭐 선택했는지 저장해놔야되네;; 계열체 번호
					m_nSelectDigimonID = it->second->s_sInfo[ 1 ].s_nDigimonID;
					cPrintMsg::PrintMsg( 30670 );
				}
			}
		}		
	}
	else
	{
		EncyclopediaContents::MAP_GROUP_CIT itGroup    = GetSystem()->GetGroupMapData().begin();
		EncyclopediaContents::MAP_GROUP_CIT itGroupEnd = GetSystem()->GetGroupMapData().end();

		for( ; itGroup != itGroupEnd ; itGroup++ )
		{
			if(itGroup->second->s_nGroupIdx == pDigimon->iDigimonId)
			{
				m_nSelectDeckIdx = itGroup->second->s_nGroupIdx;

				if( itGroup->second->s_bUse )	// 효과 해제 물어봄
				{
					cPrintMsg::PrintMsg( 30673 );
				}
				else							// 효과 적용 물어봄
				{
					cPrintMsg::PrintMsg( 30672 );
				}
			}
		}
	}	
}