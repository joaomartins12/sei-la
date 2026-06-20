
#include "stdafx.h"
#include "DigimonInfoViewer.h"

#pragma warning( disable : 4311 )

bool CDigimonInfoViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}
CDigimonInfoViewer::CDigimonInfoViewer()
:m_bisDragable(false),
m_pDigimonLV(NULL),
m_pDigimonName(NULL),
m_pDigimonHeight(NULL),
m_pDigimonType(NULL),
m_pDigimonEnchant(NULL),
m_pDig_St_Enchant(NULL),
m_pDig_St_Type(NULL),
m_pDig_St_Height(NULL),
m_pkPlayer(NULL),
m_pkAccompanies(NULL),
m_pDigimonRankImg(NULL),
mSelectedDigPos(CsPoint::ZERO),
mptLevelPos(CsPoint::ZERO),
mptElementPos(CsPoint::ZERO),
mptRankImgPos(CsPoint::ZERO),
m_pRenderDigimon(NULL),
m_pRenderTex(NULL)
{
	Construct();
}

CDigimonInfoViewer::~CDigimonInfoViewer()
{
}

void CDigimonInfoViewer::Destroy()
{
}

void CDigimonInfoViewer::DeleteResource()
{
}

void CDigimonInfoViewer::DeleteScript()
{
	m_DigimonElement.Delete();
	SAFE_NIDELETE( m_pRenderTex );

	if( g_pCharMng )
	{
		CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();

		if( pTamer != NULL )
		{
			std::map<int, u4>* p_mapSkill		= g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerSkillMap();
			std::map<int, u4>::iterator it		= p_mapSkill->begin();
			std::map<int, u4>::iterator itEnd	= p_mapSkill->end();

			for( ; it != itEnd; ++it )
			{
				if( it->first != cSkill::TACTICSHOUSE )
					continue;

				CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, it->second );
				if( net::game )
					net::game->SendTamerActiveSkillEnd( pTamerSkill->Get_TSkill_Info()->s_nIndex );

				p_mapSkill->erase( cSkill::TACTICSHOUSE );
				break;
			}
		}
	}

	// 아이템 언롹킹
	if( g_pDataMng && g_pDataMng->GetInven() )
		g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::Portable_Item, eTATICS_HOUSE);
	
	NISAFE_DELETE(m_pDigimonRankImg);
	ReleaseSelect();
	cScriptUI::DeleteScript();
}

void CDigimonInfoViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	CsPoint kSlotSizeTest = CsPoint( 46, 46);
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	InitScript( pkRoot, NULL, CsPoint( 55,125 ), CsPoint( 274, 459 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP);
	AddSprite( CsPoint( -115, 0 ), CsPoint( 506, 459 ), "TacticsHouse\\Storage_info_01.tga" );
#else
	InitScript( pkRoot, "TacticsHouse\\Storage_info_01.tga", CsPoint( 55,125 ), CsPoint( 274, 459 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP);
#endif

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_LEFT;

	NiColor kStaticColor = NiColor(101.0f/256.0f,255.0f/256.0f,201.0f/256.0f);
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init Info Texts
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_13, kStaticColor );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT("Digimon_Storage_DigimonLevel1").c_str() );
	ti.SetText(L"");
	m_pDigimonLV = AddText( &ti, CsPoint(35, 122) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_13, kStaticColor );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText(L"");
	m_pDigimonName=AddText( &ti, CsPoint(120, 122) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_12, NiColor::WHITE );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText(L"");
	m_pDigimonType=AddText( &ti, CsPoint(50, 427) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_12, NiColor::WHITE );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText(L"");
	m_pDigimonEnchant=AddText( &ti, CsPoint(50, 378) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_12, NiColor::WHITE );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText(L"");
	m_pDigimonHeight=AddText( &ti, CsPoint(50, 329) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init Static Texts
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_13, kStaticColor );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT("Digimon_Storage_DigimonTall").c_str() );
	m_pDig_St_Height =AddText( &ti, CsPoint(50, 301));
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_13, kStaticColor );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT("Digimon_Storage_DigimonInt").c_str() );
	m_pDig_St_Enchant =AddText( &ti, CsPoint(50, 352) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ti.Init(CFont::FS_13, kStaticColor );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT("Digmon_Storage_DigimonType1").c_str() );
	m_pDig_St_Type =AddText( &ti, CsPoint(50, 403) );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	m_pDigimonRankImg = NiNew cImage();
	m_pDigimonRankImg->Init( NULL, CsPoint::ZERO, CsPoint( 72, 62 ), "CommonUI\\digimon_rank.tga", false );
	m_pDigimonRankImg->SetTexToken( CsPoint( 72, 62 ) );	

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init My Digimon
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	m_pkPlayer = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pkPlayer );
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	m_pkPlayer->Init( GetRoot(), CsPoint(-8, 21), CsPoint(50, 60), CsPoint::ZERO, CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
#else
	m_pkPlayer->Init( GetRoot(), CsPoint(21, 21), CsPoint(50, 60), CsPoint::ZERO, CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
#endif
	m_pkPlayer->AddEvent( cGridListBox::GRID_SELECTED_ITEM, this, &CDigimonInfoViewer::OnItemSelected_UsingDigimon );
	m_pkPlayer->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &CDigimonInfoViewer::OnItemClicked_UsingDigimon );
	m_pkPlayer->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &CDigimonInfoViewer::OnMouseDownOnUsingItem );
	m_pkPlayer->AddEvent( cGridListBox::GRID_MOUSE_UP_ON_GRIDLIST, this, &CDigimonInfoViewer::OnMouseUpOnUsingItem );
	m_pkPlayer->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonInfoViewer::OnMouseMoveOnItem );
	m_pkPlayer->SetMouseOverImg( "Lobby\\CharacterCreate\\digimon_glow.tga" );
	m_pkPlayer->SetSelectedImg( "TacticsHouse\\Storage_select.tga" );
	m_pkPlayer->SetBackOverAndSelectedImgRender(false);
	AddChildControl(m_pkPlayer);
	cString * pItem = NiNew cString;
	cSprite* pImage = NiNew cSprite;	// 1
	pImage->Init( NULL, CsPoint::ZERO, kSlotSizeTest, "TacticsHouse\\Storage_lock.tga", false );
	cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, CsPoint( 46, 46)  );	
	if( sSprite )
		sSprite->SetAutoPointerDelete(true);
	cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint( 46, 46));
	addItem->SetItem( pItem );
	m_pkPlayer->AddItem( addItem );
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init Accompany Digimon
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	const CsPoint kAccomGridPos = CsPoint(69, 21);
#else
	const CsPoint kAccomGridPos = CsPoint(98, 21);
