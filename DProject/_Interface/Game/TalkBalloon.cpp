
#include "stdafx.h"
#include "TalkBalloon.h"

#define IF_BALLOONE_STRING_DELTA_Y		3

std::queue< cTalkBalloon::sBALLOONE* >		cTalkBalloon::sBALLOONE::g_MemPool[ MAX_TYPE ];

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

#ifdef CONSIGNMENT_SALE
			if( pBalloone->s_eType == TYPE_4 )
			{
				SAFE_NIDELETE( pBalloone->s_pBalloon5 );
				SAFE_NIDELETE( pBalloone->s_pBalloon6 );
				SAFE_NIDELETE( pBalloone->s_pBalloon7 );
			}			
#endif
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

#ifdef CONSIGNMENT_BALLOON_CASH
cTalkBalloon::sBALLOONE* cTalkBalloon::sBALLOONE::NewInstance( eTYPE type, OBJ_TYPE ObjType, NiColor nameColor, TCHAR* szFileName )
#else
cTalkBalloon::sBALLOONE* cTalkBalloon::sBALLOONE::NewInstance( eTYPE type, OBJ_TYPE ObjType, NiColor nameColor )
#endif
{
	sBALLOONE* pReturn = NULL;
	if( g_MemPool[ type ].empty() == true )
	{
		pReturn = NiNew sBALLOONE;
		pReturn->s_eType = type;		
		pReturn->s_pBalloon1 = NiNew cSprite;
		pReturn->s_pBalloon2 = NiNew cSprite;
		pReturn->s_pBalloon3 = NiNew cSprite;
		pReturn->s_pBalloonTail = NULL;
		pReturn->s_pBalloonDiscript = NULL;
		pReturn->s_pName = NULL;
#ifdef CONSIGNMENT_SALE

#ifdef CONSIGNMENT_BALLOON_CASH
		if( type == TYPE_4 || type == TYPE_6 )
#else
		if( type == TYPE_4 )	// 위탁 상점일때만 메모리 할당
#endif
		{
			pReturn->s_pBalloon5 = NiNew cSprite;;
			pReturn->s_pBalloon6 = NiNew cSprite;;
			pReturn->s_pBalloon7 = NiNew cSprite;;
		}		
		pReturn->s_bStoreSrech = false;
#endif

#ifdef CONSIGNMENT_BALLOON_CASH
		pReturn->s_bCashBalloon = false;
#endif

		switch( type )
		{
		case TYPE_1:			
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone1_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone1_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone1_3.tga", true );
			pReturn->s_pBalloonTail = NiNew cSprite;
			pReturn->s_pBalloonTail->Init( NULL, CsPoint::ZERO, CsPoint( 9, 9 ), "Balloone\\Balloone1_Tail.tga", true );

			pReturn->s_pName = NiNew cText;
			break;
		case TYPE_2:			
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone2_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone2_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone2_3.tga", true );
			pReturn->s_pBalloonTail = NiNew cSprite;
			pReturn->s_pBalloonTail->Init( NULL, CsPoint::ZERO, CsPoint( 9, 9 ), "Balloone\\Balloone2_Tail.tga", true );

			pReturn->s_pBalloonDiscript = NiNew cSprite;
			pReturn->s_pBalloonDiscript->Init( NULL, CsPoint::ZERO, CsPoint( 37, 37 ), "Balloone\\Balloone2_Tip.tga", true );

			pReturn->s_pName = NiNew cText;
			break;
		case TYPE_3:
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone3_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone3_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone3_3.tga", true );
			pReturn->s_pBalloonTail = NiNew cSprite;
			pReturn->s_pBalloonTail->Init( NULL, CsPoint::ZERO, CsPoint( 9, 9 ), "Balloone\\Balloone3_Tail.tga", true );

			pReturn->s_pName = NiNew cText;
			break;
		case TYPE_4:
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone4_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone4_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone4_3.tga", true );

#ifdef CONSIGNMENT_SALE
			pReturn->s_pBalloon5->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_1.tga", true );
			pReturn->s_pBalloon6->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_2.tga", true );
			pReturn->s_pBalloon7->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_3.tga", true );
#endif
			break;
		case TYPE_5:
			pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_1.tga", true );
			pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_2.tga", true );
			pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_3.tga", true );
			break;
#ifdef CONSIGNMENT_BALLOON_CASH
		case TYPE_6:
			{
				char szName[MAX_FILENAME] = {0, };
				char szPath[MAX_FILENAME] = "Balloone\\";

				W2M( szName, szFileName, MAX_FILENAME );
				strcat( szPath, szName );
				strcat( szPath, "_1.tga" );
				int nLen = strlen( szPath );

				pReturn->s_pBalloon1->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50), szPath, true );

				szPath[nLen - 5] = '2';
				pReturn->s_pBalloon2->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), szPath, true );

				szPath[nLen - 5] = '3';
				pReturn->s_pBalloon3->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), szPath, true );

