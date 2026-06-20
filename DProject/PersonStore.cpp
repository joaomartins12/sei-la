
#include "stdafx.h"
#include "PersonStore.h"

#define IF_PERSONSTORE_NAME_EDITBOX_POS	CsPoint( 35, 36 )
#define IF_PERSONSTORE_NAME_EDITBOX_SIZE	CsPoint( 230, 16 )
#define IF_PERSONSTORE_TEX_POS					CsPoint( 27, 462 )	//판매 수수료 텍스트 위치
#define IF_PERSONSTORE_ICON_SIZE					CsPoint( 32, 32 )	//아이콘 사이즈#endif

void cPersonStore::sINFO::Render( CsPoint pos )
{
	g_pIconMng->RenderItem( &s_Item, pos + CsPoint( 11, 9 ), false );

	if( s_Name.GetSize() == 1 )
		s_Name.Render( pos + CsPoint( 65, 15 ), 2 );
	else
		s_Name.Render( pos + CsPoint( 65, 7 ), 2 );

	s_CountText.Render( pos + CsPoint( 20, 46 ), DT_CENTER );
	
	s_MoneyIF.Render( pos + CsPoint( 88, 40 ) );

	#ifdef VERSION_USA
		s_PerItemText.Render( pos + CsPoint( 40, 44 ) );
	#else
		s_PerItemText.Render( pos + CsPoint( 60, 43 ) );
	#endif
}

cPersonStore::cPersonStore()
{
	m_pTarget = NULL;
	
	m_pBtnDynamicOK = NULL;

	m_pEditName = NULL;
	m_pStoreName = NULL;

	for( int i=0; i<IF_PERSONSTORE_WIN_COUNT; ++i )
		m_pBGWin[ i ] = NULL;
	m_pTexRate = NULL;

	m_nUsedItemID = 0;
#ifdef EMPLOYMENT_TAMERNAME
	m_pTamerName = NULL;
#endif

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = true;
#endif
}

void cPersonStore::Destroy()
{
	cBaseWindow::Delete();	
}

void cPersonStore::DeleteResource()
{
	cItemSeparate::DeleteInstance( cBaseWindow::WT_PERSONSTORE, 0 );

	DeleteScript();	

	DeleteResistItem();

	SAFE_NIDELETE( m_pBtnDynamicOK );
	SAFE_NIDELETE( m_pEditName );
	SAFE_NIDELETE( m_pStoreName );

	for( int i=0; i<IF_PERSONSTORE_WIN_COUNT; ++i )
		SAFE_NIDELETE( m_pBGWin[ i ] );

	SAFE_NIDELETE( m_pItemBlock );
	SAFE_NIDELETE( m_pTexRate );

	for( int i=0; i<IF_PERSONSTORE_VIEW_COUNT; ++i )
		SAFE_NIDELETE( m_pItemBtn[ i ] );

	// 아이템 언?
	if( g_pDataMng )
	{
		g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::PersonStore, 1 );
		g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::PersonStore, 3 );
	}
#ifdef EMPLOYMENT_TAMERNAME
	SAFE_NIDELETE( m_pTamerName );

#endif

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = false;
#endif

}

void cPersonStore::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	m_pTarget = NULL;
	m_eValueType = (eVALUE_TYPE)nValue;

	InitScript( NULL, CsPoint( 80, 80 ), CsPoint( 300, 493 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pBGWin[ 0 ] = NiNew cSprite;
	m_pBGWin[ 0 ]->Init( NULL, CsPoint::ZERO, CsPoint( 300, 493 ), "Store_G\\Win_Person01.tga", false );
	m_pBGWin[ 1 ] = NiNew cSprite;
	m_pBGWin[ 1 ]->Init( NULL, CsPoint::ZERO, CsPoint( 300, 493 ), "Store_G\\Win_Person02.tga", false );
	m_pBGWin[ 2 ] = NiNew cSprite;
	m_pBGWin[ 2 ]->Init( NULL, CsPoint::ZERO, CsPoint( 300, 493 ), "Store_G\\Win_Person03.tga", false );
	m_pBGWin[ 3 ] = NiNew cSprite;
	m_pBGWin[ 3 ]->Init( NULL, CsPoint::ZERO, CsPoint( 300, 493 ), "Store_G\\Win_Person04.tga", false );

	m_pCancelButton = AddButton( CsPoint( 278, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 276, 83 ), CsPoint( 16, 365 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 13, 83 ), CsPoint( 290, 448 ) ), IF_PERSONSTORE_VIEW_COUNT );

	CsPoint posBtn( 28, 100 );
	for( int i=0; i<IF_PERSONSTORE_VIEW_COUNT; ++i )
	{
		m_pItemBtn[ i ] = NiNew cButton;
		m_pItemBtn[ i ]->Init( GetRoot(), posBtn, CsPoint( 226, 40 ), "Store_G\\Win_Person_SelMask.tga", false );
		m_pItemBtn[ i ]->SetTexToken( CsPoint( 0, 40 ) );

		posBtn.y += 69;
	}

	m_pItemBlock = NiNew cSprite;
	m_pItemBlock->Init( NULL, CsPoint::ZERO, IF_PERSONSTORE_ICON_SIZE, "Inventory\\Block.tga", false );	

	CsPoint startRC( 32, 104 );
	for( int i=0; i<IF_PERSONSTORE_VIEW_COUNT; ++i )
	{
		m_rcRect[ i ] = CsRect( startRC, (CsSIZE)IF_PERSONSTORE_ICON_SIZE );
		startRC.y += 69;
	}
	
	int nTex = 0;
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		{
			ReadySuccess();			
			nTex = (int)( nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fPerson_Charge*100.0f );
		}
		break;
	case VT_RES_EMPLOYMENT:
		{
			ReadySuccess();
			nTex = (int)( nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fEmployment_Charge*100.0f );
		}
		break;
	
	case VT_OPEN_PERSON:
		{

		}
		break;
	case VT_OPEN_EMPLOYMENT:
		{
		}		
		break;

	default:
		assert_cs( false );
	}

	TCHAR sz[ 64 ];
	_stprintf_s( sz, 64, _LAN( "판매 수수료 : %d %%" ), nTex );
	cText::sTEXTINFO ti;
	ti.Init();
	ti.SetText( sz );
	m_pTexRate = NiNew cText;
	m_pTexRate->Init( NULL, CsPoint::ZERO, &ti, false );

	ChangeWindowTitle();
	ChangeDefaultBtnText();
}

