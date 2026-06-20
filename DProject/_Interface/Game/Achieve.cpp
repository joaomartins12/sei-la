
#include "stdafx.h"
#include "Achieve.h"

cAchieve::cAchieve()
{ 
	m_pCancelButton			= NULL;
	m_pCheckAllAchieve		= NULL;
	m_pDispTitleApplyBtn	= NULL;
	m_pAchieveCategoty		= NULL;
	m_pAchieveListBox		= NULL;
	m_pScrollBar			= NULL;
	sSprite					= NULL;

	m_DispTitleSprList.clear();

	m_nTotalPoint = 0;
	m_nMainSel = 0;
	m_nDispTitleSel = 0;
	m_nDispTitleQuest = 0;
}

cAchieve::~cAchieve()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

bool cAchieve::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	return true;
}

void cAchieve::Destroy()
{
	cBaseWindow::Delete();
}

void cAchieve::DeleteResource()
{
	DeleteScript();
}

void cAchieve::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Achieve\\Win.tga", CsPoint( 150, 100 ), CsPoint( 562, 527 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "COMMON_TXT_ACHIEVEMENT" ).c_str() );	

	m_pCancelButton = AddButton( CsPoint( 540, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cAchieve::PressCancelBtn);

	m_pCheckAllAchieve = AddButton( CsPoint( 11, 484 ), CsPoint( 165, 25 ), cButton::IMAGE_NOR_15, UISTRING_TEXT( "ACHIEVE_BTN_CHECK_ACQUIRED_ACHIEVEMENTS" ).c_str() );	
	m_pCheckAllAchieve->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cAchieve::PressCheckAllAchieveBtn);

	m_pDispTitleApplyBtn = AddButton( CsPoint( 438, 475 ), CsPoint( 70, 25 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "ACHIEVE_BTN_APPLICATION" ).c_str() );
	m_pDispTitleApplyBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cAchieve::PressTitleApplyBtn);
	m_pDispTitleApplyBtn->SetEnable(false);
	m_pDispTitleApplyBtn->SetVisible(false);	

	m_pAchieveCategoty = NiNew cTreeBox;
	m_pAchieveCategoty->Init( GetRoot(),  CsPoint(12, 70), CsPoint( 163, 440 ), NULL, false );
	m_pAchieveCategoty->setChildItemGab(CsPoint(0,10));
	m_pAchieveCategoty->AddEvent( cTreeBox::TreeEvent_Selection_Changed, this, &cAchieve::SelectedCategoty );
	AddChildControl(m_pAchieveCategoty);	

	m_pScrollBar = NiNew cScrollBar;	
	m_pScrollBar->Init( cScrollBar::TYPE_1, NULL, CsPoint( 533, 66 ), CsPoint( 16, 444 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 183, 66 ), CsPoint( 548, 511 ) ), 4, false );

	m_pAchieveListBox = NiNew cListBox;
	m_pAchieveListBox->Init( m_pRoot, CsPoint(190, 70), CsPoint( 355, 444 ), NULL, false );		
	m_pAchieveListBox->SetScrollBar(m_pScrollBar);

	AddChildControl( m_pAchieveListBox );

	CreateTitle();	
	MakeCategoty();
}

void cAchieve::Update(float const& fDeltaTime)
{	
	bool bCheckEnable = GetSystem()->IsEnableCheckAll();

	if(m_pCheckAllAchieve->IsEnable() != bCheckEnable)
	{
		m_pCheckAllAchieve->SetEnable( bCheckEnable );
	}
	
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cAchieve::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pCheckAllAchieve && m_pCheckAllAchieve->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pDispTitleApplyBtn && m_pDispTitleApplyBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if( m_pAchieveCategoty && m_pAchieveCategoty->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		return muReturn;
	}

	if( m_pAchieveListBox && m_pAchieveListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{	
		if(m_nMainSel != 4)	//업적
		{
			cListBoxItem const* pOverItem = m_pAchieveListBox->GetMouseOverItem();
			SAFE_POINTER_RETVAL(pOverItem, muReturn);

			sDispTitleSeq* pUserData = dynamic_cast<sDispTitleSeq*>(pOverItem->GetUserData());

			DisplayToolTip(false, pUserData->m_nQuestID);
		}
		else //획득타이틀
		{
			cListBoxItem const* pOverItem = m_pAchieveListBox->GetMouseOverItem();
			SAFE_POINTER_RETVAL(pOverItem, muReturn);

			sDispTitleSeq* pUserData = dynamic_cast<sDispTitleSeq*>(pOverItem->GetUserData());
			DisplayToolTip(true, pUserData->m_nQuestID);			

			cListBoxItem const* pDownItem = m_pAchieveListBox->GetMouseDownItem();
			SAFE_POINTER_RETVAL(pDownItem, muReturn);

			SelectDispTitle(pUserData->m_nSeq, pUserData->m_nQuestID);	
		}

		return muReturn;
	}

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 562, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
	{
		_UpdateMoveWindow_ForMouse();
	}

	return muReturn;
}

void cAchieve::Render()
{
	RenderScript();	
}

void cAchieve::ResetDevice()
{	
	ResetDeviceScript();
}

void cAchieve::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();	
}

