
#include "stdafx.h"
#include "BattleTypeSelectUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleTypeSelectUI::CBattleTypeSelectUI() 
{
}

CBattleTypeSelectUI::~CBattleTypeSelectUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CBattleTypeSelectUI::Destroy()
{
	DeleteScript();	
}

bool CBattleTypeSelectUI::Create()
{
	InitScript( NULL, CsPoint( g_nScreenWidth / 2 - 512, 0 ), CsPoint( 1024, g_nScreenHeight ), false );
	AddSprite( CsPoint(0, 40), CsPoint(1024, 128), "Battle\\BattleTypeSelect\\Battle_Mode_Top.tga" );

	_MakeBattleTypeList();

	// 선택 및 등록 버튼
	m_pTypeSelectBtn = AddButton( CsPoint( 1024 / 2 - (162 / 2) - 90, g_nScreenHeight - 75 ), CsPoint( 162, 42 ), CsPoint( 0, 42 ), "Battle\\Common\\Common_Btn.tga");
	if( m_pTypeSelectBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_SELECT_TYPE" ).c_str());
		m_pTypeSelectBtn->SetText( &ti );
		m_pTypeSelectBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleTypeSelectUI::_Forward );
		m_pTypeSelectBtn->SetEnable(false);
	}

	// 닫기 버튼
	m_pTypeCloseBtn = AddButton( CsPoint( 1024 / 2 - (162 / 2) + 90, g_nScreenHeight - 75 ), CsPoint( 162, 42 ), CsPoint( 0, 42 ), "Battle\\Common\\Close_Btn.tga");	
	if( m_pTypeCloseBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_CLOSE" ).c_str());
		m_pTypeCloseBtn->SetText( &ti );
		m_pTypeCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleTypeSelectUI::_Backward );
	}

	return true;
}

void CBattleTypeSelectUI::_MakeBattleTypeList()
{
	std::list<unsigned int> listInfo;
	GetSystem()->GetBattleTypeList(listInfo);

	if( listInfo.empty() )
		return;

	m_pBattleTypeListBox = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pBattleTypeListBox );

	CsPoint uiSize(317, 457);
	int nTypeCount = static_cast<int>(listInfo.size());
	if( nTypeCount < 3 )
		uiSize.x = (uiSize.x * nTypeCount) + (5 * (nTypeCount - 1));
	else
		uiSize.x = (uiSize.x * 3) + (5 * (3 - 1));

	m_pBattleTypeListBox->Init( GetRoot(), CsPoint( (1024 / 2) - (uiSize.x / 2), (g_nScreenHeight / 2) - (457 / 2) ), uiSize
		, CsPoint(5,0), CsPoint(317, 457), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );

	m_pBattleTypeListBox->SetMouseOverImg( "Battle\\BattleTypeSelect\\battle_mode_glow.tga" );
	m_pBattleTypeListBox->SetSelectedImg( "Battle\\BattleTypeSelect\\battle_mode_glow.tga" );
	m_pBattleTypeListBox->SetBackOverAndSelectedImgRender(false);
	m_pBattleTypeListBox->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CBattleTypeSelectUI::_SelectBattleType );
	AddChildControl(m_pBattleTypeListBox);	


	std::list<unsigned int>::iterator it = listInfo.begin();
	for( int n =0; it != listInfo.end(); ++it, ++n )
	{
		std::string modeImg;
		std::wstring typeName;

		switch( (*it) )
		{
		case eDuelCategory::TEAM:	
			{
				modeImg = "Battle\\BattleTypeSelect\\Battle_Mode_Team.tga";		
				typeName = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_TYPE_TEAM");	
			}
			break;
// 		case eDuelCategory::DEFENSE:
// 			{
// 				modeImg = "Battle\\BattleTypeSelect\\Battle_Mode_Team.tga";		
// 				typeName = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_TYPE_DEFANCE");	
// 			}
// 			break;
		default:
			{	// 준비중
				modeImg = "Battle\\BattleTypeSelect\\Battle_Mode_Team.tga";		
				typeName = UISTRING_TEXT("DIGIMON_BATTLE_TYPE_ONTHE_ANVIL");	
			}
			break;
		}

		cString* pString = NiNew cString;
		SAFE_POINTER_RET(pString);

		cSprite* pModeImg = NiNew cSprite;
		if( pModeImg )
		{
			pModeImg->Init( NULL, CsPoint(7,7), CsPoint( 303,442 ), modeImg.c_str(), false );		
			cString::sSPRITE* sSprite = pString->AddSprite( pModeImg );
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(typeName.c_str());

		int nHafW = ti.GetTextWidth() / 2;
		pString->AddText( &ti, CsPoint((317 / 2) - nHafW, 40));

		cGridListBoxItem* addItem  = NiNew cGridListBoxItem(n, CsPoint( 317, 457 ));
		addItem->SetItem(pString);
		addItem->SetUserData( new sBattleTypeData((*it)) );		
		m_pBattleTypeListBox->AddItem( addItem );
	}

}

BOOL CBattleTypeSelectUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pBattleTypeListBox && m_pBattleTypeListBox->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	if(m_pTypeSelectBtn && m_pTypeSelectBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pTypeCloseBtn && m_pTypeCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}


void CBattleTypeSelectUI::Render()
{
	RenderScript();
}

void CBattleTypeSelectUI::ResetDevice()
{
	ResetDeviceScript();
}

void CBattleTypeSelectUI::_Forward(void* pSender, void* pData)
{
	GetSystem()->RegisterNextStep();
}

void CBattleTypeSelectUI::_Backward(void* pSender, void* pData)
{
	GetSystem()->CloseRegisterPage();
}
// 배틀 타입 선택 버튼
void CBattleTypeSelectUI::_SelectBattleType(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pData);
	sBattleTypeData* pUserData = dynamic_cast<sBattleTypeData*>(pSelItem->GetUserData());
	SAFE_POINTER_RET( pUserData );

	switch( pUserData->m_nTypeIdx )
	{
	case eDuelCategory::TEAM:
		break;
// 	case eDuelCategory::DEFENSE:
// 		break;
	default:	// 준비중
		{
			if( m_pTypeSelectBtn )
				m_pTypeSelectBtn->SetEnable(false);
		}
		return;
	}

	GetSystem()->SetBattleTypeSelect( pUserData->m_nTypeIdx );

	if( m_pTypeSelectBtn )
		m_pTypeSelectBtn->SetEnable(true);
}

void CBattleTypeSelectUI::_ChangeStep(BRegist::eRegistState const& step)
{
	switch( step )
	{
	case BRegist::eState_Type_Select:	SetVisible(true);		break;
	case BRegist::eState_Mode_Select:	SetVisible(false);		break;
	}	
}
//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleTypeSelectUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_State, this );
	return true;
}

void CBattleTypeSelectUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eChange_State:
		{
			kStream.SetRp(0);
			BRegist::eRegistState step;
			kStream >> step;
			_ChangeStep( step );
		}		
		break;
	}
}