#endif
	const CsPoint kAccomGridSize = CsPoint(230, 60);
	const CsPoint kAccomGap = CsPoint(14, 0.0f);
	m_pkAccompanies = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pkAccompanies );

	m_pkAccompanies->Init( GetRoot(), kAccomGridPos, kAccomGridSize, 
		kAccomGap, kSlotSizeTest, cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
	AddChildControl(m_pkAccompanies);
	m_pkAccompanies->AddEvent( cGridListBox::GRID_SELECTED_ITEM, this, &CDigimonInfoViewer::OnItemSelected_AccompanyDigimon );
	m_pkAccompanies->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &CDigimonInfoViewer::OnItemClicked_AccompanyDigimon );
	m_pkAccompanies->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &CDigimonInfoViewer::OnMouseDownOnAccompanyItem );
	m_pkAccompanies->AddEvent( cGridListBox::GRID_MOUSE_UP_ON_GRIDLIST, this, &CDigimonInfoViewer::OnMouseUpOnAccompanyItem );
	m_pkAccompanies->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonInfoViewer::OnMouseMoveOnAccompanyItem );
	m_pkAccompanies->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &CDigimonInfoViewer::OnMouseRClickInAccompany );
	m_pkAccompanies->SetUsePressedMoveEvent(true);
	m_pkAccompanies->SetMouseOverImg( "Lobby\\CharacterCreate\\digimon_glow.tga" );
	m_pkAccompanies->SetSelectedImg( "TacticsHouse\\Storage_select.tga" );
	m_pkAccompanies->SetBackOverAndSelectedImgRender(false);	
	
	for(int i = 0; i < nLimit::DigimonBaseSlot - 1; ++i)
	{	
		cString * pItem = NiNew cString;
		cSprite* pImage = NiNew cSprite;	// 1
		pImage->Init( NULL, CsPoint::ZERO,CsPoint( 24, 34),  "TacticsHouse\\Storage_lock.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint( 11, 5 ), CsPoint( 24, 34)  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(i, CsPoint( 33, 41));
		addItem->SetItem( pItem );
		m_pkAccompanies->AddItem( addItem );
	}

	mptRankImgPos = CsPoint(25, 155);
	mptElementPos =  CsPoint(25, 275);
	mSelectedDigPos =  CsPoint(22,	151);
	m_pRenderTex = NiNew cRenderTex;
	if( m_pRenderTex->Init( CsPoint(243,144), "TacticsHouse\\Storage_info_02.tga"  ) == false ){
		SAFE_NIDELETE( m_pRenderTex );
	}
	//if( m_pRenderTex->Init( CsPoint(260,150), "DigimonStatus\\DigimonBack.tga"  ) == false ){
	//	SAFE_NIDELETE( m_pRenderTex );
	//}
	// 보관소 아이템 롹킹
	g_pDataMng->GetInven()->ItemLock_ItemType( nItem::Portable_Item, eTATICS_HOUSE);
}

void CDigimonInfoViewer::Update(float const& fDeltaTime)
{
	_UpdateRenderDigimon();
}

void CDigimonInfoViewer::_UpdateRenderDigimon()
{
	if( m_pRenderDigimon == NULL )
		return;

	if( m_pRenderTex == NULL )
		return;

	if( m_pRenderDigimon->IsLoad() )
	{
		// 오브젝트 위치 설정	
		CsNodeObj* pCsNode = m_pRenderDigimon->GetCsNode();
		if( pCsNode != NULL )
		{					
			m_pRenderDigimon->GetProp_Animation()->Update( g_fDeltaTime );
			m_pRenderDigimon->GetProp_Alpha()->Update( g_fDeltaTime );

			float fAniTime = m_pRenderDigimon->GetAniTime();
			pCsNode->m_pNiNode->Update( fAniTime );
		}
	}
	else
	{
		m_pRenderDigimon->CsC_AvObject::Update( g_fDeltaTime );
		m_pRenderDigimon->SetViewSight( true );
	}
}

