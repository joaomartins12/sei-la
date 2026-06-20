

#include "stdafx.h"
#include "ToolTip.h"

#define TOOLTIP_COMMENT_FONT_SIZE		CFont::FS_14
#define TOOLTIP_COMMENT_FONT			&g_pEngine->m_FontText
#define TOOLTIP_STRINGLIST_DELTA_Y		7

namespace
{
	static const size_t TOOLTIP_INPUT_MAX_CHARS = 2048;

	inline TCHAR const* SafeTooltipText( TCHAR const* pText )
	{
		return ( pText != NULL ) ? pText : _T( "" );
	}
}


cTooltip::cTooltip() : m_nCount(0),m_pAdaptParty(0)
{
	m_eType = NONE;
	m_nMainID = m_nSubID_1 = m_nSubID_2 = m_nSubID_3 = -1;
	m_pData = NULL;

	m_bRenderTooltip = false;

	m_pAdaptParty = new AdaptPartySystem;
	if( m_pAdaptParty )
		m_pAdaptParty->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_PARTYSYSTEM ) );
}
cTooltip::~cTooltip()
{ 
	m_pAdaptParty = 0;
	Delete(); 
}

void cTooltip::Delete()
{
	m_vpEditElement.Destroy();

	SAFE_NIDELETE( m_pPopupWindow );
	SAFE_NIDELETE( m_pStatUp );
	SAFE_NIDELETE( m_pStatDown );
	SAFE_NIDELETE( m_pEmptySlot );
	SAFE_NIDELETE( m_pLine );	
	SAFE_NIDELETE( m_pPost );
#ifdef GUILD_RENEWAL
	SAFE_NIDELETE( m_pGuildBuffImg );
#endif
	m_StringList.Delete();
	m_StringList_Name.Delete();
}

void cTooltip::Init()
{
	m_pPopupWindow = NiNew cPopUp;
	m_pPopupWindow->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), CsPoint( 16, 16 ), "Control_G\\Popup\\N2Dlg", "tga", false );
	m_pLine = NiNew cSprite;
	m_pLine->Init( NULL, CsPoint::ZERO, CsPoint( TOOLTIP_MAX_SIZE - 16, 1 ), "Control_G\\Popup\\N2Dlg_Line.tga", false );

	m_pStatUp = NiNew cSprite;
	m_pStatUp->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Up.tga", false );

	m_pStatDown = NiNew cSprite;
	m_pStatDown->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Down.tga", false );

	m_pEmptySlot = NiNew cSprite;
	m_pEmptySlot->Init( NULL, CsPoint::ZERO, CsPoint( 15, 15 ), "Control_G\\Tooltip\\Emptyslot.tga", false );

	m_pPost = NiNew cSprite;	
	m_pPost->Init( NULL, CsPoint::ZERO, CsPoint( 32, 19 ), "Mail\\Letter_One.tga", false );

#ifdef GUILD_RENEWAL
 	m_pGuildBuffImg = NiNew cImage;
 	m_pGuildBuffImg->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "MainBar\\Main_Guild_icon.tga", false );
#endif
	m_DelayTime.SetEndTime( 0.3f );	
}

void cTooltip::Update( float const& fDeltaTime )
{
	if( !m_bRenderTooltip )
		return;

	_Update_TimeInfo();
}

// void cTooltip::_Update()
// {
// 	_Update_TimeInfo();		
// }


void cTooltip::_Update_TimeInfo()
{
	int nCount = m_vpEditElement.Size();

	if( nCount == 0 )
		return;

	uint nTimeTS = _TIME_TS;
	sUPDATE_TIME* pUpdateTime = NULL;

	for( int i=0; i<nCount; ++i )
	{
		pUpdateTime = &m_vpEditElement[ i ];
		if( pUpdateTime == NULL || pUpdateTime->s_pText == NULL )
			continue;

		if( pUpdateTime->s_nTimeTS != ITEMINFO_INVALIDE_TIME )
		{
			pUpdateTime->s_pText->s_Text.SetText( SafeTooltipText( cTooltip::Time2Str( pUpdateTime->s_nTimeTS, nTimeTS, TT_FULL ) ) );
		}	
		else
		{
			pUpdateTime->s_pText->s_Text.SetColor( TOOLTIP_CANNOT_COLOR );
			pUpdateTime->s_pText->s_Text.SetText( SafeTooltipText( UISTRING_TEXT( "TOOLTIP_QUIT" ).c_str() ) );
		}
	}	
}

bool cTooltip::_IsEnableOverToolTip()
{
	// 팝업 떠있으면 실패 처리
	if( NULL == g_pGameIF )
		return true;

	if( NULL == g_pGameIF->GetPopup() )
		return true;

	if( g_pGameIF->GetPopup()->IsRenderPopup() )
		return false;
	return true;
}

void cTooltip::Render()
{	
	if( m_bRenderTooltip == false )
	{
		m_nMainID = -1;
		m_DelayTime.Start();
		return;
	}
	if( m_DelayTime.IsCoolTime() == true )
	{
		m_bRenderTooltip = false;
		return;
	}
	//_Update();

	//////////////////////////////////////////////////////////
	//2017-01-19-nova 화면 가로 세로 중앙을 기준으로 왼쪽 or 오른쪽 / 위쪽 or 아래쪽에 출력 되도록
	CsPoint pos = m_ptRenderPos;

	if(g_nScreenWidth / 2 < pos.x)
	{
		pos.x = pos.x - GetMaxSizeX();
	}
	else
	{
		pos.x = pos.x + m_ptPatchSize.x;
	}

	if(g_nScreenHeight / 2 < pos.y)
	{
		pos.y = pos.y - GetMaxSizeY() + m_ptPatchSize.y;
	}
	//////////////////////////////////////////////////////////

	m_pPopupWindow->Render( pos );	
	m_StringList.Render( pos + CsPoint( 10, 10 ), TOOLTIP_STRINGLIST_DELTA_Y );

	if( m_StringList_Name.GetSize() )
	{
		if( m_StringList_Name.GetSize() == 1 )
		{
			m_StringList_Name.Render( pos + CsPoint( 65, 22 ), 2 );
		}
		else
		{
			m_StringList_Name.Render( pos + CsPoint( 65, 13 ), 2 );
		}
	}	
	
	m_bRenderTooltip = false;
	m_ptRenderPoint = pos;
}

void cTooltip::Render( CsPoint ptPos, bool right )
{
	if( m_bRenderTooltip == false )
	{
		m_nMainID = -1;
		m_DelayTime.Start();
		return;
	}
	if( m_DelayTime.IsCoolTime() == true )
	{
		m_bRenderTooltip = false;
		return;
	}
	//_Update();

	if(!right)	//2017-01-19-nova 오른쪽 아래 툴팁이 아닐경우 위치에 따른 좌표 처리
	{
		//////////////////////////////////////////////////////////
		//2017-01-19-nova 툴팁창위치에 따라 왼쪽 or 오른쪽에 비교창 출력 
		if(g_nScreenWidth / 2 < m_ptRenderPos.x)
		{
			ptPos.x = ptPos.x - GetMaxSizeX();
		}
		else
		{
			ptPos.x = ptPos.x + GetMaxSizeX();
		}	
		//////////////////////////////////////////////////////////
	}

	m_pPopupWindow->Render( ptPos );	
	m_StringList.Render( ptPos + CsPoint( 10, 10 ), TOOLTIP_STRINGLIST_DELTA_Y );
	if( m_StringList_Name.GetSize() )
	{
		if( m_StringList_Name.GetSize() == 1 )
		{
			m_StringList_Name.Render( ptPos + CsPoint( 65, 22 ), 2 );
		}
		else
		{
			m_StringList_Name.Render( ptPos + CsPoint( 65, 10 ), 2 );
		}
	}	

	m_bRenderTooltip = false;
	m_ptRenderPoint = ptPos;	
}

void cTooltip::ResetDevice()
{
	m_pPopupWindow->ResetDevice();
	m_StringList.ResetDevice();
	m_StringList_Name.ResetDevice();
	m_pStatUp->ResetDevice();
	m_pStatDown->ResetDevice();
	
	if( m_pPost )
		m_pPost->ResetDevice();	
}

// void cTooltip::SetTooltip_Text( CsPoint pos, CsPoint patch, TCHAR const* szText )
// {
// 	if( _IsEnableOverToolTip() == false )
// 		return;
// 
// 	m_ptRenderPos = pos;
// 	m_ptPatchSize = patch;	
// 	m_bRenderTooltip = true;
// 
// 	// 전부 이전과 동일 하다면 생성하지말고 리턴
// 	if( ( m_eType == TEXT )&&( _tcscmp( m_szText, szText ) == 0 ) )
// 	{
// 		return;
// 	}
// 
// 	m_vpEditElement.ClearElement();
// 	m_DelayTime.Start();
// 
// 	m_StringList.Delete();
// 	m_StringList_Name.Delete();
// 	m_eType = TEXT;
// 	_tcscpy_s( m_szText, MAX_PATH, szText );
// 
// 	cText::sTEXTINFO ti;
// 	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
// 	g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_MAX_SIZE - 20, m_szText, &ti );
// 
// 	// 마무리 설정
// 	m_ptMaxSize = m_StringList.CalMaxSize( TOOLTIP_STRINGLIST_DELTA_Y ) + CsPoint( 20, 12 );
// 	m_pPopupWindow->ReSize( m_ptMaxSize );	
// }

void cTooltip::SetTooltip_Text( CsPoint pos, CsPoint patch, TCHAR const* szText, CFont::eFACE_SIZE eFontSize )
{
	if( _IsEnableOverToolTip() == false )
		return;

	m_ptRenderPos = pos;
	m_ptPatchSize = patch;
	m_bRenderTooltip = true;

	TCHAR const* pSafeText = SafeTooltipText( szText );
	std::wstring safeText( pSafeText );
	if( safeText.length() > TOOLTIP_INPUT_MAX_CHARS )
		safeText.resize( TOOLTIP_INPUT_MAX_CHARS );
	pSafeText = safeText.c_str();

	// 전부 이전과 동일 하다면 생성하지말고 리턴
	if( ( m_eType == TEXT )&&( _tcscmp( m_szText, pSafeText ) == 0 )&&( m_eFontSize == eFontSize ) )
		return;

	m_vpEditElement.ClearElement();
	m_DelayTime.Start();

	m_StringList.Delete();
	m_StringList_Name.Delete();
	m_eType = TEXT;
	_tcscpy_s( m_szText, MAX_PATH, pSafeText );

	m_eFontSize = eFontSize;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, eFontSize );
	g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_MAX_SIZE - 20, m_szText, &ti );

	m_ptMaxSize = m_StringList.CalMaxSize( TOOLTIP_STRINGLIST_DELTA_Y ) + CsPoint( 20, 12 );
	m_pPopupWindow->ReSize( m_ptMaxSize );	
}

