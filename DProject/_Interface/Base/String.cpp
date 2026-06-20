#include "stdafx.h"
#include "String.h"

void cString::sELEMENT::RenderLimit( CsPoint pos )
{
	Render( pos ); 
};
void cString::sELEMENT::SetColor( NiColor color )
{

};
void cString::sELEMENT::ResetDevice()
{

};
bool cString::sELEMENT::GetEnable() const
{
	return s_bEnable;
};

//////////////////////////////////////////////////////////////////////////
cString::sELEMENT::eTYPE cString::sTEXT::GetType()
{
	return TEXT;
};

cString::sTEXT::sTEXT()
{
	s_Text.SetVisible(true);
	s_Text.Delete();
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE;
}

cString::sTEXT::~sTEXT()
{

}

void cString::sTEXT::Delete()
{
	s_Text.SetVisible(true);
	s_Text.Delete();
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE;
}

void cString::sTEXT::Render( CsPoint pos )
{
	s_Text.Render( pos + s_ptDeltaPos );
}

void cString::sTEXT::RenderLimit( CsPoint pos )
{
	s_Text.RenderLimit( pos ); 
}

void cString::sTEXT::SetAlpha( float fAlpha )
{
	s_Text.SetAlpha( fAlpha );
}

void cString::sTEXT::SetFontSize( CFont::eFACE_SIZE FontSize )
{
	s_Text.SetFontSize( FontSize );
}

void cString::sTEXT::SetColor( NiColor color )
{
	s_Text.SetColor( color );
}

void cString::sTEXT::ResetDevice()
{
	s_Text.ResetDevice(); 
}

void cString::sTEXT::SetText( TCHAR const* sz )
{
	s_Text.SetText( sz );
}

void cString::sTEXT::SetText( TCHAR const* sz, int nWidth )
{
	s_Text.SetText( sz, nWidth );
}

// void cString::sTEXT::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sTEXT::SetEnable( bool bEnable )
{
	if( s_bEnable == bEnable )
		return;

	s_bEnable = bEnable;
	if( !s_bEnable )
	{
		s_backupColor = s_Text.GetColor();
		SetColor( FONT_GLAY );
	}
	else
	{
		SetColor( s_backupColor );
	}
}

void cString::sTEXT::SetVisible(bool bValue)
{
	s_Text.SetVisible( bValue );
}

//////////////////////////////////////////////////////////////////////////

cString::sELEMENT::eTYPE cString::sDATA_STR::GetType()
{
	return DATA_STR;
}

cString::sDATA_STR::sDATA_STR()
{
	s_bEnable = true;
	s_nValue = 0;
	s_ptSize = CsPoint::ZERO;
	memset( s_szStr, 0, sizeof(TCHAR) * 64 );
}

cString::sDATA_STR::~sDATA_STR()
{
}

void cString::sDATA_STR::Delete()
{
	s_bEnable = true;
	s_nValue = 0;
	s_ptSize = CsPoint::ZERO;
	memset( s_szStr, 0, sizeof(TCHAR) * 64 );
}

void cString::sDATA_STR::Render( CsPoint pos )
{

}
void cString::sDATA_STR::RenderLimit( CsPoint pos )
{

}
void cString::sDATA_STR::SetAlpha( float fAlpha )
{

}
void cString::sDATA_STR::SetColor( NiColor color )
{

}
void cString::sDATA_STR::ResetDevice()
{

}
// void cString::sDATA_STR::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// } 

void cString::sDATA_STR::SetEnable( bool bEnable )
{
	s_bEnable = bEnable;
}
//////////////////////////////////////////////////////////////////////////

cString::sELEMENT::eTYPE cString::sSPRITE::GetType()
{
	return SPRITE; 
}

cString::sSPRITE::sSPRITE() : s_pSprite(NULL)
{
	s_backupColor = NiColor::WHITE;
	s_bChangeSize = false;
	s_ptDeltaPos = CsPoint::ZERO;
	s_RenderPos = CsPoint::ZERO;
	s_ptSize = CsPoint::ZERO;
}
cString::sSPRITE::~sSPRITE()
{

}

void cString::sSPRITE::SetColor( NiColor color )
{
	if( s_pSprite )
		s_pSprite->SetColor( color );
}

void cString::sSPRITE::Delete()
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_bChangeSize = false;
	s_ptDeltaPos = CsPoint::ZERO;
	s_RenderPos = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE;
	if( s_AutoPointerDelete )
	{
		SAFE_NIDELETE(s_pSprite);
		s_AutoPointerDelete = false;
	}
	else
		s_pSprite = NULL;
}
void cString::sSPRITE::Render(CsPoint pos)
{
	s_RenderPos = pos + s_ptDeltaPos;
	SAFE_POINTER_RET( s_pSprite );
	(s_bChangeSize == false) ?  s_pSprite->Render( s_RenderPos ) : s_pSprite->Render( s_RenderPos, s_ptSize );
}
void cString::sSPRITE::SetAlpha( float fAlpha )
{ 
	SAFE_POINTER_RET( s_pSprite );
	s_pSprite->SetAlpha( fAlpha ); 
}
void cString::sSPRITE::ResetDevice()
{ 
	SAFE_POINTER_RET( s_pSprite );
	s_pSprite->ResetDevice();
}
// void cString::sSPRITE::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sSPRITE::SetEnable(bool bEnable)
{
	s_bEnable = bEnable;
	SAFE_POINTER_RET( s_pSprite );
	if( !s_bEnable )
	{
		s_backupColor = s_pSprite->GetColor();
		SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
	}
	else
		s_pSprite->SetColor( s_backupColor );
}

void cString::sSPRITE::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pSprite );
	s_pSprite->SetVisible(bValue);
}

void cString::sSPRITE::ChangeTexture(char const * pTextureFileName, bool bUseWorkingFolder)
{
	SAFE_POINTER_RET( s_pSprite );
	s_pSprite->ChangeTexture( pTextureFileName, bUseWorkingFolder );
}

//////////////////////////////////////////////////////////////////////////


cString::sELEMENT::eTYPE cString::sICON::GetType()
{
	return ICON;
}

cString::sICON::sICON() 
: s_bVisible(true),
s_bCoolTimeMode(false),
s_nIconType(0),
s_nIndex_1(0),
s_nIndex_2(0),
s_nIndex_3(0),
s_fAlpha(0.0f),
s_pCoolTimeSeq(NULL)
{
	s_bEnable = true;
}

cString::sICON::~sICON()
{
	s_bVisible = true;
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptIconSize = CsPoint::ZERO;
	s_nIconType = 0;
	s_nIndex_1 = 0;
	s_nIndex_2 = 0;
	s_nIndex_3 = 0;
	s_fAlpha = 1.0f;
	s_ptLastPos = CsPoint::ZERO;
	m_MouseOverCheckRect = CsRect::ZERO;
	s_bCoolTimeMode = false;
	s_pCoolTimeSeq = NULL;
}

void cString::sICON::SetColor( NiColor color )
{

}

void cString::sICON::Delete()
{
	s_bVisible = true;
	s_bEnable = true;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptIconSize = CsPoint::ZERO;
	s_nIconType = 0;
	s_nIndex_1 = 0;
	s_nIndex_2 = 0;
	s_nIndex_3 = 0;
	s_fAlpha = 1.0f;
	s_ptLastPos = CsPoint::ZERO;
	m_MouseOverCheckRect = CsRect::ZERO;
	s_bCoolTimeMode = false;
	s_pCoolTimeSeq = NULL;
}

void cString::sICON::SetAlpha( float fAlpha )
{
	s_fAlpha = fAlpha; 	
}

void cString::sICON::SetVisible(bool bValue)
{
	s_bVisible = bValue;
}

void cString::sICON::SetCoolTimeSeq(CsCoolTimeSeq* pCoolTimeSeq)
{
	s_pCoolTimeSeq = pCoolTimeSeq;
}

void cString::sICON::SetCoolTimeMode(bool bSetCoolTime)
{
	s_bCoolTimeMode = bSetCoolTime;
}
// void cString::sICON::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sICON::SetEnable(bool bEnable)
{
	s_bEnable = bEnable;
}

void cString::sICON::OnMouseOver()
{
}

void cString::sICON::OnMouseLeave()
{
}


void cString::sICON::ChangeIcon(int nIconType, int nIndex1, int nIndex2, int nIndex3)
{
	s_nIconType = nIconType;
	s_nIndex_1 = nIndex1;
	s_nIndex_2 = nIndex2;
	s_nIndex_3 = nIndex3;
}

void cString::sICON::ChangeSkillIcon(int nIconType, int nIndex1, int nIndex2 /*= 0*/, int nIndex3 /*= 0*/)
{
	switch( nIconType )
	{
	case CsSkill::IT_CHANGE3:
	case CsSkill::IT_CHANGE1: 
	case CsSkill::IT_CHANGE2:	s_nIconType = ICONITEM::SKILL_MASK;	break;			
	default:
		{
			if( nIconType >= 4000 )
				s_nIconType = ICONITEM::SKILL4;
			else if( nIconType >= 3000 )
				s_nIconType = ICONITEM::SKILL3;
			else if( nIconType >= 2000 )
				s_nIconType = ICONITEM::SKILL2;
			else
				s_nIconType = ICONITEM::SKILL1;
		}
		break;
	}

	if( ICONITEM::SKILL_MASK != s_nIconType )
		s_nIndex_1 = nIndex1 % 1000;
	else
		s_nIndex_1 = nIndex1;

	s_nIndex_2 = nIndex2;		// 스킬 인덱스
	s_nIndex_3 = nIndex3;
}

