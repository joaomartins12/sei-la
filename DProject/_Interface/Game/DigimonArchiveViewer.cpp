
#include "stdafx.h"
#include "DigimonArchiveViewer.h"
#include "DigimonInfoViewer.h"
#include "DigimonIncubatorViewer.h"
#include "DigimonTranscendenceViewer.h"

CDigimonArchiveViewer::CDigimonArchiveViewer()
:
mpOpenedNPC(NULL),
mOpenedPos(NiPoint2::ZERO),
m_pkFilterCB(NULL),
m_pkSortingCB(NULL),
m_pMagnifierBT(NULL),
m_pResetSerchingBT(NULL),
m_pCloseBT(NULL),
m_pTabMenu(NULL),
m_pkCurrViewer(NULL),
m_pkInfoViewer(NULL),
m_pkIncuViewer(NULL),
m_pkGridArchive(NULL),
mSlotSize(CsPoint::ZERO),
m_pEditSerchingDigimon(NULL),
mTTDigimonCntInArchive(NULL),
mTTDigimonOpenedCnt(NULL),
m_bisDragable(false)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_INCUBATOR_ITEM_IN_INVENTORY, this, &CDigimonArchiveViewer::OnRClick_Inventory_Item);	
}
CDigimonArchiveViewer::~CDigimonArchiveViewer()
{
	int NoUseParameter = 0;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_HIDE_VIEWER, &NoUseParameter);
	GAME_EVENT_ST.DeleteEventAll( this );
}
void CDigimonArchiveViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::Initailze_Archive:
		{
			RecvInitializeArchive();
		}
		break;
	case SystemType::Update_Viewer:
		{
			RecvUpdateArchive();
		}
		break;
	case SystemType::Selected_Digimon:
		{
			int SelectedPart = 0;
			kStream>>SelectedPart;
			RecvChangeSelectedDigimon(SelectedPart);
		}
		break;
	case SystemType::Initailze_InfoViewer:
		{
			if( m_pkInfoViewer != NULL)
				m_pkInfoViewer->UpdateViewer();
		}
		break;
	case SystemType::OnShow_IncuViewer:
		{
			if(m_pkCurrViewer != NULL)
				m_pkCurrViewer->SetVisible(false);
			
			m_pkCurrViewer = m_pkIncuViewer;
			if( m_pkCurrViewer )
			{
				m_pkCurrViewer->SetVisible(true);
				m_pkCurrViewer->UpdateViewer();
			}
			RecvUpdateArchive();
		}
		break;
	case SystemType::OnShow_TranscendViewer:
		{
			if(m_pkCurrViewer != NULL)
				m_pkCurrViewer->SetVisible(false);

			m_pkCurrViewer = m_pkTranscendViewer;
			if( m_pkCurrViewer )
			{
				m_pkCurrViewer->SetVisible(true);
				m_pkCurrViewer->UpdateViewer();
			}
			RecvUpdateArchive();
		}
		break;
	case SystemType::Update_IncuViewer:
		{
			RecvUpdateArchive();
		}
		break;
	case SystemType::Update_TranscendViewer:
		{
			RecvUpdateArchive();
		}
		break;
	case SystemType::OnShow_ByItem:
		{	
			UpdateOpenPos();
		}
		break;
	case SystemType::OnShow_ByNPC:
		{
			UpdateOpenNPC();
		}	
		break;
	case SystemType::Set_Window_Lock:
		{
			bool bLock;
			kStream >> bLock;
			SetEnableWindow(bLock);
		}
		break;
	}
}
bool CDigimonArchiveViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::Initailze_Archive, this );
	GetSystem()->Register( SystemType::Update_Viewer, this );
	GetSystem()->Register( SystemType::Selected_Digimon, this );
	GetSystem()->Register( SystemType::Initailze_InfoViewer, this );
	GetSystem()->Register( SystemType::OnShow_IncuViewer, this );
	
	GetSystem()->Register( SystemType::Update_IncuViewer, this );	
	GetSystem()->Register( SystemType::Update_TranscendViewer, this );

	GetSystem()->Register( SystemType::OnShow_ByItem, this );
	GetSystem()->Register( SystemType::OnShow_ByNPC, this );
	GetSystem()->Register( SystemType::OnShow_TranscendViewer, this );
	GetSystem()->Register( SystemType::Set_Window_Lock, this );
	return true;
}

void CDigimonArchiveViewer::Destroy()
{
	cBaseWindow::Delete();
	if(GetSystem() != NULL)
	{
		GetSystem()->UnRegisterAll( this );
		GetSystem()->UnLockRegisteredItem();
		GetSystem()->ResetDragInfo();
	}
}

