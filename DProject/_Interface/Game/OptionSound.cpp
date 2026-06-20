

#include "stdafx.h"
#include "OptionSound.h"

#pragma warning( disable : 4800 )

void cOptionSound::Destroy()
{
	cOptionBase::Delete();
}

void cOptionSound::DeleteResource()
{
	cOptionBase::DeleteResource();
}

void cOptionSound::Create(int nValue /* = 0  */)
{
	if( cOptionBase::CreateBase( UISTRING_TEXT( "OPTION_SOUND_TITLE_SOUND_SETTING" ).c_str(), nValue ) == false )
		return;
	m_pDefaultBtn = AddButton( CsPoint( 14, 556 ), CsPoint( 101, 31 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_DEFAULT" ).c_str() );
	TCHAR sz[ IF_OP_COMBO_STR_MAX_COUNT ][ IF_OP_COMBO_STR_MAX_LEN ];
	NiColor color[ IF_OP_COMBO_STR_MAX_COUNT ];
	int nData[ IF_OP_COMBO_STR_MAX_COUNT ];

	// 배경음
	_AddControl_Title( UISTRING_TEXT( "OPTION_SOUND_TITLE_BG_MUSIC" ).c_str() );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_SOUND_COMBOBOX_SOUND" ).c_str(), 2, sz, color, nData, (int)g_pResist->IsMusic(), BGM );
	_AddControl_Slider( UISTRING_TEXT( "OPTION_SOUND_SLIDER_VOLUME" ).c_str(), CsPoint( 0, 50 ), 5, (int)( g_pResist->GetMusic()*50 ), 
		UISTRING_TEXT( "OPTION_SOUND_SLIDER_SMALL" ).c_str(), UISTRING_TEXT( "OPTION_SOUND_SLIDER_BIG" ).c_str(), BGM );

	// 환경음
	_AddControl_Title( UISTRING_TEXT( "OPTION_SOUND_TITLE_GAME_SOUND" ).c_str() );

	_AddControl_Combo( UISTRING_TEXT( "OPTION_SOUND_COMBOBOX_SOUND" ).c_str(), 2, sz, color, nData, (int)g_pResist->IsSound(), SOUND );
	_AddControl_Slider( UISTRING_TEXT( "OPTION_SOUND_SLIDER_VOLUME" ).c_str(), CsPoint( 0, 50 ), 5, (int)( g_pResist->GetSound()*50 ), 
		UISTRING_TEXT( "OPTION_SOUND_SLIDER_SMALL" ).c_str(), UISTRING_TEXT( "OPTION_SOUND_SLIDER_BIG" ).c_str(), SOUND );

	// 효과음
	_AddControl_Title( UISTRING_TEXT( "OPTION_SOUND_TITLE_SOUND_EFFECT" ).c_str() );

	_AddControl_Combo( UISTRING_TEXT( "OPTION_SOUND_COMBOBOX_SOUND" ).c_str(), 2, sz, color, nData, (int)g_pResist->IsSystemSound(), SYSTEM_SOUND );
	_AddControl_Slider( UISTRING_TEXT( "OPTION_SOUND_SLIDER_VOLUME" ).c_str(), CsPoint( 0, 50 ), 5, (int)( g_pResist->GetSystemSound()*50 ), 
		UISTRING_TEXT( "OPTION_SOUND_SLIDER_SMALL" ).c_str(), UISTRING_TEXT( "OPTION_SOUND_SLIDER_BIG" ).c_str(), SYSTEM_SOUND );

	// 보이스
	_AddControl_Title( UISTRING_TEXT( "OPTION_TITLE_OTHERS" ).c_str() );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_SOUND_COMBOBOX_3D_EFFECT" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bEnableSound_3DMove, MOVE_SOUND );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_SOUND_COMBOBOX_VOICE" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bEnableVoice, VOICE );

	_ResizeScrollBar();
}

void cOptionSound::Update(float const& fDeltaTime)
{
	cOptionBase::Update(fDeltaTime);

	float fMusic = FindSlider( BGM )->s_pSlider->GetCurPosIndex()*0.02f;
	float fSound = FindSlider( SOUND )->s_pSlider->GetCurPosIndex()*0.02f;
	float fSystem = FindSlider( SYSTEM_SOUND )->s_pSlider->GetCurPosIndex()*0.02f;
	g_pSoundMgr->Set_BGM_Volume( fMusic );
	g_pSoundMgr->Set_EFM_Volume( fSound );
	g_pSoundMgr->Set_System_Volume( fSystem );

	g_pResist->SetMusic( fMusic );
	g_pResist->SetSound( fSound );
	g_pResist->SetSystemSound( fSystem );
}

cBaseWindow::eMU_TYPE
cOptionSound::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pDefaultBtn->Update_ForMouse() )
	{
	case cCheckBox::ACTION_CLICK:
		_OnDefault();
	case cCheckBox::ACTION_DOWN:
		return muReturn;		
	}

	return cOptionBase::Update_ForMouse();
}