void cAchieve::CreateTitle()
{
	CsPoint ptCompCnt = CsPoint( 11, 39 );
	CsPoint ptAchievePoint = CsPoint( 410, 39 );

	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eFontSize = CFont::FS_12;
	ti.s_eTextAlign = DT_LEFT;

	int nCompCnt = (int)GetSystem()->GetCompMap()->size();
	int nProcCnt = (int)GetSystem()->GetProcList()->size();
	std::wstring wsCount;
	DmCS::StringFn::Format( wsCount, L"%s  %.3d/%.3d", UISTRING_TEXT( "ACHIEVE_ACHIEVEMENT_COUNT" ).c_str(), nCompCnt, nCompCnt + nProcCnt );
	ti.SetText( wsCount.c_str() );
	AddText( &ti, ptCompCnt );

	AchieveContents::MAP_COMP_CIT itComp	  = GetSystem()->GetCompMap()->begin();
	AchieveContents::MAP_COMP_CIT itCompEnd = GetSystem()->GetCompMap()->end();

	for( ; itComp!=itCompEnd; ++itComp )
	{
		CsAchieve* CompMap = itComp->second;

		if(CompMap != NULL)
		{
			m_nTotalPoint += CompMap->GetInfo()->s_nPoint;
		}
	}

	std::wstring wsScore;
	DmCS::StringFn::Format( wsScore, L"%s  %.4d", UISTRING_TEXT( "ACHIEVE_ACHIEVEMENT_SCORE" ).c_str(), m_nTotalPoint );
	ti.SetText( wsScore.c_str() );
	AddText( &ti, ptAchievePoint );
}

