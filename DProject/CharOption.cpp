
#include "stdafx.h"
#include "CharOption.h"

void	CCharOption::CalNameScaleConstant()
{ 
	g_fNameScaleConstant = tan( CsD2R( CAMERA_ST.GetCameraInfo()->s_fFOV )*0.5f )*0.0135f; 
}

float CCharOption::GetNameScaleConstant()
{ 
	return g_fNameScaleConstant; 
}

void sCHAR_IMAGE::Render_L( CsPoint pos, CsPoint size /* = CsPoint::ZERO  */)
{
	if( s_pLargeImg == NULL )
		return;

	if( size == CsPoint::ZERO )
	{
		s_pLargeImg->Render( pos, CsPoint( 83, 72 ) );
	}
	else
	{
		s_pLargeImg->Render( pos, size );
	}
}

void sCHAR_IMAGE::Render_S( CsPoint pos, CsPoint size /* = CsPoint::ZERO  */)
{
	if( s_pSmallImg == NULL )
		return;

	if( size == CsPoint::ZERO )
	{
		s_pSmallImg->Render( pos, CsPoint( 50, 43 ) );
	}
	else
	{
		s_pSmallImg->Render( pos, size );
	}
}

void sCHAR_IMAGE::Render_B( CsPoint pos, CsPoint size /* = CsPoint::ZERO  */)
{
	if( s_pBossImg == NULL )
		return;

	if( size == CsPoint::ZERO )
	{
		s_pBossImg->Render( pos, CsPoint( 35, 35 ) );
	}
	else
	{
		s_pBossImg->Render( pos, size );
	}
}

void sCHAR_IMAGE::ResetDevice()
{
	if( s_pLargeImg )
		s_pLargeImg->ResetDevice();
	if( s_pSmallImg )
		s_pSmallImg->ResetDevice();
	if( s_pBossImg )
		s_pBossImg->ResetDevice();
}





float CCharOption::g_fNameScaleConstant = 1.0f;

CCharOption::CCharOption():m_pTextName(NULL),m_pTextPenName(NULL)
,m_pAchieve(NULL),m_nAchieveID(0),m_pGuildName(NULL),m_pParent(NULL),m_dwPlag(0)
,m_pIcon(NULL),m_pMatchIcon(NULL),m_nSkillCode(0),m_AchiveColor(FONT_GREEN)
{
	m_szName[ 0 ] = NULL;
	m_szPenName[ 0 ] = NULL;
	m_szGuildName[ 0 ] = NULL;
	m_szGuildMarkName[ 0 ] = NULL;
}

CCharOption::~CCharOption()
{
	Delete();
}

void CCharOption::Delete()
{
	SAFE_NIDELETE( m_pTextName );
	SAFE_NIDELETE( m_pTextPenName );
	SAFE_NIDELETE( m_pGuildName );
	SAFE_NIDELETE( m_pAchieve );
	SAFE_NIDELETE(m_pIcon);
	SAFE_NIDELETE( m_pMatchIcon );
	m_pParent = NULL;
	m_szName[ 0 ] = NULL;
	m_szPenName[ 0 ] = NULL;
	m_nAchieveID = 0;
	m_szGuildName[ 0 ] = NULL;
	m_szGuildMarkName[ 0 ] = NULL;
	m_dwPlag = 0;
	m_nSkillCode = 0;
}

void CCharOption::Init( CsC_AvObject* pParent, LPCTSTR szName )
{
	m_pParent = pParent;

	assert_cs( m_pTextName == NULL );		
	if( szName == NULL )
		return;

	ResetName( szName );
}

void CCharOption::SetName( TCHAR const* szText )
{ 
	ResetName( szText ); 
}

bool CCharOption::IsPlag( ePLAG plag )
{ 
	return ( (m_dwPlag & plag ) != 0 ); 
}

#ifdef GUILD_RENEWAL
// 테이머 이름색 변경
void CCharOption::SetNameColor( NiColor pColor )
{ 
	if( m_pTextName )
		m_pTextName->SetColor(pColor); 
}	
#endif