void CDigimonArchiveViewer::DeleteResource()
{
	DeleteScript();
	if( m_pEditSerchingDigimon )
		m_pEditSerchingDigimon->ReleaseFocus();
}
void CDigimonArchiveViewer::Create( int nValue)
{
	cBaseWindow::Init();
	InitScript( "TacticsHouse\\Storage_popup.tga", CsPoint( 24, 104 ) , CsPoint( 850, 614 ), true , IFREGION_X::CENTER, IFREGION_Y::CENTER, false);
	AddTitle( UISTRING_TEXT("Digimon_Storage_Name").c_str(), CsPoint(0,12), CFont::FS_16 );
	mSlotSize =  CsPoint( 54, 54);
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Create Digimon GridList
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_pkGridArchive = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pkGridArchive );

	m_pkGridArchive->Init( GetRoot(), CsPoint(395, 232),  CsPoint(395, 350), CsPoint::ZERO, mSlotSize, cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_SELECTED_ITEM, this, &CDigimonArchiveViewer::OnItemSelectedInDigimonList );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &CDigimonArchiveViewer::OnItemClickedInDigimonList );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &CDigimonArchiveViewer::OnMouseDownOnItem );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_MOUSE_UP_ON_GRIDLIST, this, &CDigimonArchiveViewer::OnMouseUpOnGridList );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonArchiveViewer::OnMouseMoveOnItem );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &CDigimonArchiveViewer::OnMouseRClickInArchive );
	m_pkGridArchive->AddEvent( cGridListBox::GRID_MOUSE_LEAVE, this, &CDigimonArchiveViewer::OnMouseLeaveFormArchive );
	m_pkGridArchive->SetMouseOverImg( "Lobby\\CharacterCreate\\digimon_glow.tga" );
	m_pkGridArchive->SetSelectedImg( "TacticsHouse\\Storage_select.tga" );
	m_pkGridArchive->SetBackOverAndSelectedImgRender(false);
	m_pkGridArchive->SetUsePressedMoveEvent(true);
	AddChildControl(m_pkGridArchive);

	cScrollBar* pScrollBar = NiNew cScrollBar;
	if(pScrollBar)
	{
		pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint::ZERO, CsPoint(16,390), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint(16,390) ), 2, false );
		m_pkGridArchive->SetScrollBar( pScrollBar );
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init Info Texts
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_14, NiColor(0.0f,0.7176f,0.53f) );
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( L"0/");//가지고 있는 디지몬의 수
	mTTDigimonCntInArchive = AddText( &ti, CsPoint(730, 560) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	mDefaultColor_OpenedSlotCnt = NiColor(0.0f,0.7176f,0.53f);
	ti.Init(CFont::FS_14, mDefaultColor_OpenedSlotCnt );
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( L"0");//가지고 있는 디지몬의 수
	mTTDigimonOpenedCnt = AddText( &ti, CsPoint(760, 560) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Create Close BUTTON
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_pCloseBT = AddButton( CsPoint(806, 12), CsPoint(32,32), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Create SHOW INFO BUTTON
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_pTabMenu = AddRadioButton( CsPoint(22,64), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
	if(m_pTabMenu)
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("Digimon_Storage_Memu1").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pTabMenu->SetCheckBoxText( &ti );
		m_pTabMenu->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &CDigimonArchiveViewer::OnClickMenuTab );
		m_pTabMenu->SetUserData( new sTabType(eInfoViewer ) );

		cRadioButton* pAddBtn = m_pTabMenu->AddNode( CsPoint(188,64), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
		if( pAddBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText(UISTRING_TEXT("Digimon_Storage_Memu2").c_str());
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetCheckBoxText( &ti );
			pAddBtn->SetEnable(true);
			pAddBtn->SetUserData( new sTabType( eIncubator ) );
		}

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pAddBtn = m_pTabMenu->AddNode( CsPoint(354,64), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
		if( pAddBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText( UISTRING_TEXT( "Digimon_Storage_Menu3" ).c_str() );
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetCheckBoxText( &ti );
			pAddBtn->SetEnable(true);
			pAddBtn->SetUserData( new sTabType( eTranscend ) );
		}
#endif
		
		m_pTabMenu->SetCheckIndex(0);
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Create Serching Edit Box
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AddSprite(CsPoint( 394, 141 ), CsPoint( 405, 25 ), "TacticsHouse\\Storage_search.tga");

	// ID 에딧박스	
	ti.Init( &g_pEngine->m_FontText, CFont::FS_14, NiColor::WHITE );
	ti.SetText( _T( "" ) );
	ti.s_bOutLine = false;
	m_pEditSerchingDigimon = NiNew cEditBox;
	m_pEditSerchingDigimon->Init( GetRoot(), CsPoint( 398, 144 ), CsPoint(400,20 ), &ti, false );
	m_pEditSerchingDigimon->SetEmptyMsgText( UISTRING_TEXT("Digimon_Storage_Serching_Message").c_str(), NiColor(0.14f,0.35f,0.6f) );
	m_pEditSerchingDigimon->SetFontLength( 20 );
	m_pEditSerchingDigimon->EnableUnderline( ti.s_Color );
	m_pEditSerchingDigimon->SetEnableSound( true );
	m_pEditSerchingDigimon->AddEvent( cEditBox::eEditbox_ChangeText, this, &CDigimonArchiveViewer::OnChangeSechingEditBox );
	m_pEditSerchingDigimon->AddEvent( cEditBox::eEditbox_SetFocus, this, &CDigimonArchiveViewer::OnEnterFocusSechingEditBox );
	m_pEditSerchingDigimon->AddEvent( cEditBox::eEditbox_ReleaseFocus, this, &CDigimonArchiveViewer::OnReleaseFocusSechingEditBox );
	AddChildControl( m_pEditSerchingDigimon );

	m_pMagnifierBT = AddButton( CsPoint( 774, 144 ), CsPoint( 20, 20 ), CsPoint( 0, 21 ), "TacticsHouse\\Storage_Magnifier.tga" );		
	m_pMagnifierBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonArchiveViewer::OnClickSerching);

	m_pResetSerchingBT = AddButton( CsPoint( 774, 144 ), CsPoint( 20, 20 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );
	m_pResetSerchingBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonArchiveViewer::OnClickResetSerching);
	m_pResetSerchingBT->SetVisible(false);
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init ComboBox
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	MakeFilterComboBox();
	MakeSorterComboBox();
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Create Viewer
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_pkIncuViewer = NiNew CDigimonIncubatorViewer;
	m_pkIncuViewer->Create(GetRoot());
	AddChildScriptUI( m_pkIncuViewer );

	m_pkInfoViewer = NiNew CDigimonInfoViewer;
	m_pkInfoViewer->Create(GetRoot());
	m_pkInfoViewer->SetVisible(false);
	AddChildScriptUI( m_pkInfoViewer );

	m_pkTranscendViewer = NiNew CDigimonTanscendenceViewer;
	m_pkTranscendViewer->Create(GetRoot());
	m_pkTranscendViewer->SetVisible(false);
	AddChildScriptUI( m_pkTranscendViewer );

	m_pkCurrViewer = m_pkInfoViewer;
	m_pkCurrViewer->UpdateScript(0.0f);
	m_pkCurrViewer->RenderScript();

	GetSystem()->UnLockRegisteredItem();
	GetSystem()->ResetKeyword();
}

BOOL CDigimonArchiveViewer::UpdateMouse()
{
	return TRUE;
}

bool CDigimonArchiveViewer::IsIncubatorModeOn(void)
{
	if(m_pkCurrViewer == m_pkIncuViewer)
		return true;
	return false;
}

bool CDigimonArchiveViewer::UpdateAutoClose(void)
{
	//NPC등과 거리측정정
	if( mpOpenedNPC != NULL ){
		// ZoomNpc
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_TACTICSHOUSE );	
		assert_cs( mpOpenedNPC->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)mpOpenedNPC )->CheckTamerDist() == false ){
			SetEnableWindow( true );
			GetSystem()->Req_CloseViewer(NULL);
			return false;
		}
	}
	else{
		if(mOpenedPos != NiPoint2::ZERO){
			float fCheckDist = IF_TARGET_TAMER_MAX_DIST*2.0;
			if( ( mOpenedPos - g_pCharMng->GetTamerUser()->GetPos2D() ).Length() > fCheckDist ){
				SetEnableWindow( true );
				GetSystem()->Req_CloseViewer(NULL);
				return false;
			}
		}
	}
	return true;

}

