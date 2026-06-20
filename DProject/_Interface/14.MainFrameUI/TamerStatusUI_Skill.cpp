#include "stdafx.h"
#include "TamerStatusUI_Skill.h"


#define IF_SKILL_COUNT 12

cTamerStatusUI_Skill::cTamerStatusUI_Skill()
: m_pBasicSkill(NULL),
m_pActiveSkill(NULL),
m_pPassiveSkill(NULL),
m_pBasicText(NULL),
m_pActiveText(NULL),
m_pPassiveText(NULL)
{
	Construct();
}

cTamerStatusUI_Skill::~cTamerStatusUI_Skill()
{
	Destroy();
}

bool cTamerStatusUI_Skill::Construct(void)
{
	if( !cTamerStatusUI_SubViewer::Construct() )
		return false;
	return true;
}

void cTamerStatusUI_Skill::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

void cTamerStatusUI_Skill::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	DeleteResource();
}

void cTamerStatusUI_Skill::DeleteResource()
{
	DeleteScript();
}

void cTamerStatusUI_Skill::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint::ZERO, CsPoint( 406, 676 ), true );
	m_pBasicSkill = NiNew cGridListBox;
	if( m_pBasicSkill )
	{
		m_pBasicSkill->Init( pkRoot, CsPoint( 37, 135 ), CsPoint( 332, 92 ), CsPoint( 28, 28 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 6 );
		m_pBasicSkill->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pBasicSkill->SetBackOverAndSelectedImgRender( false );
		m_pBasicSkill->SetAutoSelection( false );
		m_pBasicSkill->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT,	this, &cTamerStatusUI_Skill::_OnRClickSkill );
		m_pBasicSkill->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM,		this, &cTamerStatusUI_Skill::_OnLClickDownSkill );
		AddChildControl( m_pBasicSkill );
	}
	_MakeBasicSkill();

	m_pActiveSkill = NiNew cGridListBox;
	if( m_pActiveSkill )
	{
		m_pActiveSkill->Init( pkRoot, CsPoint( 37, 341 ), CsPoint( 332, 110 ), CsPoint( 28, 10 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 6 );
		m_pActiveSkill->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pActiveSkill->SetBackOverAndSelectedImgRender( false );
		m_pActiveSkill->SetAutoSelection( false );
		m_pActiveSkill->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT,	this, &cTamerStatusUI_Skill::_OnRClickSkill );
		m_pActiveSkill->AddEvent( cGridListBox::GRID_BUTTON_UP,				this, &cTamerStatusUI_Skill::_OnClickDeleteSkill );
		m_pActiveSkill->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM,		this, &cTamerStatusUI_Skill::_OnLClickDownSkill );
		AddChildControl( m_pActiveSkill );
	}
	_MakeActiveSkill();

	m_pPassiveSkill = NiNew cGridListBox;
	if( m_pPassiveSkill )
	{
		m_pPassiveSkill->Init( GetRoot(), CsPoint( 37, 547 ), CsPoint( 332, 92 ), CsPoint( 28, 28 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 6 );
		AddChildControl( m_pPassiveSkill );
	}
	_MakePassiveSkill();

	{	// 기본 스킬
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 81.f/255.f, 151.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_BASICSKILL_TEXT" ).c_str() );
		m_pBasicText = AddText( &ti, CsPoint( 22, 91 ) );
	}

	{	// 액티브 스킬
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 188.f/255.f, 97.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_ACTIVESKILL_TEXT" ).c_str() );
		m_pActiveText = AddText( &ti, CsPoint( 22, 297 ) );
	}
	
	{	// 패시브 스킬
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 122.f/255.f, 254.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_PASSIVESKILL_TEXT" ).c_str() );
		m_pPassiveText = AddText( &ti, CsPoint( 22, 503 ) );
	}
}

void cTamerStatusUI_Skill::Update(float const& fDeltaTime)
{

}

