
#include "stdafx.h"
#include "DigimonIncubatorViewer.h"
#define IF_INCUBATOR_DELTA_Y		50
bool CDigimonIncubatorViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}
CDigimonIncubatorViewer::CDigimonIncubatorViewer()
:m_bisDragable(false),
m_pIncuListBox(NULL),
m_pIncubatorDescTitle(NULL),
m_pIncubatorDesc(NULL),
m_fElapsedTime(0.0f)
{
	Construct();
}

CDigimonIncubatorViewer::~CDigimonIncubatorViewer()
{
}

void CDigimonIncubatorViewer::Destroy()
{
	DeleteResource();
}

void CDigimonIncubatorViewer::DeleteResource()
{
	DeleteScript();
}

void CDigimonIncubatorViewer::DeleteScript()
{
	//2017-01-11-nova 인큐베이터 창이 닫힐경우 팝업메시지도 같이 닫히도록 함
	if( cMessageBox::IsMessageBox() )
	{
		cMessageBox::DelMsg( 30625, false );	//2017-01-23-nova
		cMessageBox::DelMsg( 30626, false );
		cMessageBox::DelMsg( 30629, false );	//2017-01-23-nova
	}
	GAME_EVENT_ST.DeleteEventAll( this );
	cScriptUI::DeleteScript();
}