BOOL CDigimonInfoViewer::UpdateMouse()
{
	if(m_pkPlayer && m_pkPlayer->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	if(m_pkAccompanies && m_pkAccompanies->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	return FALSE;
}

void CDigimonInfoViewer::Render()
{
	SAFE_POINTER_RET( m_pRenderTex );
	m_pRenderTex->BeginRender();
	m_pRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_06), m_pRenderDigimon, true );	//2015-11-30-nova
	CsPoint RootPos = GetRoot()->GetClient();
	m_pRenderTex->EndRender( GET_SUBCAMERA(CAMERA_06), mSelectedDigPos + RootPos );

	if( m_pDigimonRankImg )
		m_pDigimonRankImg->Render( mptRankImgPos + RootPos );

	m_DigimonElement.Render( mptElementPos + RootPos );
}
void CDigimonInfoViewer::RenderSelectedDigimon(const cData_PostLoad::sDATA* pkDATA )
{
	CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(pkDATA->s_dwBaseDigimonID)->GetInfo();
	if(pFTDigimon)
	{
		CsModelData::sINFO* pModelInfo  = g_pModelDataMng->GetData( pFTDigimon->s_dwModelID)->GetInfo();
		if(pModelInfo)
		{
			m_pDigimonName->SetText( pkDATA->s_szName );

			float fTall = pModelInfo->s_fHeight	*	pModelInfo->s_fScale	*	pkDATA->s_fScale	*	100.0f;
			int nTall = DmCS::StringFn::FloatRounding(fTall);

			float fPercent = DmCS::StringFn::FloatRounding( pkDATA->s_fScale*100.0f, 3 );

			std::wstring wsPerscent;
			DmCS::StringFn::FloatToString( wsPerscent, fPercent, 3 );

			std::wstring szTall = UISTRING_TEXT("Digimon_Storage_DigimonInfo").c_str();
			DmCS::StringFn::ReplaceAll( szTall, L"#DigimonTall#", nTall );
			DmCS::StringFn::ReplaceAll( szTall, L"#DigimonPercent#", wsPerscent );
			DmCS::StringFn::ReplaceAll( szTall, L"#HatchLV#", pkDATA->s_HatchLevel );
			m_pDigimonHeight->SetText( szTall.c_str() );

			wstring szEnchentLv = UISTRING_TEXT("Digimon_Storage_DigimonEnchant").c_str();
			DmCS::StringFn::ReplaceAll( szEnchentLv, L"#CurrEnchantLv#", pkDATA->s_nEnchantLevel - 1);
			DmCS::StringFn::ReplaceAll( szEnchentLv, L"#MaxEnchantLv#", nLimit::MaxInchantLevel );
			m_pDigimonEnchant->SetText(szEnchentLv.c_str());

			wstring szType = UISTRING_TEXT("Digmon_Storage_DigimonType2").c_str();
			CsTacticsExplain* pkExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( pkDATA->s_dwBaseDigimonID );
			if(pkExplain != NULL && pkExplain->GetInfo() != NULL ){
				DmCS::StringFn::ReplaceAll( szType, L"#DigimonType#", pkExplain->GetInfo()->s_szTacticsName);
			}
			m_pDigimonType->SetText(szType.c_str());


			wstring szStaticLV = UISTRING_TEXT("Digimon_Storage_DigimonLevel1").c_str();
			wstring szCurrLv = UISTRING_TEXT("Digimon_Storage_DigimonLevel2").c_str();
			DmCS::StringFn::ReplaceAll( szCurrLv, L"#DigimonLevel#", pkDATA->s_nLevel );

			wstring szFinalLV;
			DmCS::StringFn::Format( szFinalLV, L"%s%s", szStaticLV.c_str(), szCurrLv.c_str());
			m_pDigimonLV->SetText(szFinalLV.c_str());

			CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pModelInfo->s_dwID )->GetInfo();
			m_DigimonElement.Delete();

			if( m_pDigimonRankImg )
			{
				bool bShow = pFTDigimon->s_nDigimonRank > 0 ? true : false;
				m_pDigimonRankImg->SetVisible(bShow);
				if( bShow )
					m_pDigimonRankImg->SetState( pFTDigimon->s_nDigimonRank - 1 );
			}			

			int nIndex;
			// - 디지몬 속성x
			nIndex = pFTDigimon->s_eAttributeType - nsCsDigimonTable::AT_NONE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );
			// - 패밀리 속성
			for( int i=0; i<3; ++i )
			{
				if( pFTDigimon->s_eFamilyType[ i ] == 0 )
					continue;
				nIndex = pFTDigimon->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
				m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
			}
			// - 자연 속성
			if( pFTDigimon->s_eBaseNatureTypes[0] != 0 )
			{
				nIndex = pFTDigimon->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
				m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
			}
			// - 자연 속성
			if( pFTDigimon->s_eBaseNatureTypes[1] != 0 )
			{
				nIndex = pFTDigimon->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
				m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
			}
			// - 자연 속성
			if( pFTDigimon->s_eBaseNatureTypes[2] != 0 )
			{
				nIndex = pFTDigimon->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
				m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
			}

			// 모델 데이터 로드
			if( m_pRenderTex == NULL )
				return;

			sCREATEINFO prop;
			prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
			prop.s_dwOptionPlag = OP_LIGHT;
			prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

			m_pRenderDigimon = g_pCharMng->AddTempDigimon( &prop, pModelInfo->s_dwID, NULL, NiPoint3::ZERO, 0.0f, NULL );
			m_pRenderDigimon->GetProp_Alpha()->SetUseDistAlpha( false );
			m_pRenderDigimon->SetViewSight( true );
		}
	}

}
void CDigimonInfoViewer::RenderEmpty( void )
{
	if( m_pDigimonName )
		m_pDigimonName->SetText( L"" );
	if( m_pDigimonHeight )
		m_pDigimonHeight->SetText( L"" );
	if( m_pDigimonEnchant )
		m_pDigimonEnchant->SetText(L"");
	if( m_pDigimonType )
		m_pDigimonType->SetText(L"");
	if( m_pDigimonLV )
		m_pDigimonLV->SetText(L"");
	if( m_pDigimonRankImg )
		m_pDigimonRankImg->SetVisible(false);

	m_DigimonElement.Delete();
	m_pRenderDigimon = NULL;
}