void CCharOption::ResetName( LPCTSTR szName )
{
	m_dwPlag |= NAME;

	_tcscpy_s( m_szName, CO_NAME_LEN, szName );
	if( m_szName[ 0 ] == NULL )
	{
		SAFE_NIDELETE( m_pTextName );
		return;
	}

	if( m_pTextName == NULL )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_32, m_pParent->GetNameColor() );	
		ti.SetText( m_szName );
		ti.s_bOutLine = false;
		m_pTextName = NiNew cText3D;
		if( m_pTextName->Init3D( &ti ) == false )
			SAFE_NIDELETE( m_pTextName );
	}
	else
	{
		m_pTextName->SetText( m_szName );		
	}	
}

void CCharOption::ResetNameColor()
{
	if( m_pAchieve )
	{
		if( IsPlag( AWAY ) )
			m_pAchieve->SetColor( FONT_RED );
		else
			m_pAchieve->SetColor( m_AchiveColor );
	}

	bool bColorChange = false;
	NiColor pColor = DEFAULT_NAMECOLOR;

	if( m_pTextName )
		pColor = m_pParent->GetNameColor();

	if(m_nSkillCode !=0 ) // 다른 유저들의 네임색상 변경
	{
		CsSkill::sINFO* pInfo = nsCsFileTable::g_pSkillMng->GetSkill( m_nSkillCode )->GetInfo();
		if(pInfo)
		{
			int nColorLv = 0;

			for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
			{
				switch( pInfo->s_Apply[i].s_nA )
				{
				case GUILD_NAMECOLOR:	// 색 변경
					if( nColorLv < 1 )
					{
						float fR = (float)( pInfo->s_Apply[i].s_nB/100 );
						float fG = (float)( (pInfo->s_Apply[i].s_nB/10) % 10 );
						float fB = (float)( pInfo->s_Apply[i].s_nB%10 );
						pColor = NiColor( fR / 10.0f, fG / 10.0f, fB / 10.0f );
						break;
						nColorLv = 1;
					}
				}
			}
		}
	}

// 	if(g_pDataMng->GetParty()->GetParty_FromName(m_szName))	//우리 파티에 있을 때,
// 		pColor = PARTY_NAMECOLOR; 

	std::pair<int, bool> isPartyMember;
	isPartyMember.first = m_pParent->GetUniqID();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_TAMERUIDX, &isPartyMember );
	if( isPartyMember.second )
		pColor = PARTY_NAMECOLOR;

#ifdef GUILD_RENEWAL
	if(g_pCharMng->GetTamerUser()->GetCharOption()->GetName() == m_szName) //자신일 때는 따로 분홍색으로 변경
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_SET_PASIVESKILL_COLOR, &pColor );
#endif

	if( m_pTextName )
		m_pTextName->SetColor(pColor);

}

void CCharOption::SetAlpha( float fAlpha )
{
	if( m_pTextName )
		m_pTextName->SetAlpha( fAlpha );
	
	if( m_pGuildName )
		m_pGuildName->SetAlpha( fAlpha );

	if( m_pTextPenName )
		m_pTextPenName->SetAlpha( fAlpha );

	if( m_pAchieve )
		m_pAchieve->SetAlpha( fAlpha );
	if( m_pIcon )
		m_pIcon->SetAlpha( fAlpha );
}

