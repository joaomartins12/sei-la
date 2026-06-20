
#include "stdafx.h"
#include "TalkBalloon.h"

#define IF_BALLOONE_STRING_DELTA_Y		3

std::queue< cTalkBalloon::sBALLOONE* >		cTalkBalloon::sBALLOONE::g_MemPool[ MAX_TYPE ];

void cTalkBalloon::EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType )
{
	TCHAR sz[ 250 ];
	_tcscpy_s( sz, 250, szMsg );

	g_pTalkBallone->SetBalloone( TYPE_3, nTargetUID, sz, bExistDelete, (sBALLOONE::OBJ_TYPE)ObjType );
}


void cTalkBalloon::sBALLOONE::GlobalShotDown()
{
	sBALLOONE* pBalloone;
	for( int i=0; i<MAX_TYPE; ++i )
	{
		while( g_MemPool[ i ].empty() == false )
		{
			pBalloone = g_MemPool[ i ].front();
			SAFE_NIDELETE( pBalloone->s_pBalloon1 );
			SAFE_NIDELETE( pBalloone->s_pBalloon2 );
			SAFE_NIDELETE( pBalloone->s_pBalloon3 );
			SAFE_NIDELETE( pBalloone->s_pBalloonTail );
			SAFE_NIDELETE( pBalloone->s_pBalloonDiscript );
			SAFE_NIDELETE( pBalloone->s_pName );
			assert_cs( pBalloone->s_StringList.GetSize() == 0 );
			assert_cs( pBalloone->s_nTargetUID == 0 );
			SAFE_NIDELETE( pBalloone );
			g_MemPool[ i ].pop();
		}
	}	
}

cTalkBalloon::sBALLOONE* cTalkBalloon::sBALLOONE::NewInstance( eTYPE type, OBJ_TYPE ObjType )
{
	sBALLOONE* pReturn = NULL;
	if( g_MemPool[ type ].empty() == true )
	{
		pReturn = NiNew sBALLOONE;
		pReturn->s_eType = type;
		pReturn->s_eObjType = ObjType;
		pReturn->s_pBalloon1 = NiNew cSprite;
		pReturn->s_pBalloon2 = NiNew cSprite;
		pReturn->s_pBalloon3 = NiNew cSprite;
		pReturn->s_pBalloonTail = NiNew cSprite;
		pReturn->s_pBalloonDiscript = NULL;
		NiColor color;
		switch( type )
		{
		case TYPE_1:
			color = NiColor::WHITE;//g_pCharMng->GetTamerUser()->GetNameColor();
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone1_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone1_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone1_3.tga", true );
			pReturn->s_pBalloonTail->Init( NULL, CsPoint::ZERO, CsPoint( 9, 9 ), "Balloone\\Balloone1_Tail.tga", true );
			break;
		case TYPE_2:
			color = NiColor::WHITE;//g_pCharMng->GetDigimonUser( 0 )->GetNameColor();
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone2_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone2_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone2_3.tga", true );
			pReturn->s_pBalloonTail->Init( NULL, CsPoint::ZERO, CsPoint( 9, 9 ), "Balloone\\Balloone2_Tail.tga", true );

			pReturn->s_pBalloonDiscript = NiNew cSprite;
			pReturn->s_pBalloonDiscript->Init( NULL, CsPoint::ZERO, CsPoint( 37, 37 ), "Balloone\\Balloone2_Tip.tga", true );
			break;
		case TYPE_3:
			color = NiColor::WHITE;//g_pCharMng->GetDigimonUser( 0 )->GetNameColor();
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone3_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone3_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone3_3.tga", true );
			pReturn->s_pBalloonTail->Init( NULL, CsPoint::ZERO, CsPoint( 9, 9 ), "Balloone\\Balloone3_Tail.tga", true );
			break;
		default:
			assert_cs( false );
		}

		cText::sTEXTINFO ti;
		ti.Init( &g_EventScene.m_FontText, CFreeFont::FS_10, color );
		pReturn->s_pName = NiNew cText;
		pReturn->s_pName->Init( NULL, CsPoint::ZERO, &ti, true );		
	}
	else
	{
		pReturn = g_MemPool[ type ].front();
		g_MemPool[ type ].pop();
	}
	
	return pReturn;
}