void CDigimonInfoViewer::ResetDeviceScript()
{
	cScriptUI::ResetDeviceScript();	
	if( m_pRenderTex )
		m_pRenderTex->ResetDevice();
	if( m_pDigimonRankImg )
		m_pDigimonRankImg->ResetDevice();
}

void CDigimonInfoViewer::OnChangeSelectedDigimon(const cData_PostLoad::sDATA* pkDATA, int SelectedPart )
{
	if(pkDATA == NULL || pkDATA->s_dwBaseDigimonID == 0 ){
		RenderEmpty();
	}
	else
	{
		RenderSelectedDigimon(pkDATA);
	}
}
void CDigimonInfoViewer::UpdateViewer(void)
{
	SAFE_POINTER_RET(m_pkPlayer);
	SAFE_POINTER_RET(m_pkAccompanies);
	DWORD UsingDigimonID = GetSystem()->GetUsingDigimon();
	cGridListBoxItem* pkItem = const_cast<cGridListBoxItem*>(m_pkPlayer->GetItemFormPos(DIGIMON::USING));
	UpdateSlotIcon(UsingDigimonID, pkItem);

	for(int i= DIGIMON::ACCOMPANY1; i <= GetSystem()->GetOpenedAccompanySlotCnt(); ++i)
	{
		pkItem = const_cast<cGridListBoxItem*>( m_pkAccompanies->GetItemFormPos(i - 1) );

		DWORD dwCompanyID = GetSystem()->GetCompanyID(i);
		UpdateSlotIcon(dwCompanyID, pkItem );
		UpdateLevelIcon(dwCompanyID, pkItem);
	}
}

void CDigimonInfoViewer::UpdateSlotIcon(DWORD dwDigimonID, cGridListBoxItem* pkItem)
{
	pkItem->GetItem()->Delete();
	if(dwDigimonID != 0)
	{
		const CsPoint kSlotSizeTest = CsPoint( 44, 44 );
		const CsPoint kSlotDelta = CsPoint( 1, 1);
		//디지몬 Icon
		CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID );
		if(pkTableInfo != NULL &&  pkTableInfo->GetInfo())
		{
			std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
			cSprite* pkSlotIcon = NiNew cSprite;	// 1
			pkSlotIcon->Init( NULL, CsPoint::ZERO, kSlotSizeTest, kPath.c_str(), false, NiColor::WHITE, false  );
			cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pkSlotIcon, kSlotDelta, kSlotSizeTest  );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}
	}
	else
	{
	}

}
void	CDigimonInfoViewer::UpdateLevelIcon(DWORD dwDigimonID,cGridListBoxItem* pkItem)
{
	if(dwDigimonID != 0)
	{
		const CsPoint kSlotSizeTest = CsPoint( 44, 44 );
		const CsPoint kSlotDelta = CsPoint( 1, 1);
		//디지몬 LV 커버
		cData_PostLoad::sDATA* pkAccomInfo = GetSystem()->GetDigimonDataInAccompany(pkItem->getID() + 1);
		if(pkAccomInfo != NULL)
		{
			std::string kPath = CDigimonArchiveViewer::GetLVToPath(pkAccomInfo->s_nLevel);
			cSprite* pGrade = NiNew cSprite;	// 1
			pGrade->Init( NULL, CsPoint::ZERO, kSlotSizeTest, kPath.c_str(), false );
			cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pGrade, kSlotDelta, kSlotSizeTest  );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}
	}

}

