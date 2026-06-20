#include "stdafx.h"
#include "GiftBoxUI.h"


#define ANI_ELITE_COUNT 3
#define ANI_UNIQUE_COUNT 4
#define ANI_RANK_ELITE 1
#define ANI_RANK_UNIQUE 2

cGiftBoxUI::cGiftBoxUI() : 
m_pItemImg( NULL ), 
m_pEffectAni( NULL ), 
m_pAcquireTxt( NULL ), 
m_bEndEffect( true )
{

}

cGiftBoxUI::~cGiftBoxUI()
{

}

bool cGiftBoxUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eAdd_GiftBoxWindow, this );
	return true;
}

void cGiftBoxUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eAdd_GiftBoxWindow:
		{
			uint nIconNo = 0;
			uint nItemCount = 0;
			std::wstring wsItemName;
			int nRank = 0;
			kStream >> nIconNo >> nItemCount >> wsItemName >> nRank;

			sOpenedItem sItem;
			sItem.SetItem( nIconNo, nItemCount, wsItemName, nRank );

			_SetGiftBox( sItem );
		}
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cGiftBoxUI::GetWindowType()
{
	return cBaseWindow::WT_GIFTBOX;
}

void cGiftBoxUI::DeleteResource()
{
	DeleteScript();
}

void cGiftBoxUI::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cGiftBoxUI::Create(int nValue /*= 0 */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, "GiftBox\\Unique_Box_bg.tga", CsPoint( (1024 - 954)/2, (768 - 294)/2 - 200 ), CsPoint( 954, 294 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::TOP );

	m_pItemImg = AddSprite( CsPoint( 954/2 - 38, 68 ), CsPoint( 80, 80 ), "" );
	
	m_pEffectAni = NiNew cSpriteAni;
	m_pEffectAni->Init( cSpriteAni::LOOP_CNT, m_pRoot, CsPoint( 954/2 - 252, -90 ), CsPoint( 504, 452 ), "GiftBox\\Unique_Effect_Box_", "dds", 20, false );
	m_pEffectAni->SetAniTime( 0.05f );
	m_pEffectAni->PlayOff();
	m_pEffectAni->SetLoopCount( ANI_ELITE_COUNT );
	m_pEffectAni->AddEvent( cSpriteAni::SPRITEANI_END, this, &cGiftBoxUI::_NextOpenedItem );
	AddChildControl( m_pEffectAni );

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_14, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "GIFTBOX_ITEMNAME_TEXT" ).c_str() );
	m_pAcquireTxt = AddText( &ti, CsPoint( 954/2, 180 ) );
}

void cGiftBoxUI::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);

	if( !m_bEndEffect )
		GetSystem()->CloseGiftBoxWindow();
}

void cGiftBoxUI::Render()
{
	RenderScript();
}

void cGiftBoxUI::ResetMap()
{
	GetSystem()->CloseGiftBoxWindow();
}

void cGiftBoxUI::ResetDevice()
{
	ResetDeviceScript();
}

void cGiftBoxUI::_AddGiftBox(sOpenedItem sItem)
{
	//if( !m_pUniqueAni->IsPlay() )
		_SetGiftBox( sItem );

	//m_lOpenList.push_back( sItem );
}

void cGiftBoxUI::_SetGiftBox(sOpenedItem sItem)
{
	// 아이템 이미지 세팅
	ICON_Dynamic::eTYPE nIconType = (ICON_Dynamic::eTYPE)(sItem.nIconNo / 100);
	cIcon* pIcon = g_pIconMng->GetIcon(nIconType);
	if( pIcon )
	{
		cSprite* IconSprite = g_pIconMng->GetSprite(nIconType);
		if( IconSprite )
		{
			int nCashshopItemIdx = sItem.nIconNo % 100;	
			int nColInTextrue = nCashshopItemIdx % pIcon->GetIndexCount().x;
			int nRowInTextrue = nCashshopItemIdx / pIcon->GetIndexCount().x;
			int Size = IconSprite->GetSize().x;

			m_pItemImg->ChangeTexture( IconSprite->GetFilePath().c_str(), 
				CsRect( CsPoint(nColInTextrue * Size, nRowInTextrue * Size), CsPoint((nColInTextrue + 1) * Size , (nRowInTextrue + 1) * Size)) );
		}
	}

	// 아이템 이름 & 개수 세팅
	std::wstring wsMsg = UISTRING_TEXT( "GIFTBOX_ITEMNAME_TEXT" );
	DmCS::StringFn::ReplaceAll( wsMsg, L"#ItemName#", sItem.wsItemName );
	DmCS::StringFn::ReplaceAll( wsMsg, L"#Count#", sItem.nItemCount );
	m_pAcquireTxt->SetText( wsMsg.c_str() );

	_SetAniEffect( sItem.nItemRank );
}

void cGiftBoxUI::_NextOpenedItem(void* pSender, void* pData)
{
// 	m_lOpenList.pop_front();
// 	if( m_lOpenList.empty() )
// 	{
// 		m_bEndEffect = false;
// 		return;
// 	}
// 	std::list< sOpenedItem >::iterator it = m_lOpenList.begin();
// 	if( it != m_lOpenList.end() )
// 		_SetGiftBox( *it );

	m_bEndEffect = false;
}

void cGiftBoxUI::_SetAniEffect(int nRank)
{
	switch( nRank )
	{
	case ANI_RANK_ELITE:
		{
			m_pEffectAni->ChangeTexture( "GiftBox\\Elite_Effect_Box_", "dds" );
			m_pEffectAni->SetLoopCount( ANI_ELITE_COUNT );
			cWindow::PlaySound( "System\\03_result_[elite].wav" );
		}
		break;
	case ANI_RANK_UNIQUE:
		{
			m_pEffectAni->ChangeTexture( "GiftBox\\Unique_Effect_Box_", "dds" );
			m_pEffectAni->SetLoopCount( ANI_UNIQUE_COUNT );
			cWindow::PlaySound( "System\\04_result_[unique]_3sec.wav" );
		}
		break;
	}

	m_pEffectAni->ResetAni();
	m_pEffectAni->PlayOn( true );

	m_bEndEffect = true;
}