void cString::sICON::Render( CsPoint pos )
{
	s_ptLastPos = pos + s_ptDeltaPos;
	if( !s_bVisible )
		return;

	switch( s_nIconType )
	{	
// 	case ICONITEM::SKILL1:
// 	case ICONITEM::SKILL2:
// 	case ICONITEM::SKILL3:
// 	case ICONITEM::SKILL4:
// 		g_pIconMng->RenderSkill( s_nIndex_1, (bool)s_nIndex_2, s_ptLastPos, s_nIndex_3 );
// 		break;
	case ICONITEM::DEvol:
		g_pIconMng->RenderDEvol( (nsCsDigimonTable::eEVOLUTION_TYPE)s_nIndex_1, s_ptLastPos, s_ptIconSize );
		break;
	case ICONITEM::DAttribute:
		g_pIconMng->RenderDAttribute( (cIconMng::eDAttribute)s_nIndex_1, s_nIndex_2, s_ptLastPos, s_ptIconSize );
		break;
	case ICONITEM::Talk:
		g_pIconMng->RenderTalk( s_ptLastPos, s_ptIconSize, s_nIndex_1 );
		break;

	case ICONITEM::ARMOR_1:
	case ICONITEM::ARMOR_2:
	case ICONITEM::ELEMENT_NO_CHECK:
	case ICONITEM::ELEMENT_CHECK:
	case ICONITEM::CHIPSET:
	case ICONITEM::ITEM_CARD:
	case ICONITEM::QUEST_ITEM:
	case ICONITEM::DT_MEMORY:
	case ICONITEM::ITEM_ICON:
	case ICONITEM::ITEM_ICON49:
		{
			cItemInfo item;
			item.m_nType  = s_nIndex_1;
			item.m_nCount = s_nIndex_2;
			item.m_nRate  = s_nIndex_3;

			m_MouseOverCheckRect.SetRect( s_ptLastPos, CsSIZE(s_ptIconSize));;

			g_pIconMng->RenderItem( &item, s_ptLastPos, s_ptIconSize, true );
		}
		break;
	case ICONITEM::CARD:
		{
			if( s_nIndex_1 >= 21 && s_nIndex_1 <= 26 )	
				g_pIconMng->RenderIcon( (ICONITEM::eTYPE)s_nIconType, s_ptLastPos, s_ptIconSize, s_nIndex_1 );
			else
			{
				cItemInfo item;
				item.m_nType  = s_nIndex_1;
				item.m_nCount = s_nIndex_2;
				item.m_nRate  = s_nIndex_3;
				m_MouseOverCheckRect.SetRect( s_ptLastPos, CsSIZE(s_ptIconSize));
				g_pIconMng->RenderItem( &item, s_ptLastPos, s_ptIconSize, true );
			}
		}
		break;
	case ICONITEM::SKILL_MASK:
		{
			switch( s_nIndex_1 )
			{
			case CsSkill::IT_CHANGE1:
			case CsSkill::IT_CHANGE2:
			case CsSkill::IT_CHANGE3:
				{
					if( -1 == s_nIndex_2 )
						break;
					SAFE_POINTER_BEK( g_pCharMng );
					CTamerUser* pUser = g_pCharMng->GetTamerUser();
					SAFE_POINTER_BEK( pUser );
					cSkill* pSkillMng = pUser->GetSkillMng();
					SAFE_POINTER_BEK( pSkillMng );
					cSkill::sINFO* pSkillInfo = pSkillMng->GetSkill( s_nIndex_2 );
					SAFE_POINTER_BEK( pSkillInfo );
					g_pIconMng->RenderSkillMask( s_nIndex_1, s_ptLastPos, pSkillInfo->s_nTacticsIndex );
				}
				break;
			default:
				g_pIconMng->RenderSkillMask( s_nIndex_1, s_ptLastPos );
				break;
			}
		}
		break;

	case ICONITEM::NUMBER:
		{
			g_pIconMng->RenderNumber( DT_CENTER, NUMBERSPRITE::WHITE, s_nIndex_1, s_ptLastPos, s_ptIconSize, false );
		}
		break;

	default:
		{
			g_pIconMng->RenderIcon( (ICONITEM::eTYPE)s_nIconType, s_ptLastPos, s_ptIconSize, s_nIndex_1 );
		}
		break;
	}

	if( s_bCoolTimeMode )
		g_pIconMng->RenderTime( s_ptLastPos, 16/*ICON_HALF*/, s_pCoolTimeSeq );
}

//////////////////////////////////////////////////////////////////////////

cString::sELEMENT::eTYPE cString::sIMAGE::GetType()
{
	return IMAGE;
}

cString::sIMAGE::sIMAGE() : s_pImage(NULL)
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE;
	s_ptImageSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_nImageIndex = 0;	
}
cString::sIMAGE::~sIMAGE()
{

}

void cString::sIMAGE::SetColor( NiColor color )
{
	if( s_pImage )
		s_pImage->SetColor( color );
}
void cString::sIMAGE::Delete()
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptImageSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_nImageIndex = 0;
	s_backupColor = NiColor::WHITE;

	if( s_AutoPointerDelete )
	{
		SAFE_NIDELETE(s_pImage);
		s_AutoPointerDelete = false;
	}
	else
	{
		s_pImage = NULL;
	}
}

void cString::sIMAGE::Render( CsPoint pos )
{
	SAFE_POINTER_RET( s_pImage );
	if( s_ptImageSize == CsPoint::ZERO )
	{
		s_pImage->Render( pos + s_ptDeltaPos, s_nImageIndex );
	}
	else
	{
		CsPoint ptOldSize = s_pImage->GetSize();
		s_pImage->SetSize( s_ptImageSize );
		s_pImage->Render( pos + s_ptDeltaPos, s_nImageIndex );
		s_pImage->SetSize( ptOldSize );
	}
}
void cString::sIMAGE::SetAlpha( float fAlpha )
{
	SAFE_POINTER_RET( s_pImage );
	s_pImage->SetAlpha( fAlpha ); 
}
void cString::sIMAGE::ResetDevice()
{
	SAFE_POINTER_RET( s_pImage );
	s_pImage->ResetDevice();
}
// void cString::sIMAGE::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sIMAGE::SetEnable(bool bEnable)
{
	s_bEnable = bEnable;

	SAFE_POINTER_RET( s_pImage );
	if( !s_bEnable )
	{
		s_backupColor = s_pImage->GetColor();
		SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
	}
	else
		SetColor( s_backupColor );
}


void cString::sIMAGE::SetImangeIndex( const int nIndex )
{
	s_nImageIndex = nIndex;
}

void cString::sIMAGE::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pImage );
	s_pImage->SetVisible(bValue);
}
//////////////////////////////////////////////////////////////////////////


cString::sELEMENT::eTYPE cString::sCHECKBOX::GetType()
{
	return CHECK_BOX;
}

cString::sCHECKBOX::sCHECKBOX()
{ 
	s_pCheckBox = NULL; 
	s_backupColor = NiColor::WHITE; 
	s_ptDeltaPos = CsPoint::ZERO;
	s_nCheckBoxIndex = 0;
}

cString::sCHECKBOX::~sCHECKBOX()
{ 
}

void cString::sCHECKBOX::SetColor( NiColor color )
{

}
void cString::sCHECKBOX::Delete()
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_nCheckBoxIndex = 0;
	s_ptDeltaPos = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE; 

// 	if( s_AutoPointerDelete )
// 	{
 		SAFE_NIDELETE(s_pCheckBox);
// 		s_AutoPointerDelete = false;
// 	}
// 	else
// 		s_pCheckBox = NULL;

	s_ptDeltaPos = CsPoint::ZERO;
}
void cString::sCHECKBOX::Render( CsPoint pos )
{
	SAFE_POINTER_RET( s_pCheckBox );
	s_pCheckBox->SetPos( pos + s_ptDeltaPos );
	s_pCheckBox->Render();
}
void cString::sCHECKBOX::SetAlpha( float fAlpha )
{
	SAFE_POINTER_RET( s_pCheckBox );
	s_pCheckBox->SetAlpha(fAlpha);
}
void cString::sCHECKBOX::ResetDevice()
{
	SAFE_POINTER_RET( s_pCheckBox );
	s_pCheckBox->ResetDevice();
}

void cString::sCHECKBOX::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pCheckBox );
	s_pCheckBox->SetVisible(bValue);
}

// void cString::sCHECKBOX::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sCHECKBOX::SetEnable(bool bEnable )
{
	s_bEnable = bEnable;
	if( s_pCheckBox )
		s_pCheckBox->SetEnable( bEnable );
}

void cString::sCHECKBOX::OnMouseOver()
{
	if( s_pCheckBox )
		s_pCheckBox->OnMouseOver();
}

void cString::sCHECKBOX::OnMouseLeave()
{
	if( s_pCheckBox )
		s_pCheckBox->OnMouseLeave();
}

void cString::sCHECKBOX::OnMouseDown()
{
	if( s_pCheckBox )
		s_pCheckBox->OnMouseDown();
}

void cString::sCHECKBOX::OnMouseUp()
{
	if( s_pCheckBox )
		s_pCheckBox->OnMouseUp();
}

void cString::sCHECKBOX::onMousePressed()
{
	if( s_pCheckBox )
		s_pCheckBox->OnMousePressed();
}


//////////////////////////////////////////////////////////////////////////
cString::sELEMENT::eTYPE cString::sMONEYIF::GetType()
{
	return MONEYIF;
}

cString::sMONEYIF::sMONEYIF():s_pMoney(NULL)
{
	s_bEnable = true;
	s_pMoney = NULL;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE;
}

cString::sMONEYIF::~sMONEYIF()
{
}