void CDigimonArchiveViewer::Update(float const& fDeltaTime)
{
	if( UpdateAutoClose() == true){
		UpdateScript(fDeltaTime);
		_UpdateMoveWindow();
		if(m_pkCurrViewer)
			m_pkCurrViewer->Update(fDeltaTime);
	}
}

void CDigimonArchiveViewer::Render(void)
{
	RenderScript();
	if(m_pkCurrViewer)
		m_pkCurrViewer->Render();
}

cBaseWindow::eMU_TYPE CDigimonArchiveViewer::Update_ForMouse(void)
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( m_pTabMenu && m_pTabMenu->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;
	if(m_pkFilterCB && m_pkFilterCB->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return muReturn;
	if(m_pkSortingCB && m_pkSortingCB->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return muReturn;
	if(m_pkGridArchive && m_pkGridArchive->Update_ForMouse(CURSOR_ST.GetPos()) )
		return MUT_NONE;
	if(m_pkCurrViewer && m_pkCurrViewer->UpdateMouse() == TRUE)
		return MUT_NONE;
	if( m_pResetSerchingBT && m_pResetSerchingBT->Update_ForMouse() == cButton::ACTION_CLICK )
		return MUT_NONE;
	//----------------------------------------------------------------------------------------------
	//Update Cancel Bt	
	//----------------------------------------------------------------------------------------------
	switch( m_pCloseBT->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		GetSystem()->Req_CloseViewer(NULL);
		return muReturn;
	default:
		return muReturn;
	}
	// 아이템 갯수 입력창 활성화시. 엔터키 or ESC키 누르면 활성화 해제.
	if(m_pEditSerchingDigimon != NULL)
	{
		if( GLOBALINPUT_ST.IsEscape() && m_pEditSerchingDigimon->IsFocus() ){
			std::wstring kKeyword = L"";
			m_pEditSerchingDigimon->SetText(L"", false);
			GetSystem()->OnClick_SetFilteringKeyword(&kKeyword);
			m_pEditSerchingDigimon->ReleaseFocus(false);
		}
		else if( m_pEditSerchingDigimon->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_NONE ){
			if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
				m_pEditSerchingDigimon->ReleaseFocus(false);
		}
	}

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 1024, 48 ) ) ) != CURSOR::BUTTON_OUTWINDOW ){
		_UpdateMoveWindow_ForMouse();
	}
	return muReturn;
}