void CCharOption::RenderName()
{
	if( m_pParent->IsRenderName() == false )
		return;

	NiPoint3 pos = m_pParent->GetPos();	
	float fLength = ( CAMERA_ST.GetWorldTranslate() - pos ).Length();

	float fScale = g_fNameScaleConstant*pow( fLength*fLength*15.0f, 1.0f/3.0f )*0.36f;
	pos.z += m_pParent->GetToolHeight() + 46.0f;

	// 길드 이름
	if( m_pGuildName )
	{
		if( m_pGuildName->IsEnableTexture() )
		{
			m_pGuildName->Render( pos, 0, 0, fScale );

			if( fScale > 1.0f )
				pos.z += 50.0f*sqrt( fScale );
			else
				pos.z += 50.0f*fScale;
		}
		else
		{
			SetGuildName( m_szGuildName );
		}
	}
	else if( IsPlag( GUILD_NAME ) )
	{
		SetGuildName( m_szGuildName );
	}

	// 이름
	if( m_pTextName )
	{
		if( m_pTextName->IsEnableTexture() )
		{
			m_pTextName->Render( pos, 0, 0, fScale );			
		}
		else
		{
			ResetName( m_szName );
		}
	}
	else if( IsPlag( NAME ) )
	{
		ResetName( m_szName );
	}

	// 업적
	if( m_pAchieve )
	{
		if( m_pAchieve->IsEnableTexture() )
		{
			if( fScale > 1.0f )
				pos.z += 40.0f*sqrt( fScale );
			else
				pos.z += 40.0f*fScale;

			m_pAchieve->Render( pos, 0, 0, fScale );			
		}
		else
		{
			if( IsPlag( AWAY ) )
				SetAway( true );
			else
				SetAchieve( m_nAchieveID );
		}
	}
	else if( IsPlag( ACHIEVE ) )
	{
		if( IsPlag( AWAY ) )
			SetAway( true );
		else
			SetAchieve( m_nAchieveID );
	}
	
	//테이머 아이콘
	if( m_pMatchIcon )
	{
		if( fScale > 1.0f )
			pos.z += 50.0f*sqrt( fScale );
		else
			pos.z += 50.0f*fScale;
		if( m_pMatchIcon->IsEnableTexture() )
			m_pMatchIcon->Render( pos, 0, 0, fScale );
	}	

	// 호
	if( m_pTextPenName )
	{
		if( m_pTextPenName->IsEnableTexture() )
		{
			if( fScale > 1.0f )
				pos.z += 60.0f*sqrt( fScale );
			else
				pos.z += 60.0f*fScale;

			m_pTextPenName->Render( pos, 0, 0, fScale );
		}
		else
		{
			SetPenName( m_szPenName );
		}
	}
	else if( IsPlag( PEN_NAME ) )
	{
		SetPenName( m_szPenName );
	}

	// 몬스터 아이콘
	if( m_pIcon )
	{
		if( m_pIcon->IsEnableTexture() )
		{
			m_pIcon->Render( pos, 0, 0, fScale );

			if( fScale > 1.0f )
				pos.z += 50.0f*sqrt( fScale );
			else
				pos.z += 50.0f*fScale;
		}		
	}	
}

void CCharOption::SetPenName( LPCTSTR szPenName )
{
	assert_cs( m_pTextPenName == NULL );
	if( szPenName == NULL )
		return;

	m_dwPlag |= PEN_NAME;

	_tcscpy_s( m_szPenName, CO_NAME_LEN, szPenName );
	if( m_szPenName[ 0 ] == NULL )
		return;

	if( m_pTextPenName == NULL )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_32, m_pParent->GetNameColor() );
		ti.SetText( m_szPenName );
		ti.s_bOutLine = false;
		m_pTextPenName = NiNew cText3D;
		if( m_pTextPenName->Init3D( &ti ) == false )
			SAFE_NIDELETE( m_pTextPenName );
	}
	else
	{
		m_pTextPenName->SetText( m_szPenName );
	}
}

void CCharOption::SetAchieve( DWORD nID )
{
	m_nAchieveID = nID;	
	m_AchiveColor = FONT_GREEN;
	CsAchieve* pAchive = nsCsFileTable::g_pAchieveMng->GetAchieve( m_nAchieveID );

	if( m_nAchieveID == 0 || NULL == pAchive  )
	{
		SAFE_NIDELETE( m_pAchieve );
		m_dwPlag &= ~ACHIEVE;
		SetTitleMark();	
		return;
	}

	CsAchieve::sINFO* pAchiveInfo= pAchive->GetInfo();

	m_dwPlag |= ACHIEVE;
	m_AchiveColor = m_pParent->GetNameColor(); 

	cText::sTEXTINFO ti;
	ti.Init(&g_pEngine->m_FontSystem, CFont::FS_24, m_AchiveColor );

	cStringList kTitle;
	g_pStringAnalysis->Cut_Parcing( &kTitle, 1024, pAchiveInfo->s_szTitle, &ti );

	std::wstring wsTitle;
	std::list< cString* >* pkList = kTitle.GetList();
	if( !pkList->empty() )
	{
		std::list< cString* >::iterator kIter = pkList->begin();
		cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
		m_AchiveColor = pText->s_Text.GetColor();
		wsTitle = pText->s_Text.GetText();
	}
	else
		wsTitle = pAchiveInfo->s_szTitle;

	if( m_pAchieve == NULL )
	{
		ti.s_Color = m_AchiveColor;
		ti.SetText( wsTitle.c_str() );
		ti.s_bOutLine = false;
		m_pAchieve = NiNew cText3D;
		if( m_pAchieve->Init3D( &ti ) == false )
		{
			SAFE_NIDELETE( m_pAchieve );
		}
	}
	else
	{
		m_pAchieve->SetColor( m_AchiveColor );
		m_pAchieve->SetText( wsTitle.c_str() );		
	}	

	SetTitleMark();	
}