void cOptionSound::Render()
{
	cOptionBase::Render();
}

void cOptionSound::ResetDevice()
{	
	cOptionBase::ResetDevice();
}

void cOptionSound::OnMoveWindow()
{
	cOptionBase::OnMoveWindow();
}

void cOptionSound::_OnOk()
{
	cOptionBase::_OnOk();
}

void cOptionSound::_OnCancel()
{
	g_pResist->SetEnableMusic( (bool)FindCombo( BGM )->s_nBackupData );
	g_pResist->SetEnableSound( (bool)FindCombo( SOUND )->s_nBackupData );
	g_pResist->SetEnableSystemSound( (bool)FindCombo( SYSTEM_SOUND )->s_nBackupData );

	float fMusic = FindSlider( BGM )->s_nBackupPos*0.02f;
	float fSound = FindSlider( SOUND )->s_nBackupPos*0.02f;
	float fSystem = FindSlider( SYSTEM_SOUND )->s_nBackupPos*0.02f;
	if( g_pSoundMgr )
	{
		g_pSoundMgr->Set_BGM_Volume( fMusic );
		g_pSoundMgr->Set_EFM_Volume( fSound );
		g_pSoundMgr->Set_System_Volume( fSystem );
	}	
	g_pResist->SetMusic( fMusic );
	g_pResist->SetSound( fSound );
	g_pResist->SetSystemSound( fSystem );

	g_pResist->m_Global.s_bEnableSound_3DMove = (BOOL)FindCombo( MOVE_SOUND )->s_nBackupData;
	g_pResist->m_Global.s_bEnableVoice = (bool)FindCombo( VOICE )->s_nBackupData;

	CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;
	if( g_pResist->IsMusic() == false )
	{
		g_pSoundMgr->DeleteMusic( true );
	}
	else
	{
		if( g_pSoundMgr->GetCurMusic() == NULL )
		{
			char const* cMusic = _LoadMusic();
			if( cMusic != NULL )
				g_pSoundMgr->PlayMusic( cMusic );
		}
	}

	cOptionBase::_OnCancel();
}

void cOptionSound::_OnDefault()
{
	g_pResist->m_Global.ResetSound();

	FindSlider( BGM )->s_pSlider->SetCurPosIndex( (int)( g_pResist->GetMusic()*50 ) );
	FindSlider( SOUND )->s_pSlider->SetCurPosIndex( (int)( g_pResist->GetSound()*50 ) );
	FindSlider( SYSTEM_SOUND )->s_pSlider->SetCurPosIndex( (int)( g_pResist->GetSystemSound()*50 ) );

	if( g_pSoundMgr )
	{
		g_pSoundMgr->Set_BGM_Volume( g_pResist->GetMusic() );
		g_pSoundMgr->Set_EFM_Volume( g_pResist->GetSound() );
		g_pSoundMgr->Set_System_Volume( g_pResist->GetSystemSound() );
	}

	FindCombo( BGM )->s_pCombo->SetCurSel_FromData( (int)g_pResist->IsMusic() );
	FindCombo( SOUND )->s_pCombo->SetCurSel_FromData( (int)g_pResist->IsSound() );
	FindCombo( SYSTEM_SOUND )->s_pCombo->SetCurSel_FromData( (int)g_pResist->IsSystemSound() );
	FindCombo( MOVE_SOUND )->s_pCombo->SetCurSel_FromData( (int)g_pResist->m_Global.s_bEnableSound_3DMove );
	FindCombo( VOICE )->s_pCombo->SetCurSel_FromData( (int)g_pResist->m_Global.s_bEnableVoice );

	if( ( g_pResist->IsMusic() == true )&&( g_pSoundMgr->GetCurMusic() == NULL ) )
	{
		char const* cMusic = _LoadMusic();
		if( cMusic != NULL )
			g_pSoundMgr->PlayMusic( cMusic );
	}
}