void	CDigimonArchiveViewer::RecvInitializeArchive(void)
{
	if(m_pkGridArchive == NULL)
		return;
	if(m_pkInfoViewer)
		m_pkInfoViewer->UpdateViewer();
	//==========================================================================================================
	// Digimon Slot In Archive 초기화
	//==========================================================================================================
	const int MaxSlotCnt = GetSystem()->GetMaxArchiveCount();
	const int OpenedCnt = GetSystem()->GetOpenedArchiveCount();
	int GridX = 6;
	int GridY = (MaxSlotCnt / GridX) + 1;
	const int StartPosX = 445;
	const int StartPosY = 232;
	const int SlotW = mSlotSize.x;
	const int SlotH = mSlotSize.y;
	CsPoint kSlotPos;
	CsPoint kSlotSize = CsPoint(SlotW, SlotH);

	std::wstring szKey;
	for(int y = 0; y < GridY; ++y)
	{
		for(int x= 0 ; x< GridX; ++x)
		{
			int iIdx = x + y * GridX;
			if(iIdx >= MaxSlotCnt)
				continue;

			kSlotPos.x = StartPosX + x * SlotW;
			kSlotPos.y = StartPosY + y * SlotH;

			cString * pItem = NiNew cString;
			cSprite* pImage = NiNew cSprite;	// 1
			pImage->Init( NULL, CsPoint::ZERO, kSlotSize, "TacticsHouse\\Storage_slot.tga", false );
			cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, mSlotSize  );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);

			if(iIdx >= OpenedCnt){
				cSprite* pImage = NiNew cSprite;	// 1
				pImage->Init( NULL, CsPoint::ZERO, CsPoint( 24, 34 ), "TacticsHouse\\Storage_lock.tga", false );
				cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint( 12, 7), CsPoint( 24, 34 )  );	
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}
			cGridListBoxItem * addItem  = NiNew cGridListBoxItem(iIdx, mSlotSize);
			addItem->SetItem( pItem );
			bool isOpened = true;
			if( iIdx >= GetSystem()->GetOpenedArchiveCount() )
			{
				isOpened = false;
			}
			addItem->SetUserData(new SlotInfo(iIdx, isOpened));
			m_pkGridArchive->AddItem( addItem );
		}
	}

	//==========================================================================================================
	// Digimon Slot In Archive 초기화
	//==========================================================================================================
	const CDigimonArchiveContents::Digimons* pkArchiveslotInfo = GetSystem()->GetDigimonsInArchive();
	if(pkArchiveslotInfo == NULL)
		return;

	CDigimonArchiveContents::DigimonsConstIter kIter = pkArchiveslotInfo->begin();
	for(; kIter != pkArchiveslotInfo->end(); ++kIter)
	{
		cGridListBoxItem* pkItem = const_cast<cGridListBoxItem*>(m_pkGridArchive->GetItemFormPos(kIter->first));
		const cData_PostLoad::sDATA* pkInfo = kIter->second->GetDigimonData();
		if(pkInfo)
		{
			DWORD kBaseDigimonID =pkInfo->s_dwBaseDigimonID;
			UpdateSlotIcon(kIter->second, pkItem);
		}
	}
	wstring kszBuffer =  UISTRING_TEXT("Digimon_Storage_SlotNumber").c_str();
	DmCS::StringFn::ReplaceAll( kszBuffer, L"#DigimonOpenedCount#", GetSystem()->GetDigimonCntInArchive());
	DmCS::StringFn::ReplaceAll( kszBuffer, L"#DigimonMaxCount#",  "");
	mTTDigimonCntInArchive->SetText(kszBuffer.c_str());

	kszBuffer = L"";//Max Cnt
	DmCS::StringFn::Format( kszBuffer, L"%d", OpenedCnt);
	mTTDigimonOpenedCnt->SetText(kszBuffer.c_str());
	if(OpenedCnt >= MaxSlotCnt)
		mTTDigimonOpenedCnt->SetColor(NiColor(1.0f, 0.1f, 0.1f));
	else
		mTTDigimonOpenedCnt->SetColor(mDefaultColor_OpenedSlotCnt);

	OnClickShowInfoViewer();
}

void CDigimonArchiveViewer::ResetSlotIcon(cGridListBoxItem* pkItem)
{
	if(pkItem != NULL)
	{
		if(pkItem->GetItem() != NULL)
		{
			pkItem->GetItem()->Delete();
			SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pkItem->GetUserData());
			if(pkSlotInfo != NULL)
			{
				pkSlotInfo->SetSlotIdx(0);
				int GetOpenedCnt = GetSystem()->GetOpenedArchiveCount();
				if(pkItem->getID() >= GetOpenedCnt)
					pkSlotInfo->isOpened = false;
				else
					pkSlotInfo->isOpened = true;

				cSprite* pImage = NiNew cSprite;	// 1
				pImage->Init( NULL, CsPoint::ZERO, mSlotSize, "TacticsHouse\\Storage_slot.tga", false );
				cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pImage, CsPoint::ZERO, mSlotSize  );	
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);

				if(pkSlotInfo->isOpened == false){
					cSprite* pImage = NiNew cSprite;	// 1
					pImage->Init( NULL, CsPoint::ZERO, CsPoint(24, 34), "TacticsHouse\\Storage_lock.tga", false );
					cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pImage, CsPoint( 15, 10 ), CsPoint( 24, 34)  );	
					if( sSprite )
						sSprite->SetAutoPointerDelete(true);
				}
			}
		}
	}
}

