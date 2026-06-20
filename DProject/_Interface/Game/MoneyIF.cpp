
#include "stdafx.h"
#include "MoneyIF.h"

cSprite*		cMoneyIF::g_pBG = NULL;
cSprite*		cMoneyIF::g_pMbox = NULL;

cSprite*		cMoneyIF::g_pUnit[ UNIT_MAX_COUNT ] = { 0, };

void cMoneyIF::GlobalShotDown()
{
	SAFE_NIDELETE( g_pBG );
	SAFE_NIDELETE( g_pMbox );
	for( int i=0; i<UNIT_MAX_COUNT; ++i )
		SAFE_NIDELETE( g_pUnit[ i ] );
}

void cMoneyIF::GlobalInit()
{
	assert_cs( g_pBG == NULL );
	assert_cs( g_pMbox == NULL );
	assert_cs( g_pUnit[ BIT ] == NULL );
	assert_cs( g_pUnit[ MEGA ] == NULL );
	assert_cs( g_pUnit[ TERA ] == NULL );
	assert_cs( g_pUnit[ DIGICORE ] == NULL );
	assert_cs( g_pUnit[ EVENT ] == NULL );
	g_pBG = NiNew cSprite;
	g_pBG->Init( NULL, CsPoint::ZERO, CsPoint( 144, 20 ), "Money\\Money_Box.tga", false );

	g_pUnit[ BIT ] = NiNew cSprite;
	g_pUnit[ BIT ]->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "Money\\Money_Bit.tga", false );

	g_pUnit[ MEGA ] = NiNew cSprite;
	g_pUnit[ MEGA ]->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "Money\\Money_Mega.tga", false );

	g_pUnit[ TERA ] = NiNew cSprite;
	g_pUnit[ TERA ]->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "Money\\Money_Tera.tga", false );

	g_pUnit[ DIGICORE ] = NiNew cSprite;
	g_pUnit[ DIGICORE ]->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "Money\\Money_Digicore.tga", false );

	g_pUnit[ EVENT ] = NiNew cSprite;
	g_pUnit[ EVENT ]->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "Money\\Event_Bit.tga", false );

	g_pMbox = NiNew cSprite;
	g_pMbox->Init( NULL, CsPoint::ZERO, CsPoint( 200, 20 ), "Money\\Money_Box.tga", false );
}

cMoneyIF::cMoneyIF():m_dwIconItemCode(0)
{
	m_pString = NULL;
	m_fAlpha = 1.0f;
	m_tempPt = CsPoint(421, 336);
}

void cMoneyIF::Delete()
{
	m_dwIconItemCode = 0;

	SAFE_NIDELETE( m_pString );
}

void cMoneyIF::Init_ForEventStore( DWORD dwItemCode, eDISP_TYPE dt, sMONEY money, bool bShowBackground, NiColor color /*FONT_WHITE*/, int eAllign /*=DT_RIGHT*/, CFont::eFACE_SIZE eFontSize /*=CFont::FS_9*/ )
{
	m_eDisplayType = dt;
	m_bShowBG = bShowBackground;
	m_Color = color;
	m_eAllign = eAllign;
	m_eFontSize = eFontSize;
	m_dwIconItemCode = dwItemCode;

	m_pString = NiNew cString;

	m_Money.SetInvalideData();
	SetMoney( money );
}

void cMoneyIF::Init( eDISP_TYPE dt, sMONEY money, bool bShowBackground, NiColor color /*FONT_WHITE*/, int eAllign /*=DT_RIGHT*/, CFont::eFACE_SIZE eFontSize /*=CFont::FS_9*/ )
{
	m_eDisplayType = dt;
	m_bShowBG = bShowBackground;
	m_Color = color;
	m_eAllign = eAllign;
	m_eFontSize = eFontSize;

	m_pString = NiNew cString;

	m_Money.SetInvalideData();
	SetMoney( money );
}

void cMoneyIF::Init_ForPropertyStore( DWORD dwNpcID )
{
	m_eDisplayType = DISP_PROPERTY;
	m_bShowBG = true;
	m_Color = FONT_WHITE;
	m_eAllign = DT_RIGHT;
	m_eFontSize = CFont::FS_9;
	m_dwNpcID = dwNpcID;

	m_pString = NiNew cString;
	m_Money.SetInvalideData();
}

void cMoneyIF::Init( eDISP_TYPE dt, sMONEY money, cItemInfo* ItemInfo, DWORD dwNpcID, int nKind, int nIcon, bool bShowBackground, NiColor color, int eAllign, CFont::eFACE_SIZE eFontSize )
{
	m_eDisplayType = dt;
	m_bShowBG = bShowBackground;
	m_Color = color;
	m_eAllign = eAllign;
	m_eFontSize = eFontSize;

	m_pString = NiNew cString;

	m_dwNpcID = dwNpcID;

	m_Money.SetInvalideData();

	if(ItemInfo)
		SetMoney( money, *ItemInfo );
}