#ifdef CONSIGNMENT_SALE
				pReturn->s_pBalloon5->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_1.tga", true );
				pReturn->s_pBalloon6->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_2.tga", true );
				pReturn->s_pBalloon7->Init( NULL, CsPoint::ZERO, CsPoint( 100, 50 ), "Balloone\\Balloone5_3.tga", true );
#endif

				pReturn->s_bCashBalloon = true;
			}
			
			break;
#endif
		default:
			assert_cs( false );
		}

		if( pReturn->s_pName )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_10, nameColor );		
			pReturn->s_pName->Init( NULL, CsPoint::ZERO, &ti, true );
		}		
	}
	else
	{
		BHPRT( "MemPool size[%d]: %d", type, g_MemPool[type].size() );

		pReturn = g_MemPool[ type ].front();
		if( pReturn->s_pName )
			pReturn->s_pName->GetTextInfo()->s_Color = nameColor;
		g_MemPool[ type ].pop();
	}

	pReturn->s_eObjType	 = ObjType;
	return pReturn;
}

void cTalkBalloon::sBALLOONE::DeleteInstance( sBALLOONE* pInst )
{
#ifdef CONSIGNMENT_BALLOON_CASH
	if( pInst->s_bCashBalloon )
	{
		pInst->s_nTargetUID = 0;
		pInst->s_StringList.Delete();

		SAFE_NIDELETE( pInst->s_pBalloon1 );
		SAFE_NIDELETE( pInst->s_pBalloon2 );
		SAFE_NIDELETE( pInst->s_pBalloon3 );
#ifdef CONSIGNMENT_SALE
// 		if( pInst->s_eType == TYPE_4 )
// 		{
			SAFE_NIDELETE( pInst->s_pBalloon5 );
			SAFE_NIDELETE( pInst->s_pBalloon6 );
			SAFE_NIDELETE( pInst->s_pBalloon7 );
/*		}	*/
#endif
		SAFE_NIDELETE( pInst->s_pBalloonTail );
		SAFE_NIDELETE( pInst->s_pBalloonDiscript );
		SAFE_NIDELETE( pInst->s_pName );
		assert_cs( pInst->s_StringList.GetSize() == 0 );
		assert_cs( pInst->s_nTargetUID == 0 );
		SAFE_NIDELETE( pInst );

		return;
	}
#endif
	pInst->s_nTargetUID = 0;
	pInst->s_StringList.Delete();
	g_MemPool[ pInst->s_eType ].push( pInst );

#ifdef CONSIGNMENT_SALE
	if( pInst->s_bStoreSrech == true )
		pInst->s_bStoreSrech = false;
#endif
}