void CDigimonArchiveViewer::UpdateSlotIcon(CDigimonArchiveContents::DigimonInArchive* pkArchiveInfo, cGridListBoxItem* pkItem)
{	
	if(pkArchiveInfo && pkArchiveInfo->GetDigimonData() && pkArchiveInfo->GetDigimonData()->s_dwBaseDigimonID != 0)
	{
		CsPoint kInnerSlotSize =  CsPoint( 40, 40 );
		CsPoint kDelta = (mSlotSize - kInnerSlotSize) / 2;
		if(pkArchiveInfo->mpData != NULL && pkArchiveInfo->mpData->s_dwBaseDigimonID > 0)
		{
			CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pkArchiveInfo->mpData->s_dwBaseDigimonID );
			if(pkTableInfo != NULL &&  pkTableInfo->GetInfo() != NULL)
			{
				std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
				cSprite* pkSlotIcon = NiNew cSprite;	// 1
				pkSlotIcon->Init( NULL, CsPoint::ZERO, kInnerSlotSize, kPath.c_str(), false, NiColor::WHITE, false );
				cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pkSlotIcon, kDelta, kInnerSlotSize  );	
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}
			std::string kPath = GetLVToPath(pkArchiveInfo->mpData->s_nLevel);
			cSprite* pGrade = NiNew cSprite;	// 1
			pGrade->Init( NULL, CsPoint::ZERO, kInnerSlotSize, kPath.c_str(), false );
			cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pGrade, kDelta, kInnerSlotSize  );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);

			if(pkArchiveInfo->mbIsInIncubator )
			{
				cSprite* pIncuImage = NiNew cSprite;	// 1
				pIncuImage->Init( NULL, CsPoint::ZERO, kInnerSlotSize, "TacticsHouse\\Storage_growth.tga", false );
				cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pIncuImage, kDelta, kInnerSlotSize  );	
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}
			else if( pkArchiveInfo->mbIsInTranscend )
			{
				cSprite* pIncuImage = NiNew cSprite;	// 1
				pIncuImage->Init( NULL, CsPoint::ZERO, kInnerSlotSize, "TacticsHouse\\Storage_check.tga", false );
				cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pIncuImage, kDelta, kInnerSlotSize  );	
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}
		}
	}
}

std::string		CDigimonArchiveViewer::GetLVToPath(int iCurrLV)
{
	std::string kPath;
	if( iCurrLV < 20 )
		kPath = "TacticsHouse\\Storage_grade_01.tga";
	else if( iCurrLV < 40 )
		kPath = "TacticsHouse\\Storage_grade_02.tga";
	else if( iCurrLV < 60 )
		kPath = "TacticsHouse\\Storage_grade_03.tga";
	else if( iCurrLV < 80 )
		kPath = "TacticsHouse\\Storage_grade_04.tga";
	else
		kPath = "TacticsHouse\\Storage_grade_05.tga";
	return kPath;
}

void CDigimonArchiveViewer::UpdateOpenPos(void)
{
	mOpenedPos = GetSystem()->GetOpenedPos();
	mpOpenedNPC = NULL;
}

void CDigimonArchiveViewer::UpdateOpenNPC(void)
{
	mpOpenedNPC = GetSystem()->GetOpenedNPC();
	mOpenedPos = NiPoint2::ZERO;
}

void CDigimonArchiveViewer::RecvUpdateArchive(void)
{	
	if( m_pkGridArchive )
	{
		const CDigimonArchiveContents::Sorted* pkSortedList = GetSystem()->GetSortedDigimons();
		if(pkSortedList != NULL )
		{
			if(GetSystem()->GetFilterMode() == 0)
				ResetGridList(GetSystem()->GetMaxArchiveCount());
			else
				ResetGridList(pkSortedList->size());
		}
		m_pkGridArchive->VisibleSort();
	}

	if( mTTDigimonCntInArchive )
	{
		wstring kszBuffer =  UISTRING_TEXT("Digimon_Storage_SlotNumber").c_str();
		DmCS::StringFn::ReplaceAll( kszBuffer, L"#DigimonOpenedCount#", GetSystem()->GetDigimonCntInArchive());
		DmCS::StringFn::ReplaceAll( kszBuffer, L"#DigimonMaxCount#",  "");
		mTTDigimonCntInArchive->SetText(kszBuffer.c_str());
	}

	if( mTTDigimonOpenedCnt )
	{
		wstring kszBuffer =  UISTRING_TEXT("Digimon_Storage_SlotNumber").c_str();
		int OpenedCnt = GetSystem()->GetOpenedArchiveCount();//kszBuffer = L"";//Max Cnt
		DmCS::StringFn::Format( kszBuffer, L"%d", OpenedCnt);
		mTTDigimonOpenedCnt->SetText(kszBuffer.c_str());
		if(OpenedCnt >= GetSystem()->GetMaxArchiveCount())
			mTTDigimonOpenedCnt->SetColor(NiColor(1.0f, 0.1f, 0.1f));
		else
			mTTDigimonOpenedCnt->SetColor(mDefaultColor_OpenedSlotCnt);
	}

	if(m_pkCurrViewer)
		m_pkCurrViewer->UpdateViewer();
}