void cTalkBalloon::sBALLOONE::DeleteInstance( sBALLOONE* pInst )
{
	pInst->s_nTargetUID = 0;
	pInst->s_StringList.Delete();
	g_MemPool[ pInst->s_eType ].push( pInst );
}

void cTalkBalloon::sBALLOONE::Init( uint nTargetUID, TCHAR* szMsg, INIT_TYPE init )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_EventScene.m_FontText, CFreeFont::FS_10 );
	//ti.s_bOutLine = false;

	s_nTargetUID = nTargetUID;

	CsC_AvObject* pTarget;
	switch( s_eObjType )
	{
	case eNOROBJ:
		//pTarget = g_pMngCollector->GetObject( s_nTargetUID );
		break;
	case eNEWOBJ:
		pTarget = g_pEventSceneMng->GetEventObject( s_nTargetUID );			
		if( pTarget != NULL )
		{			
			switch( pTarget->GetLeafRTTI() )
			{
			case RTTI_EVENTOBJECT:
				s_pName->SetText( ((CsC_EventObject*)pTarget)->GetName() );
				break;		
			case RTTI_EVENTTAMEROBJECT:
				s_pName->SetText( ((CsC_EventTamerObject*)pTarget)->GetName() );
				break;		
			}	
		}		
		break;
	case eNPCOBJ:
		pTarget = g_pNpcMng->GetNpc( s_nTargetUID );
		break;
	default:
		assert_cs(false);
		break;
	}

	if( pTarget == NULL )
	{
		pTarget = g_EventScene.GetEventTarget();	
		switch( pTarget->GetLeafRTTI() )
		{
		case RTTI_EVENTOBJECT:
			s_pName->SetText( ((CsC_EventObject*)pTarget)->GetName() );
			break;		
		case RTTI_EVENTTAMEROBJECT:
			s_pName->SetText( ((CsC_EventTamerObject*)pTarget)->GetName() );
			break;		
		}
	}

	assert_cs( pTarget != NULL );

	g_pStringAnalysis->Cut( &s_StringList, 250, szMsg, &ti );

	s_vSize = s_StringList.CalMaxSize( IF_BALLOONE_STRING_DELTA_Y );
	s_vSize.x += 60;
	s_vSize.y += 42;

	s_fLiveTime = 5.0f;
	s_eCurLive = INCREASE;

	switch( init )
	{
	case NEW:
		s_fAlpha = 0.0f;
		break;
	case EXIST:
		s_fAlpha = 1.0f;
		break;
	default:
		assert_cs( false );
	}
	
	//s_pName->SetText( pTarget->GetName() );
	s_vSize.x = CsMax( s_vSize.x, s_pName->GetStringSize().x + 20 );
	assert_cs( s_vSize.x > 6 );
	s_pBalloon1->SetSize( CsPoint( 6, s_vSize.y ) );
	s_pBalloon2->SetSize( CsPoint( s_vSize.x - 12, s_vSize.y ) );
	s_pBalloon3->SetSize( CsPoint( 6, s_vSize.y ) );	
}

bool cTalkBalloon::sBALLOONE::Update()
{
	CsC_AvObject* pTarget;
	switch( s_eObjType )
	{
	case eNOROBJ:
		//pTarget = g_pMngCollector->GetObject( s_nTargetUID );
		break;
	case eNEWOBJ:
		pTarget = g_pEventSceneMng->GetEventObject( s_nTargetUID );
		break;
	case eNPCOBJ:
		pTarget = g_pNpcMng->GetNpc( s_nTargetUID );
		break;
	default:
		assert_cs(false);
		break;
	}

	if( pTarget == NULL )
		pTarget = g_EventScene.GetEventTarget();
	if( pTarget == NULL )
		return false;

	if( (s_fLiveTime -= g_fDeltaTime) < 0.0f )
		s_eCurLive = DECREASE;

	switch( s_eCurLive )
	{
	case INCREASE:
		{
			if( ( s_fAlpha += g_fDeltaTime*2.0f ) > 1.0f )
				s_fAlpha = 1.0f;
		}
		break;
	case DECREASE:
		{
			if( ( s_fAlpha -= g_fDeltaTime*2.0f ) < 0.0f )
			{
				SetTargetAlpha( 1.0f );
				return false;
			}
		}
		break;
	default:
		assert_cs( false );
	}
	return true;
}