bool cTalkBalloon::sBALLOONE::Init( uint nTargetUID, TCHAR const* szMsg, INIT_TYPE init, float fLifeTime )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	g_pStringAnalysis->Cut( &s_StringList, 250, szMsg, &ti );

	s_vSize = s_StringList.CalMaxSize( IF_BALLOONE_STRING_DELTA_Y );
	s_vSize.x += 60;
	if( s_pName )
		s_vSize.y += 42;
	else
		s_vSize.y += 18;

	s_fLiveTime = fLifeTime;
	s_eCurLive = INCREASE;

	switch( init )
	{
	case NEW:
#ifdef CONSIGNMENT_BALLOON_CASH
		if( s_bCashBalloon )
			s_fAlpha = 0.2f;
		else
			s_fAlpha = 0.0f;
#else
		s_fAlpha = 0.0f;
#endif
		break;
	case EXIST:
		s_fAlpha = 1.0f;
		break;
	default:
		assert_cs( false );
	}

	s_nTargetUID = nTargetUID;	
	
	CsC_AvObject* pTarget = NULL;
	switch( s_eObjType )
	{
	case eNOROBJ:
	case ePERSONSTORE_OBJ:
		pTarget = g_pMngCollector->GetObject( s_nTargetUID );
		if( s_pName )
			s_pName->SetText( pTarget ? pTarget->GetName() : _T( "" ) );
		break;	
	case eNEWOBJ:
		pTarget = g_pEventSceneMng->GetEventObject( s_nTargetUID );
		if( pTarget != NULL )
		{			
			switch( pTarget->GetLeafRTTI() )
			{
			case RTTI_EVENTOBJECT:
				if( s_pName )
					s_pName->SetText( ((CsC_EventObject*)pTarget)->GetName() );
				break;		
			case RTTI_EVENTTAMEROBJECT:
				if( s_pName )
					s_pName->SetText( ((CsC_EventTamerObject*)pTarget)->GetName() );
				break;		
			}	
		}	
		break;
	case eNPCOBJ:
		//pTarget = g_pNpcMng->GetNpc( s_nTargetUID );
		if( s_pName )
			s_pName->SetText( pTarget ? pTarget->GetName() : _T( "" ) );
		break;
	default:
		assert_cs(false);
		break;
	}

	if( s_pName )
		s_vSize.x = CsMax( s_vSize.x, s_pName->GetStringSize().x + 20 );

#ifdef CONSIGNMENT_BALLOON_CASH
	if( s_bCashBalloon )
	{
		assert_cs( s_vSize.x > 6 );
		s_pBalloon1->SetSize( CsPoint( 20 , 50 ) );
		s_pBalloon2->SetSize( CsPoint( s_vSize.x - 40, 50 ) );
		s_pBalloon3->SetSize( CsPoint( 20 , 50 ) );
	}
	else
	{
		assert_cs( s_vSize.x > 6 );
		s_pBalloon1->SetSize( CsPoint( 6, s_vSize.y ) );
		s_pBalloon2->SetSize( CsPoint( s_vSize.x - 12, s_vSize.y ) );
		s_pBalloon3->SetSize( CsPoint( 6, s_vSize.y ) );
	}
#else
	assert_cs( s_vSize.x > 6 );
	s_pBalloon1->SetSize( CsPoint( 6, s_vSize.y ) );
	s_pBalloon2->SetSize( CsPoint( s_vSize.x - 12, s_vSize.y ) );
	s_pBalloon3->SetSize( CsPoint( 6, s_vSize.y ) );
#endif

#ifdef CONSIGNMENT_SALE

#ifdef CONSIGNMENT_BALLOON_CASH
	if( s_eType == TYPE_4 || s_eType == TYPE_6 )
#else
	if( s_eType == TYPE_4 )
#endif
	{
		s_pBalloon5->SetSize( CsPoint( 6, s_vSize.y ) );
		s_pBalloon6->SetSize( CsPoint( s_vSize.x - 12, s_vSize.y ) );
		s_pBalloon7->SetSize( CsPoint( 6, s_vSize.y ) );
	}	
#endif
	
	return true;
}

