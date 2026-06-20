#include "stdafx.h"
#include "NewCashshopEquipViewer.h"

NewCashshopEquipViewer::NewCashshopEquipViewer()
:m_pkLeftEquipGrid(NULL),
m_pkRightEquipGrid(NULL),
m_pkResetEquipBT(NULL),
m_pkTakeOffAllEquipBT(NULL),
m_pkTurnLeftBT(NULL),
m_pkTurnRightBT(NULL),
m_fRot(0.0f),
m_pRenderTex(NULL)
{
	Construct();
}

NewCashshopEquipViewer::~NewCashshopEquipViewer()
{
	SAFE_NIDELETE( m_pRenderTex );
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	Destroy(); 
}
void NewCashshopEquipViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case NewCashshopContents::OnShowCashshop:
		InitEquipments();
		break;
	case NewCashshopContents::Upadte_Equipment:
		int partID = 0;
		DWORD ItemID = 0;
		kStream >> partID >> ItemID;
		UpdateEquipments( partID, ItemID );
		break;
	}
}

bool NewCashshopEquipViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::OnShowCashshop, this );
	GetSystem()->Register( SystemType::Upadte_Equipment, this );
	return true;
}

void NewCashshopEquipViewer::Destroy()
{
	m_mapEquipSlot.clear();
	DeleteResource();
}

void NewCashshopEquipViewer::DeleteResource()
{
	// 테이머 제거
	if( m_pTamer )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = (CsC_Object*)m_pTamer;
		g_pThread->DeleteChar( pUnit );
		m_pTamer = NULL;
	}		

	SAFE_NIDELETE( m_pRenderTex );
}

void NewCashshopEquipViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript( pkRoot, NULL, CsPoint(707,137), CsPoint(300,392), false );
//캐릭터 랜더
//////////////////////////////////////////////////////////////////
	m_pRenderTex = NiNew cRenderTex;
	if( m_pRenderTex->Init( CsPoint(300,392), "Cash\\NewCashShop\\Store_bg_char.tga"  ) == false )
		SAFE_NIDELETE( m_pRenderTex );

	// 미리 보기용 테이머 생성
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	m_pTamer = CTamer::NewInstance();	

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	prop.s_dwOptionPlag = _OP_ALL;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
	m_pTamer->Init( 0, pUser->GetFTID(), NiPoint3::ZERO, 0, _T( "" ), &prop, 0 );
	m_pTamer->GetProp_Alpha()->SetUseDistAlpha( false );
	//////////////////////////////////////////////////////////////////
	m_pkLeftEquipGrid = NiNew cGridListBox;
	if( m_pkLeftEquipGrid )
	{
		m_pkLeftEquipGrid->Init( m_pRoot, CsPoint(27,44), CsPoint(32,310), 
			CsPoint(0,12), CsPoint(30,32), cGridListBox::LowLeftDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pkLeftEquipGrid->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &NewCashshopEquipViewer::OnRClicked_EquipItem );
		m_pkLeftEquipGrid->SetUsePressedMoveEvent(true);
		m_pkLeftEquipGrid->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkLeftEquipGrid->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkLeftEquipGrid->SetBackOverAndSelectedImgRender(false);
		AddChildControl(m_pkLeftEquipGrid);
	}
	//////////////////////////////////////////////////////////////////
	m_pkRightEquipGrid = NiNew cGridListBox;
	if( m_pkRightEquipGrid )
	{
		m_pkRightEquipGrid->Init( m_pRoot, CsPoint(242,44), CsPoint(32,310), 
			CsPoint(0,12), CsPoint(32,32), cGridListBox::LowLeftDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pkRightEquipGrid->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &NewCashshopEquipViewer::OnRClicked_EquipItem );
		m_pkRightEquipGrid->SetUsePressedMoveEvent(true);
		m_pkRightEquipGrid->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkRightEquipGrid->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkRightEquipGrid->SetBackOverAndSelectedImgRender(false);
		AddChildControl(m_pkRightEquipGrid);
	}
	////////////////////////////////////////////////////////////////////
	m_pkTurnLeftBT = AddButton( CsPoint(70,200), CsPoint(30,34),CsPoint( 0, 34 ), "Cash\\NewCashShop\\Store_btn_Rotate_L.tga" ); 	
	m_pkTurnRightBT =  AddButton( CsPoint(200,200), CsPoint(30,34), CsPoint( 0, 34 ), "Cash\\NewCashShop\\Store_btn_Rotate_R.tga" );

	m_pkResetEquipBT =  AddButton( CsPoint(50,360), CsPoint(92,38), CsPoint( 0, 38 ), "Cash\\NewCashShop\\Store_btn_G1.tga" );
	if( m_pkResetEquipBT )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_EQUIPVIEWER_REST").c_str());//L"원래대로"
		ti.s_eTextAlign = DT_CENTER;	
		m_pkResetEquipBT->SetText( &ti );
		m_pkResetEquipBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopEquipViewer::OnClick_Reset);
	}

	m_pkTakeOffAllEquipBT =  AddButton( CsPoint(150,360), CsPoint(92,38), CsPoint( 0, 38 ), "Cash\\NewCashShop\\Store_btn_G1.tga" );
	if( m_pkTakeOffAllEquipBT )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_EQUIPVIEWER_TAKEOFFALL").c_str());//L"모두벗기"
		ti.s_eTextAlign = DT_CENTER;	
		m_pkTakeOffAllEquipBT->SetText( &ti );
		m_pkTakeOffAllEquipBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopEquipViewer::OnClick_TakeOffAll);
	}

	_CreateEquipSlot();
	InitEquipments();

	//2015-11-26-nova
	GET_SUBCAMERA(CAMERA_04)->SetRotation( -m_pTamer->GetDir() );
	GET_SUBCAMERA(CAMERA_04)->_UpdateCamera();
}

