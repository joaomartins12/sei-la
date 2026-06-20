
#include "stdafx.h"
#include "SystemMsg.h"

cSystemMsg*		g_pSystemMsg = NULL;
cSystemMsg*		g_pSkillMsg = NULL;		// 몬스터 스킬 사용 시 대사

#define SYSTEM_MSG_LIFE_TIME		6.0f
#define SYSTEM_MSG_HIDE_TIME		0.6f

#define NEXT_ALPHA					0.6f

void cSystemMsg::sMSG_INFO::Delete()
{
	SAFE_NIDELETE( s_pString );		
}

void cSystemMsg::sMSG_INFO::Init()
{
	s_pString = NULL;	
}

bool cSystemMsg::sMSG_INFO::Update()
{
	s_fLifeTime -= g_fDeltaTime;

	if( s_fLifeTime < 0.0f )
	{
		return false;
	}
	else if( s_fLifeTime < SYSTEM_MSG_HIDE_TIME )
	{
		assert_cs(s_pString);
		s_pString->SetAlpha( s_fLifeTime/SYSTEM_MSG_HIDE_TIME*NEXT_ALPHA );		
	}	
	return true;
}

int cSystemMsg::sMSG_INFO::Render( int ny, bool bFirst, int nType/* = 0*/ )
{
	assert_cs( s_pString );

	switch( (eMSG_TYPE)nType )
	{
	case MSG_SYSTEM:
		if( g_pSkillMsg != NULL )
		{
			if( g_pSkillMsg->GetSysyemMessageSize() != 0 )
				s_pString->SetAlpha( 0.5f );
			else
				s_pString->SetAlpha( 1.0f );
		}
		else
			s_pString->SetAlpha( 1.0f );
		break;
	case MSG_MONSTER_SKILL:
		s_pString->SetAlpha( 1.0f );
		break;
	}

	//s_pString->Render( CsPoint( g_nScreenWidth/2, ny ), bFirst == true ? -52 : -37, DT_CENTER );
	s_pString->Render( CsPoint( g_nScreenWidth/2, ny ), 2, DT_CENTER );
	return s_pString->GetSize();
}

void cSystemMsg::sMSG_INFO::SetString( TCHAR* szText, bool bExist )
{
	TCHAR const* pSafeText = ( szText != NULL ) ? szText : _T( "" );
	assert_cs( _tcslen( pSafeText ) < SYSTEM_TEXT_LEN );	
	_tcscpy_s( s_szText, SYSTEM_TEXT_LEN, pSafeText );	

	SAFE_NIDELETE( s_pString );

	cText::sTEXTINFO ti;
	if( bExist == false )
	{
		s_fLifeTime = SYSTEM_MSG_LIFE_TIME;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16 );
	}
	else
	{
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	}

	s_pString = NiNew cStringList;
	g_pStringAnalysis->Cut_Parcing( s_pString, 750, pSafeText, &ti );

	if( bExist == true )
		s_pString->SetAlpha( NEXT_ALPHA );
}

void cSystemMsg::GlobalInit()
{
	assert_cs( g_pSystemMsg == NULL );
	g_pSystemMsg = NiNew cSystemMsg;
	g_pSystemMsg->Init();
	g_pSkillMsg = NiNew cSystemMsg;
	g_pSkillMsg->Init( cSystemMsg::MSG_MONSTER_SKILL );
}

void cSystemMsg::GlobalShotDown()
{
	if( g_pSystemMsg != NULL )
		g_pSystemMsg->Delete();
	SAFE_NIDELETE( g_pSystemMsg );
	if( g_pSkillMsg != NULL )
		g_pSkillMsg->Delete();
	SAFE_NIDELETE( g_pSkillMsg );
}

void cSystemMsg::Delete()
{
	for( int i=0; i<MAX_SYSTEM_MSG; ++i )
	{
		m_MsgInfo[ i ].Delete();
	}

	SAFE_NIDELETE( m_pBGSprite );
}

void cSystemMsg::Init( int nType /*= 0*/ )
{

	for( int i=0; i<MAX_SYSTEM_MSG; ++i )
	{
		m_MsgInfo[ i ].Init();
		m_stackMsgInfo.push( &m_MsgInfo[ i ] );
	}
	m_szLastMsg[ 0 ] = NULL;
	m_nLastIndex = -1;

	m_pBGSprite = NiNew cSprite;
	m_eMsgType = (eMSG_TYPE)nType;
	m_pBGSprite->Init( NULL, CsPoint::ZERO, CsPoint( 750, 30 ), "Region\\Region02.tga", false );
}

