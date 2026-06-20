
#include "stdafx.h"
#include "PartyWindow.h"

//////////////////////////////////////////////////////////////////////////
cPartyWindow::sPartyControls::sPartyControls():m_pDigimonImg(NULL),m_pMasterImg(NULL),
m_pDisableImg(NULL),m_pTamerHp(NULL),m_pTamerDs(NULL),m_pDigimonHp(NULL),m_pTamerLv(NULL),m_pDigimonDs(NULL),m_pDigimonLv(NULL),m_BoxItem(NULL)
{

}

cPartyWindow::sPartyControls::~sPartyControls()
{
	m_pDigimonImg = NULL;
	m_pMasterImg = NULL;
	m_pDisableImg = NULL;
	m_pTamerHp = NULL;
	m_pTamerDs = NULL;
	m_pDigimonHp = NULL;
	m_pDigimonDs = NULL;

	m_pTamerLv = NULL;
	m_pDigimonLv = NULL;
	m_BoxItem = NULL;
}
//////////////////////////////////////////////////////////////////////////
void cPartyWindow::sPartyControls::SetFaraway(bool bValue)
{
	SAFE_POINTER_RET( m_pDisableImg );
	if( m_pDisableImg->GetVisible() != bValue )
		m_pDisableImg->SetVisible( bValue );
}

cPartyWindow::cPartyWindow():m_pPartyUIList(NULL)
{
}

cPartyWindow::~cPartyWindow()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cPartyWindow::Destroy()
{
	Delete();
}

void cPartyWindow::DeleteResource()
{
	m_mapMemberControls.clear();
	DeleteScript();
}

void cPartyWindow::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint( 8, 207 ), CsPoint( 202, 260 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );

	m_pPartyUIList = NiNew cListBox;
	if( m_pPartyUIList )
	{
		m_pPartyUIList->Init( GetRoot(), CsPoint::ZERO, CsPoint( 202, 252 ), NULL, false );		
		m_pPartyUIList->SetItemGab(CsPoint(0,15));
		m_pPartyUIList->SetItemHeight( 74 );
		m_pPartyUIList->SetBackOverAndSelectedImgRender( false );
		m_pPartyUIList->SetManaulSelectChange(true);
		m_pPartyUIList->AddEvent( cListBox::LIST_SELECTED_ITEM_RIGHT_DOWN, this, &cPartyWindow::ShowMemberPopWindow );
		m_pPartyUIList->AddEvent( cListBox::LIST_SELECTED_ITEM, this, &cPartyWindow::SetTargetObject );
		m_pPartyUIList->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &cPartyWindow::SetTargetObject );		
		m_pPartyUIList->AddEvent( cListBox::LIST_ITEM_MOUSE_OVER, this, &cPartyWindow::ShowMemberTooltip );

		cImage* pOverImg = NiNew cImage;
		if( pOverImg )
		{
			pOverImg->Init( NULL, CsPoint( 0, 0), CsPoint(199, 74), "Party\\Win_Mask.tga", false );
			m_pPartyUIList->SetMouseOverImg( pOverImg, true );
		}

		cImage* pSelImg = NiNew cImage;
		if( pSelImg )
		{
			pSelImg->Init( NULL, CsPoint( 0, 0), CsPoint(199, 74), "Party\\Win_Mask.tga", false );
			m_pPartyUIList->SetSelectedImg( pSelImg, true );
		}

		AddChildControl( m_pPartyUIList );
	}

	_MakePartyMember();
}

void cPartyWindow::Update(float const& fDeltaTime)
{
// 	SAFE_POINTER_RET( g_pMngCollector );
// 	MAP_MEMBER_UI_IT it = m_mapMemberControls.begin();
// 	for( ; it != m_mapMemberControls.end(); ++it )
// 	{
// 		SMemberInfo const * pInfo = GetSystem()->GetMemberInfoFromSlotNo( it->first );
// 		SAFE_POINTER_CON( pInfo );
// 		CsC_AvObject* pDest = g_pMngCollector->GetObject( pInfo->m_TamerInfo.m_nUID );
// 
// 		bool bFaraway = ( pDest && pDest->GetProp_Alpha()->IsHideDistOut() == false ) ? false : true;
// 		it->second.SetFaraway( bFaraway );
// 	}
}