void cTooltip::SetTooltip( CsPoint pos, CsPoint patch, int nMaxSizeX, eTYPE type, __int64 nMainID, __int64 nSubID_1 /*=0*/, __int64 nSubID_2 /*=0*/, __int64 nSubID_3 /*=0*/, void* pData /*=NULL*/ )
{
	if( _IsEnableOverToolTip() == false )
		return;

	m_ptPatchSize = patch;
	m_bRenderTooltip = true;


	cItemInfo* pInfo = (cItemInfo*)pData;
	int nCount = pInfo ? pInfo->GetCount() : 0;

	// 전부 이전과 동일 하다면 생성하지말고 리턴. 이전갯수와 현재 갯수 검사하는 루틴 추가.
	if( m_nMainID == nMainID && m_eType == type &&
		m_nSubID_1 == nSubID_1 && m_nSubID_2 == nSubID_2 &&
		m_nSubID_3 == nSubID_3 && m_pData == pData  && nCount == m_nCount && pos == m_ptRenderPos)
	{
		return;
	}

	m_ptRenderPos = pos;
	m_nCount = nCount;
	m_vpEditElement.ClearElement();
	m_DelayTime.Start();
	m_StringList.Delete();
	m_StringList_Name.Delete();

	m_eType = type;
	m_nMainID = nMainID;
	m_nSubID_1 = nSubID_1;
	m_nSubID_2 = nSubID_2;
	m_nSubID_3 = nSubID_3;
	m_pData = pData;
	m_nMaxSizeX = nMaxSizeX;

	switch( m_eType )
	{
	case ITEM:				_MakeTooltip_Item();				break;
	//case ITEMCOMPARE:		_MakeTooltip_ItemCompare();			break;
	case SKILL:				_MakeTooltip_Skill();				break;
	case SKILL_SIMPLE:		_MakeTooltip_SimpleSKill();			break;
	case QUICK_EVOL:		_MakeTooltip_QuickEvol();			break;
	case QUICK_EVOL_SIMPLE:	_Simple_MakeTooltip_QuickEvol();	break;
	case STAT:				_MakeTooltip_Stat();				break;
	case STAT_EXPLAIN:		_MakeTooltip_Stat_Explain();		break;
	case ENCHANT_EXPLAIN:	_MakeTooltip_Enchant_Explain();		break;
	case D_ATTRIBUTE:		_MakeTooltip_DAttribute();			break;
	case D_GROW_TYPE:		_MakeTooltip_GrowType();			break;
	case EXP:				_MakeTooltip_Exp();					break;
	case ELEMENT:			_MakeTooltip_Element();				break;
	case WORLDMAP:			_MakeTooltip_WorldMap();			break;
	case TACTICS:			_MakeTooltip_Tactics();				break;
	case PARTY:				_MakeTooltip_Party();				break;
	case BUFFINFO:			_MakeTooltip_BuffInfo();			break;
	case GUILDUP:			_MakeTooltip_GuildUpGrade();		break;
	case BATTLE_TAG:		_MakeTooltip_BattleTag();			break;
	case TARGET_ATTACK:		_MakeTooltip_Target_Attack();		break;
	case TARGET_NATURAL:	_MakeTooltip_Target_Natural();		break;
	case TARGET_ELEMENT:	_MakeTooltip_Target_Element();		break;
	case MAIL:				_MakeTooltip_Mail();				break;
	case PLAYTIME:			_MakeTooltip_PlayTime();			break;
#ifdef SIMPLE_TOOLTIP
	case UIHELP:			_MakeTooltip_UIHelp();				break;
#endif
	case SEALMASTER:		_MakeTooltip_SealMaster();			break;
	case DECKLIST:			_MakeTooltip_DeckList();			break;
	case DECKBUFF:			_MakeTooltip_DeckBuff();			break;
#ifdef GUILD_RENEWAL
	case GUILD_LEVEL:		_MakeTooltip_GuildLevel();			break;		// 길드 레벨 툴팁
	case GUILD_CONTRIBUTION:_MakeTooltip_GuildContribution();	break;		// 길드 공헌도(개인) 툴팁
	case GUILD_SKILL:		_MakeTooltip_GuildSkill();			break;		// 길드 스킬 툴팁
	case GUILD_LOTATE:		_MakeTooltip_GuildLotate();			break;		// 길드원 위치 툴팁
	case GUILD_BUFF:		_MakeTooltip_GuildSkillBuff();		break;		// 시스템 버프의 적용 중인 길드 스킬

#endif
	case SPIRIT_DIGIMON:	_MakeTooltip_SpiritDigimon();		break;		// 스피릿 디지몬 정보
	case FRIENDSHIP_POINT:	_MakeTooltip_FriendShipPoint();		break;
	case DATA_TRADE:		_MakeTooltip_DataTrade();			break;
	default:				assert_cs( false );
	}

	// 마무리 설정
	m_ptMaxSize = m_StringList.CalMaxSize( TOOLTIP_STRINGLIST_DELTA_Y ) + CsPoint( 20, 25 );
	m_ptMaxSize.x = CsMax( nMaxSizeX, m_ptMaxSize.x );
	m_pPopupWindow->ReSize( m_ptMaxSize );
	m_pLine->SetSize( CsPoint( m_ptMaxSize.x - 16, 1 ) );

	_Update_TimeInfo();
}

void cTooltip::SetCompareTooltip( CsPoint pos, CsPoint patch, int nMaxSizeX, eTYPE type, __int64 nMainID, __int64 nSubID_1 /*=0*/, __int64 nSubID_2 /*=0*/, __int64 nSubID_3 /*=0*/, void* pData /*=NULL*/ )
{
	CsPoint ptReturn;
	m_ptRenderPos = pos;
	m_ptPatchSize = patch;
	m_bRenderTooltip = true;

	// 전부 이전과 동일 하다면 생성하지말고 리턴
	if( ( m_nMainID == nMainID )&&( m_eType == type )&&( m_nSubID_1 == nSubID_1 )&&( m_nSubID_2 == nSubID_2 )&&( m_nSubID_3 == nSubID_3 )&&( m_pData == pData ) )
	{			
		return;
	}

	m_vpEditElement.ClearElement();
	m_DelayTime.Start();
	m_StringList.Delete();
	m_StringList_Name.Delete();

	m_eType = type;
	m_nMainID = nMainID;
	m_nSubID_1 = nSubID_1;
	m_nSubID_2 = nSubID_2;
	m_nSubID_3 = nSubID_3;
	m_pData = pData;
	m_nMaxSizeX = nMaxSizeX;

	switch( m_eType )
	{
	case ITEM:			_MakeTooltip_Item(true);			break;	//2017-01-19-nova
	//case ITEMCOMPARE:	_MakeTooltip_ItemCompare();			break;
	case SKILL:			_MakeTooltip_Skill();				break;
	case QUICK_EVOL:	_MakeTooltip_QuickEvol();			break;
	case STAT:			_MakeTooltip_Stat();				break;
	case STAT_EXPLAIN:	_MakeTooltip_Stat_Explain();		break;
	case D_ATTRIBUTE:	_MakeTooltip_DAttribute();			break;
	case D_GROW_TYPE:	_MakeTooltip_GrowType();			break;
	case EXP:			_MakeTooltip_Exp();					break;
	case ELEMENT:		_MakeTooltip_Element();				break;
	case WORLDMAP:		_MakeTooltip_WorldMap();			break;
	case TACTICS:		_MakeTooltip_Tactics();				break;
	case PARTY:			_MakeTooltip_Party();				break;
	case BUFFINFO:		_MakeTooltip_BuffInfo();			break;
	case GUILDUP:		_MakeTooltip_GuildUpGrade();		break;		
	default:			assert_cs( false );
	}

	// 마무리 설정
	m_ptMaxSize = m_StringList.CalMaxSize( TOOLTIP_STRINGLIST_DELTA_Y ) + CsPoint( 20, 25 );
	m_ptMaxSize.x = CsMax( nMaxSizeX, m_ptMaxSize.x );	
	m_pPopupWindow->ReSize( m_ptMaxSize );	
	m_pLine->SetSize( CsPoint( m_ptMaxSize.x - 16, 1 ) );	
	_Update_TimeInfo();
}

void cTooltip::GetNatureString( TCHAR* sz, int nLen, int nNature )
{
	switch( nNature )
	{
	case nsCsDigimonTable::NT_FIRE:		_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_FIRE" ).c_str() );		break;
	case nsCsDigimonTable::NT_THUNDER:	_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_THUNDER" ).c_str() );		break;
	case nsCsDigimonTable::NT_STEEL:	_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_STEEL" ).c_str() );		break;		
	case nsCsDigimonTable::NT_WIND:		_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_WIND" ).c_str() );		break;		
	case nsCsDigimonTable::NT_WOOD:		_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_WOOD" ).c_str() );		break;		
	case nsCsDigimonTable::NT_WATER:	_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_WATER" ).c_str() );		break;		
	case nsCsDigimonTable::NT_DARKNESS:	_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_BLACK" ).c_str() );		break;		
	case nsCsDigimonTable::NT_EARTH:	_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_LAND" ).c_str() );		break;		
	case nsCsDigimonTable::NT_ICE:		_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_ICE" ).c_str() );		break;		
	case nsCsDigimonTable::NT_LIGHT:	_tcscpy_s( sz, nLen, UISTRING_TEXT( "TOOLTIP_NATURE_LIGHT" ).c_str() );		break;
	case nsCsDigimonTable::NT_NONE:		_tcscpy_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_TYPE_NEUTRAL" ).c_str() );	break;
	default:							assert_cs( false );
	}
}

cString* cTooltip::_AddItemIcon( int nIconID )
{
	cString* pEmptyString = NiNew cString;
	pEmptyString->AddSizeY( 6 );
	m_StringList.AddTail( pEmptyString );

	// 아이템 아이콘
	// ARMOR_1 넣으면 알아서 아이템 별로 그린다
	cString* pString = NiNew cString;
	cString::sICON* pIcon = pString->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, nIconID, 1, CsPoint( -2, -8 ) );
	if( m_pData != NULL )
	{
		pIcon->s_nIndex_3 = ( (cItemInfo*)m_pData )->m_nRate;
	}

	pIcon->s_ptSize.x = TOOLTIP_TAB_SIZE * 2;
	m_StringList.AddTail( pString );

	return pString;
}

cString* cTooltip::_AddIcon( ICONITEM::eTYPE type, int nIconID, CsPoint size /*=CsPoint( 32, 32 )*/, int nListSizeX /*=TOOLTIP_TAB_SIZE*2*/, CsPoint delta /*=CsPoint( 12, -7 )*/, cString* pString /*=NULL*/ )
{
	if( pString == NULL )
	{
		cString* pEmptyString = NiNew cString;
		pEmptyString->AddSizeY( 6 );
		m_StringList.AddTail( pEmptyString );

		pString = NiNew cString;
	}	

	cString::sICON* pIcon = NULL;
	if( type != ICONITEM::SKILL_MASK )
		pIcon = pString->AddIcon( size, type, nIconID%1000, 1, delta );
	else
		pIcon = pString->AddSkillIcon( size, nIconID, nIconID, m_nMainID, delta );

	pIcon->s_ptSize.x = nListSizeX;
	pString->CalMaxSize();
	return pString;
}


void cTooltip::_AddLine( bool bIncludeEmptyString )
{
	if( bIncludeEmptyString == true )
	{
		cString* pEmptyString = NiNew cString;
		m_StringList.AddTail( pEmptyString );
	}

	m_pLine->SetSize( CsPoint( m_nMaxSizeX - 16, 1 ) );

	cString* pString = NiNew cString;
	pString->AddSprite( m_pLine, CsPoint( -2, 0 ) );
	pString->AddSizeY( 8 );
	m_StringList.AddTail( pString );
}

void cTooltip::_AddStr( cText::sTEXTINFO* ti, TCHAR szStr, int nFontSize, NiColor niColor )
{

}