void NewCashshopEquipViewer::_CreateEquipSlot(void)
{
	m_mapEquipSlot.clear();
	cGridListBoxItem * addItem = NULL;
	if(m_pkLeftEquipGrid)
	{
		m_pkLeftEquipGrid->RemoveAllItem();
		if( m_pkLeftEquipGrid )
		{
			addItem = _EmptyEquipSlotItem(0, nTamer::Head, 0 );	
			m_pkLeftEquipGrid->AddItem( addItem );
			addItem = _EmptyEquipSlotItem(1, nTamer::Coat, 0 );
			m_pkLeftEquipGrid->AddItem( addItem );
			addItem = _EmptyEquipSlotItem(2, nTamer::Pants, 0 );
			m_pkLeftEquipGrid->AddItem( addItem );
			addItem = _EmptyEquipSlotItem(3, nTamer::Glove, 0 );
			m_pkLeftEquipGrid->AddItem( addItem );
			addItem = _EmptyEquipSlotItem(4, nTamer::Shoes, 0 );
			m_pkLeftEquipGrid->AddItem( addItem );
			addItem = _EmptyEquipSlotItem(5, nTamer::Costume, 0 );
			m_pkLeftEquipGrid->AddItem( addItem );
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
			addItem = _EmptyEquipSlotItem(6, nTamer::Bracelet, 0 );
			m_pkLeftEquipGrid->AddItem( addItem );
#endif
		}
	}

	if( m_pkRightEquipGrid )
	{
		m_pkRightEquipGrid->RemoveAllItem();
		addItem = _EmptyEquipSlotItem(0, nTamer::Ring, 0 );
		m_pkRightEquipGrid->AddItem( addItem );
		addItem = _EmptyEquipSlotItem(1, nTamer::Necklace, 0 );
		m_pkRightEquipGrid->AddItem( addItem );
		addItem = _EmptyEquipSlotItem(2, nTamer::Earring, 0 );
		m_pkRightEquipGrid->AddItem( addItem );
		addItem = _EmptyEquipSlotItem(3, nTamer::Glass, 0 );
		m_pkRightEquipGrid->AddItem( addItem );
		addItem = _EmptyEquipSlotItem(4, nTamer::EquipAura, 0 );
		m_pkRightEquipGrid->AddItem( addItem );
		addItem = _EmptyEquipSlotItem(5, nTamer::MaxParts, 0 );// 디지바이스
		m_pkRightEquipGrid->AddItem( addItem );
#ifdef SERVER_KSW_XEVOLUTION_1_180615
		addItem = _EmptyEquipSlotItem(6, nTamer::XAI, 0 );
		m_pkRightEquipGrid->AddItem( addItem );
#endif
	}
}