void CDigimonIncubatorViewer::Create(  cWindow* pkRoot, int nValue /*= 0 */ )
{
	InitScript( pkRoot, NULL, CsPoint( 44, 122 ), CsPoint(300,450 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP);
	CsPoint kGridPos = CsPoint(0, 113);
	CsPoint kGridSize = CsPoint(324, 400);
	CsPoint kIncuSlotSize =  CsPoint(300, 110);
	m_pIncuListBox = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pIncuListBox );
	m_pIncuListBox->Init( GetRoot(), CsPoint(0, 113), CsPoint(324, 400), 
		CsPoint::ZERO, CsPoint(300, 110), cGridListBox::ColumnDownRight, cGridListBox::LeftTop, NULL, false, 0 );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &CDigimonIncubatorViewer::OnItemClickedInIncuList );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_SPRITE_UP, this, &CDigimonIncubatorViewer::OnDragUpOnSprite );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &CDigimonIncubatorViewer::OnMouseDownOnItem );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_BUTTON_UP, this, &CDigimonIncubatorViewer::OnMouseUpStartIncubating );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_MOUSE_UP_ON_GRIDLIST, this, &CDigimonIncubatorViewer::OnMouseUpOnGridList );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &CDigimonIncubatorViewer::OnItemRClickedInIncuList );
	m_pIncuListBox->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonIncubatorViewer::OnMouseMoveOnItem );
	m_pIncuListBox->SetBackOverAndSelectedImgRender(false);
	AddChildControl(m_pIncuListBox);

	cScrollBar* pScrollBar = NiNew cScrollBar;
	if(pScrollBar)
	{
		pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint(0,0), CsPoint(16,390), cScrollBar::GetDefaultBtnSize(), 
			CsRect(CsPoint(0,0), CsPoint(16,390)), 1, false );
		m_pIncuListBox->SetScrollBar( pScrollBar );
	}

	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_14, NiColor::WHITE );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT("Digimon_Storage_Incubating_Title").c_str() );
	m_pIncubatorDescTitle = AddText( &ti, CsPoint(0,0) );
	m_pIncubatorDescTitle->SetVisible(false);

	NiColor kDescColor = NiColor(86.0f/256.0f,166.0f/256.0f,227.0f/256.0f);
	ti.Init(CFont::FS_11, kDescColor );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( L"" );
	m_pIncubatorDesc = AddText( &ti, CsPoint(0,25) );
	m_pIncubatorDesc->SetText(UISTRING_TEXT("Digimon_Storage_Incubating_Title1").c_str(), 330);
	m_pIncubatorDesc->SetVisible(false);
}
void CDigimonIncubatorViewer::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
	m_pIncuListBox->Update(fDeltaTime);

	//1초에 한번씩 잔여시간 업데이트 하기
	m_fElapsedTime += fDeltaTime;
	if(m_fElapsedTime > 1.0f)
	{
		UpdateRemainTime();
		m_fElapsedTime = 0.0f;
	}
}
BOOL CDigimonIncubatorViewer::UpdateMouse()
{
	if(m_pIncuListBox && m_pIncuListBox->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;
	return FALSE;
}
void CDigimonIncubatorViewer::ResetDevice()
{
	ResetDeviceScript();
}
void CDigimonIncubatorViewer::Render()
{
}
void	CDigimonIncubatorViewer::OnItemClickedInIncuList(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
}
void	CDigimonIncubatorViewer::OnItemRClickedInIncuList(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	CsPoint ptMouseLocalPos = CURSOR_ST.GetPos();
	std::list< cString::sELEMENT* >::iterator elemnetList = pItem->GetItem()->Get_sTextList();
	int listSize = pItem->GetItem()->GetListSize();
	for(int i = 0; i < listSize; ++i, ++elemnetList)
	{
		cString::sELEMENT::eTYPE tempType = (*elemnetList)->GetType();
		if(tempType == cString::sELEMENT::SPRITE)
		{
			cString::sSPRITE* sprite = (cString::sSPRITE*) (*elemnetList);
			BT_INFO* pkInfo = static_cast< BT_INFO* >(sprite->s_pSprite->GetUserData());
			if(pkInfo != NULL )
			{
				CsPoint spritePt = sprite->s_pSprite->GetTempPos();
				CsPoint size = spritePt + sprite->s_ptSize;
				CsRect spriteRect(spritePt, size);
				bool bVisible = sprite->s_pSprite->GetVisible();
				if( bVisible == true && spriteRect.PtInRect(ptMouseLocalPos))
				{
					OnRButotnUpOnSprite(pItem, sprite);
					return;
				}
			}
		}
	}
}
void	CDigimonIncubatorViewer::OnRButotnUpOnSprite(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);
	cGridListBoxItem* pkItem = static_cast<cGridListBoxItem*>(pSender);
	SAFE_POINTER_RET(pData);
	cString::sSPRITE* pSprite = static_cast< cString::sSPRITE* >(pData);
	SAFE_POINTER_RET(pSprite->s_pSprite);

	BT_INFO* pkInfo = static_cast< BT_INFO* >(pSprite->s_pSprite->GetUserData());
	if(pkInfo )
	{
		if(pkInfo->BTTYPE == BT_TYPE::_SLOT)
			GetSystem()->OnRClick_Incubator_Digimon(pkItem->getID());
		else if(pkInfo->BTTYPE == BT_TYPE::_ITEM)
			GetSystem()->OnRClick_Incubator_Item(pkItem->getID());
	}
}
void	CDigimonIncubatorViewer::OnDragUpOnSprite(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cString::sSPRITE* pSprite = static_cast< cString::sSPRITE* >(pData);
	SAFE_POINTER_RET(pSprite->s_pSprite);
	BT_INFO* pkInfo = static_cast< BT_INFO* >(pSprite->s_pSprite->GetUserData());
	if(pkInfo )
	{
		if(pkInfo->BTTYPE == BT_TYPE::_SLOT)
		{
			GetSystem()->SetDragInfo(CDigimonArchiveContents::DragedDigimonType::DEST_INCU_DIGIMON, 
				pkInfo->ParentIncuSlotIdx);
			GetSystem()->OnDragUp_ArchieveToIncubator();
		}
		else if(pkInfo->BTTYPE == BT_TYPE::_ITEM)
		{
			int InvenIdx = GetSystem()->GetLastSaveCursorIndex();
			int ItemCnt = GetSystem()->GetLastSaveCursorCount();
			GetSystem()->SetDragInfo(CDigimonArchiveContents::SRC_INVEN_ITEM, InvenIdx);
			GetSystem()->SetDragInfo(CDigimonArchiveContents::DragedDigimonType::DEST_INCU_DIGIMON, 
				pkInfo->ParentIncuSlotIdx);
			GetSystem()->OnDragUp_InvenToIncubator();
		}
	}
}
void	CDigimonIncubatorViewer::OnMouseUpStartIncubating(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cString::sBUTTON* pButton = static_cast< cString::sBUTTON* >(pData);
	SAFE_POINTER_RET(pButton);
	SAFE_POINTER_RET(pButton->s_pButton);
	BT_INFO* pkbtinfo = dynamic_cast<BT_INFO*>(pButton->s_pButton->GetUserData());
	if(pkbtinfo && pkbtinfo->BTTYPE == _START)
		GetSystem()->OnClick_StartIncubating(pkbtinfo->ParentIncuSlotIdx);
}