void cTooltip::_MakeTooltip_DAttribute()
{
#define DATTRIBUTE_COLOR		NiColor( 1, 1, 0 )

	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );

	// 스탯 이름	
	switch( m_nMainID )
	{	
	case cIconMng::DAttribute:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_DIGIMON_ATTRIBUTE" ).c_str() );			
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
			_AddLine( false );

			switch( m_nSubID_1 )
			{
			case 0:
				pString = NiNew cString;
				ti.s_Color = DATTRIBUTE_COLOR;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_NEUTRAL" ).c_str() );
				pString->AddText( &ti );
				pString->AddSizeY( 12 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatDown );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 1 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 2 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 3 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 4 );
				m_StringList.AddTail( pString );
				break;
			case 1:
				pString = NiNew cString;
				ti.s_Color = DATTRIBUTE_COLOR;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_DATA" ).c_str() );
				pString->AddText( &ti );
				pString->AddSizeY( 12 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatUp );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 0 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 2 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatDown );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 3 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 4 );
				m_StringList.AddTail( pString );
				break;
			case 2:
				pString = NiNew cString;
				ti.s_Color = DATTRIBUTE_COLOR;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_VACCINE" ).c_str() );
				pString->AddText( &ti );
				pString->AddSizeY( 12 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatUp );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 0 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 3 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;
				pString->AddSprite( m_pStatDown );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 1 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 4 );
				m_StringList.AddTail( pString );
				break;
			case 3:
				pString = NiNew cString;
				ti.s_Color = DATTRIBUTE_COLOR;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_VIRUS" ).c_str() );
				pString->AddText( &ti );
				pString->AddSizeY( 12 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatUp );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 0 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 1 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatDown );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 2 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 4 );
				m_StringList.AddTail( pString );
				break;
			case 4:
				pString = NiNew cString;
				ti.s_Color = DATTRIBUTE_COLOR;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_UNKNOWN" ).c_str() );
				pString->AddText( &ti );
				pString->AddSizeY( 12 );
				m_StringList.AddTail( pString );

				pString = NiNew cString;				
				pString->AddSprite( m_pStatUp );
				pString->TailAddSizeX( 10 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 0 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 1 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 2 );
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 3 );
				m_StringList.AddTail( pString );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case cIconMng::Family:
		{
		

			switch( m_nSubID_1 )
			{
			case 0:		// DR
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_DRAGONS_ROAR" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_DRAGONS_ROAR_EXPLAIN" ).c_str() );
				break;				
			case 1:		//Nsp
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_NATURE_SPIRITS" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_NATURE_SPIRITS_EXPLAIN" ).c_str() );
				break;
			case 2:		//ME
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_METAL_EMPIRE" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_METAL_EMPIRE_EXPLAIN" ).c_str() );
				break;
			case 3:		//WG
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_WIND_GUARDIAN" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_WIND_GUARDIAN_EXPLAIN" ).c_str() );
				break;
			case 4:		//UK
				__DAttribute_Family( UISTRING_TEXT( "COMMON_TXT_TYPE_UNKNOWN" ).c_str(), UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_UNKNOWN_DIGIMON_EXPLAIN" ).c_str() );
				break;
			case 5:		//DS
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_DEEP_SAVER" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_DEEP_SAVER_EXPLAIN" ).c_str() );
				break;
			case 6:		//DA
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_DARK_AREA" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_DARK_AREA_EXPLAIN" ).c_str() );
				break;
			case 7:		//JT
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_JUNGLE_TROOPER" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_JUNGLE_TROOPER_EXPLAIN" ).c_str() );
				break;
			case 8:		//NSo
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_NIGHTMARE_SOLDIERS" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_NIGHTMARE_SOLDIERS_EXPLAIN" ).c_str() );
				break;
			case 9:		//VB
				__DAttribute_Family( UISTRING_TEXT( "TOOLTIP_FAMILY_VIRUS_BUSTERS" ).c_str(), UISTRING_TEXT( "TOOLTIP_FAMILY_VIRUS_BUSTERS_EXPLAIN" ).c_str() );
				break;
			}
		}
		break;
	case cIconMng::Nature:
		{
			switch( m_nSubID_1 )
			{
			case 0:				// 불
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_FIRE" ).c_str(), 8, 5 );
				break;
			case 1:				// 번개
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_THUNDER" ).c_str(), 2, 6 );
				break;
			case 2:				// 강철
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_STEEL" ).c_str(), 9, 1 );
				break;
			case 3:				// 바람
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_WIND" ).c_str(), 4, 7 );
				break;
			case 4:				// 나무
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_WOOD" ).c_str(), 7, 3 );
				break;
			case 5:				// 물
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_WATER" ).c_str(), 0, 8 );
				break;
			case 6:				// 어둠
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_DARKNESS" ).c_str(), 1, 9 );
				break;
			case 7:				// 땅
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_LAND" ).c_str(), 3, 4 );
				break;
			case 8:				// 얼음
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_ICE" ).c_str(), 5, 0 );
				break;
			case 9:				// 빛
				__DAttribute_Nature( UISTRING_TEXT( "TOOLTIP_NATURE_LIGHT" ).c_str(), 6, 2 );
				break;
			case 10:			// 무속성
				__DAttribute_Nature( UISTRING_TEXT( "COMMON_TXT_TYPE_NEUTRAL" ).c_str(), -1, -2 );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}	
}

void cTooltip::__DAttribute_Family( TCHAR const* szTitle, TCHAR const* szBody )
{
#define FAMILY_COLOR			NiColor( 1, 1, 0 )

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );

	cString* pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_FAMILY_ATTRIBUTE" ).c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
	_AddLine( false );

	ti.s_Color = FAMILY_COLOR;
	ti.SetText( szTitle );
	pString = NiNew cString;
	pString->AddText( &ti );
	pString->AddSizeY( 12 );
	m_StringList.AddTail( pString );

	ti.s_Color = FONT_WHITE;
	ti.s_eFontSize = CFont::FS_10;
	g_pStringAnalysis->Cut( &m_StringList, 200, szBody, &ti );
}

void cTooltip::__DAttribute_Nature( TCHAR const* szTitle, int nUpIndex, int nDownIndex )
{
#define NATURE_COLOR			NiColor( 1, 1, 0 )

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );

	cString* pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
	_AddLine( false );
	
	pString = NiNew cString;
	ti.s_Color = NATURE_COLOR;
	ti.SetText( szTitle );
	pString->AddText( &ti );
	pString->AddSizeY( 12 );
	m_StringList.AddTail( pString );

	ti.s_Color = FONT_WHITE;
	ti.s_eFontSize = CFont::FS_10;

	switch( nUpIndex )
	{
	case -1:
		break;
	case -2:
		pString = NiNew cString;				
		pString->AddSprite( m_pStatUp );
		pString->TailAddSizeX( 10 );
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nUpIndex );		
		for( int i=0; i<10; ++i )
		{
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, i );
		}
		m_StringList.AddTail( pString );
		break;
	default:
		pString = NiNew cString;				
		pString->AddSprite( m_pStatUp );
		pString->TailAddSizeX( 10 );
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nUpIndex )->s_ptSize.x += 20;

		__Element_ID_to_String( &ti, nUpIndex );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}

	switch( nDownIndex )
	{
	case -1:
		break;
	case -2:
		pString = NiNew cString;				
		pString->AddSprite( m_pStatDown );
		pString->TailAddSizeX( 10 );
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nDownIndex );		
		for( int i=0; i<10; ++i )
		{
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, i );
		}
		m_StringList.AddTail( pString );
		break;
	default:
		pString = NiNew cString;				
		pString->AddSprite( m_pStatDown );
		pString->TailAddSizeX( 10 );
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nDownIndex )->s_ptSize.x += 20;

		__Element_ID_to_String( &ti, nDownIndex );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}
}

void cTooltip::__Element_ID_to_String( cText::sTEXTINFO* ti, int nElementID )
{
	switch( nElementID )
	{
	case CsNature::Fire:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_FIRE" ).c_str() );
		break;
	case CsNature::Thunder:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_THUNDER" ).c_str() );
		break;
	case CsNature::Steel:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_STEEL" ).c_str() );
		break;
	case CsNature::Wind:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_WIND" ).c_str() );
		break;
	case CsNature::Wood:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_WOOD" ).c_str() );
		break;
	case CsNature::Water:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_WATER" ).c_str() );
		break;
	case CsNature::Darkness:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_BLACK" ).c_str() );
		break;
	case CsNature::Earth:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_LAND" ).c_str() );
		break;
	case CsNature::Ice:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_ICE" ).c_str() );
		break;
	case CsNature::Light:
		ti->SetText( UISTRING_TEXT( "TOOLTIP_NATURE_LIGHT" ).c_str() );
		break;
	default:
		assert_cs( false );
	}
}

void cTooltip::_MakeTooltip_GrowType()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );

	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ATTACK_TYPE" ).c_str() );	
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
	_AddLine( false );

	ti.s_Color = NiColor( 1, 1, 0 );
	switch( m_nMainID )
	{
	case nsCsDigimonTable::GT_SHORT_ATTACKER:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_SA" ).c_str() );
			pString->AddText( &ti );
			pString->AddSizeY( 12 );
			m_StringList.AddTail( pString );

			ti.s_Color = FONT_WHITE;			
			ti.s_eFontSize = CFont::FS_10;
			g_pStringAnalysis->Cut( &m_StringList, 200, UISTRING_TEXT( "TOOLTIP_SHORT_ATTACKER_EXPLAIN" ).c_str(), &ti );			
		}
		break;
	case nsCsDigimonTable::GT_NEAR_ATTACKER:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_NA" ).c_str() );
			pString->AddText( &ti );
			pString->AddSizeY( 12 );
			m_StringList.AddTail( pString );

			ti.s_Color = FONT_WHITE;			
			ti.s_eFontSize = CFont::FS_10;
			g_pStringAnalysis->Cut( &m_StringList, 200, UISTRING_TEXT( "TOOLTIP_NEAR_ATTACK_EXPLAIN" ).c_str(), &ti );						
		}
		break;
	case nsCsDigimonTable::GT_QUICK_ATTACKER:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_QA" ).c_str() );
			pString->AddText( &ti );
			pString->AddSizeY( 12 );
			m_StringList.AddTail( pString );

			ti.s_Color = FONT_WHITE;			
			ti.s_eFontSize = CFont::FS_10;
			g_pStringAnalysis->Cut( &m_StringList, 200, UISTRING_TEXT( "TOOLTIP_QUICK_ATTACKER_EXPLAIN" ).c_str(), &ti );
		}
		break;
	case nsCsDigimonTable::GT_DEFENDER:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_DE" ).c_str() );
			pString->AddText( &ti );
			pString->AddSizeY( 12 );
			m_StringList.AddTail( pString );

			ti.s_Color = FONT_WHITE;			
			ti.s_eFontSize = CFont::FS_10;
			g_pStringAnalysis->Cut( &m_StringList, 200, UISTRING_TEXT( "TOOLTIP_DEFENDER_EXPLAIN" ).c_str(), &ti );
		}
		break;
	default:
		assert_cs( false );
	}
}

float cTooltip::GetExpAccessoryAttributeDamage(int type)
{
	float fVal = 0.0f;	// 속성 수치 저장 변수 (반지 목걸이 공용 사용)

	if( g_pDataMng->GetTEquip()->IsExistItem( type ) )// 반지 착용 여부
	{
		cItemInfo* pEquipItem = g_pDataMng->GetTEquip()->GetData( type );
		//해당 반지의 디지몬 레벨 제한 확인
		CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() )->GetInfo();
		int nDLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();	//디지몬 레벨
		int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();		//테이머 레벨
		//디지몬,테이머 레벨이 제한 레벨 이상일 때만
		if( pItemInfo->IsEnableTamerLv( nTLevel ) && pItemInfo->IsEnableDigimonLv( nDLevel ) )
		{
			for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
			{
				if( pEquipItem->m_nAccOption[ i ] == 0 )
					break;
				if( pEquipItem->m_nAccOption[ i ] == nItem::AttrAP )
					fVal += (float)pEquipItem->m_nAccValues[ i ];
			}
		}
		fVal = fVal * (float)pEquipItem->m_nRate * 0.01f;
	}
	return fVal;
}
void cTooltip::_MakeTooltip_Exp()
{
	cString* pString;
	NiColor color = FONT_WHITE;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, color );

	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_EXP" ).c_str() );
	pString->AddText( &ti );
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );
	TCHAR sz[ 1024 ];
	_stprintf_s( sz, 1024, _T( "%I64d / %I64d   ( %.3f %% )" ), m_nMainID, m_nSubID_1, m_nMainID / (float)m_nSubID_1 * 100.0f );
	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( sz );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	float fAttrDmg = GetExpAccessoryAttributeDamage(nTamer::Ring);	// 반지에 속성 피해 데미지 여부

	fAttrDmg += GetExpAccessoryAttributeDamage(nTamer::Necklace);	// 목걸이에 속성 피해 데미지 여부

	fAttrDmg += GetExpAccessoryAttributeDamage(nTamer::Earring);	// 귀걸이에 속성 피해 데미지 여부
	
	//m_nSubID_2 = 100;

	if( fAttrDmg != 0.0f && m_nSubID_2 != 0 )// 최종 속성피해 값이 0 이 아닌 값이 존재하면.
	{
		pString = NiNew cString;

		switch( m_nSubID_2 )
		{
		case nsCsDigimonTable::AT_DATA:				ti.SetText( UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_DATA" ).c_str() );	break;
		case nsCsDigimonTable::AT_VACCINE:			ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_VACCINE" ).c_str() );	break;
		case nsCsDigimonTable::AT_VIRUS:			ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_VIRUS" ).c_str() );	break;
		case nsCsDigimonTable::AT_UNIDENTIFIED:		ti.SetText( UISTRING_TEXT( "COMMON_TXT_TYPE_UNKNOWN" ).c_str() );	break;
		default:
			return;
		}

		ti.AddText( UISTRING_TEXT( "COMMON_TXT_ATTRIBUTE" ).c_str() );
		ti.AddText(L" ");
		ti.AddText( UISTRING_TEXT( "COMMON_TXT_ADDITIONAL_DAMAGE" ).c_str() );
		m_nSubID_2;
		_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " + %.2f%% " ), fAttrDmg );
		ti.AddText( sz );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}
}

