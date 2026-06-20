#include "stdafx.h"
// #ifdef DAMAGE_METER
// #include "Data_Damagemeter.h"
// 
// void cData_DamageMeter::Delete()
// {
// 	if( m_bIsData == false )
// 		return;
// 
// 	if( m_listDmgInfo.empty() )
// 		return;
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 	for( ; it != itEnd ; it++ )
// 	{
//  		(*it)->Delete();
// 	}
// 	
// 	m_listDmgInfo.clear();
// }
// 
// void cData_DamageMeter::Init()
// {
// 	if( m_bIsData == false )
// 		return;
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 	for( ; it != itEnd ; it++ )
// 	{
// 		(*it)->Delete();
// 	}
// 	m_listDmgInfo.clear();
// }
// 
// cData_DamageMeter::sDamageInfo* cData_DamageMeter::GetDmgInfo( u2 nUID )
// {
// 	if( m_listDmgInfo.empty() )
// 		return NULL;
// 
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 	for( ; it != itEnd ; it++ )
// 	{
// 		if( (*it)->s_nDigimonUID == nUID )
// 			return (*it);
// 	}
// 
// 	return NULL;
// }
// 
// cData_DamageMeter::sDamageInfo* cData_DamageMeter::GetDmgInfo( TCHAR* szName )
// {
// 	if( m_listDmgInfo.empty() )
// 		return NULL;
// 
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 
// 	for( ; it != itEnd ; it++ )
// 	{
// 		if( _tcscmp((*it)->s_szDigimonName, szName ) == 0 )
// 			return (*it);
// 	}
// 	return NULL;
// }
// 
// bool cData_DamageMeter::DeleteDmgInfo( u2 nUID )
// {
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 	for( ; it != itEnd ; it++ )
// 	{
// 		if( (*it)->s_nDigimonUID == nUID )
// 		{
// 			m_listDmgInfo.erase( it );
// 			return true;
// 		}
// 	}
// 
// 	return false;
// }
// 
// bool cData_DamageMeter::DeleteDmgInfo( TCHAR* szName )
// {
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 	for( ; it != itEnd ; it++ )
// 	{
// 		if( _tcscmp((*it)->s_szDigimonName, szName ) == 0 )
// 		{
// 			m_listDmgInfo.erase( it );
// 			return true;
// 		}
// 	}
// 
// 	return false;
// }
// 
// void cData_DamageMeter::OrderByType( eORDERTYPE eOrderType )
// {
// 	if( eOrderType <= ORDERTYPE_NONE || eOrderType >= ORDERTYPE_MAX )
// 		return;
// 	LIST_IT it = m_listDmgInfo.begin();
// 	LIST_IT itEnd = m_listDmgInfo.end();
// 
// 	int nSize = m_listDmgInfo.size();
// 	int nIterIdx = 0;
// 
// 	LIST_IT itComp;
// 
// 	for( ; it != itEnd ; it++ )
// 	{
// 		nIterIdx++;
// 
// 		itComp = it;
// 		itComp++;
// 		for( ; itComp != itEnd ; itComp++ )
// 		{
// 			bool bComp = false;
// 			switch( eOrderType )
// 			{
// 			case DAMAGE_DONE:		if( (*it)->s_nDamage_Done < (*itComp)->s_nDamage_Done )		bComp = true;			break;
// 			case DAMAGE_TAKEN:		if( (*it)->s_nDamage_Taken < (*itComp)->s_nDamage_Taken )	bComp = true;			break;
// 			case DPS:
// 				{
// 					u4 nCurTime = ( (*it)->s_nEndTime != 0 ) ? (*it)->s_nEndTime : _TIME_TS;
// 					u4 nDPS = 0;
// 					if( nCurTime - (*it)->s_nStartTime > 0 )
// 						nDPS = (*it)->s_nDamage_Done / ( nCurTime - (*it)->s_nStartTime );
// 
// 					
// 					nCurTime = ( (*itComp)->s_nEndTime != 0 ) ? (*itComp)->s_nEndTime : _TIME_TS;
// 					u4 nCompDPS = 0;
// 					if( nCurTime - (*itComp)->s_nStartTime > 0 )
// 						nCompDPS = (*itComp)->s_nDamage_Done / ( nCurTime - (*itComp)->s_nStartTime );
// 
// 					if( nDPS < nCompDPS )
// 						bComp = true;
// 				}
// 				break;
// 			}
// 
// 			if( bComp )
// 			{
// 				sDamageInfo* pInfo = csnew sDamageInfo;
// 				pInfo = (*itComp);
// 				(*itComp) = (*it);
// 				(*it) = pInfo;
// 			}
// 		}
// 	}
// }
// 
// void cData_DamageMeter::_SetDamageInfo( u2 nHitterUID, u2 nTargetUID, int nDamage, u4 nCurTime )
// {
// 	//=============================================================================
// 	//	입힌 데미지
// 	//=============================================================================
// 	CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
// 	if( pHitter == NULL )
// 	{
// 		ST_CHAT_PROTOCOL	CProtocol;
// 		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
// 		CProtocol.m_wStr = GetVAString( _T( "SetDamageInfo :: 싱크 내 Hitter 오브젝트 없음 ( UID : %d )" ), nHitterUID );
// 		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
// 		return;
// 	}
// 
// 	bool bFlag = true;
// 	TCHAR szDigimonName[  Language::pLength::name + 1  ];
// 	memset( szDigimonName, 0,  Language::pLength::name + 1  );
// 	switch( pHitter->GetLeafRTTI() )
// 	{
// 	case RTTI_TAMER:
// 	case RTTI_TAMER_USER:
// 		{
// 			// 테이머스킬( 강타 ) 로 인한 데미지로 들어올 수 있음
// 			CDigimon* pDigimon = g_pCharMng->GetDigimon( ((CTamer*)pHitter)->GetDigimonLink() );
// 			if( pDigimon == NULL )
// 				return;
// 			nHitterUID = pDigimon->GetUniqID();
// 			_tcscpy_s( szDigimonName, pDigimon->GetName() );
// 		}
// 		break;
// 	case RTTI_DIGIMON:
// 		_tcscpy_s( szDigimonName, ((CDigimon*)pHitter)->GetName() );
// 		break;
// 	case RTTI_DIGIMON_USER:
// 		_tcscpy_s( szDigimonName, g_pCharMng->GetDigimonUser(0)->GetName() );
// 		break;
// 	default:
// 		bFlag = false;
// 		break;
// 	}
// 
// 	int nMyUID = g_pCharMng->GetDigimonUser(0)->GetUniqID();
// 
// 	if( bFlag )
// 	{
// 		std::pair<int, bool> isMember;
// 		isMember.first = nHitterUID;
// 		GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_DIGIMONUIDX, &isMember );
// 
// 		if(isMember.second || nMyUID == nHitterUID )
// 		{
// 			// 기존에 미터기에 존재하는 놈인지 확인
// 			sDamageInfo* pInfo = GetDmgInfo( szDigimonName );
// 
// 			if( pInfo == NULL )
// 			{
// 				// 첫 데미지니까 그냥 저장
// 				pInfo = csnew sDamageInfo;
// 				pInfo->Init();
// 				pInfo->s_nDigimonUID = nHitterUID;
// 				_tcscpy_s( pInfo->s_szDigimonName, szDigimonName );
// 				pInfo->s_nDamage_Done = nDamage;
// 				pInfo->s_nStartTime = nCurTime;
// 			}
// 			else
// 			{
// 				// 존재하는 놈의 데미지
// 				if( pInfo->s_bBattle == false )
// 				{
// 					// 전투가 풀렸던 상태라면 초기화 해줘야함
// 					if( _tcscmp( pInfo->s_szDigimonName, g_pCharMng->GetDigimonUser(0)->GetName() ) == 0 )
// 					{
// 						// 자신의 전투가 풀렸던 거라면 미터기 전체 초기화
// 						Delete();
// 						pInfo->Init();
// 					}
// 					else
// 					{
// 						// 자신 이외의 파티원이라면 그 파티원만 전투상태 변경, 초기화는 안해줌
// 						pInfo->s_bBattle = true;
// 					}
// 
// 					pInfo->s_nDigimonUID = nHitterUID;
// 					_tcscpy_s( pInfo->s_szDigimonName, szDigimonName );
// 				}
// 
// 				if( pInfo->s_nStartTime == 0 )
// 					pInfo->s_nStartTime = nCurTime;
// 				pInfo->s_nDamage_Done += nDamage;
// 				DeleteDmgInfo( szDigimonName );
// 			}
// 
// 			AddDmgInfo( pInfo );
// 		}
// 	}
// 	//=============================================================================
// 	//	받은 데미지 ( 입힌 데미지와 비슷하게 처리, 타겟과 히터 바뀜 )
// 	//=============================================================================
// 	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
// 	if( pTarget == NULL )
// 	{
// 		ST_CHAT_PROTOCOL	CProtocol;
// 		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
// 		CProtocol.m_wStr = GetVAString( _T( "SetDamageInfo :: 싱크 내 Target 오브젝트 없음 ( UID : %d )" ), nTargetUID );
// 		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
// 		return;
// 	}
// 	memset( szDigimonName, 0,  Language::pLength::name + 1  );
// 	switch( pTarget->GetLeafRTTI() )
// 	{
// 	case RTTI_DIGIMON:
// 		_tcscpy_s( szDigimonName, ((CDigimon*)pTarget)->GetName() );
// 		break;
// 	case RTTI_DIGIMON_USER:
// 		_tcscpy_s( szDigimonName, g_pCharMng->GetDigimonUser(0)->GetName() );
// 
// 		break;
// 	default:
// 		return;	// 디지몬이 받는 데미지이므로 디지몬 이외에는 처리 안함
// 	}
// 
// 	std::pair<int, bool> isMember;
// 	isMember.first = nTargetUID;
// 	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_DIGIMONUIDX, &isMember );
// 	if( isMember.second	|| nMyUID == nTargetUID )
// 	{
// 		sDamageInfo* pInfo = GetDmgInfo( szDigimonName );
// 
// 		if( pInfo == NULL )
// 		{
// 			pInfo = csnew sDamageInfo;
// 			pInfo->Init();
// 			pInfo->s_nDigimonUID = nTargetUID;
// 			_tcscpy_s( pInfo->s_szDigimonName, szDigimonName );
// 			pInfo->s_nDamage_Taken = nDamage;
// 		}
// 		else
// 		{
// 			if( pInfo->s_bBattle == false )
// 			{
// 				if( _tcscmp( pInfo->s_szDigimonName, g_pCharMng->GetDigimonUser(0)->GetName() ) == 0 )
// 				{
// 					Delete();
// 					pInfo->Init();
// 				}
// 				else
// 				{
// 					pInfo->Init();
// 					pInfo->s_bBattle = true;
// 				}
// 				pInfo->s_nDigimonUID = nTargetUID;
// 
// 				_tcscpy_s( pInfo->s_szDigimonName, szDigimonName );
// 			}
// 
// 			pInfo->s_nDamage_Taken += nDamage;
// 			DeleteDmgInfo( szDigimonName );
// 		}
// 
// 		AddDmgInfo( pInfo );
// 	}
// 
// 	// 미터기 창 열려 있다면 
// 	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DAMAGE_METER ) )
// 	{
// 		OrderByType( (cData_DamageMeter::eORDERTYPE)(g_pGameIF->GetDamageMeter()->GetDispType()) );
// // 		switch( g_pGameIF->GetDamageMeter()->GetDispType() )
// // 		{
// // 		case cDamageMeter::DAMAGE_DONE:		OrderByType( cData_DamageMeter::DAMAGE_DONE );		break;
// // 		case cDamageMeter::DPS:				OrderByType( cData_DamageMeter::DPS );				break;
// // 		case cDamageMeter::DAMAGE_TAKEN:	OrderByType( cData_DamageMeter::DAMAGE_TAKEN );		break;
// // 		}
// 	}
// }
// 
// #endif // DAMAGE_METER