void cPartyWindow::_MakePartyMember()
{
	LIST_MEMBER_INFO const& listInfo  = GetSystem()->GetMemberList();
	LIST_MEMBER_INFO_CIT it = listInfo.begin();
	for( ; it != listInfo.end(); ++it )
		_AddNewMember( (*it) );

	_ChangePartyMaster();
}

void cPartyWindow::_AddNewMember( SMemberInfo const* pInfo )
{
	SAFE_POINTER_RET( m_pPartyUIList );

	cString * pItem = NiNew cString;

	sPartyControls controls;

	// 배경 이미지
	cSprite * pBack = NiNew cSprite;
	if( pBack )
	{
		pBack->Init( NULL, CsPoint(3,6), CsPoint( 192, 63 ), "Party\\win.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( pBack );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	
	// 테이머 이지미
	cSprite* pTamerImg = NiNew cSprite;
	if( pTamerImg )
	{
		std::string imgFile = pInfo->m_TamerInfo.GetImage();
		pTamerImg->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), imgFile.c_str(), true, NiColor::WHITE, false );
		cString::sSPRITE* paddImg =pItem->AddSprite( pTamerImg, CsPoint( 20, 5 ) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	// 디지몬 이미지
	
	controls.m_pDigimonImg = NiNew cSprite;
	if( controls.m_pDigimonImg )
	{		
		std::string imgFile = pInfo->m_DigimonInfo.GetImage();
		controls.m_pDigimonImg->Init( NULL, CsPoint::ZERO, CsPoint( 29, 29 ), imgFile.c_str(), true, NiColor::WHITE, false );
		cString::sSPRITE* paddImg =pItem->AddSprite( controls.m_pDigimonImg, CsPoint( 50, 39 ) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	// 테이머 네임
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		//ti.s_eTextAlign = DT_CENTER;
		ti.SetString(pInfo->m_TamerInfo.m_Name.c_str());
		pItem->AddText( &ti, CsPoint( 70, 25 ) );
	}

	// 테이머 레벨
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_8 );
		//ti.s_eTextAlign = DT_CENTER;
		std::wstring tamerlv;
		DmCS::StringFn::From( tamerlv, (int)pInfo->m_TamerInfo.m_nLevel );
		ti.SetString(tamerlv.c_str());
		controls.m_pTamerLv = pItem->AddText( &ti, CsPoint( 7, 8 ) );
	}

	// 디지몬 레벨
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_8 );
		//ti.s_eTextAlign = DT_CENTER;
		std::wstring digimonlv;
		DmCS::StringFn::From( digimonlv, (int)pInfo->m_DigimonInfo.m_nLevel );
		ti.SetString(digimonlv.c_str());
		controls.m_pDigimonLv = pItem->AddText( &ti, CsPoint( 36, 56 ) );
	}

	// 테이머 HP/DS Bar
	{
		controls.m_pTamerHp = NiNew cProgressBar;
		if( controls.m_pTamerHp )
		{
			controls.m_pTamerHp->Init( NULL, cProgressBar::BAR_3IMAGE, CsPoint::ZERO, CsPoint( 128, 7 ), CsPoint::ZERO, "TargetWindow\\HpBar1.tga", false );
			//controls.m_pTamerHp->SetRenderPosText( cProgressBar::TEXT_VALUE );
			controls.m_pTamerHp->SetRange( CsPoint( 0, pInfo->m_TamerInfo.m_BaseInfo.m_nMHP ) );			
			controls.m_pTamerHp->SetBarPos( pInfo->m_TamerInfo.m_BaseInfo.m_nHP );
			cString::sPROGRESS* pHp = pItem->AddProgress( controls.m_pTamerHp, CsPoint( 66, 7 ));
			if( pHp )
				pHp->SetAutoPointerDelete(true);
		}

		controls.m_pTamerDs = NiNew cProgressBar;
		if( controls.m_pTamerDs )
		{
			controls.m_pTamerDs->Init( NULL, cProgressBar::BAR_3IMAGE, CsPoint::ZERO, CsPoint( 128, 7 ), CsPoint::ZERO, "TargetWindow\\DsBar1.tga", false );
			//controls.m_pTamerDs->SetRenderPosText( cProgressBar::TEXT_VALUE );
			controls.m_pTamerDs->SetRange( CsPoint( 0, pInfo->m_TamerInfo.m_BaseInfo.m_nMDS ) );
			controls.m_pTamerDs->SetBarPos( pInfo->m_TamerInfo.m_BaseInfo.m_nDS );
			cString::sPROGRESS* pds = pItem->AddProgress( controls.m_pTamerDs, CsPoint( 66, 16 ));
			if( pds )
				pds->SetAutoPointerDelete(true);
		}
	}

	// 디지몬 HP/DS Bar
	{
		controls.m_pDigimonHp = NiNew cProgressBar;
		if( controls.m_pDigimonHp )
		{
			controls.m_pDigimonHp->Init( NULL, cProgressBar::BAR_3IMAGE, CsPoint::ZERO, CsPoint( 114, 12 ), CsPoint::ZERO, "TargetWindow\\HpBar1.tga", false );
			//controls.m_pDigimonHp->SetRenderPosText( cProgressBar::TEXT_VALUE );
			controls.m_pDigimonHp->SetRange( CsPoint( 0, pInfo->m_DigimonInfo.m_BaseInfo.m_nMHP ) );
			controls.m_pDigimonHp->SetBarPos( pInfo->m_DigimonInfo.m_BaseInfo.m_nHP );
			cString::sPROGRESS* pds = pItem->AddProgress( controls.m_pDigimonHp, CsPoint( 80, 41 ));
			if( pds )
				pds->SetAutoPointerDelete(true);
		}

		controls.m_pDigimonDs = NiNew cProgressBar;
		if( controls.m_pDigimonDs )
		{
			controls.m_pDigimonDs->Init( NULL, cProgressBar::BAR_3IMAGE, CsPoint::ZERO, CsPoint( 114, 6 ), CsPoint::ZERO, "TargetWindow\\DsBar1.tga", false );
			//controls.m_pDigimonDs->SetRenderPosText( cProgressBar::TEXT_VALUE );
			controls.m_pDigimonDs->SetRange( CsPoint( 0, pInfo->m_DigimonInfo.m_BaseInfo.m_nMDS ) );
			controls.m_pDigimonDs->SetBarPos( pInfo->m_DigimonInfo.m_BaseInfo.m_nDS );
			cString::sPROGRESS* pds = pItem->AddProgress( controls.m_pDigimonDs, CsPoint( 80, 54 ));
			if( pds )
				pds->SetAutoPointerDelete(true);
		}
	}

	controls.m_pMasterImg = NiNew cSprite;
	if( controls.m_pMasterImg )
	{
		controls.m_pMasterImg->Init( NULL, CsPoint::ZERO, CsPoint( 22, 25 ), "Party\\Host.tga", false );
		controls.m_pMasterImg->SetVisible(false);
		cString::sSPRITE* paddImg =pItem->AddSprite( controls.m_pMasterImg, CsPoint( 20, 25 ) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	controls.m_pDisableImg = NiNew cSprite;
	{
		if( controls.m_pDisableImg )
			controls.m_pDisableImg->Init( NULL, CsPoint(3,6), CsPoint( 192, 63 ), "Party\\Win_Disable.tga", false );
		controls.m_pDisableImg->SetAlpha( 0.6f );
		controls.m_pDisableImg->SetVisible(false);
		cString::sSPRITE* paddImg = pItem->AddSprite( controls.m_pDisableImg );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}	

	controls.m_BoxItem = NiNew cListBoxItem;
	controls.m_BoxItem->SetUserData( new sMemberIdx(pInfo->m_nSlotNo) );	
	controls.m_BoxItem->SetItem( pItem );
	m_pPartyUIList->AddItem( controls.m_BoxItem );

	m_mapMemberControls.insert( std::make_pair(pInfo->m_nSlotNo, controls) );
}

void cPartyWindow::_UpdateMemberInfo( SMemberInfo const* pInfo )
{
	SAFE_POINTER_RET(pInfo);

	MAP_MEMBER_UI_IT it = m_mapMemberControls.find( pInfo->m_nSlotNo );
	if( it == m_mapMemberControls.end() )
		return;

	if( it->second.m_pTamerLv )
	{
		std::wstring tamerlv;
		DmCS::StringFn::From( tamerlv, (int)pInfo->m_TamerInfo.m_nLevel );
		it->second.m_pTamerLv->SetText( tamerlv.c_str() );
	}

	if( it->second.m_pDigimonLv )
	{
		std::wstring digimonlv;
		DmCS::StringFn::From( digimonlv, (int)pInfo->m_DigimonInfo.m_nLevel );
		it->second.m_pDigimonLv->SetText( digimonlv.c_str() );
	}

	if( it->second.m_pTamerHp )
	{
		it->second.m_pTamerHp->SetRange( CsPoint( 0, pInfo->m_TamerInfo.m_BaseInfo.m_nMHP ) );			
		it->second.m_pTamerHp->SetBarPos( pInfo->m_TamerInfo.m_BaseInfo.m_nHP );		
	}

	if( it->second.m_pTamerDs )
	{
		it->second.m_pTamerDs->SetRange( CsPoint( 0, pInfo->m_TamerInfo.m_BaseInfo.m_nMDS ) );			
		it->second.m_pTamerDs->SetBarPos( pInfo->m_TamerInfo.m_BaseInfo.m_nDS );		
	}


	if( it->second.m_pDigimonHp )
	{
		it->second.m_pDigimonHp->SetRange( CsPoint( 0, pInfo->m_DigimonInfo.m_BaseInfo.m_nMHP ) );			
		it->second.m_pDigimonHp->SetBarPos( pInfo->m_DigimonInfo.m_BaseInfo.m_nHP );		
	}

	if( it->second.m_pDigimonDs )
	{
		it->second.m_pDigimonDs->SetRange( CsPoint( 0, pInfo->m_DigimonInfo.m_BaseInfo.m_nMDS ) );			
		it->second.m_pDigimonDs->SetBarPos( pInfo->m_DigimonInfo.m_BaseInfo.m_nDS );		
	}

	if( it->second.m_pDigimonImg )
	{
		std::string imgFile = pInfo->m_DigimonInfo.GetImage();
		if( it->second.m_pDigimonImgFileName.compare( imgFile ) != 0 )
		{
			it->second.m_pDigimonImgFileName = imgFile;
			it->second.m_pDigimonImg->ChangeTexture( imgFile.c_str(), false );
		}
	}
}

void cPartyWindow::_ChangeMemberDigimon(SMemberInfo const* pInfo)
{
	SAFE_POINTER_RET(pInfo);

	MAP_MEMBER_UI_IT it = m_mapMemberControls.find( pInfo->m_nSlotNo );
	if( it == m_mapMemberControls.end() )
		return;

	if( it->second.m_pDigimonLv )
	{
		std::wstring digimonlv;
		DmCS::StringFn::From( digimonlv, (int)pInfo->m_DigimonInfo.m_nLevel );
		it->second.m_pDigimonLv->SetText( digimonlv.c_str() );
	}

	if( it->second.m_pDigimonHp )
	{
		it->second.m_pDigimonHp->SetRange( CsPoint( 0, pInfo->m_DigimonInfo.m_BaseInfo.m_nMHP ) );			
		it->second.m_pDigimonHp->SetBarPos( pInfo->m_DigimonInfo.m_BaseInfo.m_nHP );		
	}

	if( it->second.m_pDigimonDs )
	{
		it->second.m_pDigimonDs->SetRange( CsPoint( 0, pInfo->m_DigimonInfo.m_BaseInfo.m_nMDS ) );			
		it->second.m_pDigimonDs->SetBarPos( pInfo->m_DigimonInfo.m_BaseInfo.m_nDS );		
	}

	if( it->second.m_pDigimonImg )
	{
		std::string imgFile = pInfo->m_DigimonInfo.GetImage();
		if( it->second.m_pDigimonImgFileName.compare( imgFile ) != 0 )
		{
			it->second.m_pDigimonImgFileName = imgFile;
			it->second.m_pDigimonImg->ChangeTexture( imgFile.c_str(), false );
		}
	}
}

void cPartyWindow::_ChangePartyMaster()
{
	MAP_MEMBER_UI_IT it = m_mapMemberControls.begin();
	for( ; it != m_mapMemberControls.end(); ++it )
	{
		if( it->second.m_pMasterImg )
			it->second.m_pMasterImg->SetVisible(false);
	}


	if( !GetSystem()->IsMePartyMaster() )
	{
		int nMasterSlot = GetSystem()->GetPartyMasterSlot();
		MAP_MEMBER_UI_IT it = m_mapMemberControls.find(nMasterSlot);
		if( it != m_mapMemberControls.end() )
			it->second.m_pMasterImg->SetVisible(true);
	}	
}

void cPartyWindow::SetTargetObject( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( pData );

	cListBox* pListBox = static_cast<cListBox*>(pSender);
	cListBoxItem* pListItem = static_cast<cListBoxItem*>(pData);
	sMemberIdx* pMemberIdx = dynamic_cast<sMemberIdx*>(pListItem->GetUserData());
	SAFE_POINTER_RET( pMemberIdx );

	SMemberInfo const * pInfo = GetSystem()->GetMemberInfoFromSlotNo( pMemberIdx->m_nMemberIdx );
	SAFE_POINTER_RET( pInfo );

	CsC_AvObject* pTarget = NULL;
	
	if( GetSystem()->IsBattlePlaying() )// 배틀 중일 경우 UI는 무조건 디지몬 선택되게 함.
	{
		pTarget = g_pMngCollector->GetObject( pInfo->m_DigimonInfo.m_nUID );
	}
	else
	{
		CsPoint itemMousePos = pListBox->GetItemtoMousePos( CURSOR_ST.GetPos(), pListItem );
		if( itemMousePos.y < 35 )
			pTarget = g_pMngCollector->GetObject( pInfo->m_TamerInfo.m_nUID );
		else
			pTarget = g_pMngCollector->GetObject( pInfo->m_DigimonInfo.m_nUID );
	}

	if( pTarget )
		g_pCharResMng->SetTargetMark( pTarget );
}

void cPartyWindow::ShowMemberPopWindow(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( GetSystem() );
	SAFE_POINTER_RET( g_pGameIF );

	// 배틀 중이면 안됨
	if( GetSystem()->IsBattlePlaying() )
		return;

	cPopUpWindow* pPopUpWin = g_pGameIF->GetPopup();
	SAFE_POINTER_RET( pPopUpWin );

	cListBoxItem* pListItem = static_cast<cListBoxItem*>(pData);
	sMemberIdx* pMemberIdx = dynamic_cast<sMemberIdx*>(pListItem->GetUserData());
	SAFE_POINTER_RET( pMemberIdx );

	cPopUpWindow::ePARTY_TYPE pt = GetSystem()->IsMePartyMaster() ? cPopUpWindow::PARTY_HS_OTHER : cPopUpWindow::PARTY_NR_OTHER;	
	pPopUpWin->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::PARTY_MENU, pt, pMemberIdx->m_nMemberIdx );
}