BOOL cTamerStatusUI_Skill::Update_ForMouse()
{
	if( m_pBasicSkill && m_pBasicSkill->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetTooltip( m_pBasicSkill->GetMouseOverItem() );

	if( m_pActiveSkill && m_pActiveSkill->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetTooltip( m_pActiveSkill->GetMouseOverItem() );

	if( m_pPassiveSkill && m_pPassiveSkill->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetTooltip( m_pPassiveSkill->GetMouseOverItem() );
	return FALSE;
}

void cTamerStatusUI_Skill::RenderScript()
{
	cScriptUI::RenderScript();

	if( !IsVisible() )
		return;
}

void cTamerStatusUI_Skill::ResetDevice()
{
	ResetDeviceScript();
}

void cTamerStatusUI_Skill::ReleaseSelect()
{

}

void cTamerStatusUI_Skill::UpdateViewer()
{
	std::map< int, sSkill >::iterator it = m_mapBasicSkill.begin();
	std::map< int, sSkill >::iterator itEnd = m_mapBasicSkill.end();
	for( int i = 0; it != itEnd; ++it, ++i )
	{
		int nSkillType = GetSystem()->GetSkillType( it->first, SystemType::eBasic );
		int nSkillIndex = GetSystem()->GetSkillIndex( i, SystemType::eBasic );
		CsCoolTimeSeq* pCoolTime = GetSystem()->GetSkillCoolTime( nSkillIndex );
		it->second.ChangeSkill( nSkillType, nSkillIndex, pCoolTime );
	}

	it = m_mapActiveSkill.begin();
	itEnd = m_mapActiveSkill.end();
	for( int i = 0; it != itEnd; ++it, ++i )
	{
		int nSkillType = GetSystem()->GetSkillType( it->first, SystemType::eActive );
		int nSkillIndex = GetSystem()->GetSkillIndex( i, SystemType::eActive );
		CsCoolTimeSeq* pCoolTime = GetSystem()->GetSkillCoolTime( nSkillIndex );
		it->second.ChangeSkill( nSkillType, nSkillIndex, pCoolTime );
		bool bIsCash = GetSystem()->IsCashSkill( it->first );
		it->second.SetDeleteButton( bIsCash );
	}
}

void cTamerStatusUI_Skill::OnLButtonUp(CsPoint pos)
{
	bool bSetIcon = false;
	if( m_pBasicSkill )
	{
		cGridListBoxItem const* pOverItem = m_pBasicSkill->GetMouseOverItem();
		if( _OnClickSkill( pOverItem ) )
			return;
	}

	if( m_pActiveSkill )
	{
		cGridListBoxItem const* pOverItem = m_pActiveSkill->GetMouseOverItem();
		if( _OnClickSkill( pOverItem ) )
			return;
	}

	CURSOR_ST.ReleaseIcon();
}

bool cTamerStatusUI_Skill::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	CURSOR_ST.ReleaseIcon();
	return false;
}

BOOL cTamerStatusUI_Skill::_SetTooltip(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, FALSE );

	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pSkillInfo, FALSE );

	if( -1 == pSkillInfo->nSkillIndex )
		return FALSE;

	cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pToolTip, FALSE );
#ifdef VERSION_USA
	pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), 360, cTooltip::SKILL, pSkillInfo->nSkillIndex, 0, cBaseWindow::WT_NEW_TAMERSTATUS );
#else
	pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::SKILL, pSkillInfo->nSkillIndex, 0, cBaseWindow::WT_NEW_TAMERSTATUS );
#endif
	return TRUE;
}

void cTamerStatusUI_Skill::_MakeBasicSkill()
{
	SAFE_POINTER_RET( m_pBasicSkill );
	m_pBasicSkill->RemoveAllItem();

	for( int i = 0; i < IF_SKILL_COUNT; ++i )
	{
		_AddBasicSkill( i );
	}
}