void	CDigimonIncubatorViewer::UpdateViewer(void)
{
	m_pIncubatorDescTitle->SetVisible(true);
	m_pIncubatorDesc->SetVisible(true);

	const CsPoint kGridPos = CsPoint(0,113);
	const CsPoint kGridSize = CsPoint(324,400);
	const CsPoint kIncuSlotSize =  CsPoint(300,110);
	const CsPoint kIncu_FRAME_SLOT_SIZE =  CsPoint(48,48);
	const CsPoint kIncu_Inner_SLOT_SIZE =  CsPoint(36,36);
	const CsPoint kIncu_Inner_FRAME_POS =  CsPoint(10,50);
	const CsPoint kIncu_Inner_SLOT_POS =  CsPoint(16,56);
	const CsPoint kIncu_Inner_BG_POS =  CsPoint(5,25);
	const CsPoint kIncu_Inner_BG_SIZE =  CsPoint(135,50);
	const CsPoint kIncu_Inner_ITEM_POS =  CsPoint(100,55);
	const CsPoint kIncu_Inner_ITEM_SIZE =  CsPoint(42,42);
	const CsPoint kIncu_Inner_ITEM_ICON_POS = CsPoint(106,61);
	const CsPoint kIncu_Inner_ITEM_ICON_SIZE =  CsPoint(30,30);
	const CsPoint kIncu_ADD_BT_POS =  CsPoint(160,55);
	const CsPoint kIncu_ADD_BT_SIZE =  CsPoint(130,40);
	const CsPoint kIncu_NAME_TT_POS =  CsPoint(70,10);
	const CsPoint kIncu_LV_TT_POS =  CsPoint(10,10);
	const CsPoint kIncu_ADDBT_TEXT_POS =  CsPoint(223,68);

	m_pIncuListBox->RemoveAllItem();
	const CDigimonArchiveContents::IncuSlotInfo* pkIncuSlotInfos =GetSystem()->GetIncuSlotInfo();
	CDigimonArchiveContents::IncuSlotInfo::const_iterator kIter = pkIncuSlotInfos->begin();
	int iCount =0;
	for(; kIter != pkIncuSlotInfos->end(); ++kIter, ++ iCount)
	{
		//Create List Viewer
		cString * pItem = NiNew cString;

		cSprite* pGBoxImage = NiNew cSprite;	// 1
		pGBoxImage->Init( NULL, CsPoint::ZERO, CsPoint(kIncuSlotSize), "TacticsHouse\\Storage_Gbox.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pGBoxImage, CsPoint::ZERO, CsPoint(kIncuSlotSize)  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);

		CDigimonArchiveContents::DigimonIncuSlotInfo* pkInfo = kIter->second;
		///////////////////////base Slot Sprite
		cSprite* pDigimonImage = NiNew cSprite;	// 1
		pDigimonImage->Init( NULL, CsPoint::ZERO, kIncu_FRAME_SLOT_SIZE, "TacticsHouse\\Storage_slot.tga", false );
		BT_INFO* pkbtslotinfo = new BT_INFO();
		pkbtslotinfo->BTTYPE = _SLOT;
		pkbtslotinfo->ParentIncuSlotIdx = iCount;
		pDigimonImage->SetUserData(pkbtslotinfo);
		cString::sSPRITE* sAddedDigiSprite = pItem->AddSprite( pDigimonImage, kIncu_Inner_FRAME_POS, kIncu_FRAME_SLOT_SIZE  );	
		if( sAddedDigiSprite )
			sAddedDigiSprite->SetAutoPointerDelete(true);

		//DigimonSprite
		if(pkInfo->GetReg_DigimonArchiveSlotID() > 0)
		{
			cData_PostLoad::sDATA*  pkData = GetSystem()->GetDigimonDataInArchive(pkInfo->GetReg_DigimonArchiveSlotID());
			if(pkData != NULL)
			{
				DWORD dwDigimonID = pkData->s_dwBaseDigimonID;
				if(dwDigimonID != 0)
				{
					if(nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID ) == NULL) continue;

					CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID )->GetInfo();
					if(pFTDigimon != NULL)
					{
						std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pFTDigimon->s_dwModelID );
						cSprite* pkSlotIcon = NiNew cSprite;	// 1
						pkSlotIcon->Init( NULL, CsPoint::ZERO, kIncu_Inner_SLOT_SIZE, kPath.c_str(), false, NiColor::WHITE, false  );
						cString::sSPRITE* sSprite = pItem->AddSprite( pkSlotIcon, kIncu_Inner_SLOT_POS, kIncu_Inner_SLOT_SIZE  );	
						if( sSprite )
							sSprite->SetAutoPointerDelete(true);

						BT_INFO* pkbtinfo = new BT_INFO();
						pkbtinfo->BTTYPE = _SLOT;
						pkbtinfo->ParentIncuSlotIdx = iCount;
						sSprite->s_pSprite->SetUserData(pkbtinfo);
						/////////////////////////////////////
						// Name, LV
						/////////////////////////////////////
						wstring szStaticLV = UISTRING_TEXT("Digimon_Storage_DigimonLevel1").c_str();
						wstring szCurrLv = UISTRING_TEXT("Digimon_Storage_DigimonLevel2").c_str();
						DmCS::StringFn::ReplaceAll( szCurrLv, L"#DigimonLevel#", pkData->s_nLevel );

						wstring szFinalLV;
						DmCS::StringFn::Format( szFinalLV, L"%s %s", szStaticLV.c_str(), szCurrLv.c_str());

						cText::sTEXTINFO ti;
						ti.Init(CFont::FS_12 );
						ti.SetText(szFinalLV.c_str());
						ti.s_eTextAlign = DT_LEFT;
						pItem->AddText(&ti, kIncu_LV_TT_POS);

						ti.SetText(pkData->s_szName);
						pItem->AddText(&ti, kIncu_NAME_TT_POS);
						/////////////////////////////////////
						// Eliment
						/////////////////////////////////////
						UpdateElement(pItem, pFTDigimon);

					}
				}
			}
		}
		//성장 이미지
		if(pkInfo->GetisActivated() == true)
		{
			cSprite* pDigimonImage = NiNew cSprite;	// 1
			pDigimonImage->Init( NULL, CsPoint::ZERO, kIncu_Inner_SLOT_SIZE, "TacticsHouse\\Storage_growth.tga", false );
			cString::sSPRITE* sAddedDigiSprite = pItem->AddSprite( pDigimonImage, kIncu_Inner_FRAME_POS, kIncu_FRAME_SLOT_SIZE  );	
			if( sAddedDigiSprite )
				sAddedDigiSprite->SetAutoPointerDelete(true);
		}

		//아이템 이미지
		cSprite* pItemImage = NiNew cSprite;	// 1
		pItemImage->Init( NULL, CsPoint::ZERO, kIncu_Inner_ITEM_SIZE, "TacticsHouse\\Storage_slot.tga", false );
		BT_INFO* pkbtinfo = new BT_INFO();
		pkbtinfo->BTTYPE = _ITEM;
		pkbtinfo->ParentIncuSlotIdx = iCount;
		pItemImage->SetUserData(pkbtinfo);
		cString::sSPRITE* sAddedItemSprite = pItem->AddSprite( pItemImage, kIncu_Inner_ITEM_POS, kIncu_Inner_ITEM_SIZE );	
		if( sAddedItemSprite )
			sAddedItemSprite->SetAutoPointerDelete(true);
		if(pkInfo->GetReg_ItemType() > 0)
		{
			pItem->AddIcon(kIncu_Inner_ITEM_ICON_SIZE,ICONITEM::ITEM_ICON, pkInfo->GetReg_ItemType(),1, kIncu_Inner_ITEM_ICON_POS);
		}

		cButton* pAddBtn = NiNew cButton;
		if(pAddBtn)
		{
			pAddBtn->Init( NULL, CsPoint::ZERO, kIncu_ADD_BT_SIZE, "CommonUI\\Menu_btn.tga", false );
			pAddBtn->SetTexToken( CsPoint( 0, 54 ) );
			//pAddBtn->SetMouseOnMode(true);

			BT_INFO* pkbtinfo = new BT_INFO();
			pkbtinfo->BTTYPE = _START;
			pkbtinfo->ParentIncuSlotIdx = iCount;
			pAddBtn->SetUserData(pkbtinfo);

			pAddBtn->SetEnable(true);
			pAddBtn->SetColor(NiColor::WHITE);
			if(pkInfo->GetisActivated() == false)
			{
				if(pkInfo->GetReg_ItemType() <= 0 || pkInfo->GetReg_DigimonArchiveSlotID() <= 0)
				{
					pAddBtn->SetColor(NiColor(92.0f/ 255.0f, 108.0f/ 255.0f, 132.0f/ 255.0f));
					pAddBtn->SetEnable(false);
				}
			}
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );	
			if(pkInfo->GetisActivated() == true){
				ti.SetText(pkInfo->GetRemainTime().c_str());
			}
			else{
				ti.SetText( UISTRING_TEXT("Digimon_Storage_Incubator_BT").c_str() );
			}
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetText(&ti);

			cString::sBUTTON* pAddeBT = pItem->AddButton( pAddBtn, 0, kIncu_ADD_BT_POS, kIncu_ADD_BT_SIZE, CsPoint(65,13) );
			if( pAddeBT )
				pAddeBT->SetAutoPointerDelete(true);
		}
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(iCount, kGridSize);
		addItem->SetItem( pItem );
		INCU_INFO* pkIncuinfo = new INCU_INFO();
		pkIncuinfo->IncuSlotInfo = iCount;
		addItem->SetUserData(pkIncuinfo);
		m_pIncuListBox->AddItem( addItem );
	}
}
void	CDigimonIncubatorViewer::UpdateRemainTime(void)
{
	const CDigimonArchiveContents::IncuSlotInfo* pkIncuSlotInfos =GetSystem()->GetIncuSlotInfo();
	CDigimonArchiveContents::IncuSlotInfo::const_iterator kIter = pkIncuSlotInfos->begin();
	int CurrItemCnt = m_pIncuListBox->GetItemCount();
	for(int i = 0 ; i < CurrItemCnt; ++i)
	{
		const cGridListBoxItem* pkItem = m_pIncuListBox->GetItemFormPos(i);
		if(pkItem != NULL)
		{
			INCU_INFO* pkUserData = dynamic_cast<INCU_INFO*>(pkItem->GetUserData());
			if(pkUserData != NULL)
			{
				CDigimonArchiveContents::IncuSlotInfo::const_iterator kIter = pkIncuSlotInfos->find(pkUserData->IncuSlotInfo);
				if(kIter != pkIncuSlotInfos->end())
				{
					if( kIter->second->GetisActivated() == true)
					{
						cString* pkItems = pkItem->GetItem();
						if(pkItems != NULL)
						{
							for(int i =0 ; i < pkItems->GetListSize(); ++i)
							{
								cString::sBUTTON* pkButton = dynamic_cast< cString::sBUTTON* >(pkItems->GetElement(i));
								if(pkButton)
								{
									CDigimonIncubatorViewer::BT_INFO* pUserData = dynamic_cast<CDigimonIncubatorViewer::BT_INFO*>(pkButton->s_pButton->GetUserData());
									if(pUserData)
									{
										switch(pUserData->BTTYPE)
										{
										case BT_TYPE::_START://누르는 버튼이면
											{
												cText::sTEXTINFO ti;
												ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );	
												if(kIter->second->GetisActivated() == true){
													//시간이 남아있으면
													if(kIter->second->isRemainTime() == true)
													{
														ti.SetText(kIter->second->GetRemainTime().c_str());
													}
													else
													{
														ti.SetText(L"");
													}
												}
												else{
													ti.SetText( UISTRING_TEXT("Digimon_Storage_Incubator_BT").c_str() );
												}
												ti.s_eTextAlign = DT_CENTER;
												pkButton->s_pButton->SetText(&ti);

											}
											break;
										}
									}
								}
							}	
						}
					}
				}
			}
		}
	}
}
void	CDigimonIncubatorViewer::UpdateElement(cString* pkString, CsDigimon::sINFO* pFTDigimon)
{
	CsPoint kElementpos = CsPoint(290, 10);
	const CsPoint kElementSize = CsPoint(19,19);
	const CsPoint kElementW = CsPoint(19, 0);
	int nIndex;
	// - 디지몬 속성
	nIndex = pFTDigimon->s_eAttributeType - nsCsDigimonTable::AT_NONE;
	kElementpos = kElementpos - kElementW;
	pkString->AddIcon( kElementSize, ICONITEM::DAttribute, cIconMng::DAttribute, nIndex, kElementpos );
	// - 패밀리 속성
	for( int i=0; i<3; ++i )
	{
		if( pFTDigimon->s_eFamilyType[ i ] == 0 )
			continue;
		nIndex = pFTDigimon->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
		kElementpos = kElementpos - kElementW;
		pkString->AddIcon( kElementSize, ICONITEM::DAttribute, cIconMng::Family, nIndex, kElementpos );
	}
	// - 자연 속성
	if( pFTDigimon->s_eBaseNatureTypes[0] != 0 )
	{
		nIndex = pFTDigimon->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
		kElementpos = kElementpos - kElementW;
		pkString->AddIcon(kElementSize, ICONITEM::DAttribute, cIconMng::Nature, nIndex , kElementpos);	
	}
	// - 자연 속성
	if( pFTDigimon->s_eBaseNatureTypes[1] != 0 )
	{
		nIndex = pFTDigimon->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
		kElementpos = kElementpos - kElementW;
		pkString->AddIcon( kElementSize, ICONITEM::DAttribute, cIconMng::Nature, nIndex, kElementpos );	
	}
	// - 자연 속성
	if( pFTDigimon->s_eBaseNatureTypes[2] != 0 )
	{
		nIndex = pFTDigimon->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
		kElementpos = kElementpos - kElementW;
		pkString->AddIcon( kElementSize, ICONITEM::DAttribute, cIconMng::Nature, nIndex, kElementpos );	
	}
}
int		CDigimonIncubatorViewer::OnRecvIncubator(int DigimonSlotIdx, int ItemIdx, int RemainTime)
{
	return 0;
}
int		CDigimonIncubatorViewer::OnRecvFacilitation( u1 nListItemIndexIncuList, u4 nRemainTime )
{
	if( CURSOR_ST.IsIcon() )
		CURSOR_ST.ReleaseIcon();

	cGridListBoxItem* pkListItem = const_cast<cGridListBoxItem*>(m_pIncuListBox->GetItemFormPos(nListItemIndexIncuList));
	if(pkListItem != NULL)
	{
		cString* pkItem = pkListItem->GetItem();
		for(int i =0 ; i < pkItem->GetListSize(); ++i)
		{
			cString::sBUTTON* pkButton = dynamic_cast< cString::sBUTTON* >(pkItem->GetElement(i));
			if(pkButton)
			{
				CDigimonIncubatorViewer::BT_INFO* pUserData = dynamic_cast<CDigimonIncubatorViewer::BT_INFO*>(pkButton->s_pButton->GetUserData());
				if(pUserData)
				{
					switch(pUserData->BTTYPE)
					{
					case BT_TYPE::_ITEM:
						break;
					}
					//데이터 수신중 메세지
					cMessageBox::DelMsg( 10019, false );
				}
			}
		}	
	}
	return 0;
}


