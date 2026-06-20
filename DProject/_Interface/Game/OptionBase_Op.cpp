
#include "stdafx.h"
#include "OptionBase.h"


cOptionBase::sOP_BASE* cOptionBase::sOP_BASE::NewInstance( cOptionBase* pParent, eTYPE type )
{
	switch( type )
	{
	case sOP_BASE::TITLE:			return NiNew sOP_TITLE( pParent );
	case sOP_BASE::CHECK_RADIO:		return NiNew sOP_CHECK_RADIO( pParent );
	case sOP_BASE::COMBO:			return NiNew sOP_COMBO( pParent );
	case sOP_BASE::SLIDER:			return NiNew sOP_SLIDER( pParent );	
	case sOP_BASE::CHECK_BOX:		return NiNew sOP_CHECK_BOX( pParent );
	}
	assert_cs( false );
	return NULL;
}

void cOptionBase::sOP_BASE::DeleteInstance( sOP_BASE* pInstance )
{
	assert_cs( pInstance );

	pInstance->Delete();
	switch( pInstance->GetType() )
	{
	case sOP_BASE::TITLE:			NiDelete (sOP_TITLE*)pInstance;				break;
	case sOP_BASE::CHECK_RADIO:		NiDelete (sOP_CHECK_RADIO*)pInstance;		break;
	case sOP_BASE::COMBO:			NiDelete (sOP_COMBO*)pInstance;				break;
	case sOP_BASE::SLIDER:			NiDelete (sOP_SLIDER*)pInstance;			break;
	case sOP_BASE::CHECK_BOX:		NiDelete (sOP_CHECK_BOX*)pInstance;			break;
	default:						assert_cs( false );
	}
}

//=====================================================================================================================
//
//		sOP_TITLE
//
//=====================================================================================================================

void cOptionBase::sOP_TITLE::Init( TCHAR const* szTitle, int nValue )
{
	assert_cs( s_pOptionName == NULL );
	
	cText::sTEXTINFO ti;

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 247/255.0f, 211/255.0f, 153/255.0f ) );
	//ti.SetBoldLevel( cText::sTEXTINFO::BL_2 );
	ti.SetText( szTitle );

	s_pOptionName = NiNew cText;
	s_pOptionName->Init( NULL, CsPoint::ZERO, &ti, false );
	s_nValue = nValue;
}

void cOptionBase::sOP_TITLE::Render( CsPoint pos )
{
	if(s_pOptionName)
		s_pOptionName->Render( CsPoint( pos.x + 60, pos.y ) );
}
void cOptionBase::sOP_TITLE::Delete()
{
	sOP_BASE::Delete(); 
}

//=====================================================================================================================
//
//		sOP_CHECK_RADIO
//
//=====================================================================================================================

void cOptionBase::sOP_CHECK_RADIO::Init( TCHAR const* szName, TCHAR const* szOver, int nRadioID, int nValue )
{
	assert_cs( s_pOptionName == NULL );

	s_pCheck = NiNew cCheckBox;
	s_pCheck->Init( NULL, CsPoint::ZERO, CsPoint( 195, 27 ), "MainOption\\Base\\Check.tga", false );
	s_pCheck->SetTexToken( CsPoint( 0, 27 ) );
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	ti.SetText( szName );
	s_pOptionName = NiNew cText;
	s_pOptionName->Init( NULL, CsPoint::ZERO, &ti, false );

	ti.SetText( szOver );
	s_pOverText = NiNew cText;
	s_pOverText->Init( NULL, CsPoint::ZERO, &ti, false );

	s_nRadioID = nRadioID;
	s_nValue = nValue;
}

void cOptionBase::sOP_CHECK_RADIO::Update_ForMouse()
{
	if( s_pCheck == NULL )
		return;
		
	if( s_pCheck->Update_ForMouse() == cCheckBox::ACTION_CLICK )
	{
		sOP_CHECK_RADIO* pOldCheck = s_pParent->GetCheckRadio();
		if( pOldCheck != this )
		{
			if( pOldCheck && pOldCheck->s_pCheck && pOldCheck->s_nRadioID == s_nRadioID )
			{
				pOldCheck->s_pCheck->SetCheck( false );
			}
		}
		s_pParent->SetCheckRadio( s_pCheck->IsCheck() == true ? this : NULL );
	}
}

void cOptionBase::sOP_CHECK_RADIO::Render( CsPoint pos )
{
	s_pOptionName->Render( pos );
	s_pCheck->SetPos( CsPoint( pos.x + 175, pos.y - 6 ) );
	s_pCheck->Render();
	s_pOverText->Render( CsPoint( pos.x + 272, pos.y ), DT_CENTER );
}

//=====================================================================================================================
//
//		sOP_COMBO
//
//=====================================================================================================================