void cTamerStatusUI_Skill::_AddBasicSkill(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	int nSkillType = GetSystem()->GetSkillType( nIndex, SystemType::eBasic );
	int nSkillIndex = GetSystem()->GetSkillIndex( nIndex, SystemType::eBasic );
	CsCoolTimeSeq* pCoolTime = GetSystem()->GetSkillCoolTime( nIndex );

	sSkill sBasic;
	sBasic.pSkillIcon = pControl->AddSkillIcon( CsPoint( 32, 32 ), nSkillType, nSkillType, nSkillIndex );
	if( sBasic.pSkillIcon )
	{
		if( -1 == nSkillType )
			sBasic.pSkillIcon->SetVisible( false );
		sBasic.pSkillIcon->SetAutoPointerDelete( true );
		if( pCoolTime )
			sBasic.pSkillIcon->SetCoolTimeMode( true );
		else
			sBasic.pSkillIcon->SetCoolTimeMode( false );
		sBasic.pSkillIcon->SetCoolTimeSeq( pCoolTime );
	}

	sBasic.pGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	if( sBasic.pGridItem )
	{
		sBasic.pGridItem->SetItem( pControl );
		sBasic.pGridItem->SetUserData( new sSkillInfo( nSkillType, nSkillIndex, eBasicSkill ) );
		m_pBasicSkill->AddItem( sBasic.pGridItem );
		m_mapBasicSkill.insert( std::make_pair( nIndex, sBasic ) );
	}
}

void cTamerStatusUI_Skill::_MakeActiveSkill()
{
	SAFE_POINTER_RET( m_pActiveSkill );
	m_pActiveSkill->RemoveAllItem();

	for( int i = 0; i < IF_SKILL_COUNT; ++i )
	{
		_AddActiveSkill( i );
	}
}

void cTamerStatusUI_Skill::_AddActiveSkill(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	int nSkillType = GetSystem()->GetSkillType( nIndex, SystemType::eActive );
	int nSkillIndex = GetSystem()->GetSkillIndex( nIndex, SystemType::eActive );
	bool bEnable = GetSystem()->IsCashSkill( nIndex );
	CsCoolTimeSeq* pCoolTime = GetSystem()->GetSkillCoolTime( nSkillIndex );

	sSkill sActive;
	sActive.pSkillIcon = pControl->AddSkillIcon( CsPoint( 32, 32 ), nSkillType, nSkillType, nSkillIndex );
	if( sActive.pSkillIcon )
	{
		if( -1 == nSkillType )
			sActive.pSkillIcon->SetVisible( false );
		sActive.pSkillIcon->SetAutoPointerDelete( true );
		if( pCoolTime )
			sActive.pSkillIcon->SetCoolTimeMode( true );
		else
			sActive.pSkillIcon->SetCoolTimeMode( false );
		sActive.pSkillIcon->SetCoolTimeSeq( pCoolTime );
	}

	cButton* pDeleteBtn = NiNew cButton;
	pDeleteBtn->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "System\\Ch_Close.tga", false );
	pDeleteBtn->SetTexToken( CsPoint( 0, 16 ) );
	pDeleteBtn->SetUserData( new sSkillInfo( nSkillType, nIndex, eActiveSkill ) );
	sActive.pSkillDelBtn = pControl->AddButton( pDeleteBtn, nIndex, CsPoint( 10, 37 ), CsPoint( 12, 12 ) );
	if( sActive.pSkillDelBtn )
	{
		sActive.pSkillDelBtn->SetVisible( bEnable );
		sActive.pSkillDelBtn->SetEnable( bEnable );
		sActive.pSkillDelBtn->SetAutoPointerDelete( true );
	}

	sActive.pGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 50 ) );
	if( sActive.pGridItem )
	{
		sActive.pGridItem->SetItem( pControl );
		sActive.pGridItem->SetUserData( new sSkillInfo( nSkillType, nSkillIndex, eActiveSkill ) );
		m_pActiveSkill->AddItem( sActive.pGridItem );
		m_pActiveSkill->SetItemSize( CsPoint( 32, 50 ) );
		sActive.pGridItem->setItemSize( CsPoint( 32, 50 ) );
		m_mapActiveSkill.insert( std::make_pair( nIndex, sActive ) );
	}
}

void cTamerStatusUI_Skill::_MakePassiveSkill()
{
	SAFE_POINTER_RET( m_pPassiveSkill );
	m_pPassiveSkill->RemoveAllItem();

	for( int i = 0; i < IF_SKILL_COUNT; ++i )
	{
		_AddPassiveSkill( i, GetSystem()->GetSkillType( i, SystemType::ePassive ) );
	}
}