void cString::sMONEYIF::Delete()
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_backupColor = NiColor::WHITE;

	if( s_pMoney )
	{
		NiDelete (cMoneyIF*)s_pMoney;
		s_pMoney = NULL;
	}		
}

void cString::sMONEYIF::Render( CsPoint pos )
{
	if( s_pMoney )
		( (cMoneyIF*)s_pMoney )->Render( pos + s_ptDeltaPos );
}
void cString::sMONEYIF::SetAlpha( float fAlpha )
{
	assert_cs( false );
}

void cString::sMONEYIF::SetMoney(u8 nMoney)
{
	if( s_pMoney )
		dynamic_cast<cMoneyIF*>( s_pMoney )->SetMoney( nMoney );
}

void cString::sMONEYIF::SetColor( NiColor color )
{
	if( s_pMoney )
		( (cMoneyIF*)s_pMoney )->SetColor( color );
}
void cString::sMONEYIF::ResetDevice()
{
	if( s_pMoney )
		s_pMoney->ResetDevice();
}
// void cString::sMONEYIF::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// 
// }

void cString::sMONEYIF::SetEnable(bool bEnable)
{
	s_bEnable = bEnable;
	SAFE_POINTER_RET( s_pMoney );

	if( !s_bEnable )
	{
		s_backupColor = ( (cMoneyIF*)s_pMoney )->GetColor();
		SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
	}
	else
		SetColor(s_backupColor);
}

void cString::sMONEYIF::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pMoney );
	( (cMoneyIF*)s_pMoney )->SetVisible(bValue);
}
//////////////////////////////////////////////////////////////////////////


cString::sELEMENT::eTYPE cString::sBUTTON::GetType()
{
	return BUTTON; 
}

cString::sBUTTON::sBUTTON():s_pButton(NULL)
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptButtonSize = CsPoint::ZERO;
	m_ptTextPos = CsPoint::ZERO;
	s_nValue1 = 0;
}

cString::sBUTTON::~sBUTTON()
{
}

void cString::sBUTTON::Delete()
{
	s_bEnable = true;
	s_ptSize = CsPoint::ZERO;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptButtonSize = CsPoint::ZERO;
	m_ptTextPos = CsPoint::ZERO;
	s_nValue1 = 0;
	NISAFE_DELETE( s_pButton );
}

void cString::sBUTTON::Render( CsPoint pos )
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetPos( pos + s_ptDeltaPos );
	s_pButton->SetPosText( pos + s_ptDeltaPos + m_ptTextPos );
	s_pButton->Render();
}
void cString::sBUTTON::OnMouseOver()
{
	if( s_pButton )
		s_pButton->OnMouseOver();
}

void cString::sBUTTON::OnMouseLeave()
{
	if( s_pButton )
		s_pButton->OnMouseLeave();
}

void cString::sBUTTON::OnMouseDown()
{
	if( s_pButton )
		s_pButton->OnMouseDown();
}

void cString::sBUTTON::OnMouseUp()
{
	if( s_pButton )
		s_pButton->OnMouseUp();
}

void cString::sBUTTON::onMousePressed()
{
	if( s_pButton )
		s_pButton->OnMousePressed();
}

void cString::sBUTTON::SetAlpha( float fAlpha )
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetAlpha( fAlpha );
}

void cString::sBUTTON::SetColor( NiColor color )
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetColor( color );
}

void cString::sBUTTON::SetOnMode( bool bValue )
{
	SAFE_POINTER_RET( s_pButton );
	if( !s_pButton->GetVisible() )
		return;

	if( !s_pButton->IsEnable() )
		return;

	s_pButton->SetMouseOnMode( bValue );
}

void cString::sBUTTON::ResetDevice(  )
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->ResetDevice();
}

// void cString::sBUTTON::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sBUTTON::SetEnable(bool bEnable)
{
	s_bEnable = bEnable;
	if( s_pButton )
		s_pButton->SetEnable( bEnable );
}

void cString::sBUTTON::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetVisible(bValue);
}

int cString::sBUTTON::GetValue() const
{
	return s_nValue1;
}

void cString::sBUTTON::SetValue( int nValue)
{
	s_nValue1 = nValue;
}

//////////////////////////////////////////////////////////////////////////
// Ani Button
cString::sANIBUTTON::sANIBUTTON() : s_pButton(NULL), s_nValue1(0)
{
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptButtonSize = CsPoint::ZERO;
}

cString::sANIBUTTON::~sANIBUTTON()
{

}

void cString::sANIBUTTON::SetEnable(bool bEnable)
{
	s_bEnable = bEnable;
	if( s_pButton )
		s_pButton->SetEnable( bEnable );
}

cString::sELEMENT::eTYPE cString::sANIBUTTON::GetType()
{
	return ANIBUTTON;
}

void cString::sANIBUTTON::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->Update( fDeltaTime );
}

void cString::sANIBUTTON::Delete()
{
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptButtonSize = CsPoint::ZERO;
	s_nValue1 = 0;
	NISAFE_DELETE( s_pButton );
}

void cString::sANIBUTTON::Render(CsPoint pos)
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetPos( pos + s_ptDeltaPos );
	s_pButton->Render();
}

void cString::sANIBUTTON::SetAlpha(float fAlpha)
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetAlpha( fAlpha );
}

void cString::sANIBUTTON::SetColor(NiColor color)
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetColor( color );
}

void cString::sANIBUTTON::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->SetVisible( bValue );
}

void cString::sANIBUTTON::ResetDevice()
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->ResetDevice();
}

int cString::sANIBUTTON::GetValue() const
{
	return s_nValue1;
}

void cString::sANIBUTTON::SetValue(int nValue)
{
	s_nValue1 = nValue;
}

void cString::sANIBUTTON::OnMouseOver()
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->OnMouseOver();
}

void cString::sANIBUTTON::OnMouseLeave()
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->OnMouseLeave();
}

void cString::sANIBUTTON::OnMouseDown()
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->OnMouseDown();
}

void cString::sANIBUTTON::OnMouseUp()
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->OnMouseUp();
}

void cString::sANIBUTTON::onMousePressed()
{
	SAFE_POINTER_RET( s_pButton );
	s_pButton->OnMousePressed();
}

//////////////////////////////////////////////////////////////////////////

cString::sELEMENT::eTYPE cString::sPROGRESS::GetType()
{
	return PROGRESS; 
}

cString::sPROGRESS::sPROGRESS():s_pProgressbar(NULL)
{
	s_bEnable = true;
	s_bChangeSize = false;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptSize = CsPoint::ZERO;
}

cString::sPROGRESS::~sPROGRESS()
{
}

void cString::sPROGRESS::Delete()
{
	s_bEnable = true;
	s_bChangeSize = false;;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptSize = CsPoint::ZERO;
	NISAFE_DELETE( s_pProgressbar );
}

void cString::sPROGRESS::Render( CsPoint pos )
{
	SAFE_POINTER_RET( s_pProgressbar );
//	s_pProgressbar->SetPos( s_ptDeltaPos );
// 	s_pProgressbar->SetPosText( pos + s_ptDeltaPos );
	s_pProgressbar->Render(pos + s_ptDeltaPos);
}

void cString::sPROGRESS::OnMouseOver()
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->OnMouseOver();
}

void cString::sPROGRESS::OnMouseLeave()
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->OnMouseLeave();
}

void cString::sPROGRESS::OnMouseDown()
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->OnMouseDown();
}

void cString::sPROGRESS::OnMouseUp()
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->OnMouseUp();
}

void cString::sPROGRESS::onMousePressed()
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->OnMousePressed();
}

void cString::sPROGRESS::SetAlpha( float fAlpha )
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->SetAlpha( fAlpha );
}
void cString::sPROGRESS::SetColor( NiColor color )
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->SetColor( color );
}
void cString::sPROGRESS::ResetDevice()
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->ResetDevice();
}
// void cString::sPROGRESS::SetEnable(bool bValue, int nValue)
// {
// 	s_bEnable = bValue;
// 	if( bValue )
// 		SetColor( NiColor::BLACK );
// 	else
// 	{
// 		if(nValue != INT_MAX && nValue != 0) 
// 			SetColor( NiColor( 189/255.0f, 189/255.0f, 189/255.0f ) );
// 	}
// }

void cString::sPROGRESS::SetEnable(bool bEnable )
{
	s_bEnable = bEnable;
	if( s_pProgressbar )
		s_pProgressbar->SetEnable( bEnable );
}


void cString::sPROGRESS::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pProgressbar );
	s_pProgressbar->SetVisible(bValue);
}
//////////////////////////////////////////////////////////////////////////
cString::sELEMENT::eTYPE cString::sTIMERPROGRESS::GetType()
{
	return PROGRESS; 
}

cString::sTIMERPROGRESS::sTIMERPROGRESS() : s_pTimerProgress(NULL)
{
	s_bEnable = true;
	s_bChangeSize = false;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptSize = CsPoint::ZERO;
}

cString::sTIMERPROGRESS::~sTIMERPROGRESS()
{
}

void cString::sTIMERPROGRESS::Delete()
{
	s_bEnable = true;
	s_bChangeSize = false;;
	s_ptDeltaPos = CsPoint::ZERO;
	s_ptSize = CsPoint::ZERO;
	NISAFE_DELETE( s_pTimerProgress );
}

void cString::sTIMERPROGRESS::Render( CsPoint pos )
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->SetPos( pos + s_ptDeltaPos );
	s_pTimerProgress->Render();
}

void cString::sTIMERPROGRESS::OnMouseOver()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->OnMouseOver();
}

void cString::sTIMERPROGRESS::OnMouseLeave()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->OnMouseLeave();
}