void cAchieve::MakeCategoty()
{
	//////////////////////////////////////////////////////////////////////////
	//업적
	//////////////////////////////////////////////////////////////////////////
	CsAchieveMng::sTYPE* pType = nsCsFileTable::g_pAchieveMng->GetRootType();
	std::list< CsAchieveMng::sTYPE* >::iterator it = pType->s_listChild.begin();
	std::list< CsAchieveMng::sTYPE* >::iterator itEnd = pType->s_listChild.end();

	int mainIdx = 0;
	int subIdx = 0;

	for( ; it!=itEnd; ++it )
	{
		mainIdx++;

		cTreeBoxItem* pAddItem = NiNew cTreeBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 163, 22), "Item_Make\\Make_Slot_BTn1.tga", false );
		pButton->SetTexToken(CsPoint(0,22));
		pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 163, 22) );

		cText::sTEXTINFO tiGroup;
		tiGroup.Init( CFont::FS_10, NiColor::WHITE );
		tiGroup.SetText((*it)->s_Info.s_szType);
		pItem->AddText( &tiGroup, CsPoint(25, 5) );

		pAddItem->SetItem( pItem );	

		cButton* pOpenBtn = NiNew cButton;
		SAFE_POINTER_BEK( pOpenBtn );
		pOpenBtn->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14), "Item_Make\\Make_Slot_Plus.tga", false );
		pOpenBtn->SetTexToken(CsPoint(0,14));
		pAddItem->SetOpenButton( pOpenBtn, CsPoint(5, 4) );

		cButton* pCloseBtn = NiNew cButton;
		SAFE_POINTER_BEK( pCloseBtn );
		pCloseBtn->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14), "Item_Make\\Make_Slot_Minus.tga", false );
		pCloseBtn->SetTexToken(CsPoint(0,14));
		pAddItem->SetCloseButton( pCloseBtn, CsPoint(5, 4) );

		std::list< CsAchieveMng::sTYPE* >::iterator itChild	   = (*it)->s_listChild.begin();
		std::list< CsAchieveMng::sTYPE* >::iterator itChildEnd = (*it)->s_listChild.end();

		subIdx = 0;
		for( ; itChild!=itChildEnd; ++itChild )
		{
			CsAchieveMng::sTYPE* info = (*itChild);
			if(info == NULL)
				continue;

			subIdx++;

			cTreeBoxItem* pAddSubItem = NiNew cTreeBoxItem;
			SAFE_POINTER_BEK( pAddSubItem );

			cString * pSubItem = NiNew cString;
			SAFE_POINTER_BEK( pSubItem );

			cButton* pButton = NiNew cButton;
			SAFE_POINTER_BEK( pButton );
			pButton->Init( NULL, CsPoint::ZERO, CsPoint( 163, 22), "Item_Make\\Make_Slot_BTn2.tga", false );
			pButton->SetTexToken(CsPoint(0,22));
			pSubItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 163, 22) );

			tiGroup.Init( CFont::FS_10, NiColor::WHITE );
			tiGroup.SetText( info->s_Info.s_szType );
			pSubItem->AddText( &tiGroup, CsPoint(35, 5) );

			pAddSubItem->SetItem( pSubItem );			
			pAddSubItem->SetUserData( new sCategotyType( mainIdx, subIdx ) );
			pAddItem->AddChildItem( pAddSubItem );
		}
		m_pAchieveCategoty->AddItem( pAddItem );
	}
	//////////////////////////////////////////////////////////////////////////
	//획득타이틀
	//////////////////////////////////////////////////////////////////////////
	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem;
	cString * pItem = NiNew cString;
	cButton* pButton = NiNew cButton;

	pButton->Init( NULL, CsPoint::ZERO, CsPoint( 163, 22), "Item_Make\\Make_Slot_BTn1.tga", false );
	pButton->SetTexToken(CsPoint(0,22));
	pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 163, 22) );

	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_10, NiColor::WHITE );
	tiGroup.SetText( UISTRING_TEXT( "ACHIEVE_ACQUIRED_TITLE" ).c_str() );
	pItem->AddText( &tiGroup, CsPoint(25, 5) );	
	pAddItem->SetItem( pItem );
	pAddItem->SetUserData( new sCategotyType( 4, 0 ) );

	m_pAchieveCategoty->AddItem( pAddItem );	
}