void cSystemMsg::ResetMap()
{
	std::list< sMSG_INFO* >::iterator it = m_listMsgInfo.begin();
	std::list< sMSG_INFO* >::iterator itEnd = m_listMsgInfo.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->s_fLifeTime = 0.0f;
	}
}

void cSystemMsg::Update()
{
	std::list< sMSG_INFO* >::iterator it = m_listMsgInfo.begin();
	std::list< sMSG_INFO* >::iterator itDelete = it;
	while( it != m_listMsgInfo.end() )
	{
		itDelete = it;
		++it;
		if( (*itDelete)->Update() == false )
		{
			m_stackMsgInfo.push( (*itDelete) );
			m_listMsgInfo.erase( itDelete );

			// 메세지 저장 초기화
			if( ( --m_nLastIndex ) == 0 )
			{
				m_szLastMsg[ 0 ] = NULL;
			}
		}
	}
}

void cSystemMsg::Render()
{
	if( g_pGameIF )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TALK )||
			g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_REV )||
			g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP )	)
		{
			return;
		}
	}

	if( m_listMsgInfo.empty() == false )
	{
		sMSG_INFO* pFirst = ( *m_listMsgInfo.begin() );
		int nSize = pFirst->s_pString->GetSize();
		if( pFirst->s_fLifeTime < SYSTEM_MSG_HIDE_TIME )
			m_pBGSprite->SetAlpha( pFirst->s_fLifeTime/SYSTEM_MSG_HIDE_TIME );
		else
		{
			switch( m_eMsgType )
			{
			case MSG_SYSTEM:
				{
					if( g_pSkillMsg != NULL )
					{
						if( g_pSkillMsg->GetSysyemMessageSize() != 0 )
							m_pBGSprite->SetAlpha( 0.6f );
						else
							m_pBGSprite->SetAlpha( 1.0f );
					}
					else
						m_pBGSprite->SetAlpha( 1.0f );
				}
				break;
			case MSG_MONSTER_SKILL:
				m_pBGSprite->SetAlpha( 1.0f );
				break;
			default:
				m_pBGSprite->SetAlpha( 1.0f );
				break;
			}
		}
		m_pBGSprite->Render( CsPoint( ( g_nScreenWidth - 750 )/2, 167 ), CsPoint( 750, 30*nSize ) );
	}

	bool bFirst = true;
	int nListCount;
	int nStartY = 170;	

	std::list< sMSG_INFO* >::iterator it = m_listMsgInfo.begin();
	for( ; it!=m_listMsgInfo.end(); ++it )
	{
		nListCount = (*it)->s_pString->GetSize();
		if( ( bFirst == false )&&( nListCount > 1 ) )
		{
			nStartY -= 20*(nListCount-1);
		}
		nListCount = (*it)->Render( nStartY, bFirst, m_eMsgType );
		nStartY -= 20;
		bFirst = false;
	}
}

void cSystemMsg::AddSystemMessage( TCHAR* szMsg )
{
	if( szMsg == NULL )
		return;

	if( _tcscmp( m_szLastMsg, szMsg ) == 0 )
	{
		if( m_listMsgInfo.size() )
		{
			( *m_listMsgInfo.begin() )->s_fLifeTime = SYSTEM_MSG_LIFE_TIME;
			( *m_listMsgInfo.begin() )->s_pString->SetAlpha( 1.0f );
		}		
		return;
	}

	sMSG_INFO* pInfo = NULL;

	// 스택에 비어 있는게 있다면
	if( m_stackMsgInfo.empty() == false )
	{
		pInfo = m_stackMsgInfo.top();
		m_stackMsgInfo.pop();
	}
	// 비여 있는게 없다면
	else
	{
		// 가장 마지막거 제거
		std::list< sMSG_INFO* >::iterator it = m_listMsgInfo.begin();
		std::list< sMSG_INFO* >::iterator itNext;
		for( ; it!=m_listMsgInfo.end(); ++it )
		{
			itNext = it;
			++itNext;
			if( itNext == m_listMsgInfo.end() )
			{
				pInfo = (*it);
				m_listMsgInfo.erase( it );
				break;
			}
		}
	}

	// 가장 앞에게 있다면 가져 와서 줄여주자
	if( m_listMsgInfo.empty() == false )
	{
		sMSG_INFO* pFront = ( *m_listMsgInfo.begin() );		
		pFront->SetString( m_szLastMsg, true );
	}

	assert_cs( pInfo != NULL );
	pInfo->SetString( szMsg, false );
	m_listMsgInfo.push_front( pInfo );

	_tcscpy_s( m_szLastMsg, SYSTEM_TEXT_LEN, szMsg );
	m_nLastIndex = m_listMsgInfo.size();
}