void cString::sTIMERPROGRESS::OnMouseDown()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->OnMouseDown();
}

void cString::sTIMERPROGRESS::OnMouseUp()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->OnMouseUp();
}

void cString::sTIMERPROGRESS::onMousePressed()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->OnMousePressed();
}

void cString::sTIMERPROGRESS::Start()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->Start();
}

void cString::sTIMERPROGRESS::Stop()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->Stop();
}

void cString::sTIMERPROGRESS::Reset()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->Reset();
}

void cString::sTIMERPROGRESS::SetTimer(float fCurTime, float fTotalTime)
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->SetTimer( fCurTime, fTotalTime );
}

bool cString::sTIMERPROGRESS::IsPlay() const
{
	SAFE_POINTER_RETVAL( s_pTimerProgress, false );
	return s_pTimerProgress->IsPlay();
}

void cString::sTIMERPROGRESS::SetAlpha( float fAlpha )
{
}
void cString::sTIMERPROGRESS::SetColor( NiColor color )
{
}

void cString::sTIMERPROGRESS::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->Update( fDeltaTime );
}

void cString::sTIMERPROGRESS::ResetDevice()
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->ResetDevice();
}

void cString::sTIMERPROGRESS::SetEnable(bool bEnable )
{
	s_bEnable = bEnable;
	if( s_pTimerProgress )
		s_pTimerProgress->SetEnable( bEnable );
}

void cString::sTIMERPROGRESS::SetVisible(bool bValue)
{
	SAFE_POINTER_RET( s_pTimerProgress );
	s_pTimerProgress->SetVisible(bValue);
}
//////////////////////////////////////////////////////////////////////////
CsPoint cString::GetWorldPos() const
{
	std::list< sELEMENT* >::const_iterator it = m_listText.begin();
	if( it == m_listText.end() )
		return CsPoint::ZERO;

	return (*it)->GetWorldPos();
}

void cString::Update(float const& fDeltaTime)
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->Update( fDeltaTime );
	}
}

CSGBMEMPOOL_CPP( cString::sTEXT );
CSGBMEMPOOL_CPP( cString::sSPRITE );
CSGBMEMPOOL_CPP( cString::sICON );
CSGBMEMPOOL_CPP( cString::sIMAGE );
CSGBMEMPOOL_CPP( cString::sCHECKBOX );
CSGBMEMPOOL_CPP( cString::sDATA_STR );
CSGBMEMPOOL_CPP( cString::sBUTTON );
CSGBMEMPOOL_CPP( cString::sPROGRESS );
CSGBMEMPOOL_CPP( cString::sTIMERPROGRESS );
CSGBMEMPOOL_CPP( cString::sANIBUTTON );

void cString::GlobalInit()
{
	CSGBMEMPOOL_INIT( cString::sTEXT, 16 );
	CSGBMEMPOOL_INIT( cString::sSPRITE, 8 );
	CSGBMEMPOOL_INIT( cString::sICON, 4 );
	CSGBMEMPOOL_INIT( cString::sIMAGE, 4 );
	CSGBMEMPOOL_INIT( cString::sCHECKBOX, 4 );
	CSGBMEMPOOL_INIT( cString::sDATA_STR, 4 );
	CSGBMEMPOOL_INIT( cString::sBUTTON, 4 );
	CSGBMEMPOOL_INIT( cString::sPROGRESS, 4 );
	CSGBMEMPOOL_INIT( cString::sTIMERPROGRESS, 4 );
	CSGBMEMPOOL_INIT( cString::sANIBUTTON, 4 );
}

void cString::GlobalShotDown()
{
	CSGBMEMPOOL_DELETE( cString::sTEXT );
	CSGBMEMPOOL_DELETE( cString::sSPRITE );
	CSGBMEMPOOL_DELETE( cString::sICON );
	CSGBMEMPOOL_DELETE( cString::sIMAGE );
	CSGBMEMPOOL_DELETE( cString::sCHECKBOX );
	CSGBMEMPOOL_DELETE( cString::sDATA_STR );
	CSGBMEMPOOL_DELETE( cString::sBUTTON );
	CSGBMEMPOOL_DELETE( cString::sPROGRESS );
	CSGBMEMPOOL_DELETE( cString::sTIMERPROGRESS );
	CSGBMEMPOOL_DELETE( cString::sANIBUTTON );
}

void cString::Delete()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		switch( (*it)->GetType() )
		{
		case sELEMENT::TEXT:		sTEXT::DeleteInstance( (sTEXT*)*it );			break;
		case sELEMENT::SPRITE:		sSPRITE::DeleteInstance( (sSPRITE*)*it );		break;
		case sELEMENT::ICON:		sICON::DeleteInstance( (sICON*)*it );			break;
		case sELEMENT::IMAGE:		sIMAGE::DeleteInstance( (sIMAGE*)*it );			break;
		case sELEMENT::CHECK_BOX:	sCHECKBOX::DeleteInstance( (sCHECKBOX*)*it );	break;
		case sELEMENT::DATA_STR:	sDATA_STR::DeleteInstance( (sDATA_STR*)*it );	break;
		case sELEMENT::MONEYIF:		NiDelete (sMONEYIF*)*it;						break;
		case sELEMENT::BUTTON:		sBUTTON::DeleteInstance( (sBUTTON*)*it );		break;
		case sELEMENT::PROGRESS:	sPROGRESS::DeleteInstance( (sPROGRESS*)*it );	break;
		case sELEMENT::TIMERPROGRESS:	sTIMERPROGRESS::DeleteInstance( (sTIMERPROGRESS*)*it );	break;
		case sELEMENT::ANIBUTTON:	sANIBUTTON::DeleteInstance( (sANIBUTTON*)*it ); break;
		default:					assert_cs( false );
		}
	}
	m_listText.clear();	
	_Reset();
}

void cString::DeleteElement( int nIndex )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( --nIndex < 0 )
		{
			(*it)->Delete();
			switch( (*it)->GetType() )
			{
			case sELEMENT::TEXT:		sTEXT::DeleteInstance( (sTEXT*)*it );			break;
			case sELEMENT::SPRITE:		sSPRITE::DeleteInstance( (sSPRITE*)*it );		break;
			case sELEMENT::ICON:		sICON::DeleteInstance( (sICON*)*it );			break;
			case sELEMENT::IMAGE:		sIMAGE::DeleteInstance( (sIMAGE*)*it );			break;
			case sELEMENT::CHECK_BOX:	sCHECKBOX::DeleteInstance( (sCHECKBOX*)*it );	break;
			case sELEMENT::DATA_STR:	sDATA_STR::DeleteInstance( (sDATA_STR*)*it );	break;
			case sELEMENT::MONEYIF:		NiDelete (sMONEYIF*)*it;						break;
			case sELEMENT::BUTTON:		sBUTTON::DeleteInstance( (sBUTTON*)*it );		break;
			case sELEMENT::PROGRESS:	sPROGRESS::DeleteInstance( (sPROGRESS*)*it );	break;
			case sELEMENT::TIMERPROGRESS:	sTIMERPROGRESS::DeleteInstance( (sTIMERPROGRESS*)*it );	break;
			case sELEMENT::ANIBUTTON:	sANIBUTTON::DeleteInstance( (sANIBUTTON*)*it ); break;
			default:					assert_cs( false );
			}
			m_listText.erase( it );
			CalMaxSize();
			return;
		}
	}
}

void cString::ResetDevice()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

cString::sTEXT* cString::AddText( cText::sTEXTINFO* pTextInfo, CsPoint deltaPos )
{
	sTEXT* pText = sTEXT::NewInstance();
	pText->s_Text.Init( NULL, CsPoint::ZERO, pTextInfo, true );
	pText->s_ptSize = pText->s_Text.GetStringSize();
	pText->s_ptDeltaPos = deltaPos;

 	m_listText.push_back( pText );

	m_nMaxSize.x += pText->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;

	return pText;
}

cString::sTEXT* cString::AddText( NiStencilProperty* pPropStencil, cText::sTEXTINFO* pTextInfo, CsPoint deltaPos )
{
	sTEXT* pText = sTEXT::NewInstance();
	pText->s_Text.InitStencil( NULL, CsPoint::ZERO, pTextInfo, true, pPropStencil );
	pText->s_ptSize = pText->s_Text.GetStringSize();
	pText->s_ptDeltaPos = deltaPos;

	m_listText.push_back( pText );

	m_nMaxSize.x += pText->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;

	return pText;
}


void cString::AddText_Head( cText::sTEXTINFO* pTextInfo )
{
	SAFE_POINTER_RET( pTextInfo );

	sTEXT* pText = sTEXT::NewInstance();
	pText->s_Text.Init( NULL, CsPoint::ZERO, pTextInfo, true );
	pText->s_ptSize = pText->s_Text.GetStringSize();

	m_listText.push_front( pText );

	m_nMaxSize.x += pText->s_ptSize.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;
}

void cString::SetText( int nElementIndex, TCHAR const* sz )
{
	sELEMENT* pEle = GetElement( nElementIndex );
	assert_cs( pEle != NULL );
	assert_cs( pEle->GetType() == sELEMENT::TEXT );
	sTEXT* pEleText = (sTEXT*)pEle;
	pEleText->s_Text.SetText( sz );
	pEleText->s_ptSize = pEleText->s_Text.GetStringSize();
	CalMaxSize();
}