void cAchieve::SetGroup(int mainIdx, int subIdx)	
{
	m_pAchieveListBox->SetSize( CsPoint( 355, 444 ) );
	m_pAchieveListBox->ResetDevice();

	AchieveContents::MAP_COMP_CIT itComp = GetSystem()->GetCompMap()->begin();
	AchieveContents::MAP_COMP_CIT itCompEnd = GetSystem()->GetCompMap()->end();

	for( ; itComp!=itCompEnd; ++itComp )
	{
		CsAchieve* CompMap = itComp->second;

		SAFE_POINTER_CON( CompMap );
		CsAchieve::sINFO* pAchiveInfo = CompMap->GetInfo();
		SAFE_POINTER_CON( pAchiveInfo );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cListBoxItem * addItem  = NiNew cListBoxItem;
		SAFE_POINTER_BEK( addItem );

		if(pAchiveInfo->s_nGroup == mainIdx && pAchiveInfo->s_nSubGroup == subIdx)
		{
			cSprite* m_pAchieveBG = NiNew cSprite;
			m_pAchieveBG->Init( NULL, CsPoint::ZERO, CsPoint( 330, 103 ), "Achieve\\AchieveBG.tga", false );

			sSprite = pItem->AddSprite(m_pAchieveBG);
			SAFE_POINTER_RET(sSprite);
			sSprite->SetAutoPointerDelete(true);

			cStringList kTitle;
			cText::sTEXTINFO tiTemp;
			tiTemp.Init( CFont::FS_10 );
			g_pStringAnalysis->Cut_Parcing( &kTitle, 1024, pAchiveInfo->s_szTitle, &tiTemp );

			NiColor AchiveColor;
			std::wstring wsTitle;
			std::list< cString* >* pkList = kTitle.GetList();
			if( !pkList->empty() )
			{
				std::list< cString* >::iterator kIter = pkList->begin();
				cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
				AchiveColor = pText->s_Text.GetColor();
				wsTitle = pText->s_Text.GetText();
			}
			else
				wsTitle = pAchiveInfo->s_szTitle;

			int szTitleSize = wsTitle.size();

			if( szTitleSize > 0 )
			{
				cSprite* m_pDispTitleShortBG = NiNew cSprite;
				m_pDispTitleShortBG->Init( NULL, CsPoint( 8, 72 ), CsPoint( 245, 26 ), "Achieve\\DispTitleBG.tga", false );
				sSprite = pItem->AddSprite(m_pDispTitleShortBG);		
				SAFE_POINTER_RET(sSprite);
				sSprite->SetAutoPointerDelete(true);
			}		

			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_14 );
			pItem->AddText(&ti, CsPoint( 8, 10 ));

			if( szTitleSize > 0 )
			{
				ti.Init( CFont::FS_10, AchiveColor );		
				ti.SetText( wsTitle.c_str() );
				int x = 8 + (245/2) - (ti.GetTextWidth()/2);	//중앙정렬 245(m_pDispTitleShortBG 너비)
				pItem->AddText(&ti, CsPoint( x, 78 ));
			}		

			ti.Init( CFont::FS_10 );
			ti.SetText(NULL);
			cString::sTEXT* string = pItem->AddText(&ti, CsPoint( 8, 35 ));
			string->SetText(pAchiveInfo->s_szComment, 240);	

			pItem->AddIcon(CsPoint( 30, 30 ), ICONITEM::NUMBER, pAchiveInfo->s_nPoint, 0, CsPoint( 286, 36 ));

			if(szTitleSize > 0)
			{
				if( pAchiveInfo->s_nIcon <= 255 )
					pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve, pAchiveInfo->s_nIcon, 0, CsPoint( 21, 78 ) );
				else if( pAchiveInfo->s_nIcon <= 555 )
					pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve2, pAchiveInfo->s_nIcon - 300, 0, CsPoint( 21, 78 ) );	// Achieve2는 아이콘 번호 300부터 시작
				else
					pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve3, pAchiveInfo->s_nIcon - 556, 0, CsPoint( 21, 78 ) );
			}

			addItem->Delete();
			addItem->SetItem( pItem );
			addItem->SetUserData( new sDispTitleSeq( 0, pAchiveInfo->s_nQuestID ) );
			m_pAchieveListBox->AddItem( addItem );		
		}	
	}

	AchieveContents::LIST_CIT	  pProcList = GetSystem()->GetProcList();
	AchieveContents::MAP_LIST_CIT itProc = pProcList->begin();
	AchieveContents::MAP_LIST_CIT itProcEnd = pProcList->end();
	for( ; itProc!=itProcEnd; ++itProc )
	{
		CsAchieve* ProcList = (*itProc);
		SAFE_POINTER_CON( ProcList );
		CsAchieve::sINFO* pAchiveInfo = ProcList->GetInfo();
		SAFE_POINTER_CON( pAchiveInfo );

		if( !pAchiveInfo->s_bDisplay )
			continue;

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cListBoxItem * addItem  = NiNew cListBoxItem;
		SAFE_POINTER_BEK( addItem );

		if(pAchiveInfo->s_nGroup == mainIdx && pAchiveInfo->s_nSubGroup == subIdx)
		{
			cSprite* m_pAchieveBG = NiNew cSprite;
			m_pAchieveBG->Init( NULL, CsPoint::ZERO, CsPoint( 330, 103 ), "Achieve\\AchieveBG.tga", false );

			sSprite = pItem->AddSprite(m_pAchieveBG);
			SAFE_POINTER_RET(sSprite);
			sSprite->SetAutoPointerDelete(true);

			cStringList kTitle;
			cText::sTEXTINFO tiTemp;
			tiTemp.Init( CFont::FS_10 );
			g_pStringAnalysis->Cut_Parcing( &kTitle, 1024, pAchiveInfo->s_szTitle, &tiTemp );

			NiColor AchiveColor;
			std::wstring wsTitle;
			std::list< cString* >* pkList = kTitle.GetList();
			if( !pkList->empty() )
			{
				std::list< cString* >::iterator kIter = pkList->begin();
				cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
				AchiveColor = pText->s_Text.GetColor();
				wsTitle = pText->s_Text.GetText();
			}
			else
				wsTitle = pAchiveInfo->s_szTitle;

			int szTitleSize = wsTitle.size();

			if( szTitleSize > 0 )
			{
				cSprite* m_pDispTitleShortBG = NiNew cSprite;
				m_pDispTitleShortBG->Init( NULL, CsPoint( 8, 72 ), CsPoint( 245, 26 ), "Achieve\\DispTitleBG.tga", false );
				sSprite = pItem->AddSprite(m_pDispTitleShortBG);
				SAFE_POINTER_RET(sSprite);
				sSprite->SetAutoPointerDelete(true);
			}		

			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_14 );
			ti.SetText( pAchiveInfo->s_szName );
			pItem->AddText(&ti, CsPoint( 8, 10 ));

			if( szTitleSize > 0 )
			{
				ti.Init( CFont::FS_10, AchiveColor );		
				ti.SetText( wsTitle.c_str() );
				int x = 8 + (245/2) - (ti.GetTextWidth()/2);	//중앙정렬 245(m_pDispTitleShortBG 너비)
				pItem->AddText(&ti, CsPoint( x, 78 ));	
			}			

			ti.Init( CFont::FS_10 );
			ti.SetText(NULL);
			cString::sTEXT* string = pItem->AddText(&ti, CsPoint( 8, 35 ));
			string->SetText(pAchiveInfo->s_szComment, 240);	

			cSprite* m_pAchieveMask = NiNew cSprite;
			m_pAchieveMask->Init( NULL, CsPoint::ZERO, CsPoint( 330, 103 ), "Achieve\\AchieveMask.tga", false );
			sSprite = pItem->AddSprite(m_pAchieveMask);
			SAFE_POINTER_RET(sSprite);
			sSprite->SetAutoPointerDelete(true);

			pItem->AddIcon(CsPoint( 30, 30 ), ICONITEM::NUMBER, pAchiveInfo->s_nPoint, 0, CsPoint( 286, 36 ));

			if( szTitleSize > 0 )
			{
				if( pAchiveInfo->s_nIcon <= 255 )
					pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve, pAchiveInfo->s_nIcon, 0, CsPoint( 21, 78 ) );
				else if( pAchiveInfo->s_nIcon <= 555 )
					pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve2, pAchiveInfo->s_nIcon - 300, 0, CsPoint( 21, 78 ) );	// Achieve2는 아이콘 번호 300부터 시작
				else
					pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve3, pAchiveInfo->s_nIcon - 556, 0, CsPoint( 21, 78 ) );
			}			

			addItem->Delete();
			addItem->SetItem( pItem );	
			addItem->SetUserData( new sDispTitleSeq( 0, pAchiveInfo->s_nQuestID ) );
			m_pAchieveListBox->AddItem( addItem );	
		}
	}

	m_pAchieveListBox->changeAllItemHeight(110);

	m_pAchieveCategoty->SetSelectMode(mainIdx - 1, subIdx - 1);
}