void cTalkBalloon::sBALLOONE::SetTargetAlpha( float fAlpha )
{
	CsC_AvObject* pTarget;
	switch( s_eObjType )
	{
	case eNOROBJ:
		//pTarget = g_pMngCollector->GetObject( s_nTargetUID );
		break;
	case eNEWOBJ:
		pTarget = g_pEventSceneMng->GetEventObject( s_nTargetUID );					
		break;
	case eNPCOBJ:
		pTarget = g_pNpcMng->GetNpc( s_nTargetUID );
		break;
	default:
		assert_cs(false);
		break;
	}

	if( pTarget == NULL )
		pTarget = g_EventScene.GetEventTarget();
	if( pTarget == NULL )
		return;

}

cTalkBalloon*	g_pTalkBallone = NULL;

void cTalkBalloon::GlobalInit()
{
	assert_cs( g_pTalkBallone == NULL );
	g_pTalkBallone = NiNew cTalkBalloon;
	g_pTalkBallone->Init();
}

void cTalkBalloon::GlobalShotDown()
{
	if( g_pTalkBallone )
	{
		g_pTalkBallone->Delete();
		SAFE_NIDELETE( g_pTalkBallone );
	}	

	sBALLOONE::GlobalShotDown();
}

void cTalkBalloon::Init()
{
	
}

void cTalkBalloon::PreResetMap()
{
	Delete();
}

void cTalkBalloon::Delete()
{
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	for( ; it!=itEnd; ++it )
	{
		sBALLOONE::DeleteInstance( *it );
	}
	m_BallooneList.clear();

	m_vpSort.Destroy();
	m_vpRender.Destroy();
}

void cTalkBalloon::SetBalloone( eTYPE type, uint nTargetUID, TCHAR* szMsg, bool bExistDelete /*=true*/, sBALLOONE::OBJ_TYPE Objtype /*=eNOROBJ */ )
{
	CsC_AvObject* pTarget;
	switch( Objtype )
	{
	case sBALLOONE::eNOROBJ:
		//pTarget = g_pMngCollector->GetObject( s_nTargetUID );
		break;
	case sBALLOONE::eNEWOBJ:
		pTarget = g_pEventSceneMng->GetEventObject( nTargetUID );					
		break;
	case sBALLOONE::eNPCOBJ:
		pTarget = g_pNpcMng->GetNpc( nTargetUID );
		break;
	default:
		assert_cs(false);
		break;
	}
	
	if( pTarget == NULL )
		pTarget = g_EventScene.GetEventTarget();
	if( pTarget == NULL )
		return;	

	sBALLOONE::INIT_TYPE init = sBALLOONE::NEW;
	// 같은 타겟이 있는지 살펴 본다.
	// 같은 타겟이 있다면 제거
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nTargetUID == nTargetUID )
		{
			// 제거 모드가 아니면 리턴
			if( bExistDelete == false )
				return;

			sBALLOONE::DeleteInstance( *it );
			m_BallooneList.erase( it );
			init = sBALLOONE::EXIST;
			break;
		}
	}

	sBALLOONE* pBalloone = sBALLOONE::NewInstance( type, Objtype );
	pBalloone->Init( nTargetUID, szMsg, init );
	m_BallooneList.push_back( pBalloone );
}

void cTalkBalloon::Update()
{
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	while( it != itEnd )
	{
		if( (*it)->Update() == false )
		{
			sBALLOONE::DeleteInstance( *it );
			it = m_BallooneList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void cTalkBalloon::Render()
{
	_Cal3D_to_2D();
	_Sort();

	sBALLOONE* pBalloone;
	int nCount = m_vpRender.Size();
	for( int i=0; i<nCount; ++i )
	{
		pBalloone = m_vpRender[ i ];
		pBalloone->s_pBalloon1->Render( pBalloone->s_vPos, pBalloone->s_fAlpha );
		pBalloone->s_pBalloon2->Render( pBalloone->s_vPos + CsPoint( 6, 0 ), pBalloone->s_fAlpha );
		pBalloone->s_pBalloon3->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ), pBalloone->s_fAlpha );
		pBalloone->s_pBalloonTail->Render( pBalloone->s_vPos + CsPoint( CsFloat2Int(pBalloone->s_vSize.x*0.18f), pBalloone->s_vSize.y - 1 ) , pBalloone->s_fAlpha );
		if( pBalloone->s_pBalloonDiscript )
			pBalloone->s_pBalloonDiscript->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ) + CsPoint( -23, -15 ), pBalloone->s_fAlpha );

		pBalloone->s_pName->SetAlpha( pBalloone->s_fAlpha );
		pBalloone->s_pName->Render( pBalloone->s_vPos + CsPoint( 10, 5 ) );
		pBalloone->s_StringList.SetAlpha( pBalloone->s_fAlpha );
		pBalloone->s_StringList.Render( pBalloone->s_vPos +	CsPoint( 30, 30 ), IF_BALLOONE_STRING_DELTA_Y );

		pBalloone->SetTargetAlpha( 1.0f - pBalloone->s_fAlpha );
	}
	m_vpSort.ClearElement();
	m_vpRender.ClearElement();
}