void CDigimonArchiveViewer::ResetGridList(int iViewerCnt)
{
	const CDigimonArchiveContents::Sorted* pkSortedList = GetSystem()->GetSortedDigimons();
	SAFE_POINTER_RET(pkSortedList);

	SAFE_POINTER_RET(m_pkGridArchive);
	std::list<cGridListBoxItem*>* pkAllList = m_pkGridArchive->GetListItem();
	SAFE_POINTER_RET(pkAllList);
	std::list<cGridListBoxItem*>::iterator kIter = pkAllList->begin();


	std::map<int, int> UsingSlotIdx;
	int AllCnt =0;
	for( ; kIter != pkAllList->end(); ++kIter, ++AllCnt)
	{
		ResetSlotIcon((*kIter));
		(*kIter)->SetVisible(false);
		UsingSlotIdx.insert(std::map<int, int>::value_type(AllCnt, AllCnt));
	}

	int CurrViewCount =0;
	if(GetSystem()->GetSortMode() == 0)
	{
		CDigimonArchiveContents::Sorted::const_iterator kSortedIter = pkSortedList->begin();
		for(;kSortedIter != pkSortedList->end(); ++kSortedIter)
		{
			cGridListBoxItem* pkItem =	 const_cast<cGridListBoxItem*>(m_pkGridArchive->GetItemFormPos(kSortedIter->second.iArchiveSlotIdx));
			if(pkItem != NULL){
				SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pkItem->GetUserData());
				pkSlotInfo->SetSlotIdx(kSortedIter->second.iArchiveSlotIdx);
				pkItem->SetVisible(true);

				CDigimonArchiveContents::DigimonInArchive* pkDigimonInfo = GetSystem()->GetArchiveSlotInfo(kSortedIter->second.iArchiveSlotIdx);

				UpdateSlotIcon(pkDigimonInfo,pkItem);
			}
		}
	}
	else
	{
		int CurrViewerCnt = 0;
		if(GetSystem()->GetSortMode() == CDigimonArchiveContents::EMPTY){
			CurrViewerCnt = GetSystem()->GetOpenedArchiveCount() - GetSystem()->GetDigimonCntInArchive();
		}
		CDigimonArchiveContents::Sorted::const_iterator kSortedIter = pkSortedList->begin();
		for(;kSortedIter != pkSortedList->end(); ++kSortedIter, ++CurrViewerCnt)
		{
			cGridListBoxItem* pkItem =	 const_cast<cGridListBoxItem*>(m_pkGridArchive->GetItemFormPos(CurrViewerCnt));
			if(pkItem != NULL){
				SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pkItem->GetUserData());
				pkSlotInfo->SetSlotIdx(kSortedIter->second.iArchiveSlotIdx);
				pkItem->SetVisible(true);

				CDigimonArchiveContents::DigimonInArchive* pkDigimonInfo = GetSystem()->GetArchiveSlotInfo(kSortedIter->second.iArchiveSlotIdx);
				UsingSlotIdx.erase(kSortedIter->second.iArchiveSlotIdx);
				UpdateSlotIcon(pkDigimonInfo,pkItem);
			}

		}
	}

	//빈슬롯을 보여줘야 하는 경우에
	if(GetSystem()->GetSerchingKeyword() == L"")
	{
		if(GetSystem()->GetFilterMode() == 0 )
		{
			if(GetSystem()->GetSortMode() != 0 )
			{
				int CurrViewCount =0;
				std::map<int, int>::iterator usingIdxIter = UsingSlotIdx.begin();
				kIter = pkAllList->begin();
				for( ; kIter != pkAllList->end(); ++kIter, ++CurrViewCount)
				{
					SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>((*kIter)->GetUserData());
					if(CurrViewCount < iViewerCnt)
					{
						if((*kIter)->GetVisible() == false)
						{
							int Usingidx = usingIdxIter->first;
							pkSlotInfo->SetSlotIdx(Usingidx);
							++usingIdxIter;
							(*kIter)->SetVisible(true);
						}
					}
				}
			}
			else if (GetSystem()->GetSortMode() == 0)
			{
				int CurrViewCount =0;
				kIter = pkAllList->begin();
				for( ; kIter != pkAllList->end(); ++kIter, ++CurrViewCount)
				{
					SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>((*kIter)->GetUserData());
					if(CurrViewCount < iViewerCnt)
					{
						pkSlotInfo->SetSlotIdx(CurrViewCount);
						(*kIter)->SetVisible(true);
					}
				}
			}
		}
	}
	m_pkGridArchive->releaseSelection();
	//GetSystem()->OnShowPartnerDigimonInfo();
}

void CDigimonArchiveViewer::OnLButtonDown( CsPoint pos )
{
	GetSystem()->ResetDragInfo();
}

bool CDigimonArchiveViewer::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	if(CURSOR_ST.IsIcon())
	{
		GetSystem()->SaveLastCursorInfo( CURSOR_ST.GetIconSlot(), CURSOR_ST.GetIconCount());
		CURSOR_ST.ReleaseIcon();
	}
	return true;
}

void CDigimonArchiveViewer::ResetDevice()
{	
	ResetDeviceScript();
}

void CDigimonArchiveViewer::RecvChangeSelectedDigimon(int SelectedPart)
{
	CDigimonArchiveContents::SELECT_PART eSelectedPart = (CDigimonArchiveContents::SELECT_PART)SelectedPart;
	switch(eSelectedPart)
	{
	case CDigimonArchiveContents::USING_DIGIMON:
		m_pkGridArchive->releaseSelection();
		m_pkInfoViewer->ReleaseSelect(eSelectedPart);
		m_pkTranscendViewer->ReleaseSelect( eSelectedPart );
		break;
	case CDigimonArchiveContents::ACCOMPANY_DIGIMON:
		m_pkGridArchive->releaseSelection();
		m_pkInfoViewer->ReleaseSelect(eSelectedPart);
		m_pkTranscendViewer->ReleaseSelect( eSelectedPart );
		break;
	case CDigimonArchiveContents::ARCHIVE_DIGIMOM:
		m_pkInfoViewer->ReleaseSelect();
	//	m_pkTranscendViewer->ReleaseSelect();
		break;
	case CDigimonArchiveContents::PART_END:
		m_pkGridArchive->releaseSelection();
		m_pkInfoViewer->ReleaseSelect();
		m_pkTranscendViewer->ReleaseSelect();
		break;
	default:
		break;
	}
	const cData_PostLoad::sDATA* pkSelectedDigimonInfo = GetSystem()->GetSelectedDigimon();
	if(m_pkCurrViewer)
		m_pkCurrViewer->OnChangeSelectedDigimon(pkSelectedDigimonInfo, eSelectedPart);
}	

void CDigimonArchiveViewer::OnItemSelectedInDigimonList(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
}