void cAchieve::SetDispTitle()
{
	m_DispTitleSprList.clear();

	m_pAchieveListBox->SetSize( CsPoint( 355, 392 ) );
	m_pAchieveListBox->ResetDevice();
	//////////////////////////////////////////////////////////////////////////
	// 적용하지 않음
	cString * pItem = NiNew cString;
	cListBoxItem * addItem  = NiNew cListBoxItem;

	cSprite* m_pDispTitleBG_0 = NiNew cSprite;
	m_pDispTitleBG_0->Init( NULL, CsPoint::ZERO, CsPoint( 325, 30 ), "Achieve\\DispTitleBG.tga", false );
	sSprite = pItem->AddSprite(m_pDispTitleBG_0);
	SAFE_POINTER_RET(sSprite);
	sSprite->SetAutoPointerDelete(true);

	cSprite* m_pDispTitleMask_Sel_0 = NiNew cSprite;
	m_pDispTitleMask_Sel_0->Init( NULL, CsPoint::ZERO, CsPoint( 325, 30 ), "Achieve\\DispTitleMask.tga", false );
	m_pDispTitleMask_Sel_0->SetVisible(false);
	sSprite = pItem->AddSprite(m_pDispTitleMask_Sel_0);
	SAFE_POINTER_RET(sSprite);
	sSprite->SetAutoPointerDelete(true);

	if(GetSystem()->GetDispTitle() == 0)
	{
		m_pDispTitleMask_Sel_0->SetVisible(true);
	}

	m_DispTitleSprList.push_back(m_pDispTitleMask_Sel_0);

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_11 );
	ti.SetText( UISTRING_TEXT( "ACHIEVE_NOT_APPLY" ).c_str() );
	int x = (325/2) - (ti.GetTextWidth()/2);	
	pItem->AddText(&ti, CsPoint( x, 7 ));	

	addItem->SetItem(pItem);
	addItem->SetUserData( new sDispTitleSeq(0,0) );

	m_pAchieveListBox->AddItem( addItem );	

	///////////////////////////////////////////////////////////////////////////////	
	AchieveContents::COMP_CIT	  pCompMap = GetSystem()->GetCompMap();
	AchieveContents::MAP_COMP_CIT itComp = pCompMap->begin();
	AchieveContents::MAP_COMP_CIT itCompEnd = pCompMap->end();

	int count = 0;

	for( ; itComp!=itCompEnd; ++itComp )
	{
		CsAchieve* CompMap = itComp->second;
		SAFE_POINTER_CON( CompMap );

		CsAchieve::sINFO* pAchiveInfo = CompMap->GetInfo();
		SAFE_POINTER_CON( pAchiveInfo );

		if( pAchiveInfo->s_szTitle[ 0 ] == NULL )
			continue;

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cListBoxItem * addItem  = NiNew cListBoxItem;
		SAFE_POINTER_BEK( addItem );

		count++;

		cSprite* m_pDispTitleBG = NiNew cSprite;
		m_pDispTitleBG->Init( NULL, CsPoint::ZERO, CsPoint( 325, 30 ), "Achieve\\DispTitleBG.tga", false );
		sSprite = pItem->AddSprite(m_pDispTitleBG);	
		SAFE_POINTER_RET(sSprite);
		sSprite->SetAutoPointerDelete(true);

		cSprite* m_pDispTitleMask_Sel = NiNew cSprite;
		m_pDispTitleMask_Sel->Init( NULL, CsPoint::ZERO, CsPoint( 325, 30 ), "Achieve\\DispTitleMask.tga", false );
		m_pDispTitleMask_Sel->SetVisible(false);
		sSprite = pItem->AddSprite(m_pDispTitleMask_Sel);
		SAFE_POINTER_RET(sSprite);
		sSprite->SetAutoPointerDelete(true);

		if(GetSystem()->GetDispTitle() == pAchiveInfo->s_nQuestID)
		{
			m_pDispTitleMask_Sel->SetVisible(true);		
			m_nDispTitleSel = count;
		}

		m_DispTitleSprList.push_back(m_pDispTitleMask_Sel);


		cStringList kTitle;
		cText::sTEXTINFO tiTemp;
		tiTemp.Init( CFont::FS_11 );
		g_pStringAnalysis->Cut_Parcing( &kTitle, 1024, pAchiveInfo->s_szTitle, &tiTemp );

		NiColor AchiveColor;
		std::wstring wsTitle;
		std::list< cString* >* pkList = kTitle.GetList();
		if( !pkList->empty() )
		{
			std::list< cString* >::iterator kIter = pkList->begin();
			cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
			AchiveColor = pText->s_Text.GetColor();
			wsTitle = pText->s_Text.GetText();
		}
		else
			wsTitle = pAchiveInfo->s_szTitle;

		ti.SetText( wsTitle.c_str() );
		ti.s_Color = AchiveColor;
		int x = (325/2) - (ti.GetTextWidth()/2);	
		pItem->AddText(&ti, CsPoint( x, 7 ));

		if( pAchiveInfo->s_nIcon <= 255 )
			pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve, pAchiveInfo->s_nIcon, 0, CsPoint( 21, 6 ) );
		else if( pAchiveInfo->s_nIcon <= 555 )
			pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve2, pAchiveInfo->s_nIcon - 300, 0, CsPoint( 21, 6 ) );	// Achieve2는 아이콘 번호 300부터 시작
		else
			pItem->AddIcon( CsPoint( 18, 18 ), ICON_Dynamic::Achieve3, pAchiveInfo->s_nIcon - 556, 0, CsPoint( 21, 6 ) );

		addItem->SetItem(pItem);		
		addItem->SetUserData( new sDispTitleSeq(count, pAchiveInfo->s_nQuestID) );

		m_pAchieveListBox->AddItem( addItem );			
	}

	m_pAchieveListBox->changeAllItemHeight(30);
	m_pAchieveListBox->configureScrollbars();
}