cString::sSPRITE* cString::AddSprite( cSprite* pSprite, CsPoint deltaPos /*=CsPoint::ZERO*/, CsPoint ptSize /*=CsPoint::ZERO*/ )
{
	sSPRITE* pSpriteInfo = sSPRITE::NewInstance();
	pSpriteInfo->s_bChangeSize = ( ptSize != CsPoint::ZERO );
	if( pSpriteInfo->s_bChangeSize == true )
		pSpriteInfo->s_ptSize = ptSize;
	else
		pSpriteInfo->s_ptSize = pSprite->GetSize();

	pSpriteInfo->s_pSprite = pSprite;	
	pSpriteInfo->s_ptDeltaPos = deltaPos;

	m_listText.push_back( pSpriteInfo );

	m_nMaxSize.x += pSpriteInfo->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pSpriteInfo->s_ptSize.y + deltaPos.y )
		m_nMaxSize.y = pSpriteInfo->s_ptSize.y + deltaPos.y;

	return pSpriteInfo;
}

cString::sPROGRESS*	cString::AddProgress( cProgressBar* pProgress, CsPoint deltaPos/* = CsPoint::ZERO*/, CsPoint ptSize/* = CsPoint::ZERO*/ )
{
	sPROGRESS* pProgressInfo = sPROGRESS::NewInstance();
	pProgressInfo->s_bChangeSize = ( ptSize != CsPoint::ZERO );
	if( pProgressInfo->s_bChangeSize == true )
		pProgressInfo->s_ptSize = ptSize;
	else
		pProgressInfo->s_ptSize = pProgress->GetSize();

	pProgressInfo->s_pProgressbar = pProgress;	
	pProgressInfo->s_ptDeltaPos = deltaPos;

	m_listText.push_back( pProgressInfo );

	m_nMaxSize.x += pProgressInfo->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pProgressInfo->s_ptSize.y + deltaPos.y )
		m_nMaxSize.y = pProgressInfo->s_ptSize.y + deltaPos.y;
	return pProgressInfo;
}

cString::sTIMERPROGRESS* cString::AddTimerProgress(cUI_TimerProgressBar* pTimerProgress, CsPoint deltaPos /*= CsPoint::ZERO */)
{
	SAFE_POINTER_RETVAL( pTimerProgress, NULL );
	sTIMERPROGRESS* pTimerPInfo = sTIMERPROGRESS::NewInstance();
	SAFE_POINTER_RETVAL( pTimerPInfo, NULL );
	pTimerPInfo->s_pTimerProgress = pTimerProgress;
	pTimerPInfo->s_ptSize = pTimerProgress->GetSize();
	pTimerPInfo->s_ptDeltaPos = deltaPos;

	m_listText.push_back( pTimerPInfo );

	m_nMaxSize.x += pTimerPInfo->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pTimerPInfo->s_ptSize.y + deltaPos.y )
		m_nMaxSize.y = pTimerPInfo->s_ptSize.y + deltaPos.y;
	return pTimerPInfo;
}

cString::sSPRITE* cString::AddSprite_Head( cSprite* pSprite, CsPoint deltaPos /*=CsPoint::ZERO*/, CsPoint ptSize /*=CsPoint::ZERO*/ )
{
	sSPRITE* pSpriteInfo = sSPRITE::NewInstance();
	pSpriteInfo->s_bChangeSize = ( ptSize != CsPoint::ZERO );
	if( pSpriteInfo->s_bChangeSize == true )
		pSpriteInfo->s_ptSize = ptSize;
	else
		pSpriteInfo->s_ptSize = pSprite->GetSize();

	pSpriteInfo->s_pSprite = pSprite;	
	pSpriteInfo->s_ptDeltaPos = deltaPos;

	m_listText.push_front( pSpriteInfo );

	m_nMaxSize.x += pSpriteInfo->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pSpriteInfo->s_ptSize.y + deltaPos.y )
		m_nMaxSize.y = pSpriteInfo->s_ptSize.y + deltaPos.y;

	return pSpriteInfo;
}


cString::sICON* cString::AddIcon( CsPoint size, int nIconType, int nIndex1, int nIndex2 /*=0*/, CsPoint deltaPos /*=CsPoint::ZERO*/ )
{
	sICON* pIcon = sICON::NewInstance();
	pIcon->s_ptSize = size;
	pIcon->s_nIconType = nIconType;
	pIcon->s_nIndex_1 = nIndex1;
	pIcon->s_nIndex_2 = nIndex2;
	pIcon->s_nIndex_3 = 0;
	pIcon->s_fAlpha = 1.0f;
	pIcon->s_ptDeltaPos = deltaPos;
	pIcon->s_ptIconSize = size;
	pIcon->s_ptLastPos = CsPoint::ZERO;

	m_listText.push_back( pIcon );

	m_nMaxSize.x += size.x + deltaPos.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pIcon;
}

cString::sICON* cString::AddSkillIcon(CsPoint size, int nIconType, int nIndex1, int nIndex2/*=0*/, CsPoint deltaPos /*= CsPoint::ZERO */)
{
	sICON* pIcon = sICON::NewInstance();
	pIcon->s_ptSize = size;
	pIcon->s_nIndex_2 = nIndex2;			// 스킬 인덱스
	pIcon->s_nIndex_3 = 0;
	pIcon->s_fAlpha = 1.0f;
	pIcon->s_ptDeltaPos = deltaPos;
	pIcon->s_ptIconSize = size;
	pIcon->s_ptLastPos = CsPoint::ZERO;

	switch( nIconType )
	{
	case CsSkill::IT_CHANGE3:
	case CsSkill::IT_CHANGE1: 
	case CsSkill::IT_CHANGE2:	pIcon->s_nIconType = ICONITEM::SKILL_MASK;	break;			
	default:
		{
			if( nIconType >= 4000 )
				pIcon->s_nIconType = ICONITEM::SKILL4;
			else if( nIconType >= 3000 )
				pIcon->s_nIconType = ICONITEM::SKILL3;
			else if( nIconType >= 2000 )
				pIcon->s_nIconType = ICONITEM::SKILL2;
			else
				pIcon->s_nIconType = ICONITEM::SKILL1;
		}
		break;
	}

	if( ICONITEM::SKILL_MASK != pIcon->s_nIconType )
		pIcon->s_nIndex_1 = nIndex1 % 1000;
	else
		pIcon->s_nIndex_1 = nIndex1;

	m_listText.push_back( pIcon );

	m_nMaxSize.x += size.x + deltaPos.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pIcon;
}

cString::sICON* cString::AddIcon_Head( CsPoint size, int nIconType, int nIndex1, int nIndex2 /*=0*/, CsPoint deltaPos /*=CsPoint::ZERO*/ )
{
	sICON* pIcon = sICON::NewInstance();
	pIcon->s_ptSize = size;
	pIcon->s_nIconType = nIconType;
	pIcon->s_nIndex_1 = nIndex1;
	pIcon->s_nIndex_2 = nIndex2;
	pIcon->s_nIndex_3 = 0;
	pIcon->s_fAlpha = 1.0f;
	pIcon->s_ptDeltaPos = deltaPos;
	pIcon->s_ptIconSize = size;

	m_listText.push_front( pIcon );

	m_nMaxSize.x += size.x + deltaPos.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pIcon;
}

cString::sIMAGE* cString::AddImage( cImage* pImage, int nImageIndex, CsPoint deltaPos /*=CsPoint::ZERO*/, CsPoint imageSize /*=CsPoint::ZERO*/ )
{
	CsPoint size = pImage->GetSize();

	sIMAGE* pImageInfo = sIMAGE::NewInstance();
	pImageInfo->s_pImage = pImage;
	pImageInfo->s_nImageIndex = nImageIndex;
	pImageInfo->s_ptDeltaPos = deltaPos;
	pImageInfo->s_ptSize = size;
	pImageInfo->s_ptImageSize = imageSize;

	m_listText.push_back( pImageInfo );

	m_nMaxSize.x += size.x + deltaPos.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pImageInfo;
}

cString::sIMAGE* cString::AddImage_Head( cImage* pImage, int nImageIndex, CsPoint deltaPos /*=CsPoint::ZERO*/, CsPoint imageSize /*=CsPoint::ZERO*/ )
{
	CsPoint size = pImage->GetSize();

	sIMAGE* pImageInfo = sIMAGE::NewInstance();
	pImageInfo->s_pImage = pImage;
	pImageInfo->s_nImageIndex = nImageIndex;
	pImageInfo->s_ptDeltaPos = deltaPos;
	pImageInfo->s_ptSize = size;
	pImageInfo->s_ptImageSize = imageSize;

	m_listText.push_front( pImageInfo );

	m_nMaxSize.x += size.x + deltaPos.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pImageInfo;
}

cString::sBUTTON* cString::AddButton( cButton* pButton, int nButtonValue, CsPoint deltaPos, CsPoint ptSize, CsPoint textPos  )
{
	sBUTTON* pButtonInfo = sBUTTON::NewInstance();	
	pButtonInfo->s_pButton = pButton;	
	pButtonInfo->s_ptSize = ptSize;	
	pButtonInfo->s_ptDeltaPos = deltaPos;
	pButtonInfo->s_nValue1 = nButtonValue;
	pButtonInfo->m_ptTextPos = textPos;

	m_listText.push_back( pButtonInfo );


	m_nMaxSize.x += ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < ptSize.y )
		m_nMaxSize.y = ptSize.y;

	return pButtonInfo;
}

cString::sBUTTON* cString::AddButton_Head( cButton* pButton, int nButtonValue, CsPoint deltaPos /*= CsPoint::ZERO*/, CsPoint ptSize /*= CsPoint::ZERO*/ )
{
	sBUTTON* pButtonInfo = sBUTTON::NewInstance();	
	pButtonInfo->s_pButton = pButton;	
	pButtonInfo->s_ptSize = ptSize;	
	pButtonInfo->s_ptDeltaPos = deltaPos;
	pButtonInfo->s_nValue1 = nButtonValue;

	m_listText.push_front( pButtonInfo );

	m_nMaxSize.x += ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < ptSize.y )
		m_nMaxSize.y = ptSize.y;

	return pButtonInfo;
}

