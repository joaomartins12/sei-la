
#include "stdafx.h"
#include "HelpArrow.h"

cHelpArrow*	g_pHelpArrow = NULL;

void cHelpArrow::GlobalInit()
{
	assert_cs( g_pHelpArrow == NULL );
	g_pHelpArrow = NiNew cHelpArrow;
	g_pHelpArrow->Init();	
}

void cHelpArrow::GlobalShotDown()
{
	if( g_pHelpArrow )
	{
		g_pHelpArrow->Delete();
		SAFE_NIDELETE( g_pHelpArrow );
	}		
}

void cHelpArrow::Delete()
{
	_DeleteArrowPoint();
}

void cHelpArrow::Init()
{
	_CreateArrowPoint();
}

void cHelpArrow::Update()
{
	if( !m_bArrowPointRender )
		return;

	NiPoint3 Vvec;			
	NiPoint3 vPosTamer = g_pCharMng->GetTamerUser()->GetPos();
	vPosTamer.z = 0.0f;
	Vvec = vPosTamer - m_vTargetPos;	
	Vvec.z = 0;
	m_fCurRange = Vvec.Length();
	Vvec.Unitize();

	if( m_sPosFindInfo->s_dwMapID == g_pTerrainMng->GetBackupMapID() )
	{
		if( m_fCurRange < m_sPosFindInfo->s_nRad )
		{
			cPrintMsg::PrintMsg( 30112 );	
			g_pSoundMgr->PlaySystemSound( "System\\System_quest_01.wav" );
			ReleaseArrowPoint();
		}
		else
		{
			nsCSGBFUNC::SetMaterial_Alpha_Emittance( m_ArrowPoint.m_pNiNode, 0.6f );

			float fColor;
			fColor = CsMin( m_fCurRange, 30000.0f );			

			if( m_fCurRange < 10000.0f )
				nsCSGBFUNC::Set_Emittance( m_ArrowPoint.m_pNiNode, NiColor( (10000.0f - fColor) / 10000.0f, 1.0f, 0.0f ) );									
			else if( m_fCurRange < 20000.0f )
				nsCSGBFUNC::Set_Emittance( m_ArrowPoint.m_pNiNode, NiColor( 0.0f, 1.0f, abs(10000.0f - fColor) / 10000.0f ) );						
			else if( m_fCurRange < 30000.0f )
			{				
				if( (31960.0f - fColor) / 10000.0f > 1.0f)
					nsCSGBFUNC::Set_Emittance( m_ArrowPoint.m_pNiNode, NiColor( 0.0f, 1.0f, 1.0f ) );					
				else
					nsCSGBFUNC::Set_Emittance( m_ArrowPoint.m_pNiNode, NiColor( 0.0f, (31960.0f - fColor) / 10000.0f, 1.0f ) );					
			}
			else
				nsCSGBFUNC::Set_Emittance( m_ArrowPoint.m_pNiNode, NiColor( 0.0f, 50.0f / 255.0f, 1.0f ) );					
		}
	}
	else
	{	
		nsCSGBFUNC::SetMaterial_Alpha_Emittance( m_ArrowPoint.m_pNiNode, 0.6f );
		nsCSGBFUNC::Set_Emittance( m_ArrowPoint.m_pNiNode, NiColor( 1.0f, 0.0f, 0.0f ) );		
	}

	float fRot = NiACos( NiPoint3( 0.0f, 1.0f, 0.0f ).Dot( Vvec ) );
	if( NiPoint3( 0.0f, 1.0f, 0.0f ).Cross( Vvec ).z > 0 )
		fRot = -fRot;		

	while( fRot > NI_PI )		fRot -= NI_PI*2;
	while( fRot < -NI_PI )		fRot += NI_PI*2;

	NiMatrix3 mat;
	mat.MakeZRotation( fRot );	

#ifdef KEYBOARD_MOVE
	//디지몬 이동일 경우 위치찾기 화살표 디지몬에게 표시
	if( g_pResist->m_Global.s_bMoveDigimon )
		m_vArrowPoint = g_pCharMng->GetDigimonUser(0)->GetPos();
	else
		m_vArrowPoint = g_pCharMng->GetTamerUser()->GetPos();
#else
	m_vArrowPoint = g_pCharMng->GetTamerUser()->GetPos();
#endif
// 	m_vArrowPoint.z -= 100.0f;
// #ifdef VERSION_USA
	m_vArrowPoint.z += 30.0f;
//#endif

	m_ArrowPoint.m_pNiNode->SetRotate( mat );	
	m_ArrowPoint.m_pNiNode->SetTranslate( m_vArrowPoint );
	m_ArrowPoint.m_pNiNode->Update( (float)g_fAccumTime );		
}