void cAchieve::SelectDispTitle(int seq, int questID)
{
	std::list<cSprite*>::const_iterator it		= m_DispTitleSprList.begin();
	std::list<cSprite*>::const_iterator itEnd	= m_DispTitleSprList.end();

	int count = 0;

	if(seq != m_nDispTitleSel)
	{
		for( ; it != itEnd ; ++it )
		{
			cSprite* pSprite = (*it);

			if(pSprite == NULL)
				continue;

			if(seq == count)
			{
				m_nDispTitleSel = count;
				m_nDispTitleQuest = questID;

				if(GetSystem()->GetDispTitle() != questID)
				{
					m_pDispTitleApplyBtn->SetEnable(true);
				}
				else
				{
					m_pDispTitleApplyBtn->SetEnable(false);
				}

				pSprite->SetVisible(true);
			}
			else
			{
				pSprite->SetVisible(false);
			}
			count++;
		}					
	}	
}

void cAchieve::DisplayToolTip(bool dispTitle, int questID)
{
	AchieveContents::COMP_CIT	  pCompMap = GetSystem()->GetCompMap();
	AchieveContents::MAP_COMP_CIT itComp = pCompMap->begin();
	AchieveContents::MAP_COMP_CIT itCompEnd = pCompMap->end();

	int count = 0;

	for( ; itComp!=itCompEnd; ++itComp )
	{
		CsAchieve* CompMap = itComp->second;

		if(CompMap == NULL)
			continue;		

		if( CompMap->GetInfo()->s_szTitle[0] == 0 )
			continue;

		if(questID == CompMap->GetInfo()->s_nQuestID)
		{
			RenderToolTip(CompMap->GetInfo());
			break;
		}
	}

	if(dispTitle)	//획득타이틀일경우 조건미완료까지 갈필요없음
		return;

	AchieveContents::LIST_CIT	  pProcList = GetSystem()->GetProcList();
	AchieveContents::MAP_LIST_CIT itProc = pProcList->begin();
	AchieveContents::MAP_LIST_CIT itProcEnd = pProcList->end();
	for( ; itProc!=itProcEnd; ++itProc )
	{
		CsAchieve* ProcList = (*itProc);

		if(ProcList == NULL)
			continue;
		
		if( ProcList->GetInfo()->s_bDisplay == false )
			continue;
		
		if(questID == ProcList->GetInfo()->s_nQuestID)
		{
			RenderToolTip(ProcList->GetInfo());
			break;
		}		
	}
}

