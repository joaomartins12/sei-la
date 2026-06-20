#include "stdafx.h"
#include "TutorialGuideWindow.h"

cTutorialTraceWindow::cTutorialTraceWindow()
{
}

cTutorialTraceWindow::~cTutorialTraceWindow()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cTutorialTraceWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void cTutorialTraceWindow::PreResetMap()
{
	Destroy();
}

void cTutorialTraceWindow::ResetMap()
{
	Destroy();
}

void cTutorialTraceWindow::DeleteResource()
{
	DeleteScript();	
}

void cTutorialTraceWindow::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint::ZERO, CsPoint( 60, 45 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );	

	cSpriteAni* pArrowAni = NiNew cSpriteAni;
	if( pArrowAni )
	{
		pArrowAni->Init( cSpriteAni::LOOP, m_pRoot, CsPoint::ZERO, CsPoint( 60, 45 ), "Tutorial\\tutorial_ani.tga", NULL, 11, false, CsPoint(60,0) , cSpriteAni::SPRITE_POS );
		pArrowAni->SetAniTime(0.1f);
		AddChildControl(pArrowAni);
	}
}

void cTutorialTraceWindow::_TraceItemUI()
{
	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInvenData = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInvenData );

	int nSlot =pInvenData->GetFirstSlot_Item_ID( m_CurrentData.m_dwTargetValue );
	int nInvenCount = nSlot / nLimit::FirstInven;
	cInventory* pInven = g_pGameIF->GetInventory( nInvenCount );
	SAFE_POINTER_RET( pInven );

	SetVisible(pInven->IsShowWindow());
	CsPoint itemPo = pInven->GetInvenItemPos( nSlot );
	CsPoint pos = itemPo - CsPoint(m_ptRootSize.x - 10, 0);
	SetWindowPosition( pos );
}

void cTutorialTraceWindow::_TraceTalkQeustUI()
{
	SAFE_POINTER_RET( g_pGameIF );

	SetVisible(false);
	cBaseWindow* pBase = g_pGameIF->GetWindow( m_CurrentData.m_wsTargetWindowName.c_str() );
	SAFE_POINTER_RET( pBase );

	if( cBaseWindow::WT_TALK != pBase->GetWindowType() )
		return;

	cTalk* pTalkWin = static_cast<cTalk*>(pBase);
	CsRect pControlPos = pTalkWin->GetControlPos( m_CurrentData.m_dwTargetValue, m_CurrentData.m_dwTargetValueSub );
	if( pControlPos == CsRect::ZERO )
		return;

	SetVisible( true );
	CsPoint controlPos = pControlPos.GetPos();
	BHPRT( "talk Control World Pos : %d, %d", controlPos.x, controlPos.y );
	CsPoint pos =  controlPos + CsPoint( -(m_ptRootSize.x - 10), (pControlPos.Height() - m_ptRootSize.y) / 2 );
	BHPRT( "talk Total World Pos : %d, %d", pos.x, pos.y );
	SetWindowPosition( pos );
	return;
}

void cTutorialTraceWindow::_TraceTalkWindowUI()
{
	SAFE_POINTER_RET( g_pGameIF );

	SetVisible(false);
	cBaseWindow* pBase = g_pGameIF->GetWindow( m_CurrentData.m_wsTargetWindowName.c_str() );
	SAFE_POINTER_RET( pBase );

	if( cBaseWindow::WT_TALK != pBase->GetWindowType() )
		return;

	cTalk* pTalkWin = static_cast<cTalk*>(pBase);
	pTalkWin->EnableString( 0, m_CurrentData.m_dwTargetValue, false );

	CsRect pControlPos = pTalkWin->GetControlPos( m_CurrentData.m_dwTargetValue );
	if( pControlPos == CsRect::ZERO )
		return;

	SetVisible( true );
	CsPoint controlPos = pControlPos.GetPos();
	CsPoint pos = controlPos + CsPoint( -(m_ptRootSize.x- 10), (pControlPos.Height() - m_ptRootSize.y) / 2 );
	SetWindowPosition( pos );
	return;
}

