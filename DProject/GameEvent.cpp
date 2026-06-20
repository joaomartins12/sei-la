#include "StdAfx.h"
#include "GameEvent.h"

CGameEventMng*		g_pGameEventMng = NULL;

CGameEventMng::CGameEventMng(void)
{
	m_mapAreaSceneData.clear();
	m_CurMapAreaData = NULL;	
	m_vDelta = NiPoint3::ZERO;
	m_bSkip = false;
	m_fAccumTime = -1.0f;

	m_pSprite1 = NULL;	
	m_pSprite2 = NULL;	
	m_pSkip = NULL;	
}

CGameEventMng::~CGameEventMng(void)
{
}

void CGameEventMng::EventTalk( uint nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType )
{	
	TCHAR sz[ 250 ];
	_tcscpy_s( sz, 250, szMsg );

	if( g_pEventSceneMng->IsCameraScene() == true )	
		_SetText( nTargetUID, sz );			
	else
		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_3, nTargetUID, sz, bExistDelete, (cTalkBalloon::sBALLOONE::OBJ_TYPE)ObjType );
}

void CGameEventMng::EventSave( int nSceneNum )
{
}

void CGameEventMng::Delete()
{
	std::map< UINT, sAreaEventScene* >::iterator it = m_mapAreaSceneData.begin();
	std::map< UINT, sAreaEventScene* >::iterator itEnd = m_mapAreaSceneData.end();
	for( ; it != itEnd; it++ )
	{
		it->second->Delete();
		SAFE_NIDELETE( it->second );
	}
	m_mapAreaSceneData.clear();

	if( m_CurMapAreaData != NULL )
	{		
		m_CurMapAreaData->Delete();
		SAFE_NIDELETE( m_CurMapAreaData );		
	}	

	SAFE_NIDELETE( m_pSprite1 );
	SAFE_NIDELETE( m_pSprite2 );
	SAFE_NIDELETE( m_pSkip );
	m_StringList.Delete();
}

void CGameEventMng::ResetDevice()
{	
	if( m_pSprite1 )
		m_pSprite1->ResetDevice();
	if( m_pSprite2 )
		m_pSprite2->ResetDevice();	
	if( m_pSkip )
		m_pSkip->ResetDevice();
	if( m_StringList.GetSize() > 0 )
		m_StringList.ResetDevice();	
}

void CGameEventMng::Init()
{	
	LoadAreaScene();
	m_CurMapAreaData = NULL;	

	// 이벤트 바 생성			
	float fViewSize = (float)g_nScreenHeight * 0.09f;
	m_pSprite1 = NiNew cSprite;			
	m_pSprite1->Init( NULL, CsPoint::ZERO, CsPoint( (int)g_nScreenWidth, (int)fViewSize ), false, NiColor::BLACK );				

	fViewSize = (float)g_nScreenHeight * 0.18f;
	m_pSprite2 = NiNew cSprite;			
	m_pSprite2->Init( NULL, CsPoint( 0, (int)(g_nScreenHeight - fViewSize) ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), false, NiColor::BLACK );						

	m_pSkip = NiNew cButton;
	m_pSkip->Init( NULL, CsPoint( 10, 10 ), CsPoint( 76, 26 ), "Event\\skip.tga", true );				
	m_pSkip->SetTexToken( CsPoint( 76, 26 ) );

	m_LifeTimeSeq.SetDeltaTime( 12000 );
}

void CGameEventMng::Update( float fDeltaTime )
{	
}

void CGameEventMng::Render()
{
}

void CGameEventMng::RenderText()
{
}

void CGameEventMng::LoadAreaScene()
{	
	std::map< UINT, CEventSceneData* >::iterator it = g_pEventDataMng->GetMap()->begin();
	std::map< UINT, CEventSceneData* >::iterator itEnd = g_pEventDataMng->GetMap()->end();

	for( ; it != itEnd; it++ )
	{
		if( it->second->GetCondition()->s_bOnlyArea )
		{
			CEventSceneData::sEvent_Condition*	Info = it->second->GetCondition();		

			if( Info->s_bLoop )
			{
				if( m_mapAreaSceneData.find( Info->s_dwMapID ) == m_mapAreaSceneData.end() )
				{
					sAreaEventScene* pAreaScene = NiNew sAreaEventScene;						
					m_mapAreaSceneData[ Info->s_dwMapID ] = pAreaScene;
				}					
				m_mapAreaSceneData[ Info->s_dwMapID ]->AddAreaScene( it->second );
			}
			else
			{
				if( g_pResist->m_CharEvent.Check( it->second->GetSceneNum() ) == false )
				{
					if( m_mapAreaSceneData.find( Info->s_dwMapID ) == m_mapAreaSceneData.end() )
					{
						sAreaEventScene* pAreaScene = NiNew sAreaEventScene;						
						m_mapAreaSceneData[ Info->s_dwMapID ] = pAreaScene;
					}					
					m_mapAreaSceneData[ Info->s_dwMapID ]->AddAreaScene( it->second );
				}
			}			
		}
	}
}

void CGameEventMng::ReLoadScene( DWORD dwMapID )
{
	if( m_CurMapAreaData != NULL )
	{		
		m_CurMapAreaData->Delete();
		SAFE_NIDELETE( m_CurMapAreaData );		
	}	

	std::map< UINT, sAreaEventScene* >::iterator it = m_mapAreaSceneData.find( dwMapID );	
	if( it != m_mapAreaSceneData.end() )
	{
		m_CurMapAreaData = NiNew sAreaEventScene;
		for( int i = 0; i < it->second->m_vpAreaScene.Size(); i++ )
		{
			m_CurMapAreaData->AddAreaScene( it->second->m_vpAreaScene[ i ] );
		}		
	}
	else
	{
		m_CurMapAreaData = NULL;
	}	
}