void cTalkBalloon::_Cal3D_to_2D()
{
	CsC_AvObject* pTarget;
	NiPoint3 pos, out;
	NiPoint3 vCameraPos = g_Camera.GetWorldTranslate();
	sBALLOONE* pBalloone;
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	for( ; it!=itEnd; ++it )
	{
		pBalloone = (*it);
		
		switch( pBalloone->s_eObjType )
		{
		case sBALLOONE::eNOROBJ:
			//pTarget = g_pMngCollector->GetObject( it->s_nTargetUID );
			break;
		case sBALLOONE::eNEWOBJ:
			pTarget = g_pEventSceneMng->GetEventObject( pBalloone->s_nTargetUID );
			break;
		case sBALLOONE::eNPCOBJ:
			pTarget = g_pNpcMng->GetNpc( pBalloone->s_nTargetUID );
			break;
		default:
			assert_cs(false);
			break;
		}

		if( pTarget == NULL )
			pTarget = g_EventScene.GetEventTarget();
		if( pTarget == NULL )
			continue;

		if( pTarget->IsLoad() == false )
			continue;
		if( pTarget->GetCsNode() == NULL )
			continue;
		
		// 케릭터가 사라지고 안보이는 거리라면
		if( pTarget->GetProp_Alpha()->IsHideDistOut() == true )
			continue;

		
		pos = pTarget->GetPos();
		float fLength = ( vCameraPos - pos ).Length();
		float fScale = pow( fLength*fLength*15.0f, 1.0f/3.0f )*0.43f*0.008f;		
		pos.z += pTarget->GetToolHeight() + 100.0f*fScale;
		out = g_EventScene.Cal3D_To_2D( pos );
		out.x += (int)( pBalloone->s_vSize.x*0.22f );
		
		// 근, 원 나갔다면
		if( ( out.z < 0.0f )||( out.z > 1.0f ) )
			continue;

		pBalloone->s_vPos = CsPoint( (int)out.x - pBalloone->s_vSize.x/2, (int)out.y - pBalloone->s_vSize.y/2 );
		
		// 화면을 나갔다면
		if( ( pBalloone->s_vPos.x > g_nScreenWidth )||
			( pBalloone->s_vPos.y > g_nScreenHeight )||
			( ( pBalloone->s_vPos.x + pBalloone->s_vSize.x ) < 0 )||
			( ( pBalloone->s_vPos.y + pBalloone->s_vSize.y ) < 0 ) )
		{
			continue;
		}

		pBalloone->s_fDepth = out.z;
		pBalloone->s_bFind = false;

		m_vpSort.PushBack( pBalloone );
	}
}

void cTalkBalloon::_Sort()
{
	int nSortCount = m_vpSort.Size();
	sBALLOONE* pBalloone;
	int nSrc = 0;
	while( nSrc < nSortCount )
	{
		pBalloone = m_vpSort[ nSrc ];
		if( pBalloone->s_bFind == true )
		{
			++nSrc;
			continue;
		}

		for( int d=nSrc+1; d<nSortCount; ++d )
		{
			if( m_vpSort[ d ]->s_bFind == true )
				continue;

			if( d == nSrc )
				continue;

			if( pBalloone->s_fDepth < m_vpSort[ d ]->s_fDepth )
			{
				pBalloone = m_vpSort[ d ];
			}
		}
		pBalloone->s_bFind = true;
		m_vpRender.PushBack( pBalloone );
	}
}