void	CDigimonInfoViewer::OnItemSelected_UsingDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
}

void	CDigimonInfoViewer::OnMouseDownOnUsingItem(void* pSender, void* pData)
{

}
void	CDigimonInfoViewer::OnMouseUpOnUsingItem(void* pSender, void* pData)
{
	if(CURSOR_ST.IsIcon())
		CURSOR_ST.ReleaseIcon();
}
void	CDigimonInfoViewer::OnMouseMoveOnItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnShowToolTip(CDigimonArchiveContents::USING_DIGIMON, -1);
}


void	CDigimonInfoViewer::OnItemSelected_AccompanyDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
}

void	CDigimonInfoViewer::OnItemClicked_UsingDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnShowPartnerDigimonInfo();
}

void	CDigimonInfoViewer::OnItemClicked_AccompanyDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;
	int CurrSlotIdx = pItem->getID();
	GetSystem()->OnShowDigimonInfoInAccompany(CurrSlotIdx);
}

void	CDigimonInfoViewer::OnMouseDownOnAccompanyItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	if(GetSystem()->GetNowDrag())
	{
		DWORD dwDigimonID = GetSystem()->GetCompanyID(pItem->getID()+1);
		if(dwDigimonID != 0)
		{
			DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID )->GetInfo()->s_dwModelID;	
			sCHAR_IMAGE* pkCharImage = g_pCharResMng->CharImageResLoad( dwModelID );
			if(pkCharImage != NULL)
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TACTICSHOUSE, 1, pItem->getID(), NULL );
				CURSOR_ST.SetIconSubInfo( (int)pkCharImage, 0, 0 );
				GetSystem()->ResetDragInfo();
				GetSystem()->SetDragInfo(CDigimonArchiveContents::SRC_ACCOMPANY, pItem->getID()+1);
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
void	CDigimonInfoViewer::OnMouseUpOnAccompanyItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	if(GetSystem()->GetNowDrag() == true)
	{
		//m_bisDragable= false;
		GetSystem()->SetDragInfo(CDigimonArchiveContents::DEST_ACCOMPANY, pItem->getID()+1);
		GetSystem()->ExcuteDragInfo();
	}
	GetSystem()->SetNowDrag(false);

}
void	CDigimonInfoViewer::OnMouseMoveOnAccompanyItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnShowToolTip(CDigimonArchiveContents::ACCOMPANY_DIGIMON, pItem->getID()+1);
}

void	CDigimonInfoViewer::OnMouseRClickInAccompany(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;
	GetSystem()->OnRClick_AccompanyDigimon(pItem->getID());

}
void	CDigimonInfoViewer::ReleaseSelect(void)
{
	if(m_pkPlayer != NULL)
		m_pkPlayer->releaseSelection();
	if(m_pkAccompanies != NULL)
		m_pkAccompanies->releaseSelection();

}
void	CDigimonInfoViewer::ReleaseSelect(CDigimonArchiveContents::SELECT_PART ePart)
{
	switch(ePart)
	{
	case CDigimonArchiveContents::ACCOMPANY_DIGIMON:
		if(m_pkPlayer != NULL)
			m_pkPlayer->releaseSelection();
		break;
	case CDigimonArchiveContents::USING_DIGIMON:
		if(m_pkAccompanies != NULL)
			m_pkAccompanies->releaseSelection();	
		break;

	}
}


#pragma warning( default : 4311 )