void cOptionSound::Combo_SelChange( sOP_COMBO* pCR )
{
	switch( pCR->s_nValue )
	{
	case SOUND:
		{
			g_pResist->SetEnableSound( (bool)FindCombo( SOUND )->s_pCombo->GetCurSel_Data() );
			// 기존 사운드도 정지
			if( g_pResist->IsSound() == false )
			{
				g_pSoundMgr->Play_Stop_Sound();
			}
		}	
		break;
	case SYSTEM_SOUND:
		{
			g_pResist->SetEnableSystemSound( (bool)FindCombo( SYSTEM_SOUND )->s_pCombo->GetCurSel_Data() );
			// 기존 사운드도 정지
			if( g_pResist->IsSound() == false )
			{
				g_pSoundMgr->Play_Stop_SystemSound();
			}
		}		
		break;

	case BGM:
		{
			bool bEnable = (FindCombo( BGM )->s_pCombo->GetCurSel_Data() > 0) ? true : false;
			g_pResist->SetEnableMusic( bEnable );
			if( bEnable )
			{
				if( g_pSoundMgr->GetCurMusic() == NULL )
				{
					char const* cMusic = _LoadMusic();
					if( cMusic != NULL )
						g_pSoundMgr->PlayMusic( cMusic );
				}
			}
			else// 배경음 끔
			{
				g_pSoundMgr->Set_BGM_Volume( FindSlider( BGM )->s_pSlider->GetCurPosIndex()*0.02f );	
				g_pSoundMgr->DeleteMusic( true );
				g_pResist->SetMusic( FindSlider( BGM )->s_pSlider->GetCurPosIndex()*0.02f );
			}
		}
		break;

	case MOVE_SOUND:
		{
			g_pResist->m_Global.s_bEnableSound_3DMove = FindCombo( MOVE_SOUND )->s_pCombo->GetCurSel_Data();
		}
		break;

	case VOICE:
		{
			g_pResist->m_Global.s_bEnableVoice = FindCombo( VOICE )->s_pCombo->GetCurSel_Data();
			CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;
		}
		break;

		// 종료시 처리		
	default:
		assert_cs( false );
	}
}

char const* cOptionSound::_LoadMusic()
{
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS ) 
		return "Enchant_Room.mp3";

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	if( nsCsMapTable::g_pMapListMng->IsList( dwMapID ) == false )
		return NULL;

	CsMapRegion::sINFO* pInfo = NULL;
	float fLength;
	NiPoint2 ptSrc = NiPoint2( g_pCharMng->GetTamerUser()->GetPos().x, g_pCharMng->GetTamerUser()->GetPos().y );
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID );
	CsMapRegion::LIST* pList = pGroup->GetRegionList();
	CsMapRegion::LIST_IT it = pList->begin();
	for( ; it!=pList->end(); ++it )
	{
		pInfo = (*it)->GetInfo();
		fLength = ( ptSrc - NiPoint2( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY ) ).Length();
		if( fLength < pInfo->s_nRadius )
		{
			if( pInfo->s_cBGSound[ 0 ] != NULL )
				return pInfo->s_cBGSound;
			break;
		}
	}

	CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo();	
	if( pListInfo->s_cBGSound[ 0 ] != NULL )
		return pListInfo->s_cBGSound.c_str();
	return NULL;
}

#pragma warning( default : 4800 )