void cOptionBase::sOP_COMBO::Init( TCHAR const* szName, int nArrayCount, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], NiColor* pColorArray, int* pDataArray, int nSelData, int nValue )
{
	assert_cs( s_pCombo == NULL );

	s_pCombo = NiNew cComboBox;
	s_pCombo->Init( NULL, cComboBox::CT_NORMAL, CsPoint::ZERO, CsPoint( 195, 27 ), CFont::FS_10, DT_CENTER, true, true, false );
	for( int i=0; i<nArrayCount; ++i )
	{
		s_pCombo->AddItem( strArray[ i ], pDataArray[ i ], pColorArray[ i ] );
	}
	s_pCombo->SetCurSel_FromData( nSelData );
	s_nBackupData = nSelData;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.SetText( szName );
	s_pOptionName = NiNew cText;
	s_pOptionName->Init( NULL, CsPoint::ZERO, &ti, false );

	s_nValue = nValue;
}

void cOptionBase::sOP_COMBO::Update(float const& fDeltaTime)
{
	if( s_pCombo == NULL )
		return;

	s_pCombo->Update(fDeltaTime);
}

void cOptionBase::sOP_COMBO::Update_ForMouse()
{
	if( s_pCombo == NULL )
		return;

	sOP_COMBO* pFocusCombo = s_pParent->GetFocusCombo();
	if( pFocusCombo == this )
	{
		if( s_pCombo->Update_ForMouse() >= 0 )
		{
			s_pParent->Combo_SelChange( this );
		}
		if( s_pCombo->IsRenderPopup() == false )
		{
			s_pParent->ReleaseFocusCombo();
		}
	}
	else if( pFocusCombo == NULL )
	{
		int nUFM = s_pCombo->Update_ForMouse();
		if( nUFM >= 0 )
			s_pParent->Combo_SelChange( this );
		else if( nUFM == cComboBox::CI_CLICK )
			s_pParent->SetFocusCombo( this );		
	}	
}

void cOptionBase::sOP_COMBO::Render( CsPoint pos )
{
	if( s_pParent->GetFocusCombo() == this )
	{
		s_pParent->SetFocusComboPos( pos );
		return;
	}
	s_pOptionName->Render( pos );
	s_pCombo->Render( CsPoint( pos.x + 175, pos.y - 6 ) );
}

bool cOptionBase::sOP_COMBO::IsChange()
{
	if( s_pCombo == NULL )
		return false;

	return ( s_pCombo->GetCurSel_Data() != s_nBackupData );
}



//=====================================================================================================================
//
//		sOP_SLIDER
//
//=====================================================================================================================

void cOptionBase::sOP_SLIDER::Init( TCHAR const* szName, CsPoint ptRange, int nBtnMoveRange, int nStartPos, int nValue, TCHAR const* szLow, TCHAR const* szHigh )
{
	assert_cs( s_pSlider == NULL );

	s_pBG = NiNew cSprite;
	s_pBG->Init( NULL, CsPoint::ZERO, CsPoint( 177, 4 ), "Control_G\\Slider\\BarBG.tga", false );

	s_pSlider = NiNew cSlider;
	s_pSlider->Init( NULL, CsPoint::ZERO, CsPoint( 195, 27 ), false );
	s_pSlider->SetRange( ptRange );
	s_pSlider->SetBtnMoveRange( nBtnMoveRange );
	s_pSlider->SetCurPosIndex( nStartPos );
	s_nBackupPos = nStartPos;


	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.SetText( szName );
	s_pOptionName = NiNew cText;
	s_pOptionName->Init( NULL, CsPoint::ZERO, &ti, false );

	ti.s_eFontSize = CFont::FS_8;
	ti.SetText( szLow );
	s_pLowText = NiNew cText;
	s_pLowText->Init( NULL, CsPoint::ZERO, &ti, false );
	
	ti.SetText( szHigh );
	s_pHighText = NiNew cText;
	s_pHighText->Init( NULL, CsPoint::ZERO, &ti, false );

	s_nValue = nValue;
}

void cOptionBase::sOP_SLIDER::Update(float const& fDeltaTime)
{

}

void cOptionBase::sOP_SLIDER::Update_ForMouse()
{
	if( s_pSlider == NULL )
		return;

	switch( s_pSlider->Update_ForMouse() )
	{
	case cSlider::ACTION_CLICK:
		if( s_nValue == cOptionGraphic::GAME_BRIGHT )
		{
			g_pGameIF->SetBright();
		}
		//툴팁그려줄려고 break가 없다.
	case cSlider::ACTION_ON:
		{			
			float fValue = (float)s_pSlider->GetCurPosIndex();
			_stprintf_s( s_szNum, 8, _T( "%.2f" ), fValue );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( s_BarPos, CsPoint::ZERO, s_szNum, CFont::FS_9 );
		}		
		break;	
	default:
		break;
	}	
	
}