void cTamerStatusUI_Skill::_AddPassiveSkill(int nIndex, int nIconType)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	int nSkillType = GetSystem()->GetSkillType( nIndex, SystemType::ePassive );
	int nSkillIndex = GetSystem()->GetSkillIndex( nIndex, SystemType::ePassive );

	sSkill sPassive;
	sPassive.pSkillIcon = pControl->AddSkillIcon( CsPoint( 32, 32 ), nSkillType, nSkillType, nSkillIndex );
	if( sPassive.pSkillIcon )
	{
		if( -1 == nSkillType )
			sPassive.pSkillIcon->SetVisible( false );
		sPassive.pSkillIcon->SetAutoPointerDelete( true );
	}

	sPassive.pGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	if( sPassive.pGridItem )
	{
		sPassive.pGridItem->SetItem( pControl );
		sPassive.pGridItem->SetUserData( new sSkillInfo( nSkillType, nSkillIndex, ePassiveSkill ) );
		m_pPassiveSkill->AddItem( sPassive.pGridItem );
		m_mapPassiveSkill.insert( std::make_pair( nIndex, sPassive ) );
	}
}

void cTamerStatusUI_Skill::_OnLClickDownSkill(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );

	if( _OnClickSkill( pOverItem ) )
		return;

	CURSOR_ST.ReleaseIcon();
}

void cTamerStatusUI_Skill::_OnRClickSkill(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pSkill = static_cast< cGridListBoxItem* >( pData );
	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pSkill->GetUserData() );
	SAFE_POINTER_RET( pSkillInfo );

	// use skill
	GetSystem()->UseSkill( pSkillInfo->nSkillIndex );
}

void cTamerStatusUI_Skill::_OnClickDeleteSkill(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cString::sBUTTON* pSkill = static_cast< cString::sBUTTON* >( pData );
	cButton* pSkillBtn = pSkill->s_pButton;
	SAFE_POINTER_RET( pSkillBtn );
	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pSkillBtn->GetUserData() );
	SAFE_POINTER_RET( pSkillInfo );

	// delete skill
	GetSystem()->DeleteSkill( pSkillInfo->nSkillIndex );

	CURSOR_ST.ReleaseIcon();
}

bool cTamerStatusUI_Skill::_OnClickSkill(cGridListBoxItem const* pOverItem)
{
	bool bSetItem = false;
	if( !pOverItem )
	{
		CURSOR_ST.ReleaseIcon();
		return bSetItem;
	}

	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pOverItem->GetUserData() );
	if( pSkillInfo )
	{
		if( eActiveSkill == pSkillInfo->eSType )
		{
			if( -1 != pSkillInfo->nSkillIndex )
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TSKILL, pSkillInfo->nSkillIndex, 1, NULL );
				bSetItem = true;
			}
		}
		else if( eBasicSkill == pSkillInfo->eSType )
		{
			if( -1 != pSkillInfo->nSkillIndex )
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TSKILL, pSkillInfo->nSkillIndex, 1, NULL );
				bSetItem = true;
			}
		}
	}

	if( !bSetItem )
		CURSOR_ST.ReleaseIcon();

	return bSetItem;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void cTamerStatusUI_Skill::sSkill::ChangeSkill(int nSkillType, int nSkillIndex, CsCoolTimeSeq* pCoolTime)
{
	SAFE_POINTER_RET( pGridItem );
	SAFE_POINTER_RET( pSkillIcon );

	pSkillIcon->ChangeSkillIcon( nSkillType, nSkillType, nSkillIndex );
	pSkillIcon->SetCoolTimeSeq( pCoolTime );
	if( !pCoolTime )
		pSkillIcon->SetCoolTimeMode( false );
	else
		pSkillIcon->SetCoolTimeMode( true );

	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pGridItem->GetUserData() );
	if( pSkillInfo )
	{
		pSkillInfo->nSkillIconType = nSkillType;
		pSkillInfo->nSkillIndex = nSkillIndex;
	}

	if( -1 == nSkillType )
	{
		pSkillIcon->SetVisible( false );
		pGridItem->SetEnable( false );
	}
	else
	{
		pSkillIcon->SetVisible( true );
		pGridItem->SetEnable( true );
	}
}

void cTamerStatusUI_Skill::sSkill::SetDeleteButton(bool bEnable)
{
	SAFE_POINTER_RET( pSkillDelBtn );
	pSkillDelBtn->SetEnable( bEnable );
	pSkillDelBtn->SetVisible( bEnable );
}

