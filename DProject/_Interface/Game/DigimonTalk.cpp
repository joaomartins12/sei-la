

#include "stdafx.h"
#include "DigimonTalk.h"

CsTimeSeq cDigimonTalk::m_SoundTimeSeq;

bool cDigimonTalk::Print( DWORD dwMessageID, sTalkEle* pEle /* = NULL */ )
{	
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return true;

	if( g_pResist->m_Global.s_bDigimonBalloone == false )
		return true;

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	CsTalk_Digimon* pFTDTalk = nsCsFileTable::g_pTalkMng->GetTalk_Digimon( dwMessageID );
	if( pFTDTalk == NULL )
		return false;
	
	// 확률 적용 실패
	if( g_pResist->m_Talk.s_DigimonTalk.GetTalk( dwMessageID ) != NULL )
	{
		if( rand()%10 >= g_pResist->m_Talk.s_DigimonTalk.GetTalk( dwMessageID )->s_nPersent )
		{
			return false;
		}
	}

	// 사운드
	if( m_SoundTimeSeq.IsEnable() == true )
	{
		char cSound[ MAX_PATH ] = {0, };		
		char cFile[ MAX_FILENAME ];		
		strcat_s( cSound, MAX_PATH, (char*)pDigimonUser->GetSoundDir() );
		sprintf_s( cFile, MAX_FILENAME, "\\dv_%d.wav", dwMessageID );		
		strcat_s( cSound, MAX_PATH, cFile );
		float fLength = 2.0f;
		CSound* pSound = (CSound*)pDigimonUser->PlayStaticSound( cSound );
		if( pSound != NULL )
		{
			pSound->GetPlayLength( fLength );
		}
		m_SoundTimeSeq.SetDeltaTime( (DWORD)( fLength*1000.0f ) );
		m_SoundTimeSeq.OffReset();
	}

	TCHAR sz[ 256 ] = { 0, };
	if( g_pResist->m_Talk.s_DigimonTalk.GetTalk( dwMessageID ) != NULL )
		_tcscpy_s( sz, 256, g_pResist->m_Talk.s_DigimonTalk.GetTalk( dwMessageID )->s_szTalk );
	else
		_tcscpy_s( sz, 256, pFTDTalk->GetText() );	

	g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_3, pDigimonUser->GetUniqID(), cDigimonTalk::Parcing_Talk( sz, 256, pEle ) );

	return true;
}