void cTooltip::_MakeTooltip_Element()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1, 1, 0 ) );
	__Element_ID_to_String( &ti, static_cast<int>(m_nMainID) );
	pString = NiNew cString;
	pString->AddText( &ti );
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;

	if( m_nSubID_1 < -20 )
	{
		ti.SetText( UISTRING_TEXT( "TOOLTIP_VERY_WEAK" ).c_str() );
	}
	else if( m_nSubID_1 < 0 )
	{
		ti.SetText( UISTRING_TEXT( "TOOLTIP_WEAK" ).c_str() );
	}
	else if( m_nSubID_1 == 0 )
	{
		ti.SetText( UISTRING_TEXT( "TOOLTIP_NORMAL" ).c_str() );
	}
	else if( m_nSubID_1 > 20 )
	{
		ti.SetText( UISTRING_TEXT( "TOOLTIP_VERY_STRONG" ).c_str() );
	}
	else
	{
		assert_cs( m_nSubID_1 > 0 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ELEMENT_STRONG" ).c_str() );
	}
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
}

void cTooltip::_MakeTooltip_BattleTag()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, NiColor( 1, 1, 0 ) );	
	ti.SetText( UISTRING_TEXT( "TOOLTIP_BATTLE_TAG" ).c_str() );

	pString = NiNew cString;
	pString->AddText( &ti );		
	m_StringList.AddTail( pString );

	_AddLine( false );		

	int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeLS( nItem::BattleTag, 0 );

	pString = NiNew cString;	
	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_GOLD;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_COUNT" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_WHITE;
	ti.SetText( nCount );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_BATTLETAG_EXPLAIN" ).c_str() );

	pString->AddText( &ti );
	m_StringList.AddTail( pString );
}

void cTooltip::_MakeTooltip_Target_Attack()
{	
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1, 1, 0 ) );	

	switch( m_nMainID )
	{
	case 1:
		{
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ATTACK_TYPE_DEFENSIVE" ).c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );

			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, MAX_ANAL_LEN, UISTRING_TEXT( "TOOLTIP_ATTACK_TYPE_DEFENSIVE_EXPLAIN" ).c_str(), &ti );
		}		
		break;
	case 3:
		{
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ATTACK_TYPE_AGGRESSIVE" ).c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, MAX_ANAL_LEN, UISTRING_TEXT( "TOOLTIP_ATTACK_TYPE_AGGRESSIVE_EXPLAIN" ).c_str(), &ti );
		}
		break;	
	default:
		assert_cs( false );
		break;
	}		
}

void cTooltip::_MakeTooltip_Target_Element()
{	
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1, 1, 0 ) );	

	switch( m_nMainID )
	{
	case nsCsDigimonTable::AT_NONE:
		{
			std::wstring wsAttr;
			DmCS::StringFn::Format( wsAttr, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_NONE" ).c_str() );
			ti.SetText( wsAttr.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );	

			_AddLine( false );	

			pString = NiNew cString;			
			ti.s_Color = FONT_WHITE;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_NONE_EXPLAIN" ).c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	case nsCsDigimonTable::AT_DATA:
		{
			std::wstring wsAttr;
			DmCS::StringFn::Format( wsAttr, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_DATA" ).c_str() );
			ti.SetText( wsAttr.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );	

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 2 );			
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "COMMON_TXT_TYPE_VACCINE" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 3 );	
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "COMMON_TXT_TYPE_VIRUS" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	case nsCsDigimonTable::AT_VACCINE:
		{
			std::wstring wsAttr;
			DmCS::StringFn::Format( wsAttr, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "COMMON_TXT_TYPE_VACCINE" ).c_str() );
			ti.SetText( wsAttr.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );	

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 3 );			
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "COMMON_TXT_TYPE_VIRUS" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 1 );						
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_DATA" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	case nsCsDigimonTable::AT_VIRUS:
		{
			std::wstring wsAttr;
			DmCS::StringFn::Format( wsAttr, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "COMMON_TXT_TYPE_VIRUS" ).c_str() );
			ti.SetText( wsAttr.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );	

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 1 );			
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_DATA" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, 2 );						
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "COMMON_TXT_TYPE_VACCINE" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	case nsCsDigimonTable::AT_UNIDENTIFIED:
		{
			std::wstring wsAttr;
			DmCS::StringFn::Format( wsAttr, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_DIGIMON_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "COMMON_TXT_TYPE_UNKNOWN" ).c_str() );
			ti.SetText( wsAttr.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );	

			_AddLine( false );	

			pString = NiNew cString;			
			ti.s_Color = FONT_WHITE;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_DIGIMON_TYPE_UNKNOWN_EXPLAIN" ).c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	}
}

void cTooltip::_MakeTooltip_Target_Natural()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1, 1, 0 ) );


	switch( m_nMainID )
	{
	case nsCsDigimonTable::NT_NONE:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURAL_NONE" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );	

			_AddLine( false );	

			pString = NiNew cString;
			ti.s_Color = FONT_WHITE;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_NATURAL_NONE_EXPLAIN" ).c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	case nsCsDigimonTable::NT_FIRE:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_FIRE" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	
			
			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_ICE - nsCsDigimonTable::NT_ICE );			
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_ICE" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_WATER - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_WATER" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;		
	case nsCsDigimonTable::NT_THUNDER:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_THUNDER" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_STEEL - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_STEEL" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_DARKNESS - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_DARKNESS" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_STEEL:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_STEEL" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_LIGHT - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_LIGHT" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_THUNDER - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_THUNDER" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_WIND:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_WIND" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_WOOD - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_WOOD" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_EARTH - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_LAND" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_WOOD:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_WOOD" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_EARTH - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_LAND" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_WIND - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_WIND" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_WATER:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_WATER" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_FIRE - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_FIRE" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_ICE - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_ICE" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_DARKNESS:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s",
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_DARKNESS" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_THUNDER - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_THUNDER" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_LIGHT - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_LIGHT" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_EARTH:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_LAND" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_WIND - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_WIND" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_WOOD - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_WOOD" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_ICE:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_ICE" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_WATER - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_WATER" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_FIRE - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_FIRE" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	case nsCsDigimonTable::NT_LIGHT:
		{
			std::wstring wsNature;
			DmCS::StringFn::Format( wsNature, L"%s : %s", 
				UISTRING_TEXT( "TOOLTIP_NATURAL_ATTRIBUTE" ).c_str(), 
				UISTRING_TEXT( "TOOLTIP_NATURE_LIGHT" ).c_str() );
			ti.SetText( wsNature.c_str() );

			pString = NiNew cString;
			pString->AddText( &ti );		
			m_StringList.AddTail( pString );

			_AddLine( false );	

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_DARKNESS - nsCsDigimonTable::NT_ICE );
			ti.s_Color = FONT_WHITE;
			std::wstring wsStrong = UISTRING_TEXT( "TOOLTIP_STRONG_AT" );
			DmCS::StringFn::Replace( wsStrong, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_DARKNESS" ).c_str() );
			ti.SetText( wsStrong.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nsCsDigimonTable::NT_STEEL - nsCsDigimonTable::NT_ICE );
			std::wstring wsWeak = UISTRING_TEXT( "TOOLTIP_WEAK_AT" );
			DmCS::StringFn::Replace( wsWeak, L"#Type#", UISTRING_TEXT( "TOOLTIP_NATURE_STEEL" ).c_str() );
			ti.SetText( wsWeak.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;	
	}
}

void cTooltip::_MakeTooltip_Mail()
{
	cString* pString;

	pString = NiNew cString;
	pString->AddSprite( m_pPost );
	pString->TailAddSizeX( 10 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	
	ti.SetText( UISTRING_TEXT( "ALIME_MAIL_ARRIVED" ).c_str() );
	pString->AddText( &ti );		
	
	m_StringList.AddTail( pString );	
}

void cTooltip::SetSealMasterTootipTile( CsItem::sINFO const* pFTInfo, CsMaster_Card::sINFO const& sInfo, cStringList& stringList, int iCardGrade )
{
	std::wstring wsGrade;
	switch(iCardGrade)
	{
	case nCardGrade::Normal:		wsGrade = UISTRING_TEXT( "COMMON_TXT_NORMAL" );		break;
	case nCardGrade::Bronze:		wsGrade = UISTRING_TEXT( "TOOLTIP_SEALMASTER_BRONZE" );		break;
	case nCardGrade::Silver:		wsGrade = UISTRING_TEXT( "TOOLTIP_SEALMASTER_SILVER" );		break;
	case nCardGrade::Gold:			wsGrade = UISTRING_TEXT( "TOOLTIP_SEALMASTER_GOLD" );		break;
	case nCardGrade::Platinum:		wsGrade = UISTRING_TEXT( "TOOLTIP_SEALMASTER_PLATINUM" );	break;
	case nCardGrade::Master:		wsGrade = UISTRING_TEXT( "TOOLTIP_SEALMASTER_MASTER" );		break;
	default:
		break;
	}	

	std::wstring wsText;
	if (iCardGrade == -1 )
		DmCS::StringFn::Format( wsText, L"%s", sInfo.s_szName );
	else
		DmCS::StringFn::Format( wsText, L"%s(%s)", sInfo.s_szName, wsGrade.c_str() );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	ti.s_Color = g_pDataMng->GetItemColor( pFTInfo->s_nClass );

	cString* pString = NiNew cString;
	ti.SetText( wsText.c_str() );
	pString->AddText( &ti );

	stringList.AddTail( pString );
}

void cTooltip::SetSealMasterNextCountString( cStringList& stringList, CsMaster_Card::sINFO const& sInfo, int nNextGrade, int nSealCount  )
{
	std::wstring msg;
	msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_SEALCOUNT" );
	std::wstring wsEffValue;
	int leftCount = sInfo.s_stGradeInfo[nNextGrade].s_nMax - nSealCount;
	DmCS::StringFn::From( wsEffValue, leftCount );
	DmCS::StringFn::ReplaceAll( msg, L"#sealcount#", wsEffValue );
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );
	ti.s_Color = FONT_WHITE;

	g_pStringAnalysis->Cut_Parcing( &stringList, 300, const_cast<TCHAR*>(msg.c_str()) , &ti );

	//cString* pString = NiNew cString;
	//ti.SetText( msg.c_str() );
	//pString->AddText( &ti );

	//stringList.AddTail( pString );
}

void cTooltip::SetSealMasterNextEffectString(cStringList& stringList, CsMaster_Card::sINFO const& sInfo, USHORT nEffType, int nEffValue)
{
	std::wstring msg;

	switch( nEffType )
	{
	case nCardGrade::HP:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HP" ) );
		}
		break;
	case nCardGrade::DS:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DS" ) );
		}
		break;
	case nCardGrade::AT:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AT" ) );
		}
		break;
	case nCardGrade::AS:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AS" ) );
		}
		break;
	case nCardGrade::CT:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_CT" ) );
			msg += L"%";
		}
		break;
	case nCardGrade::HT:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HT" ) );
		}
		break;
	case nCardGrade::DE:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DE" ) );
		}
		break;
	case nCardGrade::BL:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_BL" ) );
			msg += L"%";
		}
		break;
	case nCardGrade::EV:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NEXTSTATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_EV" ) );
			msg += L"%";
		}
		break;
	}
	std::wstring wsEffValue;
	if (nEffType == nCardGrade::EV || nEffType == nCardGrade::BL || nEffType == nCardGrade::CT)
	{
		float fValue = nEffValue * 0.01f;
		DmCS::StringFn::From( wsEffValue, fValue );
		int EVdotPos = wsEffValue.find('.');
		wsEffValue = wsEffValue.substr(0, EVdotPos+3);
	}
	else
		DmCS::StringFn::From( wsEffValue, nEffValue );
	DmCS::StringFn::ReplaceAll( msg, L"#value#", wsEffValue );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );
	ti.s_Color = FONT_WHITE;

	g_pStringAnalysis->Cut_Parcing( &stringList, 300, const_cast<TCHAR*>(msg.c_str()) , &ti );
	//cString* pString = NiNew cString;
	//ti.SetText( msg.c_str() );
	//pString->AddText( &ti );

	//stringList.AddTail( pString );
}