void cPersonStore::Update(float const& fDeltaTime)
{
	// 테이머와의 거리 체크
	if( ( m_pTarget->GetPos2D() - g_pCharMng->GetTamerUser()->GetPos2D() ).Length() > IF_TARGET_TAMER_MAX_DIST )
	{
		Close();
		return;
	}

	// 개인 상점 열려 있는지 체크
	switch( m_eValueType )
	{
	case VT_OPEN_PERSON:
		{
			CTamer* pTamer = g_pCharMng->GetTamer( m_nTargetIDX );
			if( pTamer == NULL )
			{
				cPrintMsg::PrintMsg( 30354 );
				Close();
				return;
			}

			if( pTamer->GetCondition()->IsCondition( nSync::Shop ) == 0 )
			{
				cPrintMsg::PrintMsg( 30354 );
				Close();
				return;
			}
		}
		break;
	case VT_RES_EMPLOYMENT_START:
	case VT_OPEN_EMPLOYMENT:
		{
			CEmployment* pEmployment = g_pCharMng->GetEmployment( m_nTargetIDX );
			if( pEmployment == NULL )
			{
				cPrintMsg::PrintMsg( 30354 );
				Close();
				return;
			}
		}
		break;
	}

	// 전투중 체크 - 소비자만 체크
	if( g_pCharMng->GetTamerUser()->IsBattle() )
	{
		switch( m_eValueType )
		{
		case VT_OPEN_PERSON:
		case VT_OPEN_EMPLOYMENT:
			Close();
			break;
		}
	}


	cItemSeparate::UpdateInstance( cBaseWindow::WT_PERSONSTORE, 0 );
	
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cPersonStore::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	// 스크롤바
	m_pScrollBar->Update_ForMouse();
	if( m_pEditName )
		m_pEditName->Update_ForMouse( CsPoint::ZERO );

	// 캔슬 버튼
	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			switch( m_eValueType )
			{			
			case VT_RES_EMPLOYMENT_START:
			case VT_RES_PERSON:
			case VT_OPEN_PERSON:
			case VT_RES_EMPLOYMENT:
			case VT_OPEN_EMPLOYMENT:
				Close( true );
				break;
			
			case VT_RES_PERSON_START:				
				Close( true );
				break;

			default:
				assert_cs( false );
			}				
		}		
		return muReturn;
	default:
		return muReturn;
	}
	
	// 승인 버튼
	if( m_pBtnDynamicOK && ( m_pBtnDynamicOK->Update_ForMouse() == cButton::ACTION_CLICK ) )
	{
		_UpdateDynamicBtn();
	}

	_UpdateItemBtn();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	int nCurPos = m_pScrollBar->GetCurPosIndex();
	int nRenderCnt = m_pScrollBar->GetRenderCount();
	CsVectorPB< sINFO* >* pVector = GetCurVector();	
	for( int i=0; i<nRenderCnt; ++i )
	{
		int nIndex = i + nCurPos;
		if( pVector->IsExistElement( nIndex ) == false )
			continue;

		if( m_rcRect[ i ].PtInRect( ptMouseLocalPos ) == FALSE )
			continue;

		cItemInfo ItemInfo = pVector->GetData( nIndex )->s_Item;
		CsPoint pos = m_rcRect[ i ].GetPos() + GetRootClient();

		g_pGameIF->GetTooltip()->SetTooltip( pos, IF_PERSONSTORE_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, ItemInfo.GetType(), cBaseWindow::WT_PERSONSTORE, 0, 0, &ItemInfo );
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( this->GetRoot()->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

cBaseWindow::eMU_TYPE
cPersonStore::Update_ForMouse_Disable()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	return cBaseWindow::Update_ForMouse_Disable();
}


void cPersonStore::_UpdateItemBtn()
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON_START:
	case VT_RES_EMPLOYMENT_START:
		return;
		
	case VT_RES_PERSON:
	case VT_OPEN_PERSON:
	case VT_RES_EMPLOYMENT:
	case VT_OPEN_EMPLOYMENT:
		break;
	default:
		assert_cs( false );
	}			

	int nCurPos = m_pScrollBar->GetCurPosIndex();
	int nRenderCnt = m_pScrollBar->GetRenderCount();
	CsVectorPB< sINFO* >* pVector = GetCurVector();	
	for( int i=0; i<IF_PERSONSTORE_VIEW_COUNT; ++i )
	{
		int nIndex = i + nCurPos;
		if( pVector->IsExistElement( nIndex ) == false )
			continue;

		if( m_pItemBtn[ i ]->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			switch( m_eValueType )
			{
			case VT_RES_PERSON:
				{
					////디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
					sINFO* pInfo = pVector->GetData(nIndex); //개인상점 등록 정보
					cItemInfo info = pInfo->s_Item; //개인상점 등록 아이템 정보

					if( info.m_nEndTime > 0 )//기간제 템이면
					{
						int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();//인벤 총 슬롯 수

						cItemInfo* m_pInven;
						for( int i=0; i<m_nInvenSlotCount; ++i )
						{
							m_pInven = g_pDataMng->GetInven()->GetData(i); //해당 슬롯의 아이템
							if(  m_pInven->IsEnable() == false )
								continue;
							if(  m_pInven->m_nType == info.m_nType && m_pInven->m_nEndTime == info.m_nEndTime ) //등록한 아이템일때
							{
								g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
								//break;
							}
						}
					}

					// 물건 빼주자
					SAFE_NIDELETE( pVector->GetData( nIndex ) );
					pVector->DeleteElement( nIndex );

					if( m_vpSellInfo.Size() == 0 )
						m_pBtnDynamicOK->SetEnable( false );
				}
				break;
			case VT_OPEN_PERSON:
				{
					m_nSelectIndex = nIndex;
					// 물건 사기 등록
					sINFO* pInfo = GetCurVector()->GetData( nIndex );
					cItemInfo info = pInfo->s_Item;

					info.m_nCount = 1;
					cItemSeparate::NewInstance( GetWindowType(), 0, cItemSeparate::BUY, &info, pInfo->s_Item.GetCount(), pInfo->s_MoneyIF.GetMoney(), cMoneyIF::DISP_FULL );
				}
				break;
			case VT_RES_EMPLOYMENT:
				{
					//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
					sINFO* pInfo = pVector->GetData(nIndex);
					cItemInfo info = pInfo->s_Item;
					
					if( info.m_nEndTime > 0 )//기간제 템이면
					{
						int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();//인벤 총 슬롯 수

						cItemInfo* m_pInven;
						for( int i=0; i<m_nInvenSlotCount; ++i )
						{
							m_pInven = g_pDataMng->GetInven()->GetData(i);//해당 슬롯의 아이템
							if(  m_pInven->IsEnable() == false )
								continue;
							if(  m_pInven->m_nType == info.m_nType && m_pInven->m_nEndTime == info.m_nEndTime ) //등록한 아이템일때
							{
								g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
								//break;
							}
						}
					}

					// 물건 빼주자
					SAFE_NIDELETE( pVector->GetData( nIndex ) );
					pVector->DeleteElement( nIndex );

					if( m_vpSellInfo.Size() == 0 )
						m_pBtnDynamicOK->SetEnable( false );
				}
				break;

			case VT_OPEN_EMPLOYMENT:
				{
					sINFO* pInfo = GetCurVector()->GetData( nIndex );
					cItemInfo info = pInfo->s_Item;
					info.m_nCount = 1;

					m_nSelectIndex = nIndex;
					// 물건 사기 등록
					cItemSeparate::NewInstance( GetWindowType(), 0, cItemSeparate::BUY, &info, pInfo->s_Item.GetCount(), pInfo->s_MoneyIF.GetMoney(), cMoneyIF::DISP_FULL );
				}
				break;
			default:
				assert_cs( false );
			}			
		}
	}
}