TCHAR* cDigimonTalk::Parcing_Talk( TCHAR* szMsg, int nLen, sTalkEle* pEle )
{
	size_t len = _tcslen( szMsg );
	assert_cs( (int)len < nLen );
	if( len == 0 )
		return szMsg;

	// 파싱해야 하는 메세지인지 검사
	if( pEle == NULL )
		return szMsg;

	// 퀘스트 인자 파싱
	TCHAR szQuestTitle[ 128 ] = {0, };
	if( pEle->s_dwQuest != 0 )
	{
		_stprintf_s( szQuestTitle, 128, _T( "%s" ), nsCsFileTable::g_pQuestMng->GetQuest( pEle->s_dwQuest )->m_szTitleText );
	}

	// NPC 인자 파싱
	TCHAR szNpc[ 128 ] = {0, };
	if( pEle->s_dwNpc != 0 )
	{
		_stprintf_s( szNpc, 128, _T( "%s" ), nsCsFileTable::g_pNpcMng->GetNpc( pEle->s_dwNpc )->GetInfo()->s_szName );
	}

	// 디지몬 이름 파싱
	TCHAR szDigimon[ 128 ] = {0, };
	if( pEle->s_dwDigimon != 0 )
	{
		_stprintf_s( szDigimon, 128, _T( "%s" ), nsCsFileTable::g_pDigimonMng->GetDigimon( pEle->s_dwDigimon )->GetInfo()->s_szName );
	}

	// 스킬 이름 파싱
	TCHAR szSkill[ 128 ] = {0, };
	if( pEle->s_dwSkill != 0 )
	{
		_stprintf_s( szSkill, 128, _T( "%s" ), nsCsFileTable::g_pSkillMng->GetSkill( pEle->s_dwSkill )->GetInfo()->s_szName );
	}

	TCHAR szOut[ FT_TALK_MSG_BODY_LEN ] = {0, };
	int nSrcMsgIndex = 0;

	TCHAR szParcing[ FT_TALK_MSG_BODY_LEN ] = {0, };
	int nPacingIndex = 0;

	bool bFindToken = false;

	for( uint i=0; i<len; ++i )
	{
		if( szMsg[ i ] == L'[' )
		{
			bFindToken = true;	
			continue;
		}

		if( szMsg[ i ] == L']' )
		{
			if( bFindToken == true ) 
			{
				bFindToken = false;

				// 여기서 파라미터 파싱
				if( (pEle->s_dwNpc != 0) && (_tcsicmp( szParcing, _T( "NPC" ) ) == 0) )
				{
					_tcscat_s( szOut, FT_TALK_MSG_BODY_LEN, szNpc );
					nSrcMsgIndex += _tcslen( szNpc );
				}
				else if( (pEle->s_dwQuest != 0) && (_tcsicmp( szParcing, _T( "QUEST" ) ) == 0) )
				{
					_tcscat_s( szOut, FT_TALK_MSG_BODY_LEN, szQuestTitle );
					nSrcMsgIndex += _tcslen( szQuestTitle );
				}
				else if( (pEle->s_dwSkill != 0) && (_tcsicmp( szParcing, _T( "SKILL" ) ) == 0) )
				{
					_tcscat_s( szOut, FT_TALK_MSG_BODY_LEN, szSkill );
					nSrcMsgIndex += _tcslen( szSkill );
				}
				else if( (pEle->s_dwDigimon != 0) && (_tcsicmp( szParcing, _T( "DIGIMON" ) ) == 0) )
				{
					_tcscat_s( szOut, FT_TALK_MSG_BODY_LEN, szDigimon );
					nSrcMsgIndex += _tcslen( szDigimon );
				}
				else
				{
					_tcscat_s( szOut, FT_TALK_MSG_BODY_LEN, szParcing );
					nSrcMsgIndex += _tcslen( szParcing );
				}

				nPacingIndex = 0;
				memset( szParcing, 0, sizeof( szParcing ) );
				szOut[ nSrcMsgIndex ] = NULL;
				continue;	
			}			
		}

		if( bFindToken == true )
		{
			szParcing[ nPacingIndex ] = szMsg[ i ];
			++nPacingIndex;
		}
		else
		{
			szOut[ nSrcMsgIndex ] = szMsg[ i ];
			++nSrcMsgIndex;
		}		

		szOut[ nSrcMsgIndex ] = NULL;
	}	

	if( bFindToken == true )
	{
		_tcscat_s( szOut, FT_TALK_MSG_BODY_LEN, szParcing );
		nSrcMsgIndex += _tcslen( szParcing ) - 1;
	}

	szOut[ nSrcMsgIndex ] = NULL;
	_tcscpy_s( szMsg, nLen, szOut );
	return szMsg;
}

//////////////////////////////////////////////////////////////////////////
//
//		window
//
//////////////////////////////////////////////////////////////////////////

void cDigimonTalk::Destroy()
{	
	cBaseWindow::Delete();		
}

void cDigimonTalk::ResetMap()
{
	Close( false );
}

void cDigimonTalk::DeleteResource()
{	
	if( m_pEdit )
		m_pEdit->ReleaseFocus();	

	SAFE_NIDELETE( m_pPersent );
	SAFE_NIDELETE( m_pMouseOnMask );
	SAFE_NIDELETE( m_pEdit );
	SAFE_NIDELETE( m_pCombo );	
	g_pCharResMng->CharImageResDelete( &m_pImage );

	DeleteScript();

	m_TalkList.Delete();
	m_strTalk.Delete();
	m_pbMacro.Destroy();
}