void cMoneyIF::SetMoney( sMONEY money )
{
	if( m_Money == money )
		return;

	m_Money = money;

	m_pString->Delete();
	switch( m_eDisplayType )
	{
	case DISP_FULL:
		_SetFullMoney();
		break;
	case DISP_SIMPLE_1:
		_SetSimpleMoney();
		break;
	case DISP_DIGICORE:
		_SetDigicore();
		break;
	case DISP_EVENT:
		_SetEvent(m_dwIconItemCode);
		break;
	}
}

void cMoneyIF::SetPropertyStorePrice( sMONEY money, int nKind, int nIcon)
{
	m_Money = money;

	switch( nKind )
	{
	case 1:
		_SetPropertyA(nIcon);
		break;
	case 2:
		_SetPropertyB(nIcon);
		break;
	case 3:
		_SetPropertyC(nIcon);
		break;
	case 4:
		_SetPropertyD(nIcon);
		break;	
	}		
}

void cMoneyIF::SetMoney( sMONEY money, cItemInfo ItemInfo )
{
	if( m_eDisplayType == DISP_PROPERTY)
	{
		if( m_Money.GetProperty_A() == money.GetProperty_A() &&
			m_Money.GetProperty_B() == money.GetProperty_B() &&
			m_Money.GetProperty_C() == money.GetProperty_C() &&
			m_Money.GetProperty_D() == money.GetProperty_D() )
			return;

		m_Money = money;

		CsItemExchange::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetExchangeItem( m_dwNpcID, ItemInfo.m_nType )->GetInfo();

		m_pString->Delete();

		if( pFTItemInfo->s_dwExchange_Code_A  != 0 )
			_SetPropertyA( pFTItemInfo->s_dwExchange_Code_A );
		if( pFTItemInfo->s_dwExchange_Code_B  != 0 )
			_SetPropertyB( pFTItemInfo->s_dwExchange_Code_B );	
		if( pFTItemInfo->s_dwExchange_Code_C  != 0 )
			_SetPropertyC( pFTItemInfo->s_dwExchange_Code_C );
		if( pFTItemInfo->s_dwExchange_Code_D  != 0 )
			_SetPropertyD( pFTItemInfo->s_dwExchange_Code_D );
	}
}
#ifndef VERSION_USA
	#define DELTA_IMAGE		0
	#define DELTA_STR		0
#else
	#define DELTA_IMAGE		-1
	#define DELTA_STR		2
#endif

void cMoneyIF::_SetFullMoney()
{
	TCHAR sz[ 32 ] = { 0, };
	m_ptSize.y = 20;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );	

	if( m_Money.GetTera() > 0 )
	{
		ti.SetText( m_Money.GetTera() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ TERA ] , CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
		m_pString->TailAddSizeX( 5 );

		_stprintf_s( sz, 32, _T( "%.3d" ), m_Money.GetMega() );
		ti.SetText( sz );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ MEGA ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
		m_pString->TailAddSizeX( 5 );

		_stprintf_s( sz, 32, _T( "%.3d" ), m_Money.GetBit() );
		ti.SetText( sz );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ BIT ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );

		m_ptSize.x = 144;
	}
	else if( m_Money.GetMega() > 0 )
	{
		ti.SetText( m_Money.GetMega() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ MEGA ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
		m_pString->TailAddSizeX( 5 );

		_stprintf_s( sz, 32, _T( "%.3d" ), m_Money.GetBit() );
		ti.SetText( sz );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ BIT ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );

		m_ptSize.x = 92;
	}
	else
	{
		ti.SetText( m_Money.GetBit() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ BIT ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );

		m_ptSize.x = 46;
	}
}