void cPersonStore::_UpdateDynamicBtn()
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		{
 			TCHAR* szStoreName = (TCHAR*)m_pEditName->GetTextAll();
			szStoreName = nBase::ltrim( szStoreName );
			std::wstring sendStoreName = nBase::rtrim( szStoreName );

			switch( Language::CheckShopName(sendStoreName) )
			{
			case Language::STRING_EMPTY:		cPrintMsg::PrintMsg( 30370 );	return;
			case Language::STRING_SIZE_MIN:
			case Language::STRING_SIZE_MAX:		return;
			case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 );	return;
			}

			
			if( nsCsFileTable::g_pCuidMng->CheckID( sendStoreName.c_str() ) == false )
			{
				cPrintMsg::PrintMsg( 10029 );
				return;
			}

			CsVectorPB< sINFO* >* pVec = GetCurVector();
			int nCnt = pVec->Size();
			if( nCnt == 0 )
				return;

			nTrade::Item* pArray = csnew nTrade::Item[ nCnt ];
			for( int i=0; i<nCnt; ++i )
			{
				sINFO* pInfo = pVec->GetData( i );

				pArray[ i ].m_nIsSell = 1;
				memcpy( &pArray[ i ].m_sItem, &pInfo->s_Item, sizeof( cItemData ) );
				pArray[ i ].m_nPrice = (n8)pInfo->s_MoneyIF.GetMoney().GetInt();
			}

			if( net::game )
			{
				net::game->SendTamerShopOpen( sendStoreName.c_str(), pVec->Size(), pArray );
				SetEnableWindow( false );
				m_pEditName->ReleaseFocus( false );
			}

			delete[] pArray;			
		}
		break;
	case VT_RES_PERSON_START:
		{
			if( net::game )
			{
				net::game->SendTamerShopReady( 2 );
				SetEnableWindow( false );
			}
		}
		break;
	case VT_RES_EMPLOYMENT:
		{
			TCHAR* szStoreName = (TCHAR*)m_pEditName->GetTextAll();
			szStoreName = nBase::ltrim( szStoreName );
			std::wstring sendStoreName = nBase::rtrim( szStoreName );

			switch( Language::CheckShopName(sendStoreName) )
			{
			case Language::STRING_EMPTY:		cPrintMsg::PrintMsg( 30370 );	return;
			case Language::STRING_SIZE_MIN:
			case Language::STRING_SIZE_MAX:		return;
			case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 );	return;
			}

			if( nsCsFileTable::g_pCuidMng->CheckID( sendStoreName.c_str() ) == false )
			{
				cPrintMsg::PrintMsg( 10029 );
				return;
			}
			CTamerUser* pUser = g_pCharMng->GetTamerUser();
			int nCnt = m_vpSellInfo.Size();
			if( 0 == nCnt )
				return;

			nTrade::Item* pArray = csnew nTrade::Item[ nCnt ];
			int nBuyVecCnt = 0;

			int nSellVecCnt = m_vpSellInfo.Size();
			for( int i=0; i<nSellVecCnt; ++i )
			{
				sINFO* pInfo = m_vpSellInfo.GetData( i );
				pArray[ i+nBuyVecCnt ].m_nIsSell = 1;
				memcpy( &pArray[ i+nBuyVecCnt ].m_sItem, &pInfo->s_Item, sizeof( cItemData ) );
				pArray[ i+nBuyVecCnt ].m_nPrice = (n8)pInfo->s_MoneyIF.GetMoney().GetInt();

				//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
				int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();

				for( int k=0; k<m_nInvenSlotCount; ++k )
				{
					 cItemInfo* m_Item = g_pDataMng->GetInven()->GetData(k);
					if( ( pInfo->s_Item.m_nType == m_Item->m_nType ) && (pInfo->s_Item.m_nEndTime == m_Item->m_nEndTime))
					{
						uint nCount = pInfo->s_Item.GetCount();
						for(int j = k; j<m_nInvenSlotCount; ++j )
						{
							m_Item = g_pDataMng->GetInven()->GetData(j);
							if(  m_Item->IsEnable() == false )
								continue;
							g_pDataMng->ItemUnlock( TO_INVEN_SID( j ) );
						}

						if( g_pDataMng->GetInven()->GetData( k )->GetCount() < nCount )
						{
							g_pDataMng->GetInven()->DecreaseItem( pInfo->s_Item.GetType(), pInfo->s_Item.m_nRate, nCount, false, true );
							//g_pDataMng->GetInven()->GetData( k )->Reset();
							break;
						}
						else
						{
							g_pDataMng->GetInven()->GetData( k )->DecreaseCount( nCount );
							break;
						}
					}
				}				
			}

			if( net::game )
			{
				nSync::Pos pos;
				pos.m_nX = (int)pUser->GetPos2D().x;
				pos.m_nY = (int)pUser->GetPos2D().y;
				net::game->SendCommissionShopOpen( pos, pUser->GetCurRot(), sendStoreName.c_str(), 0, nCnt, pArray );
				SetEnableWindow( false );
				m_pEditName->ReleaseFocus( false );
			}

			delete[] pArray;
			cPrintMsg::PrintMsg( 30351 );
		}
		break;
	case VT_RES_EMPLOYMENT_START:
		{
			if( net::game )
			{
				net::game->SendCommissionShopClose();
				SetEnableWindow( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void cPersonStore::Render()
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		m_pBGWin[ 1 ]->Render( GetRootClient() );
		break;
	case VT_RES_EMPLOYMENT:
		m_pBGWin[ 1 ]->Render( GetRootClient() );
		break;
	case VT_RES_PERSON_START:
	case VT_OPEN_PERSON:
		m_pBGWin[ 0 ]->Render( GetRootClient() );
		break;
	case VT_RES_EMPLOYMENT_START:
	case VT_OPEN_EMPLOYMENT:
		m_pBGWin[ 0 ]->Render( GetRootClient() );
		break;
	}

	RenderScript();	

	int nCurPos = m_pScrollBar->GetCurPosIndex();
	int nRenderCnt = m_pScrollBar->GetRenderCount();
	CsVectorPB< sINFO* >* pVector = GetCurVector();	
	
#ifdef IME_WINDOW_POSITION
	if( m_pEditName != NULL )
	{
		CsPoint TempPos = m_pEditName->GetDefaultPos();
		CsPoint TempStringPos = m_pEditName->GetText()->GetStringSize();
		g_nIMEWidth = GetRootClient().x + TempPos.x + TempStringPos.x;
		g_nIMEHeight = GetRootClient().y + TempPos.y + TempStringPos.y;
	}
#endif

	if( m_pBtnDynamicOK )
		m_pBtnDynamicOK->Render();
	if( m_pEditName )
		m_pEditName->Render();
	if( m_pStoreName )
		m_pStoreName->Render();
#ifdef EMPLOYMENT_TAMERNAME
	if( m_pTamerName )
		m_pTamerName->Render();
#endif
	int nOpenSlot = 99;
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_EMPLOYMENT:	
	case VT_RES_PERSON_START:
	case VT_RES_EMPLOYMENT_START:
		assert_csm1( nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject.find( m_nUsedItemID ) != nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject.end(),
					L"위탁상점 ID가 테이블에 존재 하지않습니다 ID = %d", m_nUsedItemID );
		nOpenSlot = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject[ m_nUsedItemID ]->s_nSlotCount;
		break;
	}
	
	for( int i=0; i<nRenderCnt; ++i )
	{
		int nIndex = i + nCurPos;

		if( pVector->IsExistElement( nIndex ) == false )
		{
			if( nIndex >= nOpenSlot )
			{
				m_pItemBlock->Render( GetRootClient() + m_rcRect[ i ].GetPos() );
			}
		}
		else
		{
			m_pItemBtn[ i ]->Render();
			pVector->GetData( nIndex )->Render( GetRootClient() + m_rcRect[ i ].GetPos() - CsPoint( 11, 9 ) );
		}
	}

	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_PERSON_START:
		m_pTexRate->Render( IF_PERSONSTORE_TEX_POS + GetRootClient() );
		break;
	case VT_OPEN_PERSON:
		break;

	case VT_RES_EMPLOYMENT:
	case VT_RES_EMPLOYMENT_START:
		m_pTexRate->Render( IF_PERSONSTORE_TEX_POS + GetRootClient() );
		break;

	case VT_OPEN_EMPLOYMENT:
		break;;

	default:
		assert_cs( false );
	}

	EndRenderScript();
	cItemSeparate::RenderInstance( cBaseWindow::WT_PERSONSTORE, 0 );
}

