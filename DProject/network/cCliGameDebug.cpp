


#include "stdafx.h"
#include "cCliGame.h"


void cCliGame::RecvLoseTarget(void)
{
//	assert_csm( false, _T( "음.. 몰까..?" ) );

#ifndef _DEBUG
//	return;
#endif // _DEBUG


	u4 nUID = 0;
	pop(nUID);

	//wchar msg[222];
	//swprintf(msg, "LostTarget %d", nUID);
	//
	//cCharBase* pCharBase = NULL;

	//if(g_CharMng.IsChar(nUID) == false )
	//	return;
	//
	//pCharBase = g_CharMng.GetChar(nUID);
	//assert_cs(pCharBase);

	//pCharBase->m_AttTargetPickData.Reset();	


	//// 다시 공격 가능하면 공격
	//for( int i=0; i<DIGIMON_MAX_COUNT; ++i)
	//{	
	//	if( g_PlayerUser.m_pDigimon[ i ] )
	//	{
	//		if( g_PlayerUser.m_pDigimon[ i ]->m_nUID == pCharBase->m_nUID )
	//		{
	//			if( g_PlayerUser.m_pDigimon[ i ]->GetOrderType() == ORDER::ATTACK )
	//			{
	//				if( g_PlayerUser.m_pDigimon[ i ]->m_pAttTargetObject && g_PlayerUser.m_pDigimon[ i ]->m_pAttTargetObject->m_eServerState != STATE::DEAD )
	//				{
	//					/*sPickData AttTargetPickData;
	//					AttTargetPickData.s_nUID = g_PlayerUser.m_pDigimon[ i ]->m_pAttTargetObject->m_nUID;
	//					AttTargetPickData.s_eRtti = g_PlayerUser.m_pDigimon[ i ]->m_pAttTargetObject->GetRTTI();
	//					g_PlayerUser.m_pDigimon[ i ]->SetAttackPos(&AttTargetPickData, g_PlayerUser.m_pDigimon[ i ]->m_pAttTargetObject->m_vPos, g_PlayerUser.m_pDigimon[ i ]->m_fAttRange, ORDER::ATTACK_MOVE );*/
	//				}
	//			}
	//		}
	//	}
	//}

	//g_pSystemMsg->AddSystemMsg(msg);
}

void cCliGame::RecvSyncInfo(void)
{
#ifdef _GIVE
	return;
#endif

#ifdef DEBUG_SYNC_CHAR
	u4 nUID = 0;
	u4 nType = 0;
	nSync::Pos DPos;
	n4 nDir = 0;
	pop( nUID );
	pop( nType );
	pop( DPos );
	pop( nDir );

	cType DType( nUID );
	DType.SetType( nType );

	u4 nIDX = DType.m_nIDX;
	switch( DType.m_nClass )
	{
	case nClass::Digimon:
		{
			nIDX += 2000;
		}		
		break;
	}

	float fDDir = nDir*0.001f;	
	CsC_AvObject* pTemp = g_pCharMng->GetDebugChar( nIDX );
	if( pTemp == NULL )
	{
		sCREATEINFO prop;
		prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
		prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
		prop.s_eInsertGeomType = CGeometry::CharOutLine_Gray;

		switch( DType.m_nClass )
		{
		case nClass::Monster:
			{
				if( nsCsFileTable::g_pMonsterMng->IsMonster( DType.m_nType ) == false )
					return;

				pTemp = g_pCharMng->AddDebugMonster( nIDX, &prop, DType.m_nType, NiPoint3::ZERO, 0, NULL );
			}			
			break;
		case nClass::Digimon:
			{
				if( nsCsFileTable::g_pDigimonMng->IsDigimon( DType.m_nType ) == false )
					return;

				pTemp = g_pCharMng->AddDebugDigimon( nIDX, &prop, DType.m_nType, NiPoint3::ZERO, 0, NULL );
			}
			break;
		default:
			return;			
		}		
	}

	pTemp->SetPos( NiPoint2( (float)DPos.m_nX, (float)DPos.m_nY ) );
	pTemp->SetRotation( fDDir, true );
#endif
}


void cCliGame::RecvInsufficientRange(void)
{
	int nDistance;
	int nRange;

	pop(nDistance);
	pop(nRange);

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = UISTRING_TEXT( "CHAT_RANGE_DISTANCE" );
	DmCS::StringFn::Replace( CProtocol.m_wStr, L"#Range#", nRange );
	DmCS::StringFn::Replace( CProtocol.m_wStr, L"#Distance#", nDistance );
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}