void cDigimonTalk::Create(int nValue /* = 0  */)
{	
	if( IsLive() == false )
	{
		SetRootClient( CsPoint( 450, 165 ) );
	}

	cBaseWindow::Init();			

	m_pEdit = NULL;
	m_pSelectText = NULL;	
	m_pImage = NULL;

	InitScript( "DigimonStatus\\Digimon_Talk.tga", GetRootClient(), CsPoint( 325, 423 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );		
	m_pCancelButton = AddButton( CsPoint( 300, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 300, 80 ), CsPoint( 16, 100 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 11, 75 ), CsPoint( 325, 210 ) ), 5 );	

	m_pMouseOnMask = NiNew cSprite; 
	m_pMouseOnMask->Init( NULL, CsPoint::ZERO, CsPoint( 295, 19 ), "Talk\\Common_Seletbar.tga", false );
	m_pMouseOnMask->SetAlpha( 0.4f );	

	m_pDefault = AddButton( CsPoint( 12, 389 ), CsPoint( 62, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_DEFAULT" ).c_str() );
	m_pSave = AddButton( CsPoint( 255, 389 ), CsPoint( 62, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_SAVE" ).c_str() );
	m_pDelete = AddButton( CsPoint( 185, 389 ), CsPoint( 62, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "DIGIMONTALK_DELETE" ).c_str() );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	ti.s_eFontSize = CFont::FS_9;
	ti.SetText( _T( "" ) );
	ti.s_Color = FONT_YELLOW;
	m_pSound = AddText( &ti, CsPoint( 10, 220 ) );	

	ti.s_eFontSize = CFont::FS_10;
	ti.s_bOutLine = true;		
	ti.s_eTextAlign = DT_CENTER;

	ti.s_Color = FONT_WHITE;
	ti.SetText( _T( "" ) );
	m_pSize = AddText( &ti, CsPoint( 163, 336 ) );			


	ti.s_eFontSize = CFont::FS_8;	
	cButton* pMacro;	
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_QUEST" ).c_str() );
	pMacro = AddButton( CsPoint( 12, 357 ), CsPoint( 64, 23 ), cButton::IMAGE_NOR_3, &ti );
	pMacro->SetEnable( false );
	m_pbMacro.PushBack( pMacro );

	ti.SetText( UISTRING_TEXT( "COMMON_TXT_NPC" ).c_str() );
	pMacro = AddButton( CsPoint( 77, 357 ), CsPoint( 64, 23 ), cButton::IMAGE_NOR_3, &ti );
	pMacro->SetEnable( false );
	m_pbMacro.PushBack( pMacro );

	ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGIMON_ENGLISH" ).c_str() );
	pMacro = AddButton( CsPoint( 142, 357 ), CsPoint( 64, 23 ), cButton::IMAGE_NOR_3, &ti );
	pMacro->SetEnable( false );
	m_pbMacro.PushBack( pMacro );

	ti.SetText( UISTRING_TEXT( "COMMON_TXT_SKILL_ENGLISH" ).c_str() );
	pMacro = AddButton( CsPoint( 207, 357 ), CsPoint( 64, 23 ), cButton::IMAGE_NOR_3, &ti );
	pMacro->SetEnable( false );
	m_pbMacro.PushBack( pMacro );	

	m_pPersent = NiNew cComboBox;
	m_pPersent->Init( NULL, cComboBox::CT_NORMAL, CsPoint::ZERO, CsPoint( 82, 23 ), CFont::FS_9, DT_CENTER, false, false, false );	
	for( int i = 10 ; i > -1; i-- )
	{
		std::wstring wsItem;
		DmCS::StringFn::Format( wsItem, L"%d%% %s", i * 10, UISTRING_TEXT( "DIGIMONTALK_OCCURRENCE" ).c_str() );
		m_pPersent->AddItem( wsItem.c_str(), i );
	}
	m_pPersent->SetCurSel_FromData( 10 );

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID() );
	CDigimonEvolveObj::MAP_IT itEvo = pFTEvolInfo->m_Map.begin();
	CDigimonEvolveObj::MAP_IT itEvoEnd = pFTEvolInfo->m_Map.end();

	m_pCombo = NiNew cComboBox;
	m_pCombo->Init( NULL, cComboBox::CT_NORMAL, CsPoint::ZERO, CsPoint( 155, 26 ), CFont::FS_10, DT_CENTER, false, false, false );	
	for( ; itEvo != itEvoEnd; itEvo++ )
	{
		if( itEvo->second->m_nEnableSlot == 0 )
			continue;

		DWORD dwDigimon = itEvo->second->m_dwID;
		m_pCombo->AddItem( nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimon )->GetInfo()->s_szName, dwDigimon, FONT_WHITE );			
	}
	m_pCombo->SetCurSel_FromData( g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_dwDigimonID );

	_DigimonSelect( m_pCombo->GetCurSel_Data() );

	AddTitle( UISTRING_TEXT( "COMMON_TXT_DIGIMON_TALK" ).c_str() );		

	CsTalk_Digimon::MAP_IT it = nsCsFileTable::g_pTalkMng->GetMap_Digimon()->begin();
	CsTalk_Digimon::MAP_IT itEnd = nsCsFileTable::g_pTalkMng->GetMap_Digimon()->end();
	

	ti.s_eTextAlign = DT_LEFT;
	int nIdx = 0;
	for( ; it != itEnd; it++, nIdx++ )
	{		
		ti.SetText( it->second->GetInfo()->s_szList );		

		cString* pString = NULL;
		pString = NiNew cString;		
		pString->AddText( &ti );
		pString->SetValue1( it->first );
		pString->SetValue2( nIdx );

		m_TalkList.AddTail( pString );				
	}

	m_TalkList.SetBuddy( m_pScrollBar );
	m_pScrollBar->SetRange( CsPoint( 0, m_TalkList.GetSize() + 1 ) );	 
}

cBaseWindow::eMU_TYPE
cDigimonTalk::Update_ForMouse()
{
	if( m_pSelectText )
	{
		int nComboSel = m_pPersent->Update_ForMouse();
		if( nComboSel != cComboBox::CI_INVALIDE )
		{
			switch( nComboSel )
			{
			case cComboBox::CI_INRECT:			
				return MUT_NONE;
			case cComboBox::CI_CLICK:
			case cComboBox::CI_SAME_CLICK:
				return MUT_CLICK_FOCUS;
			default:
				{					
					m_pPersent->GetCurSel_Data();					
				}				
				return MUT_CLICK_FOCUS;
			}
		}
	}

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
	{	
		m_bMask = false;
		return muReturn;
	}
	
	int nComboSel = m_pCombo->Update_ForMouse();
	if( nComboSel != cComboBox::CI_INVALIDE )
	{
		switch( nComboSel )
		{
		case cComboBox::CI_INRECT:			
			return MUT_NONE;
		case cComboBox::CI_CLICK:
		case cComboBox::CI_SAME_CLICK:
			return MUT_CLICK_FOCUS;
		default:
			{
				_DigimonSelect( m_pCombo->GetCurSel_Data() );
			}				
			return MUT_CLICK_FOCUS;
		}
	}		

	for( int i = 0; i < m_pbMacro.Size(); i++ )
	{
		cButton* pButton = m_pbMacro.GetData( i );

		if( pButton->IsEnable() == false )
			continue;

		switch( pButton->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				if( _tcslen( m_szTalk ) > 110 )
					break;

				std::wstring wsTalk;

				switch( i )
				{		
				case 0: //Quest
					{
						DmCS::StringFn::Format( wsTalk, L"%s [QUEST]", m_szTalk );
						m_pEdit->SetText( wsTalk.c_str() );
					}
					break;
				case 1: //Npc
					{
						DmCS::StringFn::Format( wsTalk, L"%s [NPC]", m_szTalk );
						m_pEdit->SetText( wsTalk.c_str() );
					}
					break;
				case 2: //Digimon
					{
						DmCS::StringFn::Format( wsTalk, L"%s [DIGIMON]", m_szTalk );
						m_pEdit->SetText( wsTalk.c_str() );
					}
					break;
				case 3: //Skill
					{
						DmCS::StringFn::Format( wsTalk, L"%s [SKILL]", m_szTalk );
						m_pEdit->SetText( wsTalk.c_str() );
					}
					break;
				}
			}
			return muReturn;
		default:
			return muReturn;
		}		
	}

	// 캔슬 버튼
	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			Close();
		}		
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pSave->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{	
			if( m_pSelectText )
			{
				bool bSuccess = m_EditTalk.s_DigimonTalk.SaveTalk( m_nSelectTalk, m_szTalk, m_pPersent->GetCurSel_Data() );
				assert_cs( bSuccess );			
			}
			g_pResist->SaveDigimonTalk( m_EditTalk, m_szDigimonName );
			cPrintMsg::PrintMsg( 30182 );			

			if( m_pCombo->GetCurSel_Data() == g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_dwDigimonID )
			{
				// 디지몬 대화 로드		
				TCHAR sz[ MAX_FILENAME ];
				M2W( sz, g_pCharMng->GetDigimonUser( 0 )->GetSoundDir(), MAX_FILENAME );	
				g_pResist->LoadDigimonTalk( sz );	
			}
		}
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pDelete->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		if(m_pEdit)	m_pEdit->SetText( _T( "" ) );
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pDefault->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		_SetDefaultTalk();	
		return muReturn;
	default:
		return muReturn;
	}		
	
	int nActive1;
	int nStartPosY = 75, nMaskSizeX = 300;
	CsPoint ptMouseOnMask = CsPoint( 0, nStartPosY ) + GetRootClient();

	switch( m_TalkList.Update_ForMouse( nActive1, m_nMouseOnMaskIndex, ptMouseOnMask, 3, CsPoint( nMaskSizeX, 29 ) ) )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		break;	
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::LBUTTON_PRESS:
		m_nSelectMask = m_nMouseOnMaskIndex;
		break;
	case CURSOR::LBUTTON_UP:	
		{
			if( m_pPersent->IsSelect() == false )
				break;

			if( m_pSelectText )
			{				
				bool bSuccess = m_EditTalk.s_DigimonTalk.SaveTalk( m_nSelectTalk, m_szTalk, m_pPersent->GetCurSel_Data() );
				assert_cs( bSuccess );			
			}
			_SetEditBox( nActive1 );
		}
		break;
	default:		
		break;
	}

	if( m_nMouseOnMaskIndex != cString::INVALIDE_STRING_VALUE )
		m_bMask = true;

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 267, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cDigimonTalk::Update(float const& fDeltaTime)
{	
	_UpdateMoveWindow();

	bool bEnable = ( m_pSelectText == NULL ) ? false : true;
	m_pDefault->SetEnable( bEnable );
	m_pSave->SetEnable( bEnable );
	m_pDelete->SetEnable( bEnable );

	if( m_pCombo )
		m_pCombo->Update(fDeltaTime);	

	if( m_pPersent )
		m_pPersent->Update(fDeltaTime);

	if( m_pEdit )
	{		
		if( _tcscmp( m_szTalk, (TCHAR*)m_pEdit->GetTextAll() ) != 0 ) 
		{
			_stprintf_s( m_szTalk, MAX_PATH, _T( "%s" ), (TCHAR*)m_pEdit->GetTextAll() );	

			m_strTalk.Delete();

			cText::sTEXTINFO ti;		
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			g_pStringAnalysis->Cut( &m_strTalk, 300, m_szTalk, &ti );
			m_strTalk.SetUnderLineCursor(); 

			std::wstring wsLimit;
			DmCS::StringFn::Format( wsLimit, L"%d/120%s", _tcslen( m_szTalk ), UISTRING_TEXT( "DIGIMONTALK_LIMIT_WORD" ).c_str() );
			m_pSize->SetText( wsLimit.c_str() );
		}		

		if( m_pEdit->IsFocus() == false )
		{
			m_strTalk.ReleaseCursor();
			m_pEdit->ReleaseFocus();
			SAFE_NIDELETE( m_pEdit );			
		}				
	}	
}