cString::sANIBUTTON* cString::AddAniButton(cAniButton* pAniButton, int nButtonValue, CsPoint deltaPos /*= CsPoint::ZERO*/, CsPoint ptSize /*= CsPoint::ZERO */)
{
	sANIBUTTON* pButtonInfo = sANIBUTTON::NewInstance();
	pButtonInfo->s_pButton = pAniButton;	
	pButtonInfo->s_ptSize = ptSize;	
	pButtonInfo->s_ptDeltaPos = deltaPos;
	pButtonInfo->s_nValue1 = nButtonValue;

	m_listText.push_back( pButtonInfo );

	m_nMaxSize.x += ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < ptSize.y )
		m_nMaxSize.y = ptSize.y;

	return pButtonInfo;
}

cString::sCHECKBOX* cString::AddCheckBox( char* cCheckBoxPath, bool bCheck, CsPoint TexToken, CsPoint CheckBoxSize, CsPoint deltaPos /*=CsPoint::ZERO*/ )
{
	sCHECKBOX* pCheckBox = sCHECKBOX::NewInstance();
	pCheckBox->s_pCheckBox = NiNew cCheckBox;
	pCheckBox->s_pCheckBox->Init( NULL, CsPoint::ZERO, CheckBoxSize, cCheckBoxPath, false );
	pCheckBox->s_pCheckBox->SetTexToken( TexToken );
	pCheckBox->s_pCheckBox->SetCheck( bCheck );
	pCheckBox->s_ptDeltaPos = deltaPos;
	pCheckBox->s_ptSize = CheckBoxSize;

	m_listText.push_back( pCheckBox );

	m_nMaxSize.x += pCheckBox->s_ptSize.x + deltaPos.x;
	if( m_nMaxSize.y < pCheckBox->s_ptSize.y )
		m_nMaxSize.y = pCheckBox->s_ptSize.y;

	return pCheckBox;
}

cString::sMONEYIF* cString::AddMoneyIF( int nDisplayType, u8 nMoney, NiColor color, CFont::eFACE_SIZE eFontSize /*=CFont::FS_9*/, int eAllign /*=DT_RIGHT*/, CsPoint deltaPos /*=CsPoint::ZERO*/, bool bShowBG /*= false*/, DWORD dwItemID )
{
	sMONEYIF* pMoney = NiNew sMONEYIF;
	pMoney->s_ptDeltaPos = deltaPos;

	pMoney->s_pMoney = NiNew cMoneyIF;
	( (cMoneyIF*)pMoney->s_pMoney )->SetEventItemID( dwItemID ); // [9/19/2016 hyun] 현욱 추가
	( (cMoneyIF*)pMoney->s_pMoney )->Init( (cMoneyIF::eDISP_TYPE)nDisplayType, nMoney, false, color, eAllign, eFontSize );
	( (cMoneyIF*)pMoney->s_pMoney )->SetShowBG( bShowBG );

	CsPoint size = ( (cMoneyIF*)pMoney->s_pMoney )->GetSize();
	pMoney->s_ptSize = size;

	m_listText.push_back( pMoney );

	m_nMaxSize.x += size.x + deltaPos.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pMoney;
}

void cString::AddPropertyPrice( DWORD dwNpcID, std::vector<int>& vecItemID, std::vector<int>& vecItemPrice )
{
	if(vecItemID.empty())
		return;
	if(vecItemPrice.empty())
		return;

	CsPoint deltaPos( 115, 35 );
	sMONEYIF* pMoney = NiNew sMONEYIF;
	pMoney->s_ptDeltaPos = deltaPos;	

	pMoney->s_pMoney = NiNew cMoneyIF;
	( (cMoneyIF*)pMoney->s_pMoney )->Init_ForPropertyStore( dwNpcID );

	sMONEY ItemPrice( vecItemPrice[0], vecItemPrice[1], vecItemPrice[2], vecItemPrice[3] );
	for( int i = 0, nKind = 1; i < vecItemPrice.size(); ++i, ++nKind )
	{
		if(vecItemPrice[i] == 0 && vecItemID[i] == 0)
			continue;

		( (cMoneyIF*)pMoney->s_pMoney )->SetPropertyStorePrice( ItemPrice, nKind, vecItemID[i] );

		CsPoint size = ( (cMoneyIF*)pMoney->s_pMoney )->GetSize();
		m_nMaxSize.x += size.x + deltaPos.x;
		if( m_nMaxSize.y < size.y )
			m_nMaxSize.y = size.y;
	}

	pMoney->s_ptSize = ( (cMoneyIF*)pMoney->s_pMoney )->GetSize();

	m_listText.push_back( pMoney );
}

void cString::AddData_Str( int nValue, TCHAR* strBuffer, int nBufferSize )
{
	assert_cs( nBufferSize <= 64 );

	sDATA_STR* pStr = sDATA_STR::NewInstance();
	pStr->s_ptSize = CsPoint::ZERO;
	pStr->s_nValue = nValue;
	_tcscpy_s( pStr->s_szStr, nBufferSize, strBuffer );
	m_listText.push_back( pStr );
}

void cString::OnMouseOver()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
		(*it)->OnMouseOver();
}

void cString::OnMouseLeave()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
		(*it)->OnMouseLeave();
}

void cString::OnMouseDown()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
		(*it)->OnMouseDown();
}

void cString::OnMouseUp()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
		(*it)->OnMouseUp();
}

void cString::onMousePressed()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
		(*it)->onMousePressed();
}

void cString::SetControlOnMode(bool bValue)
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
		(*it)->SetOnMode( bValue );
}

cString::sELEMENT* cString::Update_ForMouse_Control( CsPoint pos, cString::sELEMENT** ppActiveElement /*=NULL*/ )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;

		switch( pElement->GetType() )
		{
		case sELEMENT::CHECK_BOX:
			{
				sCHECKBOX* pCheckBox = (sCHECKBOX*)pElement;
				pCheckBox->s_pCheckBox->SetPos( pos + pCheckBox->s_ptDeltaPos );
				switch( pCheckBox->s_pCheckBox->Update_ForMouse() )
				{
				case cCheckBox::ACTION_CLICK:
					if( ppActiveElement != NULL )
						*ppActiveElement = pCheckBox;
					return pCheckBox;
				case cCheckBox::ACTION_NONE:
					break;					
				default:
					if( ppActiveElement != NULL )
						*ppActiveElement = pCheckBox;
					return NULL;
				}					
			}
			break;
		case sELEMENT::BUTTON:
			{
				sBUTTON* pButton = (sBUTTON*)pElement;
				pButton->s_pButton->SetPos( pos + pButton->s_ptDeltaPos );
				switch( pButton->s_pButton->Update_ForMouse() )
				{
				case cButton::ACTION_CLICK:
					if( ppActiveElement != NULL )
						*ppActiveElement = pButton;
					return pButton;
				case cButton::ACTION_NONE:
					break;					
				default:
					if( ppActiveElement != NULL )
						*ppActiveElement = pButton;
					return NULL;
				}					
			}
			break;
		case sELEMENT::TEXT:
		case sELEMENT::SPRITE:
		case sELEMENT::ICON:
		case sELEMENT::IMAGE:		
		case sELEMENT::MONEYIF:
		case sELEMENT::DATA_STR:	
		case sELEMENT::PROGRESS:		
		case sELEMENT::TIMERPROGRESS:
		case sELEMENT::ANIBUTTON:
			break;
		default:
			assert_cs( false );
		}		
		pos.x += pElement->s_ptSize.x;
	}
	return NULL;
}

cString::sELEMENT* cString::Update_ForMouseOver_Control( CsPoint pos )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( !(*it)->GetWorldRect().PtInRect( pos ) )
			continue;
		return (*it);		
	}
	return NULL;
}

int cString::Render( CsPoint pos, UINT Align )
{
	switch(Align)
	{
	case DT_LEFT:														break;
	case DT_CENTER:		pos.x -= CsFloat2Int( m_nMaxSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_nMaxSize.x;							break;
	}

	return Render( pos );
}

int cString::Render( CsPoint pos )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;
 		pElement->Render( pos );
		pos.x += pElement->s_ptSize.x;
	}	
	return ( pos.y += m_nMaxSize.y );
}

int cString::Render_Overlab( CsPoint pos, UINT Align )
{
	switch(Align)
	{
	case DT_LEFT:														break;
	case DT_CENTER:		pos.x -= CsFloat2Int( m_nMaxSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_nMaxSize.x;							break;
	case DT_VCENTER: 
		pos.x -= CsFloat2Int( m_nMaxSize.x*0.5f );
		pos.y -= CsFloat2Int( m_nMaxSize.y*0.5f );
		break;
	}

	return Render_Overlab( pos );
}

int cString::Render_Overlab( CsPoint pos )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;
		pElement->Render( pos );		
	}	
	return pos.y ;
}

int cString::RenderLimit( CsPoint pos, UINT Align )
{
	switch(Align)
	{
	case DT_LEFT:														break;
	case DT_CENTER:		pos.x -= CsFloat2Int( m_nMaxSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_nMaxSize.x;							break;
	}

	return RenderLimit( pos );
}

int cString::RenderLimit( CsPoint pos )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;
		pElement->RenderLimit( pos );
		pos.x += pElement->s_ptSize.x;
	}	
	return ( pos.y += m_nMaxSize.y );
}

void cString::SetAlpha(float fAlpha /*부하가적다*/ )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SetAlpha( fAlpha );
	}
}