void cTooltip::SetSealMasterNoneLevel( cStringList& stringList, CsMaster_Card::sINFO const& sInfo )
{

	std::wstring msg;
	msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_NONE_LEVEL_STATE" );
	
	std::wstring msgMaster;
	msgMaster = UISTRING_TEXT( "SEALMASTER_TOOLTIP_MASTER_STATE" );
	
	int nEffValue = sInfo.s_stGradeInfo[5].s_nEff1val;
	
	switch(sInfo.s_stGradeInfo[5].s_nEff1)
	{
	case nCardGrade::HP:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HP" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HP" ) );
		}
		break;
	case nCardGrade::DS:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DS" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DS" ) );
		}
		break;
	case nCardGrade::AT:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AT" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AT" ) );
		}
		break;
	case nCardGrade::AS:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AS" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AS" ) );
		}
		break;
	case nCardGrade::CT:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_CT" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_CT" ) );
			msgMaster = msgMaster + L"%";
		}
		break;
	case nCardGrade::HT:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HT" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HT" ) );
		}
		break;
	case nCardGrade::DE:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DE" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DE" ) );
		}
		break;
	case nCardGrade::BL:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_BL" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_BL" ) );
			msgMaster = msgMaster + L"%";
		}
		break;
	case nCardGrade::EV:
		{
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_EV" ) );
			DmCS::StringFn::ReplaceAll( msgMaster, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_EV" ) );
			msgMaster = msgMaster + L"%";
		}
		break;
	}
	std::wstring wsEffValue;
	if (sInfo.s_stGradeInfo[5].s_nEff1 == nCardGrade::EV || sInfo.s_stGradeInfo[5].s_nEff1 == nCardGrade::BL || 
		sInfo.s_stGradeInfo[5].s_nEff1 == nCardGrade::CT)
	{
		float fValue = nEffValue * 0.01f;
		DmCS::StringFn::From( wsEffValue, fValue );
		int EVdotPos = wsEffValue.find('.');
		wsEffValue = wsEffValue.substr(0, EVdotPos+3);
	}
	else
		DmCS::StringFn::From( wsEffValue, nEffValue );

	DmCS::StringFn::ReplaceAll( msgMaster, L"#value#", wsEffValue );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );
	ti.s_Color = FONT_WHITE;
	
	g_pStringAnalysis->Cut_Parcing( &stringList, 300, const_cast<TCHAR*>(msg.c_str()) , &ti );

	_AddLine( true );

	cText::sTEXTINFO timaster;
	timaster.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );
	timaster.s_Color = FONT_WHITE;

	g_pStringAnalysis->Cut_Parcing( &stringList, 300, const_cast<TCHAR*>(msgMaster.c_str()) , &timaster );
}

// 실마스터 효과 문자열 추가
void cTooltip::SetSealMasterEffectString( cStringList& stringList, CsMaster_Card::sINFO const& sInfo, USHORT nEffType, int nEffValue )
{
	std::wstring msg;
	
	switch( nEffType )
	{
	case nCardGrade::HP:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HP" ) );
		}
		break;
	case nCardGrade::DS:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DS" ) );
		}
		break;
	case nCardGrade::AT:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AT" ) );
		}
		break;
	case nCardGrade::AS:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_AS" ) );
		}
		break;
	case nCardGrade::CT:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_CT" ) );
			msg += L"%";
		}
		break;
	case nCardGrade::HT:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_HT" ) );
		}
		break;
	case nCardGrade::DE:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_DE" ) );
		}
		break;
	case nCardGrade::BL:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_BL" ) );
			msg += L"%";
		}
		break;
	case nCardGrade::EV:
		{
			msg = UISTRING_TEXT( "SEALMASTER_TOOLTIP_STATE" );
			DmCS::StringFn::ReplaceAll( msg, L"#state#", UISTRING_TEXT( "SEALMASTER_STATE_EV" ) );
			msg += L"%";
		}
		break;
	}
	
	std::wstring wsEffValue;
	if (nEffType == nCardGrade::EV || nEffType == nCardGrade::BL || nEffType == nCardGrade::CT)
	{
		float fValue = nEffValue * 0.01f;
		DmCS::StringFn::From( wsEffValue, fValue );
		int EVdotPos = wsEffValue.find('.');
		wsEffValue = wsEffValue.substr(0, EVdotPos+3);
	}
	else
		DmCS::StringFn::From( wsEffValue, nEffValue );
	DmCS::StringFn::ReplaceAll( msg, L"#value#", wsEffValue );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );
	ti.s_Color = FONT_WHITE;

	g_pStringAnalysis->Cut_Parcing( &stringList, 300, const_cast<TCHAR*>(msg.c_str()) , &ti );
}

void cTooltip::_MakeTooltip_SealMaster()
{
	SAFE_POINTER_RET(m_pData);
	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( static_cast<int>(m_nMainID) )->GetInfo();
	CsMaster_Card::sINFO	sInfo = *(CsMaster_Card::sINFO*)m_pData;
	SetSealMasterTootipTile( pFTInfo, sInfo, m_StringList, m_nSubID_2 );
	if (m_nSubID_2 == -1)
	{
		SetSealMasterNoneLevel( m_StringList, sInfo );

	}
	else if(m_nSubID_2 >= nCardGrade::Normal && m_nSubID_2 <= nCardGrade::Master )
	{
		// sInfoGrade는none~master 등급까지 총 7등급 sInfo내의 등급은 none을 제외한 6등급이다 이를 맞춰주기 위해서 -1을 해야 함
		int sInfoGrade = m_nSubID_2;
		SetSealMasterEffectString(m_StringList, sInfo, sInfo.s_stGradeInfo[sInfoGrade].s_nEff1, sInfo.s_stGradeInfo[sInfoGrade].s_nEff1val);
		
		if (m_nSubID_2 != nCardGrade::Master)
		{
			_AddLine( true );
			SetSealMasterNextCountString( m_StringList, sInfo, m_nSubID_2 + 1, m_nSubID_3 );
			SetSealMasterNextEffectString( m_StringList, sInfo, sInfo.s_stGradeInfo[m_nSubID_2 + 1].s_nEff1, sInfo.s_stGradeInfo[m_nSubID_2 + 1].s_nEff1val );
		}
	}
}