void CCharOption::SetAway( bool bAway )
{
	if( bAway == true )
	{
		m_dwPlag |= ACHIEVE | AWAY;

		if( m_pAchieve == NULL )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_24, FONT_RED );
			ti.SetText( UISTRING_TEXT( "CHAROPTION_AWAY_FROM_KEYBOARD" ).c_str() );
			ti.s_bOutLine = false;
			m_pAchieve = NiNew cText3D;
			if( m_pAchieve->Init3D( &ti ) == false )
			{
				SAFE_NIDELETE( m_pAchieve );
			}
		}
		else
		{
			m_pAchieve->SetColor( FONT_RED );
			m_pAchieve->SetText( UISTRING_TEXT( "CHAROPTION_AWAY_FROM_KEYBOARD" ).c_str() );
		}
		SetMatchIcon( m_nMatchIconID );
	}
	else
	{
		m_dwPlag &= ~AWAY;
		SetAchieve( m_nAchieveID );
	}
}

void CCharOption::_GetAchieveIcon(int& nIcon, std::string& strIconFileName)
{
	if( nIcon > 555 )
	{
		nIcon -= 556;
		strIconFileName = "Data/interface/Icon/Achieve_Icon_03.tga";
	}
	else if( nIcon > 255 )
	{
		nIcon -= 300;
		strIconFileName = "Data/interface/Icon/Achieve_Icon_02.tga";
	}
	else
		strIconFileName = "Data/interface/Icon/Achieve_Icon.tga";
}

void CCharOption::SetGuildName( LPCTSTR szGuildName )
{		
	if( szGuildName == NULL )
		return;

	if( szGuildName[ 0 ] == NULL )
		return;

	m_dwPlag |= GUILD_NAME;
	
	_tcscpy_s( m_szGuildName, Language::pLength::GuildName, szGuildName );
	TCHAR	m_szLookGuildName[ Language::pLength::GuildName + 3 ];
	_stprintf_s( m_szLookGuildName, Language::pLength::GuildName + 3, _T( "<%s>" ), szGuildName );

	if( m_pGuildName == NULL )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_24, NiColor( 243.0f / 255.0f, 228.0f / 255.0f, 42.0f / 255.0f ) );
		ti.SetText( m_szLookGuildName );
		ti.s_bOutLine = false;
		m_pGuildName = NiNew cText3D;
		if( m_pGuildName->Init3D( &ti ) == false )
		{
			SAFE_NIDELETE( m_pGuildName );
		}
		else
		{		
			if( IsPlag( GUILD_MARK ) )
			{
				m_pGuildName->AddBillBoard( m_szGuildMarkName, NiPoint2( -33.0f, 0.0f ), NiPoint2( 22, 22 ) ); 
			}
		}
	}
	else
	{
		m_pGuildName->SetText( m_szLookGuildName );		
	}	
}