void cHelpArrow::Render()
{
	if( !m_bArrowPointRender )
		return;	
	
	m_ArrowPoint.Render();		
	_RenderRange();	
}

void cHelpArrow::ReleaseArrowPoint()
{
	//화살표 초기화
	m_bArrowPointRender = false; 
	SAFE_NIDELETE( m_sPosFindInfo );
	SAFE_NIDELETE( m_pRange ); 		

	// 존맵 닫아준다.
	if( g_pGameIF )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) == true )
		{			
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );			
			g_pGameIF->GetZoneMap()->ResetRadioIdx();				
		}
	}	
}

bool cHelpArrow::SetArrowPoint( int nFileID, int nMapID, cQuestFindWay::eFindWayDestType nType, TCHAR const* szName )
{
	SAFE_NIDELETE( m_sPosFindInfo );

	m_sPosFindInfo = NiNew sPosFind;
	m_sPosFindInfo->s_nFileID = nFileID;
	m_sPosFindInfo->s_nFindWayType = nType;
	m_sPosFindInfo->s_dwMapID = nMapID;
	m_sPosFindInfo->s_nRad = 0;
	m_sPosFindInfo->s_vArrivePos = NiPoint3::ZERO;
	_tcscpy_s( m_sPosFindInfo->s_szName, NPC_NAME_LEN, szName );
		 
	m_bArrowPointRender = true;
	NiTimeController::StartAnimations( m_ArrowPoint.m_pNiNode, (float)g_fAccumTime );	

	switch( nType )
	{
	case cQuestFindWay::FindWay_Npc:
	case cQuestFindWay::FindWay_Monster:
	case cQuestFindWay::FindWay_Region:
	case cQuestFindWay::FindWay_Employment:
	case cQuestFindWay::FindWay_Map:
		FindRoot();
		break;

		break;

	default:
		assert_cs( false );
		break;
	}	

	if( m_bArrowPointRender )
	{
		g_pGameIF->GetMiniMap()->SetPosFind();	
		
		cPrintMsg::PrintMsg( 30114 );	
		cPrintMsg::PrintMsg( 30115 );
	}

	return m_bArrowPointRender;
}

//=====================================================================================
//
//	 내부 함수
//
//=====================================================================================

void cHelpArrow::_CreateArrowPoint()
{

#define ARROWPOINT_OBJECT_PATH		"Data\\EtcObject\\QuestArrow.nif"
	m_pRange = NULL;
	m_sPosFindInfo = NULL;
	m_bArrowPointRender = false;

	NiStream kStream;
	if( !kStream.Load( ARROWPOINT_OBJECT_PATH ) )
	{
		CsMessageBoxA( MB_OK, "%s Failed to load quest arrow.nif", ARROWPOINT_OBJECT_PATH );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_ArrowPoint.SetNiObject( pNode, CGeometry::Normal );		
}

void cHelpArrow::_DeleteArrowPoint()
{	
	SAFE_NIDELETE( m_pRange ); 
	SAFE_NIDELETE( m_sPosFindInfo );
	m_ArrowPoint.Delete();			
}


void cHelpArrow::_RenderRange()
{
	TCHAR szNum[ 64 ];
	_stprintf_s( szNum, 64, _T( "%.2fm" ), (m_fCurRange + m_fRange) / 100 );	

	if( m_pRange == NULL )
	{
		SAFE_NIDELETE( m_pRange );

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20 );
		ti.SetText( szNum );
		ti.s_bOutLine = false;
		m_pRange = NiNew cText3D;
		m_pRange->Init3D( &ti );
	}
	else
	{
		m_pRange->SetText( szNum );
	}	

	NiPoint3 pos = m_vArrowPoint;	
	float fLength = ( CAMERA_ST.GetWorldTranslate() - pos ).Length();
	pos.z += 325.0f + ( fLength / 30 );

	float fScale = fLength / 1500.0f * 0.8f;
	if( fScale < 1.0f )
		fScale = 1.0f;

	m_pRange->Render( pos, 0, 0, fScale );
}