TCHAR* cTooltip::Time2Str( u4 nDestTS, u4 nSrcTS, eTIMETYPE type )
{
	static TCHAR sz[ 64 ];

	if( nDestTS <= nSrcTS )
	{
		switch( type )
		{
		case TT_FULL:
		case TT_SIMPLE:
			{
				std::wstring wsSec;
				DmCS::StringFn::Format( wsSec, L"0 %s", UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
				_tcscpy_s( sz, 64, wsSec.c_str() );
			}
			break;
		case TT_SYMBOL:
			_tcscpy_s( sz, 64, L"00:00:00" );
			break;
		case  TT_LASTTIME:
			_tcscpy_s( sz, 64, UISTRING_TEXT( "COMMON_TXT_ONLINE" ).c_str() );
			break;
		default:
			assert_cs( false );
		}
		return sz;
	}
	

	int nRemain = nDestTS - nSrcTS;
	int nSec = nRemain;
	int nMin = nRemain / 60;
	int nHour = nMin / 60;
	int nDay = nHour / 24;

	switch( type )
	{
	case TT_FULL:
		{
			if( nDay > 0 )
			{
				std::wstring wsDay = UISTRING_TEXT( "COMMON_TXT_DAY_HOUR_MIN" );
				DmCS::StringFn::Replace( wsDay, L"#Day#", nDay );
				DmCS::StringFn::Replace( wsDay, L"#Hour#", nHour%24 );
				DmCS::StringFn::Replace( wsDay, L"#Min#", nMin%60 );
				_stprintf_s( sz, 64, wsDay.c_str() );
			}
			else if( nHour > 0 )
			{
				std::wstring wsHourMin = UISTRING_TEXT( "COMMON_TXT_HOUR_MIN" );
				DmCS::StringFn::Replace( wsHourMin, L"#Hour#", nHour%24 );
				DmCS::StringFn::Replace( wsHourMin, L"#Min#", nMin%60 );
				_stprintf_s( sz, 64, wsHourMin.c_str() );
			}
			else if( nMin > 0 )
			{
				std::wstring wsMin;
				DmCS::StringFn::Format( wsMin, L"%d %s", nMin%60, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
				_stprintf_s( sz, 64, wsMin.c_str() );
			}
			else if( nSec > 0 )
			{
				std::wstring wsSec;
				DmCS::StringFn::Format( wsSec, L"%d %s", nSec%60, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
				_stprintf_s( sz, 64, wsSec.c_str() );
			}
		}
		break;
	case TT_SIMPLE:
		{
			if( nDay > 0 )
			{
				std::wstring wsDay;
				DmCS::StringFn::Format( wsDay, L"%d %s", nDay, UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str() );
				_stprintf_s( sz, 64, wsDay.c_str() );
			}
			else if( nHour > 0 )
			{
				std::wstring wsHourMin = UISTRING_TEXT( "COMMON_TXT_HOUR_MIN" );
				DmCS::StringFn::Replace( wsHourMin, L"#Hour#", nHour%24 );
				DmCS::StringFn::Replace( wsHourMin, L"#Min#", nMin%60 );
				_stprintf_s( sz, 64, wsHourMin.c_str() );
			}
			else if( nMin > 0 )
			{
				std::wstring wsMin;
				DmCS::StringFn::Format( wsMin, L"%d %s", nMin%60, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
				_stprintf_s( sz, 64, wsMin.c_str() );
			}
			else if( nSec > 0 )
			{
				std::wstring wsSec;
				DmCS::StringFn::Format( wsSec, L"%d %s", nSec%60, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
				_stprintf_s( sz, 64, wsSec.c_str() );
			}
		}
		break;
	case TT_SYMBOL:
		{
			_stprintf_s( sz, 64, _T( "%.2d:%.2d:%.2d" ), nHour, nMin%60, nSec%60 );
		}
		break;
	case TT_LASTTIME:	// 접속 시간 표시 위해 만듬
		{
			if( nDay > 0 )
			{
				std::wstring wsDay;
				DmCS::StringFn::Format( wsDay, L"%d %s", nDay, UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str() );
				_stprintf_s( sz, 64, wsDay.c_str() );
			}
			else if( nHour > 0 )
			{
				std::wstring wsHour;
				DmCS::StringFn::Format( wsHour, L"%d %s", nHour%24, UISTRING_TEXT( "COMMON_TXT_HOUR" ).c_str() );
				_stprintf_s( sz, 64, wsHour.c_str() );
			}
			else if( nMin > 0 )
			{
				std::wstring wsMin;
				DmCS::StringFn::Format( wsMin, L"%d %s", nMin%60, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
				_stprintf_s( sz, 64, wsMin.c_str() );
			}
			else if( nSec > 0 )
			{
				_stprintf_s( sz, 64, UISTRING_TEXT( "TOOLTIP_LESS_A_MIN" ).c_str() );
			}
		}
		break;
	default:
		assert_cs( false );
	}
	return sz;
}

#ifdef VERSION_USA
	#define DECK_TOOLTIP_TAB_SIZE	TOOLTIP_TAB_SIZE * 3
#else
	#define DECK_TOOLTIP_TAB_SIZE	TOOLTIP_TAB_SIZE * 2
#endif

void cTooltip::_MakeTooltip_DeckBuff()	// 덱 선택으로 인해 적용 중인 버프아이콘 툴팁
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13 );

	cString* pString;
	TCHAR sz[ ENCY_EXPLAIN_LEN ];

	u4 nOptIdx = (u4)m_nMainID;	// 적용중인 효과 중 옵션 Idx
	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 효과 적용 중인 덱 Idx

	std::pair<int,USHORT> sdfOpt;
	sdfOpt.first = nOptIdx;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );

	USHORT nOption = sdfOpt.second;

	// 덱 정보
	CsEncy_OptData::sINFO* pOptInfo = nsCsFileTable::g_pDigimonMng->GetOptData(nOption)->GetInfo();
	
	//아이콘
	pString = _AddIcon( ICONITEM::CARD, (UINT)pOptInfo->s_nIcon );

	//덱 효과 이름
	std::wstring wsName;
	DmCS::StringFn::Format( wsName, L"%s(%s)", pOptInfo->s_szOptName, UISTRING_TEXT( "TOOLTIP_DECK" ).c_str() );
	ti.SetText( wsName.c_str() );

	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( false );

	//=================================================================================================================
	//
	//		내용
	//
	//=================================================================================================================	
	//발동조건 표시
	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_GOLD;	
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ACTIVATION_CONDITION" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = DECK_TOOLTIP_TAB_SIZE;

	ti.s_Color = FONT_WHITE;

	std::pair<int,USHORT> sdfCon;
	sdfCon.first = nOptIdx;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_CONDITION, &sdfCon );
	USHORT nCondition = sdfCon.second;

	switch( nCondition )	//옵션 발동 조건 ( 1:패시브, 2:확률발동, 3:확률발동+지속시간 )
	{
	case EncyclopediaContents::COND_PASSIVE:	
		{
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ACTIVATION_ALWAYS" ).c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	case EncyclopediaContents::COND_ACTIVE:
	case EncyclopediaContents::COND_ACTIVE_TIME:
			{
				std::pair<int,USHORT> sdfNat;
				sdfNat.first = nOptIdx;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_NATTYPE, &sdfNat );
				USHORT natType = sdfNat.second;

			std::pair<int,UINT> sdfProb;
			sdfProb.first = nOptIdx;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_PROB, &sdfProb );
			UINT nProb = sdfProb.second;

				sz[ 0 ] = 0;
				switch( natType )//발동 효과 체크 타입 ( 0:사용안함-패시브 1:일반공격시 , 2:스킬공격시 )
				{
				case EncyclopediaContents::ATK_NORMAL:	_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_NORMAL_ATTACK" ).c_str() );	break;
				case EncyclopediaContents::ATK_SKILL:		_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_SKILL_ATTACK" ).c_str() );	break;
				}
				if( sz[ 0 ] != 0 )
				{
					ti.SetText( sz );
					pString->AddText( &ti );
				}

			std::wstring wsProb;
			DmCS::StringFn::Format( wsProb, L"%.1f", ((float)nProb/100) );
			std::wstring wsText = UISTRING_TEXT( "TOOLTIP_ACTIVATION_PROBABILITY" );
			DmCS::StringFn::Replace( wsText, L"#Prob#", wsProb.c_str() );

			ti.SetText( wsText.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	default:	break;
	}

	//발동효과 표시
	pString = NiNew cString;
	ti.s_Color = FONT_GOLD;	
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ACTIVATION_EFFECT" ).c_str() );

	pString->AddText( &ti )->s_ptSize.x = DECK_TOOLTIP_TAB_SIZE;	

	std::pair<int,USHORT> sdfVal;
	sdfVal.first = nOptIdx;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_VAL, &sdfVal );
	USHORT nVal = sdfVal.second;
	
	switch( nOption )//옵션
	{
	case EncyclopediaContents::DECK_ATTACK_UP:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );					break;
	case EncyclopediaContents::DECK_HP_UP:			_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_HP" ).c_str() );					break;
	case EncyclopediaContents::DECK_SKILL_UP:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_SKILL_DAMAGE" ).c_str() );				break;
	case EncyclopediaContents::DECK_CRIT_UP:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_CRITICAL_HIT_DAMAGE" ).c_str() );				break;
	case EncyclopediaContents::DECK_SKILL_COOL:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_RESET_COOLDOWN" ).c_str() );	break;
	case EncyclopediaContents::DECK_ATK_SPEED:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" ).c_str() );break;
	default:	break;
	}

	std::wstring wsChange;
	if( nVal != 0 )
	{
		//TCHAR szInt[ 64 ];
		if( nOption == EncyclopediaContents::DECK_SKILL_UP ||	nOption == EncyclopediaContents::DECK_ATTACK_UP )
		{
			DmCS::StringFn::Format( wsChange, L"%d%% %s", nVal, UISTRING_TEXT( "COMMON_TXT_ADDITIONAL_DAMAGE" ).c_str() );
		}
		else
		{
			DmCS::StringFn::Format( wsChange, L"%d%% %s", nVal, UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
		}
	}
	ti.s_Color = FONT_WHITE;
	ti.SetText( wsChange.c_str() );

	pString->AddText( &ti );				

	m_StringList.AddTail( pString );	

	// 지속시간 표시
	pString = NiNew cString;
	ti.s_Color = FONT_GOLD;	
	ti.SetText( UISTRING_TEXT( "TOOLTIP_LASTING_TIME" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = DECK_TOOLTIP_TAB_SIZE;

	ti.s_Color = FONT_WHITE;

	switch( nCondition )	//옵션 발동 조건
	{
	case EncyclopediaContents::COND_PASSIVE:		ti.SetText( UISTRING_TEXT( "TOOLTIP_PERMANENT_EFFECT_ENABLED" ).c_str() );		break;
	case EncyclopediaContents::COND_ACTIVE:			ti.SetText( UISTRING_TEXT( "TOOLTIP_INSTANT_EFFECT_ENABLED" ).c_str() );		break;
	case EncyclopediaContents::COND_ACTIVE_TIME:
		{
			std::pair<int,u4> sdfTime;
			sdfTime.first = nOptIdx;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_TIME, &sdfTime );
			std::wstring wsTime = UISTRING_TEXT( "TOOLTIP_AFTER_ACTIVATION_SEC" );
			DmCS::StringFn::Replace( wsTime, L"#Sec#", sdfTime.second );
			ti.SetText( wsTime.c_str() );
		}
		break;
	}
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
	_AddLine( true );

	//=================================================================================================================
	//
	//		설명
	//
	//=================================================================================================================		

	_tcscpy_s( sz, ENCY_EXPLAIN_LEN, pOptInfo->s_szOptExplain );

	ti.s_Color = FONT_WHITE;
	g_pStringAnalysis->Cut_Parcing( &m_StringList, m_nMaxSizeX-20, sz, &ti );

}

void cTooltip::_MakeTooltip_DeckList()		// 덱 UI 툴팁
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13 );

	cString* pString = NiNew cString;
	TCHAR sz[ ENCY_EXPLAIN_LEN ];

	u4 nDeckIdx = (u4)m_nMainID;	//덱 Idx
	//=================================================================================================================
	//
	//		덱 이름
	//
	//=================================================================================================================
	
	TCHAR szName[ 64 ];
	_tcscpy_s( szName, nsCsFileTable::g_pDigimonMng->GetDeck( nDeckIdx )->GetInfo()->s_szGroupName );	
	ti.SetText( szName );

	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( false );

	//=================================================================================================================
	//
	//		덱 설명
	//
	//=================================================================================================================	
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;

	TCHAR szExplain[ 512 ];
	_tcscpy( szExplain, nsCsFileTable::g_pDigimonMng->GetDeck( nDeckIdx )->GetInfo()->s_szExplain );
#ifdef VERSION_USA
	g_pStringAnalysis->Cut( &m_StringList, 325, szExplain, &ti );
#else
	g_pStringAnalysis->Cut( &m_StringList, 205, szExplain, &ti );
#endif
	//=================================================================================================================
	//
	//		덱 효과
	//
	//=================================================================================================================	

	GS2C_RECV_ENCYCLOPEDIA_GETDECKIDX recv;
	recv.iDeckIdx = nDeckIdx;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_USE_GETDECK, &recv );

	for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
	{
		std::pair<int,USHORT> sdfOpt;
		sdfOpt.first = i+100;			//마우스로 선택된 설명 100을 더해서 구분
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );
		USHORT nOption = sdfOpt.second;

		std::pair<int,USHORT> sdfCon;
		sdfCon.first = i+100;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_CONDITION, &sdfCon );
		USHORT nCondition = sdfCon.second;

		std::pair<int,USHORT> sdfVal;
		sdfVal.first = i+100;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_VAL, &sdfVal );
		USHORT nVal = sdfVal.second;

		std::pair<int,USHORT> sdfNat;
		sdfNat.first = i+100;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_NATTYPE, &sdfNat );
		USHORT natType = sdfNat.second;

		std::pair<int,UINT> sdfProb;
		sdfProb.first = i+100;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_PROB, &sdfProb );
		UINT nProb = sdfProb.second;

		std::pair<int,u4> sdfTime;
		sdfTime.first = i+100;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_TIME, &sdfTime );
		UINT nTime = sdfTime.second;

		if( nOption == 0 )
			continue;
		_AddLine( false );

		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;	
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ACTIVATION_CONDITION" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = DECK_TOOLTIP_TAB_SIZE;

		ti.s_Color = FONT_WHITE;

		switch( nCondition )//옵션 발동 조건
		{
		case EncyclopediaContents::COND_PASSIVE:	
			{
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ACTIVATION_ALWAYS" ).c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}
			break;
		case EncyclopediaContents::COND_ACTIVE:
			case EncyclopediaContents::COND_ACTIVE_TIME:
				{
					sz[ 0 ] = 0;
					switch( natType )//발동 효과 체크 타입
					{
					case EncyclopediaContents::ATK_NORMAL:	_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_NORMAL_ATTACK" ).c_str() );	break;
					case EncyclopediaContents::ATK_SKILL:		_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_SKILL_ATTACK" ).c_str() );	break;
					}
					if( sz[ 0 ] != 0 )
					{
						ti.SetText( sz );
						pString->AddText( &ti );
					}

				std::wstring wsProb;
				DmCS::StringFn::Format( wsProb, L"%.1f", ((float)nProb/100) );
				std::wstring wsText = UISTRING_TEXT( "TOOLTIP_ACTIVATION_PROBABILITY" );
				DmCS::StringFn::Replace( wsText, L"#Prob#", wsProb.c_str() );

				ti.SetText( wsProb.c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}
			break;
		default:	break;
		}

		//발동효과 표시
		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;	
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ACTIVATION_EFFECT" ).c_str() );

		pString->AddText( &ti )->s_ptSize.x = DECK_TOOLTIP_TAB_SIZE;

		switch( nOption )//옵션
		{
		case EncyclopediaContents::DECK_ATTACK_UP:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );					break;
		case EncyclopediaContents::DECK_HP_UP:			_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_HP" ).c_str() );					break;
		case EncyclopediaContents::DECK_SKILL_UP:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_SKILL_DAMAGE" ).c_str() );				break;
		case EncyclopediaContents::DECK_CRIT_UP:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_CRITICAL_HIT_DAMAGE" ).c_str() );				break;
		case EncyclopediaContents::DECK_SKILL_COOL:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_RESET_COOLDOWN" ).c_str() );	break;
		case EncyclopediaContents::DECK_ATK_SPEED:		_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" ).c_str() );break;
		default:	break;
		}

		if( nVal != 0 )
		{
			std::wstring wsText;
			if( nOption == EncyclopediaContents::DECK_SKILL_UP ||
				nOption == EncyclopediaContents::DECK_ATTACK_UP )
				DmCS::StringFn::Format( wsText, L" %d%% %s", nVal, UISTRING_TEXT( "COMMON_TXT_ADDITIONAL_DAMAGE" ).c_str() );
			else
				DmCS::StringFn::Format( wsText, L" %d%% %s", nVal, UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );

			_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, wsText.c_str() );
		}
		ti.s_Color = FONT_WHITE;
		ti.SetText( sz );

		pString->AddText( &ti );				

		m_StringList.AddTail( pString );	

		// 지속시간 표시
		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;	
		ti.SetText( UISTRING_TEXT( "TOOLTIP_LASTING_TIME" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = DECK_TOOLTIP_TAB_SIZE;

		ti.s_Color = FONT_WHITE;

		switch( nCondition )//옵션 발동 조건
		{
		case EncyclopediaContents::COND_PASSIVE:		ti.SetText( UISTRING_TEXT( "TOOLTIP_PERMANENT_EFFECT_ENABLED" ).c_str() );		break;
		case EncyclopediaContents::COND_ACTIVE:			ti.SetText( UISTRING_TEXT( "TOOLTIP_INSTANT_EFFECT_ENABLED" ).c_str() );		break;
		case EncyclopediaContents::COND_ACTIVE_TIME:
			{
				std::wstring wsTime = UISTRING_TEXT( "TOOLTIP_AFTER_ACTIVATION_SEC" );
				DmCS::StringFn::Replace( wsTime, L"#Sec#", nTime );
				ti.SetText( wsTime.c_str() );
			}
			break;
		}
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}
}