void CDigimonArchiveViewer::OnMouseDownOnItem(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pItem->GetUserData());
	if(pkSlotInfo->isOpened == false)
		return;
	int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
	if(GetSystem()->GetNowDrag())
	{
		DWORD TestModelID = GetSystem()->GetModelID(CurrSlotIdx);
		CDigimonArchiveContents::DigimonInArchive* pkSlotInfo = GetSystem()->GetArchiveSlotInfo(CurrSlotIdx);
		if(pkSlotInfo != NULL && false == pkSlotInfo->mbIsInIncubator && false == pkSlotInfo->mbIsInTranscend )
		{
			sCHAR_IMAGE* pkCharImage = g_pCharResMng->CharImageResLoad( TestModelID );
			if(pkCharImage != NULL)
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TACTICSHOUSE, 1, CurrSlotIdx, NULL );
				CURSOR_ST.SetIconSubInfo( (int)pkCharImage, 0, 0 );
				GetSystem()->ResetDragInfo();
				GetSystem()->SetDragInfo(CDigimonArchiveContents::SRC_ARCHIVE, CurrSlotIdx);
			}
			g_pCharResMng->CharImageResDelete(&pkCharImage);
		}
	}
	else
	{
		GetSystem()->SetNowDrag(true);
		//m_bisDragable = true;
		CURSOR_ST.SetButtonState(CURSOR::eTYPE::BUTTON_NONE);
	}
}

void CDigimonArchiveViewer::OnMouseUpOnGridList(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pItem->GetUserData());
	if(pkSlotInfo->isOpened == false)
	{
		GetSystem()->ResetDragInfo();
		return;
	}

	if( GetSystem()->GetNowDrag() == true)
	{
		int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
		GetSystem()->SetDragInfo(CDigimonArchiveContents::DEST_ARCHIVE, CurrSlotIdx);
		GetSystem()->ExcuteDragInfo();
	}
	GetSystem()->SetNowDrag(false);
}

void CDigimonArchiveViewer::OnMouseMoveOnItem(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkSlotInfo);
	if(pkSlotInfo->isOpened == false)
		return;
	int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
	GetSystem()->OnShowToolTip(CDigimonArchiveContents::ARCHIVE_DIGIMOM, CurrSlotIdx + 1000);
}

void CDigimonArchiveViewer::OnItemClickedInDigimonList(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkSlotInfo);
	if(pkSlotInfo->isOpened == false)
	{
		const int LockedSlotIdx = -1;
		GetSystem()->OnShowDigimonInfoInArchieve(LockedSlotIdx);
		return;
	}
	
	if( m_pkCurrViewer == m_pkTranscendViewer )
		return;
		
	int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
	GetSystem()->OnShowDigimonInfoInArchieve(CurrSlotIdx);
}

void CDigimonArchiveViewer::OnClickMenuTab(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);
	SAFE_POINTER_RET(pRadioButton);
	sTabType* userData = dynamic_cast<sTabType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);
	switch(userData->eViewerType)
	{
	case eTabType::eInfoViewer:	OnClickShowInfoViewer();	break;
	case eTabType::eIncubator:	OnClickShowIncuViewer();	break;
	case eTabType::eTranscend:	OnClickShowTranscendViewer();	break;
	default:
		break;
	}
	cWindow::PlaySound( cWindow::SD_At2 );
}

void CDigimonArchiveViewer::OnClickShowInfoViewer(void)
{
	if(m_pkCurrViewer != NULL)
		m_pkCurrViewer->SetVisible(false);
	m_pkCurrViewer = m_pkInfoViewer;
	m_pkCurrViewer->SetVisible(true);
	GetSystem()->Req_ShowArchiveViewer();
}

void CDigimonArchiveViewer::OnClickShowIncuViewer(void)
{
	GetSystem()->Req_ShowIncubatorViewer();
}

void CDigimonArchiveViewer::OnClickShowTranscendViewer(void)
{
	GetSystem()->Req_ShowTranscendViewer();
}

void	CDigimonArchiveViewer::OnClickChangeFilterMode(void* pSender, void* pData)
{
	if(m_pkFilterCB && GetSystem())
	{
		int CurrSelIdx = m_pkFilterCB->GetCurSel_Idx();
		GetSystem()->OnClick_SetFilteringMode(&CurrSelIdx);
	}
}
void	CDigimonArchiveViewer::OnClickChangeSorterMode(void* pSender, void* pData)
{
	if(m_pkSortingCB && GetSystem())
	{
		int CurrSelIdx = m_pkSortingCB->GetCurSel_Idx();
    		GetSystem()->OnClick_SetSortingMode(&CurrSelIdx);
	}
}
void	CDigimonArchiveViewer::MakeFilterComboBox(void)
{
	m_pkFilterCB = NiNew cComboBox;
	SAFE_POINTER_RET(m_pkFilterCB);
	m_pkFilterCB->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(394, 114), CsPoint( 190, 22 ), CFont::FS_10, DT_CENTER, false, false, false);
	m_pkFilterCB->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
	m_pkFilterCB->SetComboListWindow( "SealMaster\\dropdown_list_0" );

	std::wstring combotext = UISTRING_TEXT("Digimon_Storage_Aball");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_GREEN );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abno");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abda");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abva");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abvi");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abun");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );

	combotext =  UISTRING_TEXT("Digimon_Storage_Abice");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_AbWater");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abfire");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abearth");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abwind");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abwood");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Ablight");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abdark");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Abthun");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Absteel");
	m_pkFilterCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	m_pkFilterCB->SetCurSel( 0 );

	m_pkFilterCB->AddEvent(cComboBox::COMBO_SELECTITEM, this, &CDigimonArchiveViewer::OnClickChangeFilterMode);
	AddChildControl(m_pkFilterCB);
}
void	CDigimonArchiveViewer::MakeSorterComboBox(void)
{
	m_pkSortingCB = NiNew cComboBox;
	SAFE_POINTER_RET(m_pkSortingCB);
	m_pkSortingCB->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(608, 114), CsPoint( 190, 22 ), CFont::FS_10, DT_CENTER, false, false, false);
	m_pkSortingCB->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
	m_pkSortingCB->SetComboListWindow( "SealMaster\\dropdown_list_0" );

	std::wstring combotext = UISTRING_TEXT("Digimon_Storage_Range");
	m_pkSortingCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_GREEN );
	combotext =  UISTRING_TEXT("Digimon_Storage_Rangelow");
	m_pkSortingCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Rangehigh");
	m_pkSortingCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Rangeempty");
	m_pkSortingCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Rangetrust");
	m_pkSortingCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("Digimon_Storage_Rangename");
	m_pkSortingCB->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );

	m_pkSortingCB->SetCurSel( 0 );

	m_pkSortingCB->AddEvent(cComboBox::COMBO_SELECTITEM, this, &CDigimonArchiveViewer::OnClickChangeSorterMode);
	AddChildControl(m_pkSortingCB);

}