/**< 위치 찾기 함수 */
void cHelpArrow::FindRoot()
{		
	SAFE_POINTER_RET( m_sPosFindInfo );

	if( g_pTerrainMng->GetBackupMapID() == m_sPosFindInfo->s_dwMapID ) // 목표 위치가 현재 위치와 같은 맵이라면
		SetArrivePosInfo();
		
	else // 목표 위치가 현재 위치의 맵과 다른 맵이라면 길찾기
	{
		m_fRange = FLT_MAX;

		DWORD dwCurrentMapID = g_pTerrainMng->GetBackupMapID();
		CsMapPortalGroup* pFTpotalG = nsCsMapTable::g_pMapPortalMng->GetGroup( dwCurrentMapID );
		SAFE_POINTER_RET( pFTpotalG );
		
		// 현재 위치한 맵의 포탈 리스트를 가져온다
		CsMapPortal::MAP* pPortalMap = pFTpotalG->GetPortalMap();
		SAFE_POINTER_RET( pPortalMap );

		NiPoint3 vPosTamer = g_pCharMng->GetTamerUser()->GetPos();
		vPosTamer.z = 0.0f;

		CsMapPortal::MAP_IT it = pPortalMap->begin();
		CsMapPortal::MAP_IT itEnd = pPortalMap->end();
		for( ; it != itEnd; it++ )
		{			
			SAFE_POINTER_CON( it->second );

			CsMapPortal::sINFO* pInfo = it->second->GetInfo();
			SAFE_POINTER_CON( pInfo );

			float fRange = 0.0f;			
			bool bFind = _FindNode( pInfo->s_dwDestMapID, dwCurrentMapID, fRange );
			if( bFind ) // 찾았다
			{
				assert_cs( fRange < FLT_MAX );
				NiPoint3 Vvec;		
				float fTotalRange;
				Vvec = vPosTamer - NiPoint3( (float)pInfo->s_nSrcTargetX, (float)pInfo->s_nSrcTargetY, 0.0f );	
				Vvec.z = 0;
				fTotalRange = Vvec.Length() + fRange;

				if( m_fRange > fTotalRange  )
				{
					m_fRange = fRange;
					m_vTargetPos = NiPoint3( (float)pInfo->s_nSrcTargetX, (float)pInfo->s_nSrcTargetY, 0.0f );
				}		
			}
			m_vRoot.Destroy();
		}		
	}
}