#ifdef GUILD_RENEWAL
void cTooltip::_MakeTooltip_GuildLevel()
{
	cString* pString = NiNew cString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	

	//=================================================================================================================
	//	길드레벨
	//=================================================================================================================	
	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
	u4 nGuildLevel = guildInfo.s_nGuildLevel;
	std::wstring wsLevel;
	DmCS::StringFn::Format( wsLevel, L"%s : %d", UISTRING_TEXT( "COMMUNITY_GUILD_LEVEL" ).c_str(), nGuildLevel );
	ti.SetText( wsLevel.c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine(false);

	//=================================================================================================================
	//	필요 명성
	//=================================================================================================================	
	pString = NiNew cString;

	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_LEVELUP_NEED_FAME" ).c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	//=================================================================================================================
	//	Exp
	//=================================================================================================================	
	pString = NiNew cString;
	u4 nReqExp = nsCsFileTable::g_pBaseMng->GetGuildBase()->s_mapLevelInfo[nGuildLevel]->s_nReqExp;

	u4 nCurExp = guildInfo.s_nExp;
	std::wstring wsExp;
	DmCS::StringFn::Format( wsExp, L"%d / %d", nCurExp, nReqExp );
	ti.SetText( wsExp.c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

}


void cTooltip::_MakeTooltip_GuildContribution()
{
	cString* pString = NiNew cString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	
	TCHAR sz[TOOLTIP_MAX_TEXT_LEN];

	//=================================================================================================================
	//	타이틀(공헌도)
	//=================================================================================================================	
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_CONTRIBUTION_POINT" ).c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine(false);

	//=================================================================================================================
	//	공헌도 획득량
	//=================================================================================================================	
	pString = NiNew cString;

	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_ACQUIRED_CONTRIBUTION_POINT" ).c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	cCommunityContent::sGUILD_PERSON_INFO myInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_MYINFO, &myInfo );
	u4 nContribution = myInfo.s_nContribution;
	_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d" ), nContribution );
	ti.SetText( sz );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine(false);
	//=================================================================================================================
	//	공헌도 획득 방법
	//=================================================================================================================	
	pString = NiNew cString;

	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_METHOD_ACQUIRE_POINT" ).c_str() );
	pString->AddText( &ti );
	pString->AddSizeY( 10 );
	m_StringList.AddTail( pString );
	
	std::map< int, CsBaseMng::sGUILDBASE::sGUILDPOINT* >::iterator it = nsCsFileTable::g_pBaseMng->GetGuildBase()->s_mapPointInfo.begin();
	std::map< int, CsBaseMng::sGUILDBASE::sGUILDPOINT* >::iterator itEnd =  nsCsFileTable::g_pBaseMng->GetGuildBase()->s_mapPointInfo.end();
	
	for( ; it != itEnd ; it++ )
	{
		if( it->second == NULL )
			continue;

		pString = NiNew cString;
		u4 nPoint = it->second->s_nPoint;

		std::wstring wsMethod;
		switch( it->second->s_nType )
		{
		case CsBaseMng::sGUILDBASE::RAIDMONSTER:
			wsMethod = UISTRING_TEXT( "TOOLTIP_GUILD_ACQUIRE_POINT_RAID" );
			break;
		case CsBaseMng::sGUILDBASE::QUEST:
			wsMethod = UISTRING_TEXT( "TOOLTIP_GUILD_ACQUIRE_POINT_QUEST" );
			break;
		case CsBaseMng::sGUILDBASE::HATCH:
			wsMethod = UISTRING_TEXT( "TOOLTIP_GUILD_ACQUIRE_POINT_HATCH" );
			break;
		case CsBaseMng::sGUILDBASE::KILL_100:
			wsMethod = UISTRING_TEXT( "TOOLTIP_GUILD_ACQUIRE_POINT_MONSTER" );
			break;
		}
		std::wstring wsMsg;
		DmCS::StringFn::Format( wsMsg, L"%s : %d", wsMethod.c_str(), nPoint );
		ti.SetText( wsMsg.c_str() );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}
}

void cTooltip::_MakeTooltip_GuildSkill()
{
	cString* pString;

	pString = NiNew cString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	

	int nType =  (int)m_nMainID;
	int nCurScroll = (int)m_nSubID_1;
	int nY = (int)m_nSubID_2;
	int nX = (int)m_nSubID_3;

	//=================================================================================================================
	//		스킬 정보 얻어오기
	//=================================================================================================================	
	CsGuild_Skill* pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill( nY + nCurScroll );
	std::list<CsGuild_Skill::sINFO*>::iterator it_Act = pGuildSkill->GetList()->begin();
	while( nX > 0 )
	{
		it_Act++;
		nX--;
	}
	std::pair<u4, cCommunityContent::sGUILDSKILL_INFO> usingSkill;
	usingSkill.first = (*it_Act)->s_nSkillCode ;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_USING_SKILL, &usingSkill );

	cCommunityContent::sGUILDSKILL_INFO sUsingSkill = usingSkill.second;
	//=================================================================================================================
	//		사용 유무 체크
	//=================================================================================================================	
	bool bUsing = false;	// 적용 중인 스킬 판단
	u4 nTime = 0;

	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
	// 레벨 조건 확인 && 현재 사용 중인 스킬 목록에 있는지 확인
	if( (*it_Act)->s_nReqLevel <= guildInfo.s_nGuildLevel)
	{
		bUsing = true;
		nTime = sUsingSkill.s_nEndTime;
	}

	//=================================================================================================================
	//		아이콘
	//=================================================================================================================	
	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( (*it_Act)->s_nSkillCode );
	int nIconID = pSkill->GetInfo()->s_nIcon;

	if( nIconID >= 4000 )
		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL4, nIconID%1000, 2, CsPoint::ZERO );
	else if( nIconID >= 3000 )
		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL3, nIconID%1000, 3, CsPoint::ZERO );
	else if( nIconID >= 2000 )
		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL2, nIconID%1000, 4, CsPoint::ZERO );
	else
		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL1, nIconID%1000, 5, CsPoint::ZERO );

	//=================================================================================================================
	//		스킬 이름
	//=================================================================================================================	
	ti.SetText( (*it_Act)->s_szSkillName );
	ti.s_eFontSize = CFont::FS_12;
	pString->AddText( &ti, CsPoint( 10, 10 ) );
	m_StringList.AddTail( pString );

	_AddLine(false);

	//=================================================================================================================
	//		적용 여부( 스킬 사용 시) or 필요GSP( 스킬 사용 전 )
	//=================================================================================================================	
	pString = NiNew cString;

	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	if( nType == 0 )	// Base 스킬
	{
		if( bUsing )	// 적용
		{
			ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_SKILL_APPLY" ).c_str() );
		}
		else			// 미 적용
		{
			ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_SKILL_NOT_APPLY" ).c_str() );
			ti.s_Color = FONT_RED;
		}
		pString->AddText( &ti );
	}
	else if( bUsing )	// Active 스킬 - 적용 중
	{
		std::wstring wsLevel = UISTRING_TEXT( "TOOLTIP_GUILD_SKILL_APPLY_LEVEL" );
		DmCS::StringFn::Replace( wsLevel, L"#Level#", (*it_Act)->s_nSkillLevel );

		std::wstring wsMsg;
		DmCS::StringFn::Format( wsMsg, L"%s %s", UISTRING_TEXT( "COMMON_TXT_SKILL" ).c_str(), wsLevel.c_str() );

		ti.SetText( wsMsg.c_str() );
		pString->AddText( &ti );
	}
	else		// Active 스킬 - 미적용
	{
		_AddGuildSkillState( pSkill->GetInfo(), NULL, FONT_GLAY );

		std::wstring wsMsg;
		DmCS::StringFn::Format( wsMsg, L"%s : %d", UISTRING_TEXT( "TOOLTIP_GUILD_NEED_SKILL_POINT" ).c_str(), (*it_Act)->s_nConsGsp );

		ti.SetText( wsMsg.c_str() );
		pString->AddText( &ti );
	}

	m_StringList.AddTail( pString );

	ti.s_Color = FONT_WHITE;

	//=================================================================================================================
	//		스킬효과( 스킬 사용 시 ) or 필요 길드레벨( 스킬 사용 전 )
	//=================================================================================================================	
	pString = NiNew cString;

	if( bUsing )	// 스킬 적용 중일 때 (Base & Active)
	{
		if( nType == 1 )
			_AddGuildSkillState( pSkill->GetInfo() );
	}
	else	// 스킬 미적용 중일 때 (Base & Active)
	{
		std::wstring wsMsg;
		DmCS::StringFn::Format( wsMsg, L"%s : %d%s", UISTRING_TEXT( "TOOLTIP_GUILD_NEED_GUILD_LEVEL" ).c_str(), 
			(*it_Act)->s_nReqLevel, UISTRING_TEXT( "COMMON_TXT_SHORT_LEVEL" ).c_str() );

		ti.SetText( wsMsg.c_str() );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}

	//=================================================================================================================
	//		남은 사용기간( 스킬 사용 시 ) or 사용 가능 시간( 스킬 사용 전 )
	//=================================================================================================================	
	pString = NiNew cString;
	bool bEnable = true;
	if( bUsing )	// 적용 중일 때
	{
		std::wstring wsMsg;
		if( nType == 0 )	// Base 스킬 - 적용 중
		{
			if( nTime < _TIME_TS )	// 쿨타임 체크, 현재 사용 가능함
			{
				bEnable = false;
//				wsMsg = _LAN( "사용 가능" );
			}
			else	// 쿨타임 중일 때
			{
				DmCS::StringFn::Format( wsMsg, L"%s : %s", UISTRING_TEXT( "TOOLTIP_COOL_TIME" ).c_str(), 
					cTooltip::Time2Str( nTime, _TIME_TS, cTooltip::TT_SIMPLE ) );
			}
		}
		else		// Active 스킬 - 적용 중
		{
			DmCS::StringFn::Format( wsMsg, L"%s : %s", UISTRING_TEXT( "TOOLTIP_GUILD_REMAIN_TIME" ).c_str(),
				cTooltip::Time2Str( nTime, _TIME_TS, cTooltip::TT_SIMPLE ) );
		}

		if( bEnable )
		{
			ti.SetText( wsMsg.c_str() );
			pString->AddText( &ti );
		}
	}
	else	// 미적용 중일 때
	{
		if( nType == 1 )	// Active 스킬 - 미 적용 중
		{
			std::wstring wsMsg;
			DmCS::StringFn::Format( wsMsg, L"%s : %s", UISTRING_TEXT( "TOOLTIP_GUILD_SKILL_TIME" ).c_str(), 
				cTooltip::Time2Str( (u4)pSkill->GetInfo()->s_fDamageTime, 0, cTooltip::TT_SIMPLE ) );

			ti.SetText( wsMsg.c_str() );
			pString->AddText( &ti );
		}
	}

	if( bEnable )
		m_StringList.AddTail( pString );

	_AddLine(false);
	//=================================================================================================================
	//		스킬 설명
	//=================================================================================================================	
	pString = NiNew cString;
	std::wstring wsComment = (*it_Act)->s_szSkillComment;
	g_pStringAnalysis->Cut_Parcing( &m_StringList, 180, wsComment.c_str(), &ti );
	m_StringList.AddTail( pString );
}

void cTooltip::_MakeTooltip_GuildLotate()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );

	std::pair<int, cCommunityContent::sGUILD_PERSON_INFO*> guildPersonInfo;
	guildPersonInfo.first = (int)m_nMainID;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_PERSON_INFO, &guildPersonInfo );
	int nIdx = (int)m_nMainID;	// 길드 정보 Idx

	//=================================================================================================================
	//	길드원 이름
	//=================================================================================================================	
	ti.SetText( guildPersonInfo.second->s_szName.c_str() );
	cString* pString = NiNew cString;
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine(false);

	//=================================================================================================================
	//	길드원 위치
	//=================================================================================================================	
	SAFE_POINTER_RET( nsCsMapTable::g_pMapListMng );
	CsMapList* pFTMapList = nsCsMapTable::g_pMapListMng->GetList( guildPersonInfo.second->s_dwMapID );
	SAFE_POINTER_RET( pFTMapList );
	CsMapList::sINFO* pMapInfo = pFTMapList->GetInfo();
	SAFE_POINTER_RET( pMapInfo );

	std::wstring wsChannel = UISTRING_TEXT( "TOOLTIP_CHANNEL" );
	DmCS::StringFn::Replace( wsChannel, L"#Num#", guildPersonInfo.second->s_nChannel );

	std::wstring wsTooltip;
	DmCS::StringFn::Format( wsTooltip, L"%s%s", pMapInfo->s_szMapDiscript.c_str(), wsChannel.c_str() );

	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( wsTooltip.c_str() );

	pString = NiNew cString;
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
}