void	CDigimonArchiveViewer::OnClickSerching(void* pSender, void* pData)
{
	if(m_pEditSerchingDigimon && GetSystem())
	{
		std::wstring kKeyword = m_pEditSerchingDigimon->GetText()->GetText();
		GetSystem()->OnClick_SetFilteringKeyword(&kKeyword);
		m_pEditSerchingDigimon->GetText()->SetText(L"");
	}
}
void	CDigimonArchiveViewer::OnClickResetSerching(void* pSender, void* pData)
{
	if(m_pEditSerchingDigimon && GetSystem())
	{
		std::wstring kKeyword = L"";
		GetSystem()->OnClick_SetFilteringKeyword(&kKeyword);
		m_pEditSerchingDigimon->GetText()->SetText(L"");
		m_pEditSerchingDigimon->ReleaseFocus();
		m_pMagnifierBT->SetVisible(true);
		m_pResetSerchingBT->SetVisible(false);
	}

}

void	CDigimonArchiveViewer::OnMouseRClickInArchive(void* pkSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;
	SlotInfo* pkSlotInfo =	dynamic_cast<SlotInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkSlotInfo);
	//슬롯이 잠겨 있는데
	if(pkSlotInfo->isOpened == false)
	{
		int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
		GetSystem()->OnRClick_LockedArchiveSlot(CurrSlotIdx);
		
	}
	//슬롯이 열려 있는데
	else
	{
		//R 버튼 클릭했는데 현재 인큐베이터가 보여진다면
		if(m_pkCurrViewer == m_pkIncuViewer )
		{
			int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
			GetSystem()->OnRClick_Archive_Digimon_In_IncuMode(CurrSlotIdx);
		}
		//R 버튼 클릭했는데 현재 정보창이 보여진다면
		else if (m_pkCurrViewer == m_pkInfoViewer)
		{
			int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
			GetSystem()->OnRClick_Archive_Digimon_In_InfoMode(CurrSlotIdx);
		}
		//R 버튼 클릭했는데 현재 정보창이 보여진다면
		else if (m_pkCurrViewer == m_pkTranscendViewer)
		{
			int CurrSlotIdx = pkSlotInfo->GetSlotIdx();
			GetSystem()->OnRClick_Archive_Digimon_In_TranscendMode(CurrSlotIdx);
		}
	}
}
void	CDigimonArchiveViewer::OnMouseLeaveFormArchive(void* pkSender, void* pData)
{


}
bool CDigimonArchiveViewer::Close( bool bSound)
{
	return cBaseWindow::Close(bSound);

}
bool CDigimonArchiveViewer::OnEscapeKey()
{
	GetSystem()->Req_CloseViewer(NULL);
	return true;
}
void CDigimonArchiveViewer::OnChangeSechingEditBox(void* pkSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	std::wstring kKeyword = m_pEditSerchingDigimon->GetText()->GetText();
	if(kKeyword == L"")
	{
		m_pMagnifierBT->SetVisible(true);
		m_pResetSerchingBT->SetVisible(false);
	}	
	else
	{
		m_pMagnifierBT->SetVisible(false);
		m_pResetSerchingBT->SetVisible(true);
	}
	GetSystem()->OnClick_SetFilteringKeyword(&kKeyword);
}
void CDigimonArchiveViewer::OnEnterFocusSechingEditBox(void* pkSender, void* pData)
{
	SAFE_POINTER_RET( pData );
}
void CDigimonArchiveViewer::OnReleaseFocusSechingEditBox(void* pkSender, void* pData)
{
	SAFE_POINTER_RET( pData );
}
void CDigimonArchiveViewer::OnRClick_Inventory_Item(void* pkInvenSlotIdx)
{
	SAFE_POINTER_RET( pkInvenSlotIdx );
	int* pkRClickedInvenslotIdx = (int*)pkInvenSlotIdx;
	int RClickedInvenslotIdx = *pkRClickedInvenslotIdx;
	if( m_pkCurrViewer == m_pkIncuViewer && 
		RClickedInvenslotIdx >= 0)
	{
		GetSystem()->OnRClick_Inventory_Item(RClickedInvenslotIdx);
	}
}

// #pragma warning( default : 4311 )