bool CCharOption::SetTitleMark()
{
	assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );	
	
	if( m_pTextName == NULL ) 
	{	
		ResetName( m_szName );	
	}
	else
	{
		m_pTextName->DeleteBillboard();		
	}

	// 업적 마크 셋팅
	if( m_nAchieveID != 0 )
	{	
		CsAchieve* pAchieve = nsCsFileTable::g_pAchieveMng->GetAchieve( m_nAchieveID );	
		assert_cs( pAchieve != NULL );
		int nIcon = pAchieve->GetInfo()->s_nIcon;
		if( nIcon != 0 )
		{
			std::string strFileName;
			NiSourceTexture* pTex = NULL;
			if( nIcon > 555 )
			{
				nIcon -= 556;
				strFileName = "Data/interface/Icon/Achieve_Icon_03.tga";
			}
			else if( nIcon > 255 )
			{
				nIcon -= 300;
				strFileName = "Data/interface/Icon/Achieve_Icon_02.tga";
			}
			else
				strFileName = "Data/interface/Icon/Achieve_Icon.tga";

			if( m_pTextName )
				m_pTextName->AddBillBoard( strFileName.c_str(), NiPoint2( -36.0f, 5.0f ), ( nIcon % 16 ) / 16.0f, ( ( nIcon % 16 ) + 1 ) / 16.0f, 
				( nIcon / 16 ) / 16.0f, ( ( nIcon / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}						
	}	

	// pc방 마크 셋팅
	CTamer* pTamer = (CTamer*)m_pParent;
#ifdef PC_BANG_SERVICE
	if( pTamer && pTamer->GetCondition()->IsCondition( nSync::PCBang ) == true )
	{
		if( m_pTextName )
			m_pTextName->AddBillBoard( "Data\\Interface\\Event\\PC_Bang.tga", NiPoint2( 39.0f, 0.0f ), NiPoint2( 25, 25 ) ); 		
	}
#endif

	SetMatchIcon( m_nMatchIconID );
	return true;
}

void CCharOption::SetGuildUID( int nGuildUID )
{
	m_nGuildUID = nGuildUID;
#ifdef GUILD_MARK_DOWN
	g_pDataMng->GetGuildMark()->MarkDownload( this );	
#endif		
}

bool CCharOption::SetGuildMark( bool bUse, char* szFileName )
{
	if( szFileName != NULL )
		strcpy_s( m_szGuildMarkName, MAX_PATH, szFileName );

	if( bUse && ( m_szGuildMarkName[ 0 ] != NULL ) )
	{
		m_dwPlag |= GUILD_MARK;
	}

	if( IsPlag( GUILD_MARK ) )
	{
		if( m_pGuildName == NULL )		
			return false;
		else
		{
			if( m_pGuildName->GetBillBoardCount() > 0 )
				m_pGuildName->DeleteBillboard();
			m_pGuildName->AddBillBoard( m_szGuildMarkName, NiPoint2( -33.0f, 0.0f ), NiPoint2( 22, 22 ) ); 
		}
	}

	return true;
}

bool CCharOption::SetIconMark( DWORD nID )
{
	m_nIconID = nID;
	
	if( m_pTextName == NULL ) 
	{	
		ResetName( m_szName );	
	}
	else
	{
		m_pTextName->DeleteBillboard();		
	}

	if( m_nIconID != 0 )
	{	
		CsMonster* pMonster = nsCsFileTable::g_pMonsterMng->GetMonster( m_nIconID );	
		assert_cs( pMonster != NULL );
		int nIcon1 = pMonster->GetInfo()->s_nIcon1;
		int nIcon2 = pMonster->GetInfo()->s_nIcon2;
		int nIcon3 = pMonster->GetInfo()->s_nIcon3;
		int nIcon4 = pMonster->GetInfo()->s_nIcon4;
		int nIcon5 = pMonster->GetInfo()->s_nIcon5;
		int nIcon6 = pMonster->GetInfo()->s_nIcon6;

		//data 클래스에서 텍스쳐파일을 init하고 불러온다.  
		std::string strIconFileName;
		if( nIcon1 != 0 )
		{
			_GetAchieveIcon( nIcon1, strIconFileName );
			m_pTextName->AddBillBoard( strIconFileName.c_str(), NiPoint2( -36.0f, 5.0f ), ( nIcon1 % 16 ) / 16.0f, ( ( nIcon1 % 16 ) + 1 ) / 16.0f, 
				( nIcon1 / 16 ) / 16.0f, ( ( nIcon1 / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}
		if( nIcon2 != 0 )
		{
			_GetAchieveIcon( nIcon2, strIconFileName );
			m_pTextName->AddBillBoard( strIconFileName.c_str(), NiPoint2( 36.0f, 5.0f ), ( nIcon2 % 16 ) / 16.0f, ( ( nIcon2 % 16 ) + 1 ) / 16.0f, 
				( nIcon2 / 16 ) / 16.0f, ( ( nIcon2 / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}
		if( nIcon3 != 0 )
		{
			_GetAchieveIcon( nIcon3, strIconFileName );
			m_pTextName->AddBillBoard( strIconFileName.c_str(), NiPoint2( -30.0f, 50.0f ), ( nIcon3 % 16 ) / 16.0f, ( ( nIcon3 % 16 ) + 1 ) / 16.0f, 
				( nIcon3 / 16 ) / 16.0f, ( ( nIcon3 / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}
		if( nIcon4 != 0 )
		{
			_GetAchieveIcon( nIcon4, strIconFileName );
			m_pTextName->AddBillBoard( strIconFileName.c_str(), NiPoint2( 30.0f, 50.0f ), ( nIcon4 % 16 ) / 16.0f, ( ( nIcon4 % 16 ) + 1 ) / 16.0f, 
				( nIcon4 / 16 ) / 16.0f, ( ( nIcon4 / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}
		if( nIcon5 != 0 )
		{
			_GetAchieveIcon( nIcon5, strIconFileName );
			m_pTextName->AddBillBoard( strIconFileName.c_str(), NiPoint2( -30.0f, -50.0f ), ( nIcon4 % 16 ) / 16.0f, ( ( nIcon4 % 16 ) + 1 ) / 16.0f, 
				( nIcon4 / 16 ) / 16.0f, ( ( nIcon4 / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}
		if( nIcon6 != 0 )
		{
			_GetAchieveIcon( nIcon6, strIconFileName );
			m_pTextName->AddBillBoard( strIconFileName.c_str(), NiPoint2( 30.0f, -50.0f ), ( nIcon4 % 16 ) / 16.0f, ( ( nIcon4 % 16 ) + 1 ) / 16.0f, 
				( nIcon4 / 16 ) / 16.0f, ( ( nIcon4 / 16 ) + 1 ) / 16.0f, NiPoint2( 26, 26 ) );
		}
	}	

	return true;
}

bool CCharOption::SetMatchIcon( DWORD nTeam/*0 = 팀없음, 1 = A팀, 2 = B팀*/ ) 
{
	if( nTeam < A_TEAM || nTeam > B_TEAM  )//팀이 없으면 아이콘 셋팅 필요없어 
		return false;

#ifdef SDM_MASTERS_MATCHING_ICON_HIDE_20170220
	m_nMatchIconID = 0;
#else
	m_nMatchIconID = nTeam;
#endif

	if( m_pMatchIcon != NULL )//이미 아이콘이 있는 상태면 지워주고
		m_pMatchIcon->DeleteBillboard();
	else
	{
		//아이콘 정보가 없을 땐 초기화
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_24, m_pParent->GetNameColor() );
		ti.SetText( _T(" ") );	// SetText 안해주면 Init3D에서 false로 나옴
		ti.s_bOutLine = false;
		m_pMatchIcon = NiNew cText3D;
		if( m_pMatchIcon->Init3D( &ti ) == false )
		{
			SAFE_NIDELETE( m_pMatchIcon );
			return false;
		}
	}
	
	CsPoint ptNameSize;
	if( m_pAchieve )
	{
		ptNameSize = m_pAchieve->GetStringSize();
	}
	else
	{
		ptNameSize = m_pTextName->GetStringSize();
	}

	if( m_nMatchIconID != 0 )//팀이 정해져 있으면 팀별로 아이콘 셋팅
	{	
		int nIconIdx = m_nMatchIconID -1;
 		m_pMatchIcon->AddBillBoard( "Data/interface/MastersMatch/MastersMatch_TeamIcon.tga", NiPoint2( 0.0f , 0.0f ), ( nIconIdx % 5 ) / 5.0f, ( ( nIconIdx % 5 ) + 1 ) / 5.0f, 
 			( nIconIdx / 5 ) / 5.0f, ( ( nIconIdx / 5 ) + 1 ) / 5.0f, NiPoint2( 26, 26 ) );
	}	

	return true;
}

void CCharOption::SetSkillCode(int iSkillCode)
{
	m_nSkillCode = iSkillCode;
}

void CCharOption::DeleteGuildName()
{ 
	SAFE_NIDELETE( m_pGuildName ); 
	memset( m_szGuildName, 0 , sizeof( m_szGuildName ) ); 
}