void cString::SetColor( NiColor color )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SetColor( color );
	}
}

void cString::TailAddSizeX( int nSizeX )
{
	if( m_listText.empty() )
	{
		HeadAddSizeX( nSizeX );
	}
	else
	{
		std::list< sELEMENT* >::iterator it = m_listText.end();
		--it;
		( *it )->s_ptSize.x += nSizeX;
		m_nMaxSize.x += nSizeX;
	}
}

void cString::HeadAddSizeX( int nSizeX )
{	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );	
	ti.SetText( _T( " " ) );//chu8820 - 스트링 X사이즈 추가 시 텍스트 셋 실패 ( C009 )
	AddText_Head( &ti );

	std::list< sELEMENT* >::iterator it = m_listText.begin();
	if( m_listText.end() == it )
		return;

	( *it )->s_ptSize.x += nSizeX;
	m_nMaxSize.x += nSizeX;
}
// void cString::SetEnable( bool bValue, int nValue)
// {
// 	std::list< sELEMENT* >::iterator it = m_listText.begin();
// 	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
// 	
// 	for( ; it!=itEnd; ++it )
// 		(*it)->SetEnable( bValue , nValue);
// }

void cString::SetEnable( bool bEnable )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
		(*it)->SetEnable( bEnable );
}

bool cString::GetEnabled() const
{
	if( m_listText.empty() )
		return true;
	std::list< sELEMENT* >::const_iterator it = m_listText.begin();
	return (*it)->GetEnable();
}

void cString::CalMaxSize()
{
	m_nMaxSize = CsPoint::ZERO;
	CsPoint size;

	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		size = (*it)->s_ptSize;
		m_nMaxSize.x += size.x;
		if( m_nMaxSize.y < size.y )
			m_nMaxSize.y = size.y;
	}
}
void cString::CalMaxVSize(void)
{
	m_nMaxSize = CsPoint::ZERO;
	CsPoint size;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		size = (*it)->s_ptSize;
		if( m_nMaxSize.x < size.x )
			m_nMaxSize.x = size.x;
		m_nMaxSize.y +=size.y; 
	}
}

CsPoint	cString::GetMaxSize()
{ 
	return m_nMaxSize; 
}	

void cString::SetMaxSize( CsPoint ptMaxSize )
{ 
	m_nMaxSize = ptMaxSize; 
}

void cString::SetValue1( int nValue1 )
{ 
	m_nValue1 = nValue1; 
}

int cString::GetValue1()
{ 
	return m_nValue1; 
}

void cString::SetValue2( int nValue2 )
{ 
	m_nValue2 = nValue2;
}

int cString::GetValue2()
{ 
	return m_nValue2; 
}

void cString::SetValue3( int nValue3 )
{ 
	m_nValue3 = nValue3; 
}

int cString::GetValue3()
{ 
	return m_nValue3; 
}

void cString::SetValue4( int nValue4 )
{ 
	m_nValue4 = nValue4; 
}

int cString::GetValue4()
{ 
	return m_nValue4; 
}

void cString::SetValuePointer1( void* pValuePointer )
{ 
	m_pValuePointer1 = pValuePointer; 
}

void* cString::GetValuePointer1()
{ 
	return m_pValuePointer1; 
}

void cString::AddSizeY( int nSizeY )
{ 
	m_nMaxSize.y += nSizeY; 
}

void cString::SetSizeY( int nSizeY )
{ 
	m_nMaxSize.y = nSizeY; 
}

int cString::GetListSize()
{ 
	return (int)m_listText.size(); 
}

//====================================================================================================
//
//		Find
//
//====================================================================================================

cString::sELEMENT* cString::GetElement( int nIndex )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( --nIndex < 0 )
			return *it;
	}
	assert_cs( false );
	return NULL;
}
int	cString::GetListIndex( void* pPointer )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( int i = 0; it!=itEnd; ++it, ++i )
	{ 
		if( *it == pPointer )
		{
			return i;
		}
	}
	return -1;

}

bool cString::IsText( TCHAR* szText )
{
	sTEXT* pText;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() != sELEMENT::TEXT )
			continue;

		pText = (sTEXT*)(*it);
		if( _tcscmp( pText->s_Text.GetTextInfo()->GetText(), szText ) == 0 )
			return true;
	}
	return false;
}

bool cString::IsIncludeText( std::wstring const& findText, bool btolower )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	for( ; it != m_listText.end(); ++it )
	{
		SAFE_POINTER_CON( *it );
		if( (*it)->GetType() != sELEMENT::TEXT )
			continue;

		sTEXT* pText = (sTEXT*)(*it);
		std::wstring wsText = pText->s_Text.GetTextInfo()->GetText();
		if( btolower )
			std::transform(wsText.begin(), wsText.end(), wsText.begin(), tolower);	//	tolower : 대문자 -> 소문자로

		size_t strSizeT = wsText.find(findText);
		if( strSizeT != std::wstring::npos)	// 같은 문자가 있으면.
			return true;
	}
	return false;
}

cString::sELEMENT* cString::FindElement( int nElementType )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() == nElementType )
			return *it;
	}
	return NULL;
}

void cString::FindElement( int nElementType, std::list<sELEMENT*>& rList )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() == nElementType )
			rList.push_back(*it);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//		CStringList
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

cStringList::cStringList() : m_bIsVisible( true ), m_pScrollBar( NULL ), m_pUnderLineCursor( NULL ), m_fCursorTime( 0.0f )
{

}

cStringList::~cStringList()
{
	Delete();
	m_pScrollBar = NULL;
	m_pUnderLineCursor = NULL;
}

void cStringList::Delete( bool bDeleteUnderLine /*=true*/ )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		SAFE_NIDELETE( (*it) );
	}
	m_list.clear();	

	if( bDeleteUnderLine == true )
		SAFE_NIDELETE( m_pUnderLineCursor );

	// 커서 언더라인 바로 보이게
	m_fCursorTime = 0.0f;
}

void cStringList::ResetDevice()
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}

	if( m_pUnderLineCursor )
		m_pUnderLineCursor->ResetDevice();
}


// void cStringList::SetEnable( bool bValue )
// {
// 	std::list< cString* >::iterator it = m_list.begin();
// 	std::list< cString* >::iterator itEnd = m_list.end();
// 	for( ; it!=itEnd; ++it )
// 		(*it)->SetEnable( bValue,  (*it)->GetValue1() );
// }

int cStringList::Update_ForMouse( cString** ppSelect, int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize )
{
	*ppSelect = NULL;

	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			if( (*it)->GetValue1() != cString::INVALIDE_STRING_VALUE )
			{
				CURSOR::eTYPE ct = CURSOR_ST.CheckClickBox( CsRect( pos, CheckSize ) );
				if( ct != CURSOR::BUTTON_OUTWINDOW )
				{
					nActiveIndex1 = (*it)->GetValue1();
					nActiveIndex2 = (*it)->GetValue2();
					*ppSelect = *it;
					return ct;
				}
			}
			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	else
	{		
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			if( (*it)->GetValue1() != cString::INVALIDE_STRING_VALUE )
			{
				CURSOR::eTYPE ct = CURSOR_ST.CheckClickBox( CsRect( pos, CheckSize ) );
				if( ct != CURSOR::BUTTON_OUTWINDOW )
				{
					nActiveIndex1 = (*it)->GetValue1();
					nActiveIndex2 = (*it)->GetValue2();
					*ppSelect = *it;
					return ct;
				}
			}
			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );

			if( --nRender < 1 )
				break;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}

	nActiveIndex1 = cString::INVALIDE_STRING_VALUE;
	nActiveIndex2 = cString::INVALIDE_STRING_VALUE;
	return CURSOR::BUTTON_OUTWINDOW;
}


int cStringList::Update_ForMouse( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize )
{
	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			if( (*it)->GetValue1() != cString::INVALIDE_STRING_VALUE )
			{
				CURSOR::eTYPE ct = CURSOR_ST.CheckClickBox( CsRect( pos, CheckSize ) );
				if( ct != CURSOR::BUTTON_OUTWINDOW )
				{
					nActiveIndex1 = (*it)->GetValue1();
					nActiveIndex2 = (*it)->GetValue2();
					return ct;
				}
			}
			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	else
	{		
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			if( (*it)->GetValue1() != cString::INVALIDE_STRING_VALUE )
			{
				CURSOR::eTYPE ct = CURSOR_ST.CheckClickBox( CsRect( pos, CheckSize ) );
				if( ct != CURSOR::BUTTON_OUTWINDOW )
				{
					nActiveIndex1 = (*it)->GetValue1();
					nActiveIndex2 = (*it)->GetValue2();
					return ct;
				}
			}
			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );

			if( --nRender < 1 )
				break;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}

	nActiveIndex1 = cString::INVALIDE_STRING_VALUE;
	nActiveIndex2 = cString::INVALIDE_STRING_VALUE;
	return CURSOR::BUTTON_OUTWINDOW;
}

cString::sELEMENT* 
cStringList::Update_ForMouse_Control( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, cString::sELEMENT** ppActiveElement /*=NULL*/ )
{
	cString::sELEMENT* pElement = NULL;
	if( ppActiveElement != NULL )
	{
		*ppActiveElement = NULL;
	}
	

	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pElement = (*it)->Update_ForMouse_Control( pos, ppActiveElement );
			if( pElement != NULL )
			{
				nActiveIndex1 = (*it)->GetValue1();
				nActiveIndex2 = (*it)->GetValue2();
				return pElement;
			}

			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	else
	{		
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pElement = (*it)->Update_ForMouse_Control( pos, ppActiveElement );
			if( pElement != NULL )
			{
				nActiveIndex1 = (*it)->GetValue1();
				nActiveIndex2 = (*it)->GetValue2();
				return pElement;
			}

			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );

			if( --nRender < 1 )
				break;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}

	nActiveIndex1 = cString::INVALIDE_STRING_VALUE;
	nActiveIndex2 = cString::INVALIDE_STRING_VALUE;
	return NULL;
}