bool cTalkBalloon::sBALLOONE::Update(float const& fDeltaTime)
{
	CsC_AvObject* pTarget = NULL;

	switch( s_eObjType )
	{
	case eNOROBJ:
	case ePERSONSTORE_OBJ:
		pTarget = g_pMngCollector->GetObject( s_nTargetUID );
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
		return false;

	if( s_fLiveTime != LIVE_UNLIMIT )
	{
		if( (s_fLiveTime -= g_fDeltaTime) < 0.0f )
			s_eCurLive = DECREASE;
	}	

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
	CsC_AvObject* pTarget = NULL;
	
	switch( s_eObjType )
	{
	case eNOROBJ:
	case ePERSONSTORE_OBJ:
		pTarget = g_pMngCollector->GetObject( s_nTargetUID );
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
		return;

	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_TAMER:
	case RTTI_TAMER_USER:
	case RTTI_TUTORIAL_TAMER:
		( (CTamer*)pTarget )->GetCharOption()->SetAlpha( fAlpha );
		break;
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
	case RTTI_TUTORIAL_DIGIMON:
		( (CDigimon*)pTarget )->GetCharOption()->SetAlpha( fAlpha );
		break;
	case RTTI_EVENTOBJECT:
	case RTTI_EVENTTAMEROBJECT:
	case RTTI_EMPLOYMENT:
	case RTTI_TUTORIAL_MONSTER:
		break;
	default:
		assert_cs( false );
	}
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
	BHPRT( "BalloonList Size(Delete) : %d", m_BallooneList.size() );

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

void cTalkBalloon::Delete( uint nTargetUID )
{
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	while( it != itEnd )
	{
		if( (*it)->s_nTargetUID == nTargetUID )
		{
			(*it)->SetTargetAlpha( 1.0f );
			sBALLOONE::DeleteInstance( *it );
			it = m_BallooneList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

cTalkBalloon::sBALLOONE* cTalkBalloon::SetBalloone( eTYPE type, uint nTargetUID, TCHAR const* szMsg, bool bExistDelete /*=true*/, sBALLOONE::OBJ_TYPE Objtype /*= sBALLOONE::eNOROBJ*/, float fLifeTime /*=5.0f*/ )
{
	//if( g_pResist->IsTutorial() )
	//{
	//	return NULL;
	//}

	CsC_AvObject* pTarget = NULL;
	switch( Objtype )
	{
	case sBALLOONE::eNOROBJ:
	case sBALLOONE::ePERSONSTORE_OBJ:
		pTarget = g_pMngCollector->GetObject( nTargetUID );
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
		return NULL;	

	sBALLOONE::INIT_TYPE init = sBALLOONE::NEW;
	// 같은 타겟이 있는지 살펴 본다.
	// 같은 타겟이 있다면 제거
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nTargetUID == nTargetUID )
		{
			if( (*it)->s_eObjType != Objtype )
			{
				// 존재 하는 타입이 특정 타입이라면 리턴 ex)개인 상점 문구
				switch( (*it)->s_eObjType )
				{
				case sBALLOONE::ePERSONSTORE_OBJ:
					return (*it);
				}
			}			

			// 제거 모드가 아니면 리턴
			if( bExistDelete == false )
				return (*it);

			sBALLOONE::DeleteInstance( *it );
			m_BallooneList.erase( it );
			init = sBALLOONE::EXIST;
			break;
		}
	}

#ifdef CONSIGNMENT_BALLOON_CASH
	TCHAR* szFileName = nsCsFileTable::g_pBaseMng->IsBallonNewType( pTarget->GetFTID() );

	sBALLOONE* pBalloone = sBALLOONE::NewInstance( type, Objtype, pTarget->GetNameColor(), szFileName );
#else
	sBALLOONE* pBalloone = sBALLOONE::NewInstance( type, Objtype, pTarget->GetNameColor() );
#endif
	pBalloone->Init( nTargetUID, szMsg, init, fLifeTime );
	m_BallooneList.push_back( pBalloone );

	BHPRT( "BalloonList Size(Set) : %d", m_BallooneList.size() );

	return pBalloone;
}

void cTalkBalloon::Update(float const& fDeltaTime)
{
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	while( it != itEnd )
	{
		if( (*it)->Update(fDeltaTime) == false )
		{
			sBALLOONE::DeleteInstance( *it );
			it = m_BallooneList.erase( it );
			BHPRT( "BalloonList Size(Update) : %d", m_BallooneList.size() );
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
#ifdef CONSIGNMENT_SALE
		if( pBalloone->s_bStoreSrech == false )
		{
#ifdef CONSIGNMENT_BALLOON_CASH
			if( pBalloone->s_bCashBalloon )
			{
				pBalloone->s_pBalloon1->Render( CsPoint(pBalloone->s_vPos.x, pBalloone->s_vPos.y - 10), pBalloone->s_fAlpha );
				pBalloone->s_pBalloon2->Render( CsPoint(pBalloone->s_vPos.x, pBalloone->s_vPos.y - 10) + CsPoint( 20 , 0 ), pBalloone->s_fAlpha );
				pBalloone->s_pBalloon3->Render( CsPoint(pBalloone->s_vPos.x, pBalloone->s_vPos.y - 10) + CsPoint( pBalloone->s_vSize.x - 20, 0 ), pBalloone->s_fAlpha );
			}
			else
			{
				pBalloone->s_pBalloon1->Render( pBalloone->s_vPos, pBalloone->s_fAlpha );
				pBalloone->s_pBalloon2->Render( pBalloone->s_vPos + CsPoint( 6, 0 ), pBalloone->s_fAlpha );
				pBalloone->s_pBalloon3->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ), pBalloone->s_fAlpha );
			}
#else
			pBalloone->s_pBalloon1->Render( pBalloone->s_vPos, pBalloone->s_fAlpha );
			pBalloone->s_pBalloon2->Render( pBalloone->s_vPos + CsPoint( 6, 0 ), pBalloone->s_fAlpha );
			pBalloone->s_pBalloon3->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ), pBalloone->s_fAlpha );
#endif
		}
		else
		{
			pBalloone->s_pBalloon5->Render( pBalloone->s_vPos, pBalloone->s_fAlpha );
			pBalloone->s_pBalloon6->Render( pBalloone->s_vPos + CsPoint( 6, 0 ), pBalloone->s_fAlpha );
			pBalloone->s_pBalloon7->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ), pBalloone->s_fAlpha );
		}
#else
		pBalloone->s_pBalloon1->Render( pBalloone->s_vPos, pBalloone->s_fAlpha );
		pBalloone->s_pBalloon2->Render( pBalloone->s_vPos + CsPoint( 6, 0 ), pBalloone->s_fAlpha );
		pBalloone->s_pBalloon3->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ), pBalloone->s_fAlpha );