bool cPersonStore::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_EMPLOYMENT:
		break;
	default:
		return false;
	}

	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			cItemInfo* pItem = g_pDataMng->GetInven()->GetData( nIconSlot );
			if( g_pDataMng->IsTradeItem( pItem ) == true )
			{
				int nTotalCnt = g_pDataMng->GetInven()->GetCount_Item_ID_Rate( pItem->GetType(), pItem->m_nRate );
				int nResCnt = GetResItemCount( pItem->GetType(), pItem->m_nRate );
				int nEnableCnt = nTotalCnt - nResCnt;
				if( nEnableCnt > 0 )
				{
					cItemInfo info = *pItem;
					info.m_nCount = nIconCount;
					cItemSeparate::NewInstance( cBaseWindow::WT_PERSONSTORE, 0, cItemSeparate::SELL_RES, &info, nEnableCnt, (u8)0, cMoneyIF::DISP_FULL );
				}
				else
				{
					cPrintMsg::PrintMsg( 30366 );
				}
			}

			CURSOR_ST.ReleaseIcon();
		}
		return true;
	}

	return false;
}

void cPersonStore::ResetDevice()
{	
	ResetDeviceScript();

	if( m_pBtnDynamicOK )
		m_pBtnDynamicOK->ResetDevice();

	if( m_pEditName )
		m_pEditName->ResetDevice();

	if( m_pStoreName )
		m_pStoreName->ResetDevice();

	if( m_pItemBlock )
		m_pItemBlock->ResetDevice();

	for( int i=0; i<IF_PERSONSTORE_VIEW_COUNT; ++i )
		m_pItemBtn[ i ]->ResetDevice();

	for( int i=0; i<IF_PERSONSTORE_WIN_COUNT; ++i )
		m_pBGWin[ i ]->ResetDevice();
#ifdef EMPLOYMENT_TAMERNAME
	if( m_pTamerName )
		m_pTamerName->ResetDevice();
#endif
}

