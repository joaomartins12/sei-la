
#include "stdafx.h"
#include "TamerStatus.h"

#ifdef USE_BARCODE_REDER

void cTamerStatus::_DeleteBarcode()
{
	__SetRootSize( CsPoint( 255, 424 ) );

	SAFE_NIDELETE( m_pBarcodeBG );
	SAFE_NIDELETE( m_pBtnBarcodeResist );
	SAFE_NIDELETE( m_pBtnBarcodeUnResist );

	m_pCurBtnBarcode = NULL;

	m_BarcodeStateText.Delete();
	m_BarcodeState = 0;
}


void cTamerStatus::_InitBarcode()
{
	if( g_pResist->m_Global.s_bUseBarcode == false )
		return;

	__SetRootSize( CsPoint( 255, 461 ) );

	m_pBarcodeBG = NiNew cSprite;
	m_pBarcodeBG->Init( m_pRoot, CsPoint( 0, 424 + 22 ), CsPoint( 255, 37 ), "TamerStatus\\BarcodeBG.tga", false );

	m_pBtnBarcodeResist = NiNew cButton;
	m_pBtnBarcodeResist->Init( m_pRoot, CsPoint( 180, 430 + 22 ), CsPoint( 65, 24), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_REGISTER" ).c_str(), false );
	m_pBtnBarcodeUnResist = NiNew cButton;
	m_pBtnBarcodeUnResist->Init( m_pRoot, CsPoint( 180, 430 + 22 ), CsPoint( 65, 24 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_DETACH_MONEY" ).c_str(), false );

	m_pCurBtnBarcode = m_pBtnBarcodeResist;

	m_BarcodeState = 0;
	_CalBarcodeState();
	_InitBarcodeState_Text();
}

void cTamerStatus::_InitBarcodeState_Text()
{
	m_BarcodeStateText.Delete();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_GAME_SCANNER" ).c_str() );
	m_BarcodeStateText.AddText( &ti )->s_ptSize.x = 70;

	cCondition* pCondition = g_pCharMng->GetTamerUser()->GetCondition();
	// 바코드가 장착되어 있는가
	if( pCondition->IsCondition( nSync::Scanner0 ) == true )
	{
		ti.s_Color = FONT_WHITE;
		TCHAR sz[ 128 ] = {0, };
		if( pCondition->IsCondition( nSync::Scanner2 ) == true )
		{
			_tcscat_s( sz, 128, _T( "한정/" ) );
		}
		else
		{
			_tcscat_s( sz, 128, _T( "일반/" ) );
		}

		if( pCondition->IsCondition( nSync::Scanner1 ) == true )
		{
			_tcscat_s( sz, 128, _T( "스캔&버프" ) );
			m_pCurBtnBarcode = m_pBtnBarcodeUnResist;
			m_pCurBtnBarcode->SetEnable( true );
		}
		else
		{
			_tcscat_s( sz, 128, _T( "스캔" ) );
			m_pCurBtnBarcode = m_pBtnBarcodeResist;
			m_pCurBtnBarcode->SetEnable( true );
		}
		ti.SetText( sz );
		m_BarcodeStateText.AddText( &ti );
	}
	else
	{
		ti.s_Color = TOOLTIP_CANNOT_COLOR;
		ti.SetText( UISTRING_TEXT( "GAMESCANNER_NOTCONNECT" ).c_str() );
		m_BarcodeStateText.AddText( &ti );
	
		m_pCurBtnBarcode = m_pBtnBarcodeResist;
		m_pCurBtnBarcode->SetEnable( false );
	}
}

bool cTamerStatus::_CalBarcodeState()
{
	uint bs = 0;

	cCondition* pCondition = g_pCharMng->GetTamerUser()->GetCondition();
	if( pCondition->IsCondition( nSync::Scanner0 ) )
		bs |= nSync::Scanner0;
	if( pCondition->IsCondition( nSync::Scanner1 ) )
		bs |= nSync::Scanner1;
	if( pCondition->IsCondition( nSync::Scanner2 ) )
		bs |= nSync::Scanner2;

	if( bs == m_BarcodeState )
		return false;

	m_BarcodeState = bs;
	return true;
}

void cTamerStatus::_UpdateBarcode()
{
	if( g_pResist->m_Global.s_bUseBarcode == false )
	{
		_DeleteBarcode();
		return;
	}

	if( m_pBarcodeBG == NULL )
	{
		_InitBarcode();
	}
	
	if( _CalBarcodeState() == true )
	{
		_InitBarcodeState_Text();
	}
}

bool cTamerStatus::_UpdateBarcode_ForMouse()
{
	if( m_pCurBtnBarcode == NULL )
		return false;

	switch( m_pCurBtnBarcode->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			// 등록
			if( m_pCurBtnBarcode == m_pBtnBarcodeResist )
			{
				cPrintMsg::PrintMsg( 20039 );
			}
			// 해제
			else
			{
				assert_cs( m_pCurBtnBarcode == m_pBtnBarcodeUnResist );
				cPrintMsg::PrintMsg( 20040 );
			}
		}
		return true;
	default:
		return true;
	}

	return false;
}

void cTamerStatus::_RenderBarcode()
{
	if( m_pBarcodeBG )
		m_pBarcodeBG->Render();
	if( m_pCurBtnBarcode )
		m_pCurBtnBarcode->Render();

	if( m_BarcodeStateText.GetListSize() )
		m_BarcodeStateText.Render( CsPoint( 10, 435 + 22 ) + GetRootClient() );
}

void cTamerStatus::_ResetDevice_Barcode()
{
	if( m_pBtnBarcodeResist )
		m_pBtnBarcodeResist->ResetDevice();
	if( m_pBtnBarcodeUnResist )
		m_pBtnBarcodeUnResist->ResetDevice();
	if( m_pBarcodeBG )
		m_pBarcodeBG->ResetDevice();

	m_BarcodeStateText.ResetDevice();
}

#endif