bool cHelpArrow::_FindNode( DWORD dwDestMapID/*목적지*/, DWORD dwSrcMapID/*현재 위치한 맵*/, float& fRange )
{	
	bool bFind = false;
	if( dwDestMapID < 1 )
		return bFind;	
	CsMapPortalGroup* pFTpotalG = nsCsMapTable::g_pMapPortalMng->GetGroup( dwDestMapID );
	SAFE_POINTER_RETVAL( pFTpotalG, bFind );

	// 목적지 맵에 존재하는 포탈 리스트
	CsMapPortal::MAP* pPortalMap = pFTpotalG->GetPortalMap();
	SAFE_POINTER_RETVAL( pPortalMap, bFind );

	// 목적지 맵에 포탈이 없다면 실패
	if( pPortalMap->empty() )
		return bFind;

	float fOpti = 0.0f;
	m_vRoot.PushBack( CsPoint( dwDestMapID, dwSrcMapID ) );	//x는 destmap y는 srcmap

	CsMapPortal::MAP_IT it = pPortalMap->begin();
	for( ; it != pPortalMap->end(); ++it )
	{	
		CsMapPortal::sINFO* pMapInfo = it->second->GetInfo();
		SAFE_POINTER_CON( pMapInfo );

		if( pMapInfo->s_dwDestMapID == pMapInfo->s_dwSrcMapID ) // 포탈의 목적지와 도착지가 같다면 다음 포탈
			continue;

		if( dwDestMapID == m_sPosFindInfo->s_dwMapID ) // 목적지가 경로 설정된 목적지와 같다면
		{					
			switch( m_sPosFindInfo->s_nFindWayType )
			{
			case cQuestFindWay::FindWay_Npc:
				{					
					if( dwSrcMapID == pMapInfo->s_dwDestMapID )
					{
						CsMapNpc* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_sPosFindInfo->s_nFileID );
						assert_cs( pFTMapNpc );

						NiPoint3 vPos( (float)pFTMapNpc->GetInfo()->s_nInitPosX, (float)pFTMapNpc->GetInfo()->s_nInitPosY, 0.0f );
						NiPoint3 vPos2( (float)pMapInfo->s_nSrcTargetX, (float)pMapInfo->s_nSrcTargetY, 0.0f );			
						m_sPosFindInfo->s_vArrivePos = vPos;

						NiPoint3 vLen = vPos2 - vPos;
						vLen.z = 0.0f;
						fRange += vLen.Length();
						return true;
					}
					else
						continue;
				}
				break;			
			case cQuestFindWay::FindWay_Monster:
				{
					if( dwSrcMapID == pMapInfo->s_dwDestMapID )
					{
						NiPoint3 vPos( (float)pMapInfo->s_nSrcTargetX, (float)pMapInfo->s_nSrcTargetY, 0.0f );
						_FindMonster( vPos, fRange );
						return true;
					}
					else
						continue;	
				}
				break;

			case cQuestFindWay::FindWay_Map:
				return true;

			case cQuestFindWay::FindWay_Region:
				{
					if( dwSrcMapID == pMapInfo->s_dwDestMapID )
					{
						CsMapRegionGroup* pRegionGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_sPosFindInfo->s_nFileID ); // 지역 테이블 읽어온다
						SAFE_POINTER_RETVAL( pRegionGroup, false );

						CsMapRegion::LIST* pRegionList = pRegionGroup->GetRegionList(); // 지역리스트를 가져온다
						SAFE_POINTER_RETVAL( pRegionList, false);

						CsMapRegion::LIST_IT iter	 =	pRegionList->begin();
						CsMapRegion::LIST_IT enditer =	pRegionList->end();

						NiPoint3 vPos;
						for( int i = 0; iter != enditer; ++iter )
						{
							if( m_sPosFindInfo->s_nFileID == i )
							{
								CsMapRegion::sINFO* pRegionInfo = (*iter)->GetInfo();
								SAFE_POINTER_RETVAL(pRegionInfo, false);

								vPos.x = (float)pRegionInfo->s_nCenterX;
								vPos.y = (float)pRegionInfo->s_nCenterY;
								vPos.z = 0.f;
								break;
							}
						}

						NiPoint3 vPos2( (float)pMapInfo->s_nSrcTargetX, (float)pMapInfo->s_nSrcTargetY, 0.0f );			
						m_sPosFindInfo->s_vArrivePos = vPos;

						NiPoint3 vLen = vPos2 - vPos;
						vLen.z = 0.0f;
						fRange += vLen.Length();
						return true;
					}
					else
						continue;
				}break;

			case cQuestFindWay::FindWay_Employment:
				{
					if( dwSrcMapID != pMapInfo->s_dwDestMapID )
						continue;

					NiPoint2 iCharpos = NiPoint2::ZERO;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_POS, &iCharpos);

					NiPoint3 vPos( iCharpos.x, iCharpos.y, 0.0f );
					NiPoint3 vPos2( (float)pMapInfo->s_nSrcTargetX, (float)pMapInfo->s_nSrcTargetY, 0.0f );
					m_sPosFindInfo->s_vArrivePos = vPos;
					NiPoint3 vLen = vPos2 - vPos;
					vLen.z = 0.0f;
					fRange += vLen.Length();					
				}
				return true;

			default:
				assert_cs(false);
			}	
		}
		else
		{	
			if( pMapInfo->s_dwDestMapID == dwSrcMapID )// 같은 맵으로 이동하는 포탈은 제외 시킴
				continue;

			for( int i = 0; i < m_vRoot.Size(); i++ )
			{	
				//한번 들갔다 온길이면 경로 탐색X
				if( m_vRoot[ i ].y == pMapInfo->s_dwDestMapID )
					return false;
			}

			float fCurRange = 0.0f;
			bool bFindNode = _FindNode( pMapInfo->s_dwDestMapID, dwDestMapID, fCurRange );
			if( bFindNode )
			{
				NiPoint3 vPos; 
				NiPoint3 vPos2;
				CsMapPortalGroup* pFTpotalG = nsCsMapTable::g_pMapPortalMng->GetGroup( dwDestMapID );
				CsMapPortal::MAP_IT itMap = pFTpotalG->GetPortalMap()->begin();
				CsMapPortal::MAP_IT itMapEnd = pFTpotalG->GetPortalMap()->end();
				for( ; itMap != itMapEnd; itMap++ )
				{
					if( itMap->second->GetInfo()->s_dwDestMapID == dwSrcMapID )
						vPos = NiPoint3( (float)itMap->second->GetInfo()->s_nSrcTargetX, (float)itMap->second->GetInfo()->s_nSrcTargetY, 0.0f );
					if( itMap->second->GetInfo()->s_dwDestMapID == pMapInfo->s_dwDestMapID )
						vPos2 = NiPoint3( (float)itMap->second->GetInfo()->s_nSrcTargetX, (float)itMap->second->GetInfo()->s_nSrcTargetY, 0.0f );
				}

				NiPoint3 vLen = vPos2 - vPos;
				vLen.z = 0.0f;

				fCurRange += vLen.Length();
				if( fOpti > fCurRange || fOpti == 0.0f )
				{
					fRange -= fOpti;
					fOpti = fCurRange;
					fRange += fOpti;
				}							
				bFind = true;
			}
		}		
	}				
	return bFind;
}