void cPersonStore::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	if( m_pBtnDynamicOK )
		m_pBtnDynamicOK->ResetDevice();	

	if( m_pEditName )
		m_pEditName->ResetDevice();

	if( m_pStoreName )
		m_pStoreName->ResetDevice();

	if( m_pItemBlock )
		m_pItemBlock->ResetDevice();

	for( int i=0; i<IF_PERSONSTORE_VIEW_COUNT; ++i )
		m_pItemBtn[ i ]->ResetDevice();

	for( int i=0; i<IF_PERSONSTORE_WIN_COUNT; ++i )
		m_pBGWin[ i ]->ResetDevice();
#ifdef EMPLOYMENT_TAMERNAME
	if( m_pTamerName )
		m_pTamerName->ResetDevice();
#endif
}

bool cPersonStore::Close( bool bSound /* = true */ )
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		{
			if( net::game )
			{
				net::game->SendTamerShopReady( 3 );	
				SetEnableWindow( false );
				//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
				CsVectorPB< sINFO* >* pVec = GetCurVector();
				int nCnt = pVec->Size();

				for( int i=0; i<nCnt; ++i )
				{
					//sINFO* pInfo = pVec->GetData( i );
					int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();

					cItemInfo* m_pInven;
					for( int k=0; k<m_nInvenSlotCount; ++k )
					{
						m_pInven = g_pDataMng->GetInven()->GetData(k);
						if(  m_pInven->IsEnable() == false )
							continue;
						g_pDataMng->ItemUnlock( TO_INVEN_SID( k ) );
					}
				}
				
				return true;
			}
		}
		break;
	case VT_RES_PERSON_START:
		{
			cPrintMsg::PrintMsg( 30375 );
			return true;
		}
		break;
	case VT_RES_EMPLOYMENT:
		{
			if( net::game )
			{
				net::game->SendTamerShopReady( 5 );
				SetEnableWindow( false );

				//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
				CsVectorPB< sINFO* >* pVec = GetCurVector();
				int nCnt = pVec->Size();

				for( int i=0; i<nCnt; ++i )
				{
					//sINFO* pInfo = pVec->GetData( i );
					int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();

					cItemInfo* m_pInven;
					for( int k=0; k<m_nInvenSlotCount; ++k )
					{
						m_pInven = g_pDataMng->GetInven()->GetData(k);
						if(  m_pInven->IsEnable() == false )
							continue;
						g_pDataMng->ItemUnlock( TO_INVEN_SID( k ) );
					}
				}
				return true;
			}
		}
		break;
	case VT_RES_EMPLOYMENT_START:
		{
		}
		break;

	case VT_OPEN_PERSON:
		{
		}
		break;

	case VT_OPEN_EMPLOYMENT:
		{
		}
		break;
	default:
		assert_cs( false );
	}

	return cBaseWindow::Close( bSound );
}

void	cPersonStore::ClosePerson()
{
	//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
	CsVectorPB< sINFO* >* pVec = GetCurVector();
	int nCnt = pVec->Size();

	for( int i=0; i<nCnt; ++i )
	{
		//sINFO* pInfo = pVec->GetData( i );
		int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();

		cItemInfo* m_pInven;
		for( int k=0; k<m_nInvenSlotCount; ++k )
		{
			m_pInven = g_pDataMng->GetInven()->GetData(k);
			if(  m_pInven->IsEnable() == false )
				continue;
			g_pDataMng->ItemUnlock( TO_INVEN_SID( k ) );
		}
	}
}

//==============================================================================================================
//
//		판매 등록
//
//==============================================================================================================

void cPersonStore::ReadySuccess()
{	
	SetTarget( g_pCharMng->GetTamerUser() );
	if( m_pStoreName == NULL )
	{
		TCHAR szInit[ 64 ] = {0, };		
		_stprintf_s( szInit, 64, _LAN( "%s 상점" ), g_pCharMng->GetTamerUser()->GetName() );
		SetEditName( szInit );
	}
	else
	{
		SetEditName( (TCHAR*)m_pStoreName->GetText() );
	}	
}

void cPersonStore::OpenSuccess()
{
	SetEnableWindow( true );
	
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		{
			m_eValueType = VT_RES_PERSON_START;
			////디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
			//CsVectorPB< sINFO* >* pVec = GetCurVector();
			//int nCnt = pVec->Size();

			//for( int i=0; i<nCnt; ++i )
			//{
			//	//sINFO* pInfo = pVec->GetData( i );
			//	int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();

			//	cItemInfo* m_pInven;
			//	for( uint k=0; k<m_nInvenSlotCount; ++k )
			//	{
			//		m_pInven = g_pDataMng->GetInven()->GetData(k);
			//		if(  m_pInven->IsEnable() == false )
			//			continue;
			//		g_pDataMng->ItemUnlock( TO_INVEN_SID( k ) );
			//	}
			//}
		}
		
		break;
	default:
		assert_cs( false );
	}
	
	TCHAR* szName = (TCHAR*)m_pEditName->GetTextAll();
	SetStoreName( szName );

	ChangeWindowTitle();
	ChangeDefaultBtnText();
}