void		cStringList::Render_Overlab_RenderText( CsPoint pos, UINT Align )
{
	CsPoint ptCursor;
	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			cString::sTEXT* pText =	(cString::sTEXT*)(*it)->FindElement( cString::sELEMENT::TEXT );

			(*it)->Render_Overlab( pos, pText->s_Text.Get_TextInfo().s_eTextAlign );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();

			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			(*it)->Render_Overlab( pos, Align );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();


			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}

	if( m_pUnderLineCursor )
	{
		assert_cs( m_pUnderLineCursor != NULL );
		m_fCursorTime += g_fDeltaTime;
		if( m_fCursorTime > 1.0f )
		{
			m_fCursorTime = 0;
		}

		// 커서 깜박임
		if( m_fCursorTime < 0.5f )
		{   
			m_pUnderLineCursor->Render( pos + CsPoint( ptCursor.x, -ptCursor.y ) );
		}
	}

}
void cStringList::Render_Overlab( CsPoint pos, UINT Align /* = DT_LEFT */ )
{
	if( !m_bIsVisible )
		return;

	CsPoint ptCursor;
	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			cString::sTEXT* pText =	(cString::sTEXT*)(*it)->FindElement( cString::sELEMENT::TEXT );
			
			pos.y = (*it)->Render_Overlab( pos, pText->s_Text.Get_TextInfo().s_eTextAlign );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();
			
			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			(*it)->Render_Overlab( pos, Align );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();

			
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}

	if( m_pUnderLineCursor )
	{
		assert_cs( m_pUnderLineCursor != NULL );
		m_fCursorTime += g_fDeltaTime;
		if( m_fCursorTime > 1.0f )
		{
			m_fCursorTime = 0;
		}

		// 커서 깜박임
		if( m_fCursorTime < 0.5f )
		{   
			m_pUnderLineCursor->Render( pos + CsPoint( ptCursor.x, -ptCursor.y ) );
		}
	}
}

void cStringList::Render( CsPoint pos, int nDeltaY, UINT Align /* = DT_LEFT */ )
{
	if( !m_bIsVisible )
		return;

	CsPoint ptCursor;
	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pos.y = (*it)->Render( pos, Align );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();

			pos.y += nDeltaY;
			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pos.y = (*it)->Render( pos, Align );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();

			pos.y += nDeltaY;
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}

	if( m_pUnderLineCursor )
	{
		assert_cs( m_pUnderLineCursor != NULL );
		m_fCursorTime += g_fDeltaTime;
		if( m_fCursorTime > 1.0f )
		{
			m_fCursorTime = 0;
		}

		// 커서 깜박임
		if( m_fCursorTime < 0.5f )
		{   
			m_pUnderLineCursor->Render( pos + CsPoint( ptCursor.x, -ptCursor.y ) );
		}
	}
}

void cStringList::RenderEnd( CsPoint pos, int nDeltaY, UINT Align /* = DT_LEFT */ )
{
	if( !m_bIsVisible )
		return;

	CsPoint ptCursor;
	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pos.y = (*it)->Render( pos, Align );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();

			pos.y += nDeltaY;
			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetMaxPosIndex() - m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pos.y = (*it)->Render( pos, Align );
			if( m_pUnderLineCursor )
				ptCursor = (*it)->GetMaxSize();

			pos.y += nDeltaY;
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;			
		}		
	}

	if( m_pUnderLineCursor )
	{
		assert_cs( m_pUnderLineCursor != NULL );
		m_fCursorTime += g_fDeltaTime;
		if( m_fCursorTime > 1.0f )
		{
			m_fCursorTime = 0;
		}

		// 커서 깜박임
		if( m_fCursorTime < 0.5f )
		{   
			m_pUnderLineCursor->Render( pos + CsPoint( ptCursor.x, -ptCursor.y ) );
		}
	}
}

void cStringList::Render_LineHeight( CsPoint pos )
{
	if( !m_bIsVisible )
		return;

	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			(*it)->Render( pos );
			pos.y += m_StrInfo.s_nLineHeight;

			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pos.y = (*it)->Render( pos );
			pos.y += m_StrInfo.s_nLineHeight;
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;
		}
	}
}


void cStringList::RenderLimit( CsPoint pos, int nDeltaY, UINT Align )
{
	if( !m_bIsVisible )
		return;

	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pos.y = (*it)->RenderLimit( pos, Align );
			pos.y += nDeltaY;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pos.y = (*it)->RenderLimit( pos, Align );
			pos.y += nDeltaY;
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;
		}
	}
}

void cStringList::SetAlpha(float fAlpha /*부하가적다*/ )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SetAlpha( fAlpha );
	}
}

void cStringList::AddHead( cString* pString )
{
	m_list.push_front( pString );
}

void cStringList::AddTail( cString* pString )
{
	m_list.push_back( pString );
}

void cStringList::AddTailEmpty( CFont::eFACE_SIZE eFontSize )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, eFontSize );
	ti.SetText( _T( "" ) );

	cString* pString = NiNew cString;
	pString->AddText( &ti );
	AddTail( pString );
}

void cStringList::AddIndexAfter( cString* pString, int nIndex )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (--nIndex) < 0 )
		{
			m_list.insert( ++it, pString );
			return;
		}
	}
	assert_cs( false );
}

void cStringList::RemoveHead()
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.begin();
	SAFE_NIDELETE(*it);
	m_list.erase( it );	
}

void cStringList::RemoveTail()
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.end();
	--it;
	
	SAFE_NIDELETE(*it);
	m_list.erase( it );
}

void cStringList::RemoveIndex( int nIndex )
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (--nIndex) < 0 )
		{
			SAFE_NIDELETE(*it);
			m_list.erase( it );	
			return;
		}
	}	
}

cString* cStringList::GetString( int nIndex )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (--nIndex) < 0 )
			return (*it);
	}
	assert_cs( false );
	return NULL;
}

int cStringList::GetStringPosY( int nStartY, int nIndex, int nDeltaY )
{
	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			if( (--nIndex) < 0 )
				return nStartY;
			nStartY += (*it)->GetMaxSize().y + nDeltaY;
		}
	}
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				if( (--nIndex) < 0 )
					return INVALIDE_FIND;

				continue;
			}

			if( (--nIndex) < 0 )
				return nStartY;

			nStartY += (*it)->GetMaxSize().y + nDeltaY;

			if( --nRender < 1 )
				break;
		}
	}
	return INVALIDE_FIND;
}

CsPoint cStringList::GetLastPos( int nDeltaY )
{
	CsPoint ptPos = CsPoint::ZERO;

	if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();

		for( ; it!=itEnd; ++it )
		{	
			ptPos.x = (*it)->GetMaxSize().x;
			ptPos.y += (*it)->GetMaxSize().y + nDeltaY;
		}			
	}
	else
	{
		int nIndex = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nIndex > 0 )
			{
				--nIndex;				
				continue;
			}			

			ptPos.x = (*it)->GetMaxSize().x;
			ptPos.y += (*it)->GetMaxSize().y + nDeltaY;			

			if( (--nRender) < 0 )
				return ptPos;
		}
	}

	return ptPos;
}

cString* cStringList::GetTail()
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.end();
	--it;
	return *it;
}

CsPoint cStringList::CalMaxSize( int nDeltaY )
{
	CsPoint size = CsPoint::ZERO;
	CsPoint src;
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		// Defensive — the talk-balloon pool occasionally produces a NULL/dangling
		// entry after a Delete()/Init() reuse race (cTip::Update random digimon
		// tip path), and dereferencing it crashes inside cString::GetMaxSize+0xa.
		// Skipping NULLs here lets the balloon render without the rest of its
		// strings being lost.
		if( *it == NULL )
			continue;
		src = (*it)->GetMaxSize();
		if( size.x < src.x )
			size.x = src.x;
		size.y += ( src.y + nDeltaY );
	}
	return size;
}

void cStringList::SetSameMaxWidth( int nWidthX )
{
	CsPoint size = CsPoint::ZERO;
	CsPoint src;
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		src = (*it)->GetMaxSize();
		src.x = nWidthX;
		(*it)->SetMaxSize( src );
	}
}


void cStringList::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
}

//====================================================================================================
//
//		Find
//
//====================================================================================================

int cStringList::FindText( TCHAR* szText )
{
	int nIndex = 0;
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->IsText( szText ) == true )
		{
			return nIndex;
		}

		++nIndex;
	}
	return INVALIDE_FIND;
}

bool cStringList::IsIncludeText( std::wstring const& findText, bool btolower )
{
	std::list< cString* >::iterator it = m_list.begin();
	for( ; it != m_list.end(); ++it )
	{
		SAFE_POINTER_CON( *it );
		if( (*it)->IsIncludeText( findText, btolower ) )
			return true;			
	}
	return false;
}

//====================================================================================================
//
//		Buddy
//
//====================================================================================================

void cStringList::SetBuddy( cScrollBar* pScrollBar )
{
	m_pScrollBar = pScrollBar;
}


//====================================================================================================
//
//		UnderLineCursor
//
//====================================================================================================
void cStringList::SetUnderLineCursor( NiColor color )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10, color );	
	ti.SetText( _T( "_" ) );
	ti.s_bOutLine = false;	

	m_pUnderLineCursor = NiNew cText;
	m_pUnderLineCursor->Init( NULL, CsPoint::ZERO, &ti, false );
}