void cHelpArrow::_FindMonster( NiPoint3 vPos, float& fRange )
{	
	SAFE_POINTER_RET( m_sPosFindInfo );
	SAFE_POINTER_RET( nsCsMapTable::g_pMapMonsterMng );

	CsMapMonsterMng::sMapMonster* pMapMonster = nsCsMapTable::g_pMapMonsterMng->GetMonster( m_sPosFindInfo->s_nFileID );
	SAFE_POINTER_RET( pMapMonster );
	std::map< DWORD, CsMapMonster::LIST >* pList = pMapMonster->GetMonsterList();
	SAFE_POINTER_RET( pList );

	std::map< DWORD, CsMapMonster::LIST >::iterator MonList = pList->find( m_sPosFindInfo->s_dwMapID );
	if( MonList == pList->end() )
		return;

	CsMapMonster::LIST_IT it = MonList->second.begin();
	CsMapMonster::LIST_IT itEnd = MonList->second.end();
	
	float fShortRange = 2147483647.0f;
	for( ; it != itEnd; it++ )
	{		
		CsMapMonster::sINFO* pInfo = (*it)->GetInfo();
		SAFE_POINTER_CON( pInfo );

		NiPoint3 vPos2( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY, 0.0f );
		NiPoint3 vLen = vPos2 - vPos;
		vLen.z = 0.0f;

		if( vLen.Length() < fShortRange )
		{
			fShortRange = vLen.Length();
			m_sPosFindInfo->s_vArrivePos = vPos2;
			m_sPosFindInfo->s_nRad = pInfo->s_nRadius;
		}
	}
	fRange += fShortRange;	
}