void cPersonStore::Open2ReadySuccess()
{
	SetEnableWindow( true );

	switch( m_eValueType )
	{
	case VT_RES_PERSON_START:
		m_eValueType = VT_RES_PERSON;
		cPrintMsg::PrintMsg( 30352 );
		break;
	case VT_RES_EMPLOYMENT_START:
		m_eValueType = VT_RES_EMPLOYMENT;
		cPrintMsg::PrintMsg( 30353 );
		break;
	default:
		assert_cs( false );
	}
	
	ReadySuccess();

	ChangeWindowTitle();
	ChangeDefaultBtnText();
}

void cPersonStore::OpenMyEmployment()
{
	switch( m_eValueType )
	{
	case VT_OPEN_EMPLOYMENT:
		m_eValueType = VT_RES_EMPLOYMENT_START;
		break;
	default:
		assert_cs( false );
		return;
	}
	ChangeWindowTitle();
	ChangeDefaultBtnText();
}

void cPersonStore::DeleteResistItem()
{
	int nCnt = m_vpSellInfo.Size();
	for( int i=0; i<nCnt; ++i )
		SAFE_NIDELETE( m_vpSellInfo[ i ] );
	m_vpSellInfo.Destroy();
}

u8 cPersonStore::_CalTexMoney( u8 nNeedMoney )
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON_START:
		return nNeedMoney - (int)( nNeedMoney*nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fPerson_Charge );

	case VT_OPEN_PERSON:
		return nNeedMoney;

	case VT_OPEN_EMPLOYMENT:
		{
			return nNeedMoney;
		}
		break;
	}

	assert_cs( false );
	return 0;
}

void cPersonStore::ItemTrade_BuySuccess( int nSlotIndex, int nCount )
{
	sINFO* pInfo = GetCurVector()->GetData( nSlotIndex );
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pInfo->s_Item.GetType() )->GetInfo();
	u8 nNeedMoney = pInfo->s_MoneyIF.GetMoney().GetInt()*nCount;

	bool bChangeUI = false;
	int  SellItemIndex = 0;

	switch( m_eValueType )
	{
	case VT_RES_PERSON_START:
		{
			// 돈 증가
			g_pDataMng->GetInven()->IncreaseMoney( _CalTexMoney( nNeedMoney ), true );

			// 인벤 아이템 감소
			//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
			int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();

			for( int k=0; k<m_nInvenSlotCount; ++k )
			{
				cItemInfo* m_Item = g_pDataMng->GetInven()->GetData(k);
				if( ( pInfo->s_Item.m_nType == m_Item->m_nType ) && (pInfo->s_Item.m_nEndTime == m_Item->m_nEndTime))
				{
					//int InvenCount = pInfo->s_Item.GetCount();
					//g_pDataMng->ItemUnlock( TO_INVEN_SID( k ) );
					/*for(int i = k; i<m_nInvenSlotCount; ++i )
					{
						m_Item = g_pDataMng->GetInven()->GetData(i);
						if(  m_Item->IsEnable() == false )
							continue;
						g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
					}*/

					// [3/17/2016 hyun] 팔린 아이템의 인벤 인덱스 저장
					SellItemIndex = k;

					if( g_pDataMng->GetInven()->GetData( k )->GetCount() < (uint)nCount )
					{
						g_pDataMng->GetInven()->DecreaseItem( pInfo->s_Item.GetType(), pInfo->s_Item.m_nRate, nCount, false, true );
						//g_pDataMng->GetInven()->GetData( k )->Reset();
						break;
					}
					else
					{
						g_pDataMng->GetInven()->GetData( k )->DecreaseCount( nCount );
						break;
					}
				}
			}
			//g_pDataMng->GetInven()->DecreaseItem( pInfo->s_Item.GetType(), pInfo->s_Item.m_nRate, nCount, false, true );

			// UI 변경
			bChangeUI = true;

			cPrintMsg::PrintMsg( 30355, pFTItem->s_szName );
		}		
		break;
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		{
			// 돈 감소
			g_pDataMng->GetInven()->DecreaseMoney( _CalTexMoney( nNeedMoney ) );

			// 인벤 아이템 증가
			cItemData ItemData;
			memcpy( &ItemData, &pInfo->s_Item, sizeof( cItemData ) );
			ItemData.m_nCount = nCount;
			g_pDataMng->GetInven()->ItemCrop( &ItemData );

			// UI 변경
			bChangeUI = true;

			// 윈도우 다시 동작 가능
			SetEnableWindow( true );

			cPrintMsg::PrintMsg( 30356, pFTItem->s_szName );
		}
		break;
	default:
		assert_cs( false );
	}

	if( bChangeUI == true )
	{
		pInfo->s_Item.m_nCount -= nCount;
		pInfo->s_CountText.SetText( pInfo->s_Item.m_nCount );

		// 상점에 올린 품목중에 한품목이 모두 팔렸다면 지워주자
		if( pInfo->s_Item.m_nCount == 0 )
		{
			SAFE_NIDELETE( pInfo );
			GetCurVector()->DeleteElement( nSlotIndex );
			g_pDataMng->ItemUnlock(SellItemIndex); // [3/17/2016 hyun] 다팔렸다면 인벤에 락을 해제
		}
	}
}