void cDigimonTalk::Render()
{
	RenderScript();	
	CsPoint ptDigimonImgPos, ptDigimonImgSize, ptList, ptTalkstr, ptSelect, ptPersent, ptDigimonCB ;//디지몬 초상화, 토크리스트, 작성한 토크, 선택한 토크, 발생확률, 디지몬 콤보박스
	CsPoint ptMaskSize, ptTalkstrPos, ptTalkstrSize;//마우스오버 마스크 크기, 입력 토크창 배경용 마스크 크기
	int nStartPosY, nStrDeltaY, nMaskPosX;//토크리스트 시작 Y좌표, 리스트 Y좌표 +값, 마우스오버 마스크 X좌표
	ptDigimonImgPos = m_pRoot->GetClient() + CsPoint( 12, 39 );
	ptDigimonImgSize = CsPoint( 32, 32 );
	ptList = GetRootClient() + CsPoint( 11, 89 );
	ptTalkstr = GetRootClient() + CsPoint( 13, 253 );
	ptSelect = GetRootClient() + CsPoint( 11, 195 );
	ptPersent = GetRootClient() + CsPoint( 237, 218 );
	ptDigimonCB = GetRootClient() + CsPoint( 62, 42 );
	ptMaskSize = CsPoint( 295, 19 );
	ptTalkstrPos = GetRootClient() + CsPoint( 5, 250 );
	ptTalkstrSize = CsPoint( 315, 110 );
	nStartPosY = 85;
	nStrDeltaY = 3;
	nMaskPosX = 5;

	if( m_pImage != NULL )
		m_pImage->Render_L( ptDigimonImgPos, ptDigimonImgSize );	
		//pCharImage->Render_S( CsPoint( m_pRoot->GetClient().x + 14, m_pRoot->GetClient().y + 39 ), CsPoint( 32, 32 ) );	
	
	m_TalkList.Render( ptList, nStrDeltaY );

	if( m_bMask )
	{
		int nPosY = m_TalkList.GetStringPosY( nStartPosY, m_nMouseOnMaskIndex, nStrDeltaY );

		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( nMaskPosX, nPosY ) + GetRootClient();
			m_pMouseOnMask->Render( pos );	
		}
	}

	if( m_nSelectMask != cString::INVALIDE_STRING_VALUE )
	{
		int nPosY = m_TalkList.GetStringPosY( nStartPosY, m_nSelectMask, nStrDeltaY );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( nMaskPosX, nPosY /*+ 2*/ ) + GetRootClient();
			m_pMouseOnMask->Render( pos );	
		}
	}

	m_strTalk.Render( ptTalkstr, 0 );	

	if( m_pEdit != NULL )
	{
		m_pSelectText->Render( ptSelect );
		m_pMouseOnMask->Render( ptTalkstrPos, ptTalkstrSize );
		m_pMouseOnMask->SetSize( ptMaskSize );

		m_pPersent->Render( ptPersent );
	}

	m_pCombo->Render( ptDigimonCB );
}