void CDigimonIncubatorViewer::OnMouseUpOnGridList(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	CsPoint ptMouseLocalPos = CURSOR_ST.GetPos();//MousePosToWindowPos( CURSOR_ST.GetPos() );//
	std::list< cString::sELEMENT* >::iterator elemnetList = pItem->GetItem()->Get_sTextList();
	int listSize = pItem->GetItem()->GetListSize();
	for(int i = 0; i < listSize; ++i, ++elemnetList)
	{
		cString::sELEMENT::eTYPE tempType = (*elemnetList)->GetType();
		if(tempType == cString::sELEMENT::SPRITE)
		{
			cString::sSPRITE* sprite = (cString::sSPRITE*) (*elemnetList);
			BT_INFO* pkInfo = static_cast< BT_INFO* >(sprite->s_pSprite->GetUserData());
			if(pkInfo != NULL )
			{
				CsPoint spritePt = sprite->s_pSprite->GetTempPos();
				CsPoint size = spritePt + sprite->s_ptSize;
				CsRect spriteRect(spritePt, size);
				bool bVisible = sprite->s_pSprite->GetVisible();
				if( bVisible == true && spriteRect.PtInRect(ptMouseLocalPos))
				{
					OnDragUpOnSprite(pItem, sprite);
					return;
				}
			}
		}
	}
}