void cPersonStore::ItemTrade_BuyFail( int nResult )
{
	SetEnableWindow( true );
	switch( m_eValueType )
	{
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		{
			switch( nResult )
			{
			case 1:
				cPrintMsg::PrintMsg( 30369 );
				SetTarget( m_pTarget );
				break;
			default:			assert_cs( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

bool cPersonStore::_ResistItem( CsVectorPB< sINFO* >* pVector, cItemInfo* pItem, sMONEY MoneyPerItem )
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_EMPLOYMENT:
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		break;
	default:
		assert_cs( false );
		return false;
	}

	CsBaseMng::sPERSON_STORE::sOBJECT_INFO* pObject = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject[ m_nUsedItemID ];
	// 슬롯 갯수 체크
	if( pObject->s_nSlotCount <= pVector->Size() )
	{
		cPrintMsg::PrintMsg( 30367 );
		return false;
	}

	// 기존의 아이템에 같은 타입이 있는지 체크
	sINFO* pInfo = GetInfo( pItem->GetType(), pItem->m_nRate, MoneyPerItem );
	if( pInfo == NULL || pItem->m_nEndTime > 0)//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
	{
		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() )->GetInfo();	

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, g_pDataMng->GetItemColor( pFTItem ) );

		pInfo = NiNew sINFO;

		pInfo->s_Item = *pItem;
		g_pStringAnalysis->Cut( &pInfo->s_Name, 150, pFTItem->s_szName, &ti );	

		ti.s_Color = FONT_WHITE;
		ti.SetText( pInfo->s_Item.GetCount() );
		pInfo->s_CountText.Init( NULL, CsPoint::ZERO, &ti, false );

		ti.SetText( _LAN( "개당" ) );
		pInfo->s_PerItemText.Init( NULL, CsPoint::ZERO, &ti, false );

		pInfo->s_MoneyIF.Init( cMoneyIF::DISP_FULL, MoneyPerItem, true );

		pVector->PushBack( pInfo );

		m_pScrollBar->SetRange( CsPoint( 0, pVector->Size() ) );
		//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
		if( pItem->m_nEndTime > 0 ) //기간제 아이템일때
		{
			int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();
			
			cItemInfo* m_pInven;
			for( int i=0; i<m_nInvenSlotCount; ++i )
			{
				m_pInven = g_pDataMng->GetInven()->GetData(i);
				if(  m_pInven->IsEnable() == false )
					continue;
				if(  m_pInven->m_nType == pItem->m_nType && m_pInven->m_nEndTime == pItem->m_nEndTime ) //타입과 기간이 같으면 잠궈줘 중복 등록 안되게
				{
					g_pDataMng->ItemLock( TO_INVEN_SID( i ) );
					//break;
				}
			}
		}
	}
	else
	{
		pInfo->s_Item.m_nCount += pItem->GetCount();
		pInfo->s_CountText.SetText( pInfo->s_Item.GetCount() );
	}

	if( m_pBtnDynamicOK )
		m_pBtnDynamicOK->SetEnable( true );

	switch( m_eValueType )
	{
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		{
			int m_nInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount();//인벤 총 슬롯 수

			cItemInfo* m_pInven;
			for( int i=0; i<m_nInvenSlotCount; ++i )
			{
				m_pInven = g_pDataMng->GetInven()->GetData(i); //해당 슬롯의 아이템
				if(  m_pInven->IsEnable() == false )
					continue;
				g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
				//break;

			}
		}
		break;
	case VT_RES_PERSON:
	case VT_RES_EMPLOYMENT:
		break;
	default:
		assert_cs( false );
		return false;
	}
	return true;
}

void cPersonStore::ResistItem_Sell( cItemInfo* pItem, sMONEY MoneyPerItem )
{
	if( _ResistItem( &m_vpSellInfo, pItem, MoneyPerItem ) == false )
		return;	
}

void cPersonStore::Item_Buy( cItemInfo* pItem )
{
	sINFO* pInfo = GetCurVector()->GetData( m_nSelectIndex );
	if( pInfo->s_Item.GetType() != pItem->GetType() )
	{
		assert_cs( false );
		return;
	}

	if( pInfo->s_Item.m_nRate != pItem->m_nRate )
	{
		assert_cs( false );
		return;
	}

	if( pInfo->s_Item.GetCount() < pItem->GetCount() )
	{
		assert_cs( false );
		return;
	}

	// 인벤토리 여유공간체크
	g_pDataMng->GetInven()->Backup_Save();

	cItemInfo ItemInfo = *pItem;
	if( g_pDataMng->GetInven()->ItemCrop( &ItemInfo ) != 0 )
	{
		cPrintMsg::PrintMsg( 11015 );
		g_pDataMng->GetInven()->Backup_Load();
		return;
	}
	g_pDataMng->GetInven()->Backup_Load();

	// 금액 체크
	u8 nNeedMoney = pInfo->s_MoneyIF.GetMoney().GetInt()*pItem->GetCount();
	if( nNeedMoney > g_pDataMng->GetInven()->GetMoney().GetInt() )
	{
		cPrintMsg::PrintMsg( 11003 );
		return;
	}

	cItemData ItemData;
	memcpy( &ItemData, pItem, sizeof( cItemData ) );
	switch( m_eValueType )
	{
	case VT_OPEN_PERSON:
		net::game->SendTamerShopBuy( m_pTarget->GetUniqID(), m_nSelectIndex, &ItemData, (n8)pInfo->s_MoneyIF.GetMoney().GetInt() );		
		break;
	case VT_OPEN_EMPLOYMENT:
		net::game->SendCommissionShopBuy( m_pTarget->GetUniqID(), m_nSelectIndex, &ItemData, (n8)pInfo->s_MoneyIF.GetMoney().GetInt() );
		break;
	default:
		assert_cs( false );
	}
	SetEnableWindow( false );
}

//==============================================================================================================
//
//		기타
//
//==============================================================================================================

void cPersonStore::ChangeWindowTitle()
{
	TCHAR szOk[ 64 ] = {0, };
	bool bEnableEdit = false;
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		AddTitle( _LAN( "개인 상점 등록" ) );
		break;
	case VT_RES_EMPLOYMENT:
		AddTitle( _LAN( "위탁 상점 등록" ) );
		break;
	case VT_RES_PERSON_START:
	case VT_OPEN_PERSON:
		AddTitle( _LAN( "개인 상점" ) );
		break;
	case VT_RES_EMPLOYMENT_START:
	case VT_OPEN_EMPLOYMENT:
		AddTitle( _LAN( "위탁 상점" ) );
		break;
	default:
		assert_cs( false );
	}
}