cGridListBoxItem* NewCashshopEquipViewer::_EmptyEquipSlotItem( int const& gridIndx, int const& nPartsID, DWORD const& dwItemID )
{
	cString * pItem = NiNew cString;
	cString::sICON* pIcon = pItem->AddIcon( CsPoint(32,32), ICONITEM::ARMOR_1, 0, 1 );
	if( pIcon )
		pIcon->SetAutoPointerDelete(true);

	if( 0 == dwItemID )
		pIcon->SetVisible(false);

	cGridListBoxItem * addItem  = NiNew cGridListBoxItem(gridIndx, CsPoint( 32, 32 ));
	addItem->SetItem( pItem );
	addItem->SetUserData( new EquipItemInfo( dwItemID, nPartsID ) );

	sEquipSlotData eSlotData;
	eSlotData.s_GridItem = addItem;
	eSlotData.s_SlotIcon = pIcon;
	m_mapEquipSlot.insert( std::make_pair( nPartsID, eSlotData ) );
	return addItem;
}

void NewCashshopEquipViewer::InitEquipments(void)
{
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
#else
	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT + 1 ];
#endif
	const NewCashshopContents::Equipments& pkEquipments = GetSystem()->GetEquipments_Origin();
	NewCashshopContents::EquipmentCIter kIter = pkEquipments.begin();
	for(; kIter != pkEquipments.end(); ++kIter)
	{
		CsC_PartObject::sCHANGE_PART_INFO* kEquipInfo = const_cast<CsC_PartObject::sCHANGE_PART_INFO*>(&kIter->second);
		cp[kEquipInfo->s_nPartIndex] = *kEquipInfo;

		_UpdateEquipSlotData( kEquipInfo->s_nPartIndex, kEquipInfo->s_nFileTableID );
	}
	m_pTamer->ThreadLoad_Part(cp);
}

void NewCashshopEquipViewer::_UpdateEquipSlotData( int const& nParts, DWORD const& dwItemId )
{
	std::map<int, sEquipSlotData>::iterator it = m_mapEquipSlot.find( nParts );
	if( it == m_mapEquipSlot.end() )
		return;

	if( it->second.s_GridItem )
	{
		EquipItemInfo* pItemInfo = dynamic_cast< EquipItemInfo* >(it->second.s_GridItem->GetUserData());
		if( pItemInfo )
			pItemInfo->itemid = dwItemId;
	}

	if( it->second.s_SlotIcon )
	{
		it->second.s_SlotIcon->ChangeIcon( ICONITEM::ARMOR_1, dwItemId, 1 );
		if( dwItemId > 0 )
			it->second.s_SlotIcon->SetVisible(true);
		else
			it->second.s_SlotIcon->SetVisible(false);
	}
}

void	NewCashshopEquipViewer::UpdateScript(float fDeltaTime)
{
	cScriptUI::UpdateScript( fDeltaTime );

	if( m_pTamer )
	{
		m_pTamer->Update( fDeltaTime );
		if( m_pTamer->IsLoad() && ( m_pTamer->GetViewSight() == false ) )
		{
			float fAniTime = m_pTamer->GetAniTime();
			m_pTamer->GetProp_Animation()->GetActorMng()->Update( fAniTime );
			m_pTamer->GetCsNode()->m_pNiNode->UpdateSelected( fAniTime );
			m_pTamer->UpdateVisible( fAniTime );
		}
	}
}