void cPartyWindow::ShowMemberTooltip(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pGameIF );

	cListBoxItem* pListItem = static_cast<cListBoxItem*>(pData);
	sMemberIdx* pMemberIdx = dynamic_cast<sMemberIdx*>(pListItem->GetUserData());
	SAFE_POINTER_RET( pMemberIdx );

	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	CsPoint pos = GetRootClient();	
	pTooltip->SetTooltip( pos, m_pRoot->GetSize(), 240, cTooltip::PARTY, pMemberIdx->m_nMemberIdx );
}

void cPartyWindow::_DeleteMemberUIList( int const& nDelSlotIdx )
{
	MAP_MEMBER_UI_IT it = m_mapMemberControls.find(nDelSlotIdx);
	if( it != m_mapMemberControls.end() )
		m_mapMemberControls.erase( it );

	SAFE_POINTER_RET( m_pPartyUIList );
	size_t size = m_pPartyUIList->GetItemCount();
	for( size_t n = 0; n < size; ++n )
	{
		cListBoxItem const* pItem = m_pPartyUIList->GetItemFormPos(n);
		SAFE_POINTER_CON( pItem );
		sMemberIdx* pMemberIdx = dynamic_cast<sMemberIdx*>(pItem->GetUserData());
		SAFE_POINTER_CON( pMemberIdx );
		if( pMemberIdx->m_nMemberIdx == nDelSlotIdx )
		{
			m_pPartyUIList->RemoveItem( n );
			break;
		}
	}
}