void cPersonStore::ChangeDefaultBtnText()
{
	switch( m_eValueType )
	{
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		SAFE_NIDELETE( m_pBtnDynamicOK );
		return;
	}

	if( m_pBtnDynamicOK == NULL )
	{
		m_pBtnDynamicOK = NiNew cButton;
		m_pBtnDynamicOK->Init( GetRoot(), CsPoint( 182, 456 ), CsPoint( 85, 27 ), cButton::IMAGE_NOR_13, _T( "" ), false );
	}

	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		m_pBtnDynamicOK->SetText( _LAN( "판매시작" ) );	
		m_pBtnDynamicOK->SetEnable( m_vpSellInfo.Size() != 0 );
		break;
	case VT_RES_PERSON_START:
		m_pBtnDynamicOK->SetText( _LAN( "판매중지" ) );
		break;

	case VT_RES_EMPLOYMENT:
		m_pBtnDynamicOK->SetText( _LAN( "위탁상점 오픈" ) );
		m_pBtnDynamicOK->SetEnable( false );
		break;
	case VT_RES_EMPLOYMENT_START:
		m_pBtnDynamicOK->SetText( _LAN( "위탁상점 회수" ) );
		break;

	default:
		assert_cs( false );
	}
}

void cPersonStore::SetTarget( CsC_AvObject* pTarget )
{
	assert_cs( pTarget != NULL );
	m_pTarget = pTarget;
	m_nTargetIDX = m_pTarget->GetIDX();

	// 상점 이름
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_PERSON_START:
	case VT_RES_EMPLOYMENT:
	case VT_RES_EMPLOYMENT_START:
		break;
	
	case VT_OPEN_PERSON:
		SetEnableWindow( false );
		if( net::game )
		{
			net::game->SendTamerShopItemList( m_pTarget->GetUniqID(), 0 );
		}
		break;
	case VT_OPEN_EMPLOYMENT:
		SetEnableWindow( false );
		if( net::game )
		{
			net::game->SendTamerShopItemList( m_pTarget->GetUniqID(), 1 );
		}
		break;
	default:
		assert_cs( false );
	}
}

void cPersonStore::SetStoreName( TCHAR const* sz )
{
	g_pDataMng->GetPersonStore()->SetTempStoreName( sz, Language::pLength::ShopName );	

	if( m_pStoreName == NULL )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, FONT_GOLD );
		ti.SetText( sz );

		m_pStoreName = NiNew cText;
		m_pStoreName->Init( GetRoot(), IF_PERSONSTORE_NAME_EDITBOX_POS + CsPoint( 0, 2 ), &ti, false );
	}
	else
	{
		m_pStoreName->SetText( sz );
	}

	SAFE_NIDELETE( m_pEditName );
}

void cPersonStore::SetEditName( TCHAR const* sz )
{
	cText::sTEXTINFO tiEdit;
	tiEdit.Init();
	tiEdit.SetText( sz );		

	assert_cs( m_pEditName == NULL );
	m_pEditName = NiNew cEditBox;
	m_pEditName->Init( GetRoot(), IF_PERSONSTORE_NAME_EDITBOX_POS, IF_PERSONSTORE_NAME_EDITBOX_SIZE, &tiEdit, false );
	m_pEditName->EnableUnderline();
	m_pEditName->EnablePaste();
	m_pEditName->SetFontLength( Language::pCountry::MAX_LenShopName );	
	m_pEditName->SetFocus();

	g_IME.Native_On( GAMEAPP_ST.GetHWnd() );
	SAFE_NIDELETE( m_pStoreName );

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}


CsVectorPB< cPersonStore::sINFO* >*
cPersonStore::GetCurVector()
{
	return &m_vpSellInfo;
}

cPersonStore::sINFO*
cPersonStore::GetInfo( int nItemID, int nRate, sMONEY MoneyPerItem )
{
	CsVectorPB< sINFO* >* pVec = GetCurVector();
	int nCnt = pVec->Size();
	for( int i=0; i<nCnt; ++i )
	{
		sINFO* pInfo = pVec->GetData( i );
		if( ( pInfo->s_Item.GetType() == nItemID )&&
			( pInfo->s_Item.m_nRate == nRate )&&
			( pInfo->s_MoneyIF.GetMoney() == MoneyPerItem ) )
		{
			return pInfo;
		}
	}
	return NULL;
}

int cPersonStore::GetResItemCount( int nItemID, int nRate )
{
	int nRes = 0;
	CsVectorPB< sINFO* >* pVec = GetCurVector();
	int nCnt = pVec->Size();
	for( int i=0; i<nCnt; ++i )
	{
		sINFO* pInfo = pVec->GetData( i );
		if( ( pInfo->s_Item.m_nType == nItemID )&&
			( pInfo->s_Item.m_nRate == nRate ) )
		{
			nRes += pInfo->s_Item.GetCount();
		}
	}
	return nRes;
}
#ifdef EMPLOYMENT_TAMERNAME
void cPersonStore::SetTamerName( TCHAR const* sz )
{
	TCHAR szTamerNameInIt[ 64 ] = {0, };		
	_stprintf_s( szTamerNameInIt, 64, _T( "< %s >" ), sz );

	if( m_pTamerName == NULL )
	{

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, FONT_WHITE );
		ti.s_eTextAlign = DT_CENTER;

		ti.SetText( szTamerNameInIt );

		m_pTamerName = NiNew cText;
		m_pTamerName->Init( GetRoot(), CsPoint( 150, 62 ), &ti, false );
	}
	else
	{
		m_pTamerName->SetText( szTamerNameInIt );
	}
}
#endif

bool cPersonStore::OnMacroKey(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_ESCAPE:
				{
					Close();
					return true;
				}
				break;			
			}
		}
		break;
	}
	return false;
}