void cTooltip::_AddGuildSkillState( CsSkill::sINFO* pFTInfo, cString* pString, NiColor pColor, CsPoint ptDeltaPos )
{
	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ] = {0, };
	TCHAR szVal[ TOOLTIP_MAX_TEXT_LEN ] = {0, };

	if( pString == NULL )
		pString = NiNew cString;
	// 기본 스킬레벨은 1, 추후 스킬레벨 쓸 일 있으면 변경
	int nCurSkillLevel = 1;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );
	ti.s_Color = pColor;

	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTInfo->s_Apply[ i ].s_nID == 0 )
			break;

		// 스킬 효과 값
		int nSkillAtt = FMCommon::GetSkillAtt( pFTInfo->s_dwID, nCurSkillLevel, i );
		_stprintf_s( szVal, _T(" + %.2f%%"), (float)nSkillAtt*0.01f );

		bool bVal = false;	// + %d 여부
		int nAdd = 0;		// 0: 추가스트링 없음, 1: 증가, 2: 감소

		switch( pFTInfo->s_Apply[ i ].s_nA )
		{
		case GUILD_SHOUT:			// 길드 광고 확성기
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_SHOUT" ).c_str() );
			break;
		case GUILD_MEMBERUP:		// 길드 인원 확장 + 10
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_MEMBERUP" ).c_str() );
			_stprintf_s( szVal, _T(" + %d"), nSkillAtt );
			bVal = true;
			break;
		case GUILD_SUMMON:			// 길드원 소환
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_MEMBER_SUMMON" ).c_str() );
			break;
		case GUILD_NAMECOLOR:		// 이름색 변경
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_CHANGE_NAME_COLOR" ).c_str() );
			break;
		case GUILD_NATUREUP:		// 속성 경험치 추가
			_tcscpy_s( sz, UISTRING_TEXT( "COMMON_TXT_ATTRIBUTE_EXP" ).c_str() );
			bVal = true;
			nAdd = 1;
			break;
		case GUILD_SCANREDUCE:		// 스캔 비용 감소
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_SCAN_REDUCE" ).c_str() );
			bVal = true;
			nAdd = 2;
			break;
		case GUILD_SCANBITUP:		// 회수 시 돈 획득
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_SCAN_BIT" ).c_str() );
			bVal = true;
			nAdd = 1;
			break;
		case GUILD_HATCHUP:			// 알 부화 확률 증가
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_HATCHUP" ).c_str() );
			bVal = true;
			nAdd = 1;
			break;
		case GUILD_CROPBITUP:		// 사냥시 돈 획득
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_CROP_BIT" ).c_str() );
			bVal = true;
			nAdd = 1;
			break;
		case GUILD_MOVEUP:			// 이동속도
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_SKILL_SPEED" ).c_str() );
			bVal = true;
			nAdd = 1;
			break;
		case GUILD_INCHAPAYREDUCE:	// 강화 비용
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_ENCHANT_REDUCE" ).c_str() );
			bVal = true;
			nAdd = 2;
			break;
		case GUILD_INCHARATEUP:		// 강화 확률 증가
			_tcscpy_s( sz, UISTRING_TEXT( "TOOLTIP_GUILD_ENCHANTUP" ).c_str() );
			bVal = true;
			nAdd = 1;
			break;
		default:
			break;
		}
		if( bVal )
			_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szVal );

		switch( nAdd )
		{
		case 1:		_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );	break;
		case 2:		_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );	break;
		}
		ti.SetText(sz);
		pString->AddText( &ti, ptDeltaPos );
		m_StringList.AddTail( pString );
	}
}

void cTooltip::_MakeTooltip_GuildSkillBuff()
{
	cString* pString = NiNew cString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	


	int nType =  (int)m_nMainID;

	//=================================================================================================================
	//		아이콘
	//=================================================================================================================	
	pString->AddImage(m_pGuildBuffImg, 0);

	//=================================================================================================================
	//		버프 이름
	//=================================================================================================================	
	ti.SetText( UISTRING_TEXT( "COMMUNITY_BTN_GUILD_SKILL" ).c_str() );
	ti.s_eFontSize = CFont::FS_12;
	pString->AddText( &ti, CsPoint( 10, 10 ) );
	m_StringList.AddTail( pString );

	_AddLine(false);

	//=================================================================================================================
	//		적용 중인 길드 스킬 리스트
	//=================================================================================================================	
	cCommunityContent::UsingSkillMap usingSkillMap;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_USINGSKILL_MAP, &usingSkillMap );

	cCommunityContent::UsingSkillMapItr it = usingSkillMap.begin();
	cCommunityContent::UsingSkillMapItr itEnd = usingSkillMap.end();

	CsSkill* pSkill;
	for( ; it != itEnd ; it++ )
	{
		pSkill = nsCsFileTable::g_pSkillMng->GetSkill(it->second.s_nSkillCode);
		if( pSkill == NULL )
			continue;

		// 베이스스킬이면 pass
		if( nsCsFileTable::g_pBaseMng->_IsGuildBaseSkill( it->second.s_nSkillCode ) )
			continue;

		//=================================================================================================================
		//	스킬 아이콘
		//=================================================================================================================	
		pString = NiNew cString;
		int nIconID = pSkill->GetInfo()->s_nIcon;
		 
	 	if( nIconID >= 4000 )
	 		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL4, nIconID%1000, 2, CsPoint::ZERO );
	 	else if( nIconID >= 3000 )
	 		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL3, nIconID%1000, 3, CsPoint::ZERO );
	 	else if( nIconID >= 2000 )
	 		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL2, nIconID%1000, 4, CsPoint::ZERO );
	 	else
	 		pString->AddIcon( CsPoint( 32,32 ), ICONITEM::SKILL1, nIconID%1000, 5, CsPoint::ZERO );
		//=================================================================================================================
		//	스킬 효과
		//=================================================================================================================	
		_AddGuildSkillState( pSkill->GetInfo(), pString, FONT_WHITE, CsPoint( 10, 10 ) );
	}
	
	_AddLine(false);

	//=================================================================================================================
	//	버프 설명
	//=================================================================================================================	
	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_10;
	g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_MAX_SIZE - 20, UISTRING_TEXT( "TOOLTIP_GUILD_SKILL_EFFECT_APPLY" ).c_str(), &ti );
}

#endif

void cTooltip::_MakeTooltip_SpiritDigimon()
{
	// MainID = 소유 인터페이스
	// SubID_1 = 슬롯 인덱스

	CsDigimon::sINFO* pFTDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( (DWORD)m_nSubID_1 )->GetInfo(); 

	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	ti.s_Color = FONT_GOLD;

	// 여유 줄
	pString = NiNew cString;
	pString->AddSizeY( 1 );
	m_StringList.AddTail( pString );

	// 이름	
	pString = NiNew cString;
	ti.SetText( pFTDigimonInfo->s_szName );	
	pString->AddText( &ti );
	pString->AddSizeY( 3 );
	m_StringList.AddTail( pString );

	_AddLine( false );

	// Evol속성
	pString = NiNew cString;
#ifdef VERSION_USA
	pString->AddIcon( CsPoint( 80, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#else
	pString->AddIcon( CsPoint( 60, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#endif

	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	// Attribute
	pString = NiNew cString;
	// 0기반 인덱스로 전환

	// - 성장 속성
	int nIndex = pFTDigimonInfo->s_nDigimonType - 1;
	pString->AddIcon( CsPoint( 30, 19 ), ICONITEM::DGrowType, nIndex );

	// - 디지몬 속성
	nIndex = pFTDigimonInfo->s_eAttributeType - nsCsDigimonTable::AT_NONE;
	pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );
	// - 패밀리 속성
	for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
	{
		if( pFTDigimonInfo->s_eFamilyType[ i ] != 0 )
		{
			nIndex = pFTDigimonInfo->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
		}
	}

	// - 자연 속성
	if( pFTDigimonInfo->s_eBaseNatureTypes[0] != 0 )
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
	// - 자연 속성
	if( pFTDigimonInfo->s_eBaseNatureTypes[1] != 0 )
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
	// - 자연 속성
	if( pFTDigimonInfo->s_eBaseNatureTypes[2] != 0 )
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}

	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	//=================================================================================================================
	//	필요레벨
	//=================================================================================================================	
	if( m_nSubID_2 != 0 )
	{
		TCHAR sz[TOOLTIP_MAX_TEXT_LEN] = {0, };
		pString = NiNew cString;
		ti.s_eFontSize = CFont::FS_10;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_REQUIRE_LEVEL" ).c_str() );
#ifdef VERSION_TH
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
#else
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 2;
#endif

		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_WHITE;
		ti.SetText( (int)m_nSubID_2 );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

}

void cTooltip::_MakeTooltip_FriendShipPoint()
{
	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_12);

	UINT	MaxLineSize = 400;
	
	g_pStringAnalysis->Cut_Parcing( &m_StringList, MaxLineSize, (TCHAR*)UISTRING_TEXT( "FRIENDSHIP_TOOLTIP_TITLE" ).c_str(),&ti );
	
	_AddLine( false );

	ti.Init(CFont::FS_10);
	g_pStringAnalysis->Cut_Parcing( &m_StringList, MaxLineSize, (TCHAR*)UISTRING_TEXT( "FRIENDSHIP_TOOLTIP_MESSAGE" ).c_str(), &ti );
}

void cTooltip::_MakeTooltip_DataTrade()	//	DATA_TRADE_SYSTEM
{
	//	m_nMainID = 디지몬 BaseID
	//	m_nSubID_1 = 슬롯 인덱스
	//	m_nSubID_2 = 상태
	SAFE_POINTER_RET(nsCsFileTable::g_pEvolMng);

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_nMainID );
	SAFE_POINTER_RET(pFTEvolInfo);
	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( static_cast<int>(m_nSubID_1) );
	SAFE_POINTER_RET(pFTEvolObj);
	DWORD dwDestModelID = pFTEvolObj->m_dwID;

	SAFE_POINTER_RET(nsCsFileTable::g_pDigimonMng);
	SAFE_POINTER_RET(nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID ));
	CsDigimon::sINFO* pFTDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID )->GetInfo(); 
	SAFE_POINTER_RET(pFTDigimonInfo);

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );

	// 상태 - 이름색
	switch( m_nSubID_2 )
	{
	case CDigimonUser::eActive:
		ti.s_Color = FONT_GOLD;
		break;
	case CDigimonUser::eInActive:
		ti.s_Color = TOOLTIP_CANNOT_COLOR;
		break;
	case CDigimonUser::eClosed:
	case CDigimonUser::eNone:
		ti.s_Color = NiColor( 0.5f, 0.5f, 0.5f );	
		break;
	default:
		assert_cs( false );
	}

	cString* pString;

	// 여유 줄
	pString = NiNew cString;
	pString->AddSizeY( 1 );
	m_StringList.AddTail( pString );

	// 이름	
	pString = NiNew cString;
	ti.SetText( pFTDigimonInfo->s_szName );	
	pString->AddText( &ti );
	pString->AddSizeY( 3 );
	m_StringList.AddTail( pString );

	_AddLine( false );

	// Evol속성
	pString = NiNew cString;
#ifdef VERSION_USA
	pString->AddIcon( CsPoint( 80, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#else
	pString->AddIcon( CsPoint( 60, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#endif

	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	// Attribute
	pString = NiNew cString;	// 0기반 인덱스로 전환

	int nIndex = pFTDigimonInfo->s_nDigimonType - 1;	// - 성장 속성
	pString->AddIcon( CsPoint( 30, 19 ), ICONITEM::DGrowType, nIndex );

	nIndex = pFTDigimonInfo->s_eAttributeType - nsCsDigimonTable::AT_NONE;	// - 디지몬 속성
	pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );

	for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )	// - 패밀리 속성
	{
		if( pFTDigimonInfo->s_eFamilyType[ i ] != 0 )
		{
			nIndex = pFTDigimonInfo->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
		}
	}
	if( pFTDigimonInfo->s_eBaseNatureTypes[0] != 0 )	// - 자연 속성
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
	if( pFTDigimonInfo->s_eBaseNatureTypes[1] != 0 )	// - 자연 속성
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
	if( pFTDigimonInfo->s_eBaseNatureTypes[2] != 0 )	// - 자연 속성
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}

	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	// 추가(상세정보) 조건 표시 하지 않음. 필요 DS, 오픈 레벨, 오픈 퀘스트등등의 상세정보.
}