void cDigimonTalk::ResetDevice()
{	
	ResetDeviceScript();	

	m_TalkList.ResetDevice();
	m_strTalk.ResetDevice();
	
	if( m_pMouseOnMask )
		m_pMouseOnMask->ResetDevice();

	if( m_pEdit )
		m_pEdit->ResetDevice();

	if( m_pCombo != NULL )
		m_pCombo->ResetDevice();
	
	if( m_pPersent )
		m_pPersent->ResetDevice();
}

void cDigimonTalk::_SetEditBox( int nWordIdx )
{	
	m_nSelectTalk = nWordIdx;

	SAFE_NIDELETE( m_pEdit );	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	m_pEdit = NiNew cEditBox;	
	m_pEdit->Init( m_pRoot, CsPoint::ZERO, CsPoint( 255, 49 ), &ti, false );	
	m_pEdit->SetFontLength( 120 );
	m_pEdit->EnableUnderline();	
	m_pEdit->EnablePaste();
	m_pEdit->ReleaseFocus();

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();

	m_pEdit->SetFocus();
	if( m_EditTalk.s_DigimonTalk.GetTalk( m_nSelectTalk ) == NULL )
	{
		_SetDefaultTalk();	
		m_pPersent->SetCurSel_FromData( 10 ); 
	}
	else
	{		
		m_pEdit->SetText( m_EditTalk.s_DigimonTalk.GetTalk( m_nSelectTalk )->s_szTalk );
		if( m_pPersent->SetCurSel_FromData( m_EditTalk.s_DigimonTalk.GetTalk( m_nSelectTalk )->s_nPersent )	== false )
		{
			m_pPersent->SetCurSel( 0 );
		}
	}		

	if( m_pSelectText != NULL)
	{
		m_pSelectText->SetColor( FONT_WHITE );
	}

	_SetMacroButton( nWordIdx );

	m_pSelectText = m_TalkList.GetString( m_nMouseOnMaskIndex );	
	m_pSelectText->SetColor( FONT_GREEN );

	TCHAR szFile[ MAX_FILENAME ];
	_stprintf_s( szFile, MAX_FILENAME, _T( "StaticSound\\..\\dv_%d.wav" ), nWordIdx );
	m_pSound->SetText( szFile );

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}