void CDigimonIncubatorViewer::OnMouseDownOnItem(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->ResetDragInfo();
	CsPoint ptMouseLocalPos = CURSOR_ST.GetPos();//MousePosToWindowPos( CURSOR_ST.GetPos() );//
	std::list< cString::sELEMENT* >::iterator elemnetList = pItem->GetItem()->Get_sTextList();
	int listSize = pItem->GetItem()->GetListSize();
	for(int i = 0; i < listSize; ++i, ++elemnetList)
	{
		cString::sELEMENT::eTYPE tempType = (*elemnetList)->GetType();
		if(tempType == cString::sELEMENT::SPRITE)
		{
			cString::sSPRITE* sprite = (cString::sSPRITE*) (*elemnetList);
			BT_INFO* pkInfo = static_cast< BT_INFO* >(sprite->s_pSprite->GetUserData());
			if(pkInfo != NULL )
			{
				CsPoint spritePt = sprite->s_pSprite->GetTempPos();
				CsPoint size = spritePt + sprite->s_ptSize;
				CsRect spriteRect(spritePt, size);
				bool bVisible = sprite->s_pSprite->GetVisible();
				if( bVisible == true && spriteRect.PtInRect(ptMouseLocalPos))
				{
					OnMouseDownOnSprite(pItem, sprite);
					return;
				}
			}
		}
	}
}
void  CDigimonIncubatorViewer::OnMouseMoveOnItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	CsPoint ptMouseLocalPos = CURSOR_ST.GetPos();//MousePosToWindowPos( CURSOR_ST.GetPos() );//
	std::list< cString::sELEMENT* >::iterator elemnetList = pItem->GetItem()->Get_sTextList();
	int listSize = pItem->GetItem()->GetListSize();
	for(int i = 0; i < listSize; ++i, ++elemnetList)
	{
		cString::sELEMENT::eTYPE tempType = (*elemnetList)->GetType();
		if(tempType == cString::sELEMENT::SPRITE)
		{
			cString::sSPRITE* sprite = (cString::sSPRITE*) (*elemnetList);
			if(sprite)
			{
				BT_INFO* pkInfo = static_cast< BT_INFO* >(sprite->s_pSprite->GetUserData());
				if(pkInfo != NULL )
				{
					CsPoint spritePt = sprite->s_pSprite->GetTempPos();
					CsPoint size = spritePt + sprite->s_ptSize;
					CsRect spriteRect(spritePt, size);
					bool bVisible = sprite->s_pSprite->GetVisible();
					if( bVisible == true && spriteRect.PtInRect(ptMouseLocalPos))
					{
						OnMouseMoveOnSprite(pItem, sprite);
						return;
					}
				}
			}
		}
	}
}
void	CDigimonIncubatorViewer::OnMouseDownOnSprite(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pSender);
	if(pItem->GetVisible()== false)
		return;
	GetSystem()->ResetDragInfo();
	SAFE_POINTER_RET(pData);
	cString::sSPRITE* pSprite = static_cast< cString::sSPRITE* >(pData);
	SAFE_POINTER_RET(pSprite->s_pSprite);

	BT_INFO* pkInfo = static_cast< BT_INFO* >(pSprite->s_pSprite->GetUserData());
	if(pkInfo )
	{
		if(pkInfo->BTTYPE == BT_TYPE::_SLOT)
		{
			if(GetSystem()->GetNowDrag())
			{
				CDigimonArchiveContents::IncuSlotInfo* pkSlotInfo = const_cast<CDigimonArchiveContents::IncuSlotInfo*>(GetSystem()->GetIncuSlotInfo());
				CDigimonArchiveContents::IncuSlotInfo::iterator kIter = pkSlotInfo->find(pItem->getID());
				int RegArchiveSlotIdx = kIter->second->GetReg_DigimonArchiveSlotID();
				if( RegArchiveSlotIdx> 0)
				{
					cData_PostLoad::sDATA* pkData =  GetSystem()->GetDigimonDataInArchive(RegArchiveSlotIdx);
					if(pkData != NULL && pkData->s_dwBaseDigimonID != 0)
					{
						if(  nsCsFileTable::g_pDigimonMng->GetDigimon(pkData->s_dwBaseDigimonID) != NULL)
						{
							CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(pkData->s_dwBaseDigimonID)->GetInfo();
							if(pFTDigimon != NULL)
							{
								sCHAR_IMAGE* pkCharImage = g_pCharResMng->CharImageResLoad( pFTDigimon->s_dwModelID );
								if(pkCharImage != NULL)
								{
									CURSOR_ST.SetIcon( CURSOR_ICON::CI_TACTICSHOUSE, 1, pItem->getID(), NULL );
									CURSOR_ST.SetIconSubInfo( (int)pkCharImage, 0, 0 );
									GetSystem()->ResetDragInfo();
									GetSystem()->SetDragInfo(CDigimonArchiveContents::SRC_INCU, pItem->getID());
									GetSystem()->SetNowDrag(true);//
								}
								g_pCharResMng->CharImageResDelete(&pkCharImage);
							}
						}
					}
				}
			}
			else
			{

				GetSystem()->SetNowDrag(true);//m_bisDragable = true;
				CURSOR_ST.SetButtonState(CURSOR::eTYPE::BUTTON_NONE);
			}
		}
	}
}
void	CDigimonIncubatorViewer::OnMouseMoveOnSprite(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pkSender);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pkSender);
	if(pItem->GetVisible()== false)
		return;
	SAFE_POINTER_RET(pData);
	cString::sSPRITE* pSprite = static_cast< cString::sSPRITE* >(pData);
	SAFE_POINTER_RET(pSprite->s_pSprite);
	BT_INFO* pkInfo = dynamic_cast< BT_INFO* >(pSprite->s_pSprite->GetUserData());
	if(pkInfo )
	{
		if(pkInfo->BTTYPE == BT_TYPE::_SLOT)
			GetSystem()->OnShowToolTip(CDigimonArchiveContents::INCUBATING_DIGIMON, pItem->getID());
		else if(pkInfo->BTTYPE == BT_TYPE::_ITEM)
			GetSystem()->OnShowToolTip(CDigimonArchiveContents::INCUBATING_ITEM, pItem->getID());
	}
}

void	CDigimonIncubatorViewer::ReleaseSelect(void)
{
	if(m_pIncuListBox != NULL)
		m_pIncuListBox->releaseSelection();
}