cBaseWindow::eMU_TYPE
cPartyWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;	

	if(m_pPartyUIList && m_pPartyUIList->Update_ForMouse(CURSOR_ST.GetPos()) )
		return muReturn;

	return MUT_OUT_WINDOW;
}

void cPartyWindow::Render()
{
	RenderScript();
}

void cPartyWindow::ResetDevice()
{
	ResetDeviceScript();
}

void cPartyWindow::_RefreshMemberUI()
{
	m_mapMemberControls.clear();
	if( m_pPartyUIList )
		m_pPartyUIList->RemoveAllItem();
	_MakePartyMember();
}

bool cPartyWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eADD_NEW_PARTY_MEMBER, this );
	GetSystem()->Register( SystemType::eCHANGE_PARTY_MASTER, this );
	GetSystem()->Register( SystemType::eUPDATE_MEMBER_INFO, this );
	GetSystem()->Register( SystemType::eMEMBER_MAP_CHANGE, this );	
	GetSystem()->Register( SystemType::eMEMBER_DIGIMON_CHANGE, this );	
	GetSystem()->Register( SystemType::eMEMBER_LEAVE, this );	
	GetSystem()->Register( SystemType::eMEMBER_BANISH, this );	
	GetSystem()->Register( SystemType::eMEMBER_SET_TARGET, this );	
	GetSystem()->Register( SystemType::eMEMBER_RELEASE_TARGET, this );	
	GetSystem()->Register( SystemType::eMEMBER_FARAWAY, this );		
	GetSystem()->Register( SystemType::eMEMBER_REFRESH, this );	
	
	return true;
}

void cPartyWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eMEMBER_REFRESH:
		{
			_RefreshMemberUI();
		}
		break;
	case SystemType::eADD_NEW_PARTY_MEMBER:
		{
			kStream.SetRp(0);
			char addSlotNum = 0;
			kStream >> addSlotNum;

			SMemberInfo const * pInfo = GetSystem()->GetMemberInfoFromSlotNo( addSlotNum );
			if( pInfo )
				_AddNewMember( pInfo );
		}break;
	case SystemType::eCHANGE_PARTY_MASTER:
		{
			_ChangePartyMaster();
		}
		break;
	case SystemType::eUPDATE_MEMBER_INFO:
		{
			char updateSlot = 0;
			kStream >> updateSlot;

			SMemberInfo const * pInfo = GetSystem()->GetMemberInfoFromSlotNo( updateSlot );
			if( pInfo )
				_UpdateMemberInfo( pInfo );
		}
		break;
	case SystemType::eMEMBER_MAP_CHANGE:
		break;
	case SystemType::eMEMBER_DIGIMON_CHANGE:
		{
			char updateSlot = 0;
			kStream >> updateSlot;

			SMemberInfo const * pInfo = GetSystem()->GetMemberInfoFromSlotNo( updateSlot );
			if( pInfo )
				_ChangeMemberDigimon( pInfo );
		}
		break;
	case SystemType::eMEMBER_BANISH:
	case SystemType::eMEMBER_LEAVE:
		{
			kStream.SetRp(0);
			char leaveMemberSlot = 0;
			kStream >> leaveMemberSlot;
			_DeleteMemberUIList( leaveMemberSlot );
		}
		break;
	case SystemType::eMEMBER_SET_TARGET:
		{
			int nSlotNum = 0;
			kStream >> nSlotNum;
			_SelectedMemberTarget( nSlotNum );
		}
		break;
	case SystemType::eMEMBER_RELEASE_TARGET:
		{
			int nSlotNum = 0;
			kStream >> nSlotNum;
			_ReleasedMemberTarget( nSlotNum );
		}
		break;
	case SystemType::eMEMBER_FARAWAY:
		{
			int nSlotNum = 0;
			bool bFaraway = false;
			kStream >> nSlotNum >> bFaraway;
			_ChangeFaraway( nSlotNum, bFaraway );
		}
		break;
	}
}

void cPartyWindow::_SelectedMemberTarget(int const& nSlot)
{
	SAFE_POINTER_RET(m_pPartyUIList);
	MAP_MEMBER_UI_IT it = m_mapMemberControls.find( nSlot );
	if( it == m_mapMemberControls.end() )
		return;

	m_pPartyUIList->SetSelectedItemFromItemPtr( it->second.m_BoxItem, false );	
}

void cPartyWindow::_ReleasedMemberTarget(int const& nSlot)
{
	SAFE_POINTER_RET(m_pPartyUIList);
	MAP_MEMBER_UI_IT it = m_mapMemberControls.find( nSlot );
	if( it == m_mapMemberControls.end() )
		return;

	m_pPartyUIList->UnSelectionItem( it->second.m_BoxItem );	
}


void cPartyWindow::_ChangeFaraway(int const& nSlot, bool bFaraway)
{
	SAFE_POINTER_RET(m_pPartyUIList);
	MAP_MEMBER_UI_IT it = m_mapMemberControls.find( nSlot );
	if( it == m_mapMemberControls.end() )
		return;

	it->second.SetFaraway( bFaraway );
}