void cHelpArrow::SetArrivePosInfo()
{
	switch( m_sPosFindInfo->s_nFindWayType )
	{
	case cQuestFindWay::FindWay_Npc:
		{
			CsMapNpc* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_sPosFindInfo->s_nFileID );
			SAFE_POINTER_RET( pFTMapNpc );

			CsMapNpc::sINFO* pInfo = pFTMapNpc->GetInfo();
			SAFE_POINTER_RET( pInfo );

			m_vTargetPos = NiPoint3( (float)pInfo->s_nInitPosX, (float)pInfo->s_nInitPosY, 0.0f );
			m_sPosFindInfo->s_vArrivePos = NiPoint3( (float)pInfo->s_nInitPosX, (float)pInfo->s_nInitPosY, 0.0f );
			m_sPosFindInfo->s_nRad = 300;
		}
		break;		
	case cQuestFindWay::FindWay_Monster:
		{
			CsMapMonsterMng::sMapMonster* pMapMonster = nsCsMapTable::g_pMapMonsterMng->GetMonster( m_sPosFindInfo->s_nFileID );
			SAFE_POINTER_RET( pMapMonster );

			std::map< DWORD, CsMapMonster::LIST >* pMonsterList = pMapMonster->GetMonsterList();
			SAFE_POINTER_RET( pMonsterList );

			std::map< DWORD, CsMapMonster::LIST >::iterator it = pMonsterList->find( m_sPosFindInfo->s_dwMapID );
			if( it == pMonsterList->end() )
				return;

			float fShortRange = FLT_MAX;
			NiPoint3 vPosTamer = g_pCharMng->GetTamerUser()->GetPos();
			vPosTamer.z = 0.0f;		

			CsMapMonster::LIST_IT subit = it->second.begin();
			for( ; subit != it->second.end(); ++subit )
			{
				CsMapMonster::sINFO * pInfo = (*subit)->GetInfo();
				SAFE_POINTER_CON( pInfo );

				NiPoint3 vPos( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY, 0.0f );					 
				NiPoint3 vLen = vPos - vPosTamer;
				vLen.z = 0.0f;

				if( vLen.Length() < fShortRange )
				{
					fShortRange = vLen.Length();
					m_vTargetPos = vPos;
					m_sPosFindInfo->s_vArrivePos = vPos;
					m_sPosFindInfo->s_nRad = pInfo->s_nRadius;
				}
			}	

		}
		break;
	case cQuestFindWay::FindWay_Map:
		{
			//맵은 거리가 없으므로 예외적 여기서 release
			cPrintMsg::PrintMsg( 30112 );	
			if( g_pSoundMgr )
				g_pSoundMgr->PlaySystemSound( "System\\System_quest_01.wav" );
			ReleaseArrowPoint();
		}
		break;
	case cQuestFindWay::FindWay_Region:
		{
			CsMapNpc* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_sPosFindInfo->s_nFileID );
			SAFE_POINTER_RET( pFTMapNpc );

			CsMapNpc::sINFO* pInfo = pFTMapNpc->GetInfo();
			SAFE_POINTER_RET( pInfo );

			m_vTargetPos = NiPoint3( (float)pInfo->s_nInitPosX, (float)pInfo->s_nInitPosY, 0.0f );
			m_sPosFindInfo->s_vArrivePos = NiPoint3( (float)pInfo->s_nInitPosX, (float)pInfo->s_nInitPosY, 0.0f );
			m_sPosFindInfo->s_nRad = 500;
		}break;

	case cQuestFindWay::FindWay_Employment:
		{
			if( g_pDataMng )
			{
				NiPoint2 v2 = NiPoint2::ZERO;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_POS, &v2);

				m_vTargetPos = NiPoint3( v2.x, v2.y, 0 );
				m_sPosFindInfo->s_vArrivePos = m_vTargetPos;
				m_sPosFindInfo->s_nRad = 500;
			}
		}
		break;

	default:
		assert_cs(false);
	}		
	m_fRange = 0.0f;
}