void cDigimonTalk::_SetDefaultTalk()
{
	if(nsCsFileTable::g_pTalkMng == NULL) return;

	CsTalk_Digimon* pFTDTalk = nsCsFileTable::g_pTalkMng->GetTalk_Digimon( m_nSelectTalk );	
	if(m_pEdit && pFTDTalk)
		m_pEdit->SetText( pFTDTalk->GetText() );
}

void cDigimonTalk::_DigimonSelect( DWORD dwID )
{
	m_nSelectMask = cString::INVALIDE_STRING_VALUE;
 	m_strTalk.Delete();
	m_pSize->SetText( _T( "" ) );
	m_pSound->SetText( _T( "" ) );	
	memset( m_szTalk, 0, sizeof( TCHAR ) * MAX_PATH );
	
	M2W( m_szDigimonName, nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName, MAX_FILENAME );	
	_LoadEditDigimonTalk();

	if( m_pEdit )
		m_pEdit->ReleaseFocus();	
	SAFE_NIDELETE( m_pEdit );

	if( m_pSelectText )
	{
		m_pSelectText->SetColor( FONT_WHITE );
		m_pSelectText = NULL;
	}

	g_pCharResMng->CharImageResDelete( &m_pImage );	
	m_pImage = g_pCharResMng->CharImageResLoad( nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_dwModelID );			
}