BOOL NewCashshopEquipViewer::UpdateMouse(void)
{
	if(IsVisible() == false)
		return FALSE;

	if(m_pkLeftEquipGrid && m_pkLeftEquipGrid->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverGrid = m_pkLeftEquipGrid->GetMouseOverItem();
		if(pOverGrid)
		{
			EquipItemInfo* pItemInfo = dynamic_cast< EquipItemInfo* >(pOverGrid->GetUserData());
			if( pItemInfo && pItemInfo->itemid > 0 )
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pOverGrid->GetWorldPos(), CsPoint(32,32), 360, cTooltip::ITEM, pItemInfo->itemid, cBaseWindow::WT_NEW_CASHSHOP);
		}
		return TRUE;
	}
	
	if(m_pkRightEquipGrid && m_pkRightEquipGrid->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverGrid = m_pkRightEquipGrid->GetMouseOverItem();
		if(pOverGrid)
		{
			EquipItemInfo* pItemInfo = dynamic_cast< EquipItemInfo* >(pOverGrid->GetUserData());
			if( pItemInfo && pItemInfo->itemid > 0)
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pOverGrid->GetWorldPos(), CsPoint(32,32), 360, cTooltip::ITEM, pItemInfo->itemid, cBaseWindow::WT_NEW_CASHSHOP);
		}
		return TRUE;
	}

	if(m_pkResetEquipBT && m_pkResetEquipBT->Update_ForMouse())
		return TRUE;
	if(m_pkTakeOffAllEquipBT && m_pkTakeOffAllEquipBT->Update_ForMouse())
		return TRUE;
	if(m_pkTurnLeftBT && m_pkTurnLeftBT->Update_ForMouse() == cButton::ACTION_PRESS )
	{
		m_fRot -= 0.2f;	
		GET_SUBCAMERA(CAMERA_04)->SetTranslate(m_pTamer->GetPos());	//2015-11-26-nova
		GET_SUBCAMERA(CAMERA_04)->SetRotation(m_fRot, 0.0f);
		GET_SUBCAMERA(CAMERA_04)->_UpdateCamera();
		g_pRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_04)->GetCameraObj() );	
		return TRUE;
	}
	if(m_pkTurnRightBT && m_pkTurnRightBT->Update_ForMouse() == cButton::ACTION_PRESS)
	{
		m_fRot += 0.2f;	
		GET_SUBCAMERA(CAMERA_04)->SetTranslate(m_pTamer->GetPos());
		GET_SUBCAMERA(CAMERA_04)->SetRotation(m_fRot, 0.0f);
		GET_SUBCAMERA(CAMERA_04)->_UpdateCamera();
		g_pRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_04)->GetCameraObj() );
		return TRUE;
	}
	
	return FALSE;
}

void NewCashshopEquipViewer::RenderScript()
{
	if( !IsVisible() )
		return;

	if( m_pRenderTex )
	{
		m_pRenderTex->BeginRender(NiColorA(1.0f,1.0f,1.0f,0.0f), NiRenderer::CLEAR_ALL);
		m_pRenderTex->RenderObjectIncludeEffect( GET_SUBCAMERA(CAMERA_04), m_pTamer, 350.0f );	//2015-11-26-nova m_Camera -> 전역카메라의 서브카메라 사용
		m_pRenderTex->EndRender( GET_SUBCAMERA(CAMERA_04), GetTotalPosition()/* + RenderPos*/);
	}

	cScriptUI::RenderScript();
}

void	NewCashshopEquipViewer::ResetDeviceScript()
{
	cScriptUI::ResetDeviceScript();
	if( m_pRenderTex )
		m_pRenderTex->ResetDevice();
}

void   NewCashshopEquipViewer::OnRClicked_EquipItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	EquipItemInfo* pkEquipment =	dynamic_cast<EquipItemInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkEquipment);

	BHPRT("OnRClicked_EquipItem PARTID[%d] ITEID[%d]",pkEquipment->partid, pkEquipment->itemid);
	GetSystem()->OnClick_TakeOffEquipmentItem(pkEquipment->partid, pkEquipment->itemid);
}

void NewCashshopEquipViewer::UpdateEquipments(int const& PartId, DWORD const& ItemID)
{
	if( m_pTamer && PartId < nTamer::MaxParts )
	{
		CsC_PartObject::sCHANGE_PART_INFO cp;
		cp.s_nFileTableID = ItemID;
		cp.s_nPartIndex = PartId;
		cp.s_nRemainTime = _TIME_TS + 300; 
		m_pTamer->ChangePart( &cp );
	}

	_UpdateEquipSlotData( PartId, ItemID );
}

void NewCashshopEquipViewer::OnClick_Reset(void* pSender, void* pData)
{
	GetSystem()->OnClick_ReturnOriginally();
}
void NewCashshopEquipViewer::OnClick_TakeOffAll(void* pSender, void* pData)
{
	GetSystem()->OnClick_TakeOffAll();
}