void cMoneyIF::_SetSimpleMoney()
{
	m_ptSize.y = 20;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );	

	m_ptSize.x = 0;

	if( m_Money.GetTera() > 0 )
	{
		ti.SetText( m_Money.GetTera() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ TERA ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 52;
	}
	if( m_Money.GetMega() > 0 )
	{
		ti.SetText( m_Money.GetMega() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ MEGA ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 46;
	}
	if( ( m_Money.GetBit() > 0 )||( m_ptSize.x == 0 ) )
	{
		ti.SetText( m_Money.GetBit() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddSprite( g_pUnit[ BIT ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 46;
	}
}

void cMoneyIF::_SetDigicore()
{
	m_ptSize.y = 20;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );
	ti.SetText( (int)m_Money.GetInt() );
	m_pString->AddText( &ti );
	m_pString->TailAddSizeX( 2 );
	//m_pString->AddSprite( g_pUnit[ DIGICORE ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
	m_pString->AddIcon( CsPoint( 12, 12 ), ICONITEM::ARMOR_1, 5000/*디지코어 아이템 번호*/, 1, CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) ); // 현욱 추가
	m_pString->TailAddSizeX( 5 );
	m_ptSize.x += 46;
}

void cMoneyIF::_SetEvent( DWORD dwItemCode )
{
	m_ptSize.y = 20;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );
	ti.SetText( (int)m_Money.GetInt() );
	m_pString->AddText( &ti );
	m_pString->TailAddSizeX( 2 );
	if( 0 == dwItemCode )
		m_pString->AddSprite( g_pUnit[ EVENT ], CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
	else
	{
		m_pString->AddIcon( CsPoint( 12, 12 ), ICONITEM::ARMOR_1, dwItemCode, 1, CsPoint( 0, m_eFontSize/6 + DELTA_IMAGE ) );
	}
	m_pString->TailAddSizeX( 5 );
	m_ptSize.x += 46;
}

void cMoneyIF::_SetPropertyA(int nIconID)
{
	if( m_eDisplayType == DISP_PROPERTY)
	{
		m_ptSize.y = 20;

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );
		ti.SetText( m_Money.GetProperty_A() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddIcon( CsPoint( 16, 16 ), ICONITEM::ARMOR_1, nIconID, 1, CsPoint( 0, m_eFontSize/8 + DELTA_IMAGE - 3 ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 46;
	}
}

void cMoneyIF::_SetPropertyB(int nIconID)
{
	if( m_eDisplayType == DISP_PROPERTY)
	{
		m_ptSize.y = 20;

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );
		ti.SetText( (int)m_Money.GetProperty_B() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddIcon( CsPoint( 16, 16 ), ICONITEM::ARMOR_1, nIconID, 1, CsPoint( 0, m_eFontSize/8 + DELTA_IMAGE - 3 ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 46;
	}
}

void cMoneyIF::_SetPropertyC(int nIconID)
{
	if( m_eDisplayType == DISP_PROPERTY)
	{
		m_ptSize.y = 20;

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );
		ti.SetText( (int)m_Money.GetProperty_C() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddIcon( CsPoint( 16, 16 ), ICONITEM::ARMOR_1, nIconID, 1, CsPoint( 0, m_eFontSize/8 + DELTA_IMAGE - 3 ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 46;
	}
}

void cMoneyIF::_SetPropertyD(int nIconID)
{
	if( m_eDisplayType == DISP_PROPERTY)
	{
		m_ptSize.y = 20;

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, m_eFontSize, m_Color );
		ti.SetText( (int)m_Money.GetProperty_D() );
		m_pString->AddText( &ti );
		m_pString->TailAddSizeX( 2 );
		m_pString->AddIcon( CsPoint( 16, 16 ), ICONITEM::ARMOR_1, nIconID, 1, CsPoint( 0, m_eFontSize/8 + DELTA_IMAGE - 3 ) );
		m_pString->TailAddSizeX( 5 );

		m_ptSize.x += 46;
	}
}

void cMoneyIF::ResetDevice()
{ 
	cWindow::ResetDevice();
	if( m_pString )
		m_pString->ResetDevice(); 
}

void cMoneyIF::SetColor_BeforeSetMoney( NiColor color )
{
	m_Color = color;
}

void cMoneyIF::SetColor( NiColor color )
{
	if( m_Color == color )
		return;

	m_Color = color;
	//m_pString->SetColor( color );
}

NiColor cMoneyIF::GetColor() const
{
	return m_Color;
}

void cMoneyIF::Render()
{
	Render(	m_ptClient );
}

void cMoneyIF::Render( CsPoint  pos )
{
	if( !m_bVisible )
		return;

	if( m_eDisplayType == DISP_PROPERTY && m_bShowBG == true )
		g_pMbox->Render( pos - CsPoint(55, 0) );
	else if( m_bShowBG == true )
		g_pBG->Render( pos );

	for( int i=0; i<UNIT_MAX_COUNT; ++i )
		g_pUnit[ i ]->SetSize( CsPoint( m_eFontSize+3, m_eFontSize+3 ) );

	if( m_eAllign == DT_RIGHT )
	{
		m_pString->Render( pos + CsPoint( 138, 3 + DELTA_STR ), DT_RIGHT );
	}
	else
	{
		assert_cs( m_eAllign == DT_LEFT );
		m_pString->Render( pos );
	}
}

void cMoneyIF::SetEventItemID( DWORD itemID )
{
	m_dwIconItemCode = itemID;
}

cString* cMoneyIF::GetString() const
{
	return m_pString;
}