#endif
		if( pBalloone->s_pBalloonTail )
			pBalloone->s_pBalloonTail->Render( pBalloone->s_vPos + CsPoint( CsFloat2Int(pBalloone->s_vSize.x*0.18f), pBalloone->s_vSize.y - 1 ) , pBalloone->s_fAlpha );
		if( pBalloone->s_pBalloonDiscript )
			pBalloone->s_pBalloonDiscript->Render( pBalloone->s_vPos + CsPoint( pBalloone->s_vSize.x - 6, 0 ) + CsPoint( -23, -15 ), pBalloone->s_fAlpha );

		if( pBalloone->s_pName )
		{
			pBalloone->s_pName->SetAlpha( pBalloone->s_fAlpha );
			pBalloone->s_pName->Render( pBalloone->s_vPos + CsPoint( 10, 5 ) );

			pBalloone->s_StringList.SetAlpha( pBalloone->s_fAlpha );
			pBalloone->s_StringList.Render( pBalloone->s_vPos +	CsPoint( 30, 30 ), IF_BALLOONE_STRING_DELTA_Y );
		}
		else
		{
			pBalloone->s_StringList.SetAlpha( pBalloone->s_fAlpha );
			pBalloone->s_StringList.Render( pBalloone->s_vPos +	CsPoint( 30, 10 ), IF_BALLOONE_STRING_DELTA_Y );
		}

		pBalloone->SetTargetAlpha( 1.0f - pBalloone->s_fAlpha );
	}
	m_vpSort.ClearElement();
	m_vpRender.ClearElement();
}