void cOptionBase::sOP_SLIDER::Render( CsPoint pos )
{	
	s_pOptionName->Render( pos );

	CsPoint vControl = CsPoint( pos.x + 175, pos.y - 6 );
	s_pLowText->Render( CsPoint( 19, 17 ) + vControl );
	s_pHighText->Render( CsPoint( 159, 17 ) + vControl );
	s_pBG->Render( CsPoint( 9, 11 ) + vControl );
	s_pSlider->Render( vControl );	

	s_BarPos = s_pSlider->GetBarPos() + vControl;
	s_BarPos.y -= 30;
}



//=====================================================================================================================
//
//		sOP_CHECK_BOX
//
//=====================================================================================================================

void cOptionBase::sOP_CHECK_BOX::Delete()
{
	sOP_BASE::Delete();
	for(int i=0;i<s_nCheckBoxCnt;i++)
	{
		SAFE_NIDELETE( s_pCheck[i] );
		SAFE_NIDELETE( s_pCheckText[i] );
	}
}


void cOptionBase::sOP_CHECK_BOX::Init( TCHAR const* szName, int nCheckBoxCnt, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], int* pDataArray, bool bBattle, bool bOnly, int nValue )
{
	s_nCheckBoxCnt = nCheckBoxCnt;
	s_nValue = nValue;
	s_bOnly = bOnly;
	s_bBattle = bBattle;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.SetText( szName );
	s_pOptionName = NiNew cText;
	s_pOptionName->Init( NULL, CsPoint::ZERO, &ti, false );

	for( int i = 0 ; i < s_nCheckBoxCnt ; i++ )
	{
		s_pCheck[i] = NiNew cCheckBox;
		s_pCheck[i]->Init( NULL, CsPoint::ZERO, CsPoint( 17, 17 ), "MainOption\\Graphic\\\\Option_Graphic_Check.tga", false );
		s_pCheck[i]->SetTexToken( CsPoint( 17, 0 ) );
		if( pDataArray[i] == 0 )
			s_pCheck[i]->SetCheck( false );
		else
			s_pCheck[i]->SetCheck( true );

		ti.SetText( strArray[i] );
		s_pCheckText[i] = NiNew cText;
		s_pCheckText[i]->Init( NULL, CsPoint::ZERO, &ti, false );
	}
}

void cOptionBase::sOP_CHECK_BOX::Update_ForMouse()
{
	if( s_bBattle == false &&  nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return;

	for( int i = 0 ; i < s_nCheckBoxCnt ; i++ )
	{
		bool bIsCheck = s_pCheck[i]->IsCheck();
		if( s_pCheck[i]->Update_ForMouse() == cCheckBox::ACTION_CLICK )
		{
			if( s_bOnly )	// 단일 선택일 경우
			{
				// 자신이 선택되어 있을 때
				if( bIsCheck )
				{
					s_pCheck[i]->SetCheck( true );
					return;
				}
				// 나 이외에 이미 선택된 놈 있으면 지우고 선택
				for( int j = 0 ; j < s_nCheckBoxCnt ; j++ )
				{
					if( s_pCheck[j]->IsCheck() )
					{
						s_pCheck[j]->SetCheck( false );
					}
				}
				// 자신 선택
				s_pCheck[i]->SetCheck( true );
			}
			else		// 다중 선택일 경우
			{
				// 자신이 선택 중이 아닐 땐 그냥 자신 선택
				if( bIsCheck == false )
				{
					s_pCheck[i]->SetCheck( true );
					return;
				}

				// 자신이 선택 중일 때
				// 다른놈 중 선택된 놈이 있으면 자신 삭제
				for( int j = 0 ; j < s_nCheckBoxCnt ; j++ )
				{
					if( i == j )
						continue;
					if( s_pCheck[j]->IsCheck() )
					{
						s_pCheck[i]->SetCheck( false );
						return;
					}
				}
				// 선택된 놈이 없으면 자신은 남아있어야 함
				s_pCheck[i]->SetCheck( true );
			}
		}
	}	
}

void cOptionBase::sOP_CHECK_BOX::Render( CsPoint pos )
{
	s_pOptionName->Render( pos );

	pos = pos + CsPoint( 215, 0 );
	for( int i = 0 ; i < s_nCheckBoxCnt ; i++ )
	{
#ifdef VERSION_TH
		s_pCheckText[i]->Render( CsPoint( (pos.x - 5) + (i*100), (pos.y - 2) ), DT_CENTER );
#else
		s_pCheckText[i]->Render( CsPoint( pos.x + (i*100), pos.y ), DT_CENTER );
#endif

		s_pCheck[i]->SetPos( CsPoint( pos.x + (i*100) + 30, pos.y - 2 ) );
		s_pCheck[i]->Render();
	}
}

void cOptionBase::sOP_CHECK_BOX::ResetDevice()
{
	sOP_BASE::ResetDevice();

	for( int i = 0 ; i < s_nCheckBoxCnt ; i++ )
	{
		if( s_pCheck[i] )
			s_pCheck[i]->ResetDevice();
		
		if( s_pCheckText[i] )
			s_pCheckText[i]->ResetDevice();
	}
}