CGameEventMng::sAreaEventScene* CGameEventMng::GetAreaScene( UINT MapID )
{
	std::map< UINT, sAreaEventScene* >::iterator it = m_mapAreaSceneData.find( MapID );
	if( it != m_mapAreaSceneData.end() )
	{
		return it->second;
	}	
	return NULL;
}

void CGameEventMng::EventStart()
{		
}

void CGameEventMng::EventEnd()
{	
	// 페이드 아웃	
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.6f );

	CAMERA_ST.LoadCameraState();
	
	g_pGameIF->SetRenderIF( true );		

	g_pResist->SetEnableSound( m_bIsSound );
	CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;
	g_pCharMng->GetTamerUser()->PlayBGM();

	m_bSkip = false;
	m_vDelta = NiPoint3::ZERO;	
	
	m_StringList.Delete();

	g_pMngCollector->ReleaseScene();
}

void CGameEventMng::EventSkip()
{
	if( m_bSkip )
		return;
	
	m_bSkip = true;
	// 이벤트 본것 기록 저장
	EventSave( g_pEventSceneMng->GetScene()->GetSceneNum() );
	
	g_pEventSceneMng->ResetScene();	
	EventEnd();		
}

void CGameEventMng::_SetText( uint nTargetUID, TCHAR* szText )
{
	m_LifeTimeSeq.OffReset();
	m_StringList.Delete();
	m_StringList.AddTailEmpty( CFont::FS_8 );

	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20, NiColor( 1.0f, 1.0f, 170.f / 255.0f ) );					
	ti.s_bOutLine = false;		
	ti.s_eTextAlign = DT_LEFT;

	CsC_AvObject* pTarget = g_pEventSceneMng->GetEventObject( nTargetUID );
	if( pTarget != NULL )
	{			

#ifdef VERSION_USA
		TCHAR				szName[ 64 ];
		switch( nTargetUID )
		{			
		case 1183344213:				_tcscpy_s( szName, 64, L"<Destroyer> Bearmon" ); break;		
		case 2155260802:				_tcscpy_s( szName, 64, L"IceDevimon of Lure" );	break;	
		case 2334953242:				_tcscpy_s( szName, 64, L"<Destroyer> Gotsumon" ); break;		
		case 2488427042:				_tcscpy_s( szName, 64, L"IceDevimon of Lure" );	break;	
		case 2987527771:				_tcscpy_s( szName, 64, L"Digimon Kaiser" );	break;	
		case 332208904:					_tcscpy_s( szName, 64, L"Akihiro Kurata" );	break;	
		case 337611911:					_tcscpy_s( szName, 64, L"<Royal knights> Gallantmon" );	break;
		case 339215476:					_tcscpy_s( szName, 64, L"<Royal knights> Gallantmon" );	break;
		case 998246825:					_tcscpy_s( szName, 64, L"GizumonAT" );	break;	
		case 998246826:					_tcscpy_s( szName, 64, L"GizumonAT" );	break;	
		case 998246827:					_tcscpy_s( szName, 64, L"GizumonAT" );	break;	
		case 2587154152:				_tcscpy_s( szName, 64, L"GizumonAT" );	break;	
		case 2184852451:				_tcscpy_s( szName, 64, L"<Wanted> Deputymon" );	break;
		case 1054583918:				_tcscpy_s( szName, 64, L"<Western station manager> Hawkmon" );	break;
		case  249450997:				_tcscpy_s( szName, 64, L"Starmon of Passion" );	break;	 
		case 1065179065:				_tcscpy_s( szName, 64, L"SuperStarmon" );	break;	 
		case 3260231609:				_tcscpy_s( szName, 64, L"Pajiramon" );	break;	 
		case 3982312157:				_tcscpy_s( szName, 64, L"Devimon" );	break;	 
		case 420216346:					_tcscpy_s( szName, 64, L"Starmon of Passion" );	break;	 
		default:						_tcscpy_s( szName, 64, L"" );	break;
		}

		g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, szName, &ti );
#else
		switch( pTarget->GetLeafRTTI() )
		{		
		case RTTI_NPC:
			g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, ((CNpc*)pTarget)->GetName(), &ti );
			break;
		case RTTI_EVENTOBJECT:			
			g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, ((CsC_EventObject*)pTarget)->GetName(), &ti );
			break;		
		case RTTI_EVENTTAMEROBJECT:			
			g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, ((CsC_EventTamerObject*)pTarget)->GetName(), &ti );
			break;		
		default:
			assert_cs( false );
			break;
		}	
#endif
	
	}		

	m_StringList.AddTailEmpty( CFont::FS_8 );	

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, FONT_WHITE );					
	ti.s_bOutLine = false;
	//ti.s_eTextAlign = DT_CENTER;	
	g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, szText, &ti );	

	_SetAlpha( ALPA_INCREASE, 0.0f );
}

void CGameEventMng::_SetAlpha( eALPA_TYPE eAlpha, float fAlpha )
{
	m_eAlpa = eAlpha;
	m_fAlpaValue = fAlpha;
	m_StringList.SetAlpha( fAlpha );
}