void cDigimonTalk::_SetMacroButton( int nWordIdx )
{
	DWORD dwParam = nsCsFileTable::g_pTalkMng->GetTalk_Digimon( nWordIdx )->GetInfo()->s_dwParam;

	for( int i = 0; i < m_pbMacro.Size(); i++ )
	{
		switch( i )
		{
		case 0:	m_pbMacro[ i ]->SetEnable( ( dwParam & CsTalk_Digimon::TP_QUEST ) ? true : false );		break;
		case 1:	m_pbMacro[ i ]->SetEnable( ( dwParam & CsTalk_Digimon::TP_NPC ) ? true : false );			break;
		case 2:	m_pbMacro[ i ]->SetEnable( ( dwParam & CsTalk_Digimon::TP_DIGIMON ) ? true : false );		break;
		case 3:	m_pbMacro[ i ]->SetEnable( ( dwParam & CsTalk_Digimon::TP_SKILL ) ? true : false );		break;
		}
	}

}

void cDigimonTalk::_LoadEditDigimonTalk()
{
	m_EditTalk.Reset();	

	TCHAR sz[ MAX_PATH ];
	_stprintf_s( sz, MAX_PATH, _T( "UserData\\%s\\DT_%s.arf" ), g_pResist->m_szID, m_szDigimonName );

	// 접근 안된다면 패스
	if( _taccess_s( sz, 0 ) == 0 )
	{
		FILE* fp = NULL;
		_tfopen_s( &fp, sz, _T( "rb" ) );
		if( fp != NULL )
		{
			READ_RESIST_FILE( m_EditTalk, RES_DIGIMON_TALK_VERSION );					

		/*	fread( &m_EditTalk.s_dwVersion,  sizeof(DWORD),1, fp );
			fread( &m_EditTalk.s_dwStructSize, sizeof(DWORD),1, fp );
			for( int i = 0; i < MAX_TALK; i++ )
			{
				fread( &m_EditTalk.s_DigimonTalk.m_DigimonTalk[ i ],  ( sizeof( int ) + sizeof( TCHAR ) * MAX_LEN ),1, fp );
			}*/
			
			fclose( fp );
		}
	}	
}