void cTutorialTraceWindow::_TraceWindowControlUI()
{
	SAFE_POINTER_RET( g_pGameIF );

	cBaseWindow* pBase = g_pGameIF->GetWindow( m_CurrentData.m_wsTargetWindowName.c_str() );
	if( NULL == pBase )	
	{
		SetVisible(false);		
		return;
	}

	cWindow* pControl = NULL;
	if(0 == m_CurrentData.m_wsTargetName.compare( "ZoneMapControl" ) )
	{
		if( cBaseWindow::WT_ZONEMAP != pBase->GetWindowType() )
		{
			SetVisible(false);
			return;
		}
		pControl = static_cast<cZoneMap*>(pBase)->GetMapObjectControl( m_CurrentData.m_dwTargetValue, m_CurrentData.m_dwTargetValueSub );
	}
	else if( cBaseWindow::WT_ENCHANT_TACTICS == pBase->GetWindowType() )
	{
		if( 0 == m_CurrentData.m_wsTargetName.compare( "State_AT" ) || 
			0 == m_CurrentData.m_wsTargetName.compare( "State_BL" ) || 
			0 == m_CurrentData.m_wsTargetName.compare( "State_CT" ) || 
			0 == m_CurrentData.m_wsTargetName.compare( "State_EV" ) )
		{
			CsPoint pPos = static_cast<cEnchantTactics*>(pBase)->StateControlPos( m_CurrentData.m_wsTargetName );
			if( pPos != CsPoint::ZERO )
			{
				SetWindowPosition( pPos );
				SetVisible( true );
			}
			else
				SetVisible( false );
			return;
		}
		else
			pControl = pBase->GetChildControl( m_CurrentData.m_wsTargetName );
	}
	else
		pControl = pBase->GetChildControl( m_CurrentData.m_wsTargetName );

	if( NULL == pControl || !pControl->GetVisible() )
	{
		SetVisible( false );
		return;
	}

	CsPoint centerPos = (pControl->GetSize() - m_ptRootSize) / 2;

	CsPoint pos = pControl->GetClient() + CsPoint(-(m_ptRootSize.x- 10), centerPos.y);

	SetWindowPosition( pos );
	SetVisible( true );
}

void cTutorialTraceWindow::_TraceQuickSlotUI()
{
	SAFE_POINTER_RET( g_pGameIF );

	cQuickSlot* pQuickSlot = NULL;
	if( DmCS::StringFn::iCmp( m_CurrentData.m_wsTargetName.c_str(), "Digimon" ) )
		pQuickSlot = g_pGameIF->GetQuickSlot( cQuickSlot::Q_DIGIMON );
	else if( DmCS::StringFn::iCmp( m_CurrentData.m_wsTargetName.c_str(), "Tamer" ) )
		pQuickSlot = g_pGameIF->GetQuickSlot( cQuickSlot::Q_TAMER );
	SAFE_POINTER_RET( pQuickSlot );

	pQuickSlot->LockSlotAll();
	int nSlotIndex = pQuickSlot->GetSlotIndex( m_CurrentData.m_dwTargetValue, m_CurrentData.m_dwTargetValueSub );
	if( nSlotIndex == -1 )
	{
		SetVisible( false );
		return;
	}

	pQuickSlot->UnlockSlot( nSlotIndex );

	CsPoint centerPos = (CsPoint( 32, 32 ) - m_ptRootSize) / 2;
	CsPoint pos = pQuickSlot->GetSlotPos( nSlotIndex ) + CsPoint(-(m_ptRootSize.x- 10), centerPos.y);

	SetWindowPosition( pos );
	SetVisible( true );
}

void cTutorialTraceWindow::Update(float const& fDeltaTime)
{
	if( m_CurrentData.m_wsTargetWindowName.empty() )
	{
		Close();
		return;
	}

	switch( m_CurrentData.m_nType )
	{
	case sTraceData::eItem:		_TraceItemUI();				break;
	case sTraceData::eQuest:	_TraceTalkQeustUI();		break;
	case sTraceData::eWindow:	_TraceTalkWindowUI();		break;
	case sTraceData::eControl:	_TraceWindowControlUI();	break;
	case sTraceData::eQuickSlot:_TraceQuickSlotUI();		break;
	} 

	UpdateScript( fDeltaTime );
}

cBaseWindow::eMU_TYPE cTutorialTraceWindow::Update_ForMouse()
{
	return MUT_OUT_WINDOW;
}

void cTutorialTraceWindow::Render()
{
	RenderScript();	
}


void cTutorialTraceWindow::ResetDevice()
{	
	ResetDeviceScript();
}

void cTutorialTraceWindow::_UpdateUITraceData( sTraceData const& newData )
{		
	m_CurrentData = newData;
	if(0 == m_CurrentData.m_wsTargetWindowName.compare("empty") )
		m_CurrentData.m_wsTargetWindowName.clear();
	if(0 == m_CurrentData.m_wsTargetName.compare("empty") )
		m_CurrentData.m_wsTargetName.clear();
}

//////////////////////////////////////////////////////////////////////////
bool cTutorialTraceWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdateUITrace, this );
	return true;
}

void cTutorialTraceWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eUpdateUITrace:
		{
			sTraceData newData;
			kStream >> newData.m_wsTargetWindowName >> newData.m_nType >> newData.m_wsTargetName >> newData.m_dwTargetValue >> newData.m_dwTargetValueSub;
			_UpdateUITraceData( newData );
		}
		break;
	}
}