void cAchieve::RenderToolTip(CsAchieve::sINFO* pFTInfo)
{
	uint nBuffID = pFTInfo->s_nBuffCode;

	if( nBuffID != 0 )
	{
		cBuffData::sBUFF_DATA pBuffData;
		pBuffData.s_pBuffTable = nsCsFileTable::g_pBuffMng->GetBuff(nBuffID);
		pBuffData.s_nLifeTime = 0;
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 24, 24 ), 300, cTooltip::BUFFINFO,	IF_BUFF_ACHIEVE, 0, 0, 0, pFTInfo );
	}
	else
	{
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 15, 0 ), pFTInfo->s_szName );
	}
}

void cAchieve::PressCancelBtn( void* pSender, void* pData )
{
	Close();
}

void cAchieve::PressCheckAllAchieveBtn( void* pSender, void* pData )
{
	GetSystem()->CheckAllAchieve();
}

void cAchieve::PressTitleApplyBtn( void* pSender, void* pData )
{
	if( g_pCharMng->GetTamerUser()->IsServerDie() == false )
	{
		if( net::game )
			net::game->SendTamerAchievement( (u2)m_nDispTitleQuest );				

		Close();
	}
	else
	{
		cPrintMsg::PrintMsg( 20050 );
	}
}

void cAchieve::SelectedCategoty(void* pSender, void* pData)
{
	cTreeBoxItem* pItem = static_cast<cTreeBoxItem*>(pData);
	sCategotyType* pUserData = dynamic_cast<sCategotyType*>(pItem->GetUserData());
	SAFE_POINTER_RET( pUserData );

	m_pAchieveListBox->RemoveAllItem();

	m_nMainSel = pUserData->m_nMainIdx;

	if(pUserData->m_nMainIdx != 4)	//업적
	{
		m_pDispTitleApplyBtn->SetVisible(false);
		SetGroup(pUserData->m_nMainIdx, pUserData->m_nSubIdx);
	}
	else							//획득타이틀
	{
		m_pDispTitleApplyBtn->SetVisible(true);
		m_pDispTitleApplyBtn->SetEnable(false);
		SetDispTitle();
	}
}