cTalkBalloon::sBALLOONE*
cTalkBalloon::GetData( int nTargetUID )
{
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	for( ; it!=itEnd; ++it )
	{
		if( nTargetUID == (*it)->s_nTargetUID )
			return (*it);
	}
	return NULL;
}

void cTalkBalloon::_Cal3D_to_2D()
{
	CsC_AvObject* pTarget;
	NiPoint3 pos, out;
	NiPoint3 vCameraPos = CAMERA_ST.GetWorldTranslate();
	sBALLOONE* pBalloone;
	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	std::list< sBALLOONE* >::iterator itEnd = m_BallooneList.end();
	for( ; it!=itEnd; ++it )
	{
		pBalloone = (*it);

		pTarget = NULL;
		switch( pBalloone->s_eObjType )
		{
		case sBALLOONE::eNOROBJ:
		case sBALLOONE::ePERSONSTORE_OBJ:
			pTarget = g_pMngCollector->GetObject( pBalloone->s_nTargetUID );
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
			continue;

		if( pTarget->IsLoad() == false )
			continue;
		if( pTarget->GetCsNode() == NULL )
			continue;
		
		// 케릭터가 사라지고 안보이는 거리라면
		if( pTarget->GetProp_Alpha()->IsHideDistOut() == true )
			continue;

		// 캐릭터가 비활성화 상태이면
		if( pTarget->IsEnableObject() == false )
			continue;

		pos = pTarget->GetPos();
		float fLength = ( vCameraPos - pos ).Length();
		float fScale = pow( fLength*fLength*15.0f, 1.0f/3.0f )*0.43f*CCharOption::GetNameScaleConstant();		
		pos.z += pTarget->GetToolHeight() + 100.0f*fScale;

		switch( pBalloone->s_eType )
		{
		case TYPE_1:
		case TYPE_2:
		case TYPE_3:		
		case TYPE_5:
			break;
		case TYPE_4:
#ifdef CONSIGNMENT_BALLOON_CASH
		case TYPE_6:
#endif
			pos.z -= 70.0f;
			break;
		default:
			assert_cs( false );
		}

		out = g_pEngine->Cal3D_To_2D( pos );

		switch( pBalloone->s_eType )
		{
		case TYPE_1:
		case TYPE_2:
		case TYPE_3:
			out.x += (int)( pBalloone->s_vSize.x*0.22f );
			break;
		case TYPE_4:
		case TYPE_5:
#ifdef CONSIGNMENT_BALLOON_CASH
		case TYPE_6:
#endif
			break;
		default:
			assert_cs( false );
		}		

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

int cTalkBalloon::FindBallooneName( std::wstring const& strFindName )
{
	int nFindCount = 0;
#ifdef CONSIGNMENT_SALE
	std::wstring serachName = strFindName;
	std::transform(serachName.begin(), serachName.end(), serachName.begin(), tolower);

	std::list< sBALLOONE* >::iterator it = m_BallooneList.begin();
	for( ; it != m_BallooneList.end(); ++it )
	{
		SAFE_POINTER_CON( *it );
		switch( (*it)->s_eType )
		{
		case cTalkBalloon::TYPE_4:
#ifdef CONSIGNMENT_BALLOON_CASH
		case cTalkBalloon::TYPE_6:
#endif
			break;
		default:
			continue;
		}

		if( (*it)->s_bStoreSrech ) // 재검색 시 기존 검색한 것은 다시 복원한다.
			(*it)->s_bStoreSrech = false;
		if( serachName.empty() )
			continue;

		if( (*it)->s_StringList.IsIncludeText( serachName, true ) )
		{
			(*it)->s_bStoreSrech = true;	// 찾은 상점 체크
			nFindCount++;
		}
	}
#endif

	return nFindCount;
}