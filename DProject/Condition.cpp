
#include "stdafx.h"
#include "Condition.h"


cCondition::cCondition()
{
	m_nCondition = m_nConditionBackup = 0;	
	m_pParent = NULL;	
}

void cCondition::Delete()
{
	m_pParent = NULL;
	m_nCondition = m_nConditionBackup = 0;	
}

void cCondition::Init( CsC_AvObject* pParent )
{
	m_pParent = pParent;
	assert_cs( m_nCondition == 0 );
	assert_cs( m_nConditionBackup == 0 );	
}

void cCondition::SetCondition( uint nCondition )
{
	assert_cs( m_pParent != NULL );
	if( m_nCondition == nCondition )
		return;

	m_nConditionBackup = m_nCondition;
	m_nCondition = nCondition;

	if( m_pParent->IsServerDie() == true )
		return;

	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Scanner0 ) == false )
	{
		_ApplyCondition( nCondition, nSync::Scanner0 );
	}
	// 등록 해제만 변경되는 경우
	else if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Scanner1 ) == false )
	{
		_ApplyCondition( nCondition, nSync::Scanner1 );
	}

	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Die ) == false )
	{		
		_ApplyCondition( nCondition, nSync::Die );
	}
	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Battle ) == false )
	{		
		_ApplyCondition( nCondition, nSync::Battle );
	}
	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Fatigue ) == false )
	{		
		_ApplyCondition( nCondition, nSync::Fatigue );
	}	
	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Ride ) == false )
	{
		_ApplyCondition( nCondition, nSync::Ride );
	}
	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Away ) == false )
	{
		_ApplyCondition( nCondition, nSync::Away );
	}
	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::PCBang ) == false )
	{
		_ApplyCondition( nCondition, nSync::PCBang );
	}

	// ?상태가 변하면 레이디 ?상태는 체크하지 않겠다
	if( IsSameCondition( nCondition, m_nConditionBackup, nSync::Shop ) == false )
	{
		_ApplyCondition( nCondition, nSync::Shop );
	}
	else if( IsSameCondition( nCondition, m_nConditionBackup, nSync::ShopReady ) == false )
	{
		_ApplyCondition( nCondition, nSync::ShopReady );
	}
	

	m_nConditionBackup = m_nCondition;
	_ApplyEtc();	
}

void cCondition::_ApplyCondition( uint nCondition, nSync::eCondition cd )
{
	// 존재 한다면 빼죽고 존재 안한다면 추가해주자
	if( IsCondition( m_nConditionBackup, cd ) == true )
	{
		_RemoveCondition( nCondition, cd );
	}
	else
	{
		_AddCondition( nCondition, cd );
	}
}

void cCondition::_AddCondition( uint nCondition, nSync::eCondition cd )
{
	switch( cd )
	{
	case nSync::Scanner0:
		{
			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;

			CsC_PartObject::sCHANGE_PART_INFO cp;
			// 일반판 1, 한정판 2
			if( nCondition & nSync::Scanner2 )
			{
				cp.s_nFileTableID = 2;
				if( nCondition & nSync::Scanner1 )
					pTamer->GetBuff()->SetBuff( nsBuff::BK_BARCODE_BLACK, 0, UINT_MAX, 0 );
			}
			else
			{
				cp.s_nFileTableID = 1;
				if( nCondition & nSync::Scanner1 )
					pTamer->GetBuff()->SetBuff( nsBuff::BK_BARCODE_WHITE, 0, UINT_MAX, 0 );
			}

			cp.s_nPartIndex = nsCONDITION_PART::Barcode;
			pTamer->ChangePart( &cp );
		}
		break;
	case nSync::Scanner1:
		{
			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;

			// 일반판 1, 한정판 2
			if( nCondition & nSync::Scanner2 )
				pTamer->GetBuff()->SetBuff( nsBuff::BK_BARCODE_BLACK, 0, UINT_MAX, 0 );
			else
				pTamer->GetBuff()->SetBuff( nsBuff::BK_BARCODE_WHITE, 0, UINT_MAX, 0 );
		}
		break;
	case nSync::Die:
		m_pParent->SetDie();
		break;
	case nSync::Battle:
		m_pParent->SetBattle();
		break;
	case nSync::Fatigue:
		m_pParent->GetBaseStat()->EnableFatigue( true );
		m_pParent->SetDefault_IdleAnimation();
		break;
	case nSync::Ride:
		{			
			if( m_pParent->IsKindOf( RTTI_TAMER ) )
			{
				CTamer* pTamer = (CTamer*)m_pParent;
				CDigimon* pDigimon = g_pCharMng->GetDigimon( pTamer->GetDigimonLink() );
				if( pDigimon )
				{
					pDigimon->SetRide();
					pTamer->SetRide();					
				}
			}
			else
			{
				assert_cs( m_pParent->IsKindOf( RTTI_DIGIMON ) );
				CDigimon* pDigimon = (CDigimon*)m_pParent;				
				CTamer* pTamer = g_pCharMng->GetTamer( pDigimon->GetTamerLink() );
				if( pTamer )
				{
					pDigimon->SetRide();
					pTamer->SetRide();					
				}
			}
		}
		break;
	case nSync::Away:
		{
			if( m_pParent->IsKindOf( RTTI_TAMER ) )
			{
				CTamer* pTamer = (CTamer*)m_pParent;				
				pTamer->GetCharOption()->SetAway( true );
			}
			else
			{
				assert_cs( false );
			}			
		}		
		break;
	case nSync::ShopReady:
		{
			m_pParent->SetPause( CsC_AvObject::PAUSE_PATH, true );
			m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
			m_pParent->SetDefault_IdleAnimation();
			m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );

			ContentsStream kTmp;
			uint checkid = m_pParent->GetUniqID();
			std::wstring wsShopTitle = UISTRING_TEXT( "CONDITION_SYNC_PREPARING_SHOP" );
			int nType = cTalkBalloon::TYPE_5;
			kTmp << checkid << wsShopTitle << nType;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_BALLOON_TITLE, &kTmp );

			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;
			CDigimon* pDigimon = g_pCharMng->GetDigimon( pTamer->GetDigimonLink() );
			if( pDigimon )
			{
				pDigimon->SetPause( CsC_AvObject::PAUSE_PATH, true );
				pDigimon->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
				pDigimon->SetDefault_IdleAnimation();
				pDigimon->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );
				pDigimon->SetEnableObject( false );
			}
		}
		break;
	case nSync::Shop:
		{
			m_pParent->SetPause( CsC_AvObject::PAUSE_PATH, true );
			m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
			m_pParent->SetAnimation( ANI::IDLE_SIT_DOWN );
			m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );

//			std::wstring storeName;
//			GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_GET_STORENAME, &storeName);
//
//#ifdef CONSIGNMENT_BALLOON_CASH
//			CsC_AvObject* pTarget = g_pMngCollector->GetObject( m_pParent->GetUniqID() );
//			TCHAR* szFileName = nsCsFileTable::g_pBaseMng->IsBallonNewType( pTarget->GetFTID() );
//			if( szFileName != NULL )
//			{
//				g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_6, m_pParent->GetUniqID(), storeName.c_str(), true,
//					cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
//			}
//			else
//			{
//				g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, m_pParent->GetUniqID(), storeName.c_str(), true,
//					cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
//			}
//#else
//			g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, m_pParent->GetUniqID(), storeName.c_str(), true,
//										cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
//#endif

			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;
			CDigimon* pDigimon = g_pCharMng->GetDigimon( pTamer->GetDigimonLink() );
			if( pDigimon )
			{
				pDigimon->SetPause( CsC_AvObject::PAUSE_PATH, true );
				pDigimon->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
				pDigimon->SetDefault_IdleAnimation();
				pDigimon->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );
				pDigimon->SetEnableObject( false );
			}
		}
		break;	
	case nSync::PCBang:
		{
			switch( m_pParent->GetLeafRTTI() )
			{
			case RTTI_TAMER:
			case RTTI_TAMER_USER:
				{
					CTamer* pTamer = (CTamer*)m_pParent;					
					if( pTamer )
					{
						pTamer->GetCharOption()->SetTitleMark();
					}
				}
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

void cCondition::_RemoveCondition( uint nCondition, nSync::eCondition cd )
{
	switch( cd )
	{
	case nSync::Scanner0:
		{
			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;
			pTamer->DeletePart( nsCONDITION_PART::Barcode );

			pTamer->GetBuff()->ReleaseBuff( nsBuff::BK_BARCODE_WHITE );
			pTamer->GetBuff()->ReleaseBuff( nsBuff::BK_BARCODE_BLACK );
		}
		break;
	case nSync::Scanner1:
		{
			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;
			pTamer->GetBuff()->ReleaseBuff( nsBuff::BK_BARCODE_WHITE );
			pTamer->GetBuff()->ReleaseBuff( nsBuff::BK_BARCODE_BLACK );
		}
		break;
	case nSync::Battle:
		{
			m_pParent->SetBattleEnd();
			if( (m_pParent->GetLeafRTTI() == RTTI_MONSTER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER)
				&& nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
			{
				//징표 삭제.. 서버에서 ID 주는게 아니라서 다 찾아봐야됨;
				std::map<DWORD, CDigimon*>* pVector = g_pCharMng->GetDigimonMap();
				std::map<DWORD, CDigimon*>::iterator it = pVector->begin();
				for( ; it != pVector->end(); ++it )
				{
					CDigimon* pDigimon = it->second;
					SAFE_POINTER_CON( pDigimon );
					CsC_EffectProp::SKILL_EFF_MAP* pMap = pDigimon->GetProp_Effect()->_GetLoopSkillEffectMap();
					CsC_EffectProp::SKILL_EFF_MAP_IT ItEff = pMap->begin();
					CsC_EffectProp::SKILL_EFF_MAP_IT ItEffEnd = pMap->end();

					while( ItEff != ItEffEnd )
					{
						CsC_AvObject* pBuffEffect = ItEff->second;
						pMap->erase(ItEff);
						pDigimon->GetProp_Effect()->DeleteEffect(pBuffEffect);

						ItEff = pMap->begin();
						ItEffEnd = pMap->end();
					}
				}
				// 전투 시 닫은 오브젝트 체크
				//g_pDataMng->GetMapObject()->CheckActorFlag();
			}
		}
		break;
	case nSync::Fatigue:
		{
			m_pParent->GetBaseStat()->EnableFatigue( false );
			m_pParent->SetDefault_IdleAnimation();
		}
		break;

	case nSync::Ride:
		{
			if( m_pParent->IsKindOf( RTTI_TAMER ) )
			{
				CTamer* pTamer = (CTamer*)m_pParent;
				CDigimon* pDigimon = g_pCharMng->GetDigimon( pTamer->GetDigimonLink() );
				assert_cs( pDigimon );
				if( pDigimon )
				{
					pDigimon->SetRideEnd();
					pTamer->SetRideEnd();
				}
			}
			else
			{
				assert_cs( m_pParent->IsKindOf( RTTI_DIGIMON ) );
				CDigimon* pDigimon = (CDigimon*)m_pParent;				
				CTamer* pTamer = g_pCharMng->GetTamer( pDigimon->GetTamerLink() );
				assert_cs( pTamer );
				if( pTamer )
				{
					pDigimon->SetRideEnd();
					pTamer->SetRideEnd();					
				}
			}
		}
		break;
	case nSync::Away:
		{
			if( m_pParent->IsKindOf( RTTI_TAMER ) )
			{
				CTamer* pTamer = (CTamer*)m_pParent;
				pTamer->GetCharOption()->SetAway( false );
			}
			else
			{
				assert_cs( false );
			}			
		}
		break;
	case nSync::ShopReady:
		{
			m_pParent->SetPause( CsC_AvObject::PAUSE_PATH, false );
			m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
			m_pParent->SetDefault_IdleAnimation();

			// 발룬창 등록 해제
			if( g_pTalkBallone )
				g_pTalkBallone->Delete( m_pParent->GetUniqID() );

			assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
			CTamer* pTamer = (CTamer*)m_pParent;
			CDigimon* pDigimon = g_pCharMng->GetDigimon( pTamer->GetDigimonLink() );
			if( pDigimon )
			{
				pDigimon->SetPause( CsC_AvObject::PAUSE_PATH, false );
				pDigimon->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
				pDigimon->SetDefault_IdleAnimation();
				pDigimon->SetEnableObject( true );
			}
		}
		break;
	case nSync::Shop:
		{
			// 발룬창 등록 해제
			if( g_pTalkBallone )
				g_pTalkBallone->Delete( m_pParent->GetUniqID() );

			if( IsSameCondition( nCondition, m_nConditionBackup, nSync::ShopReady ) == false )
			{
				_ApplyCondition( nCondition, nSync::ShopReady );
			}
			else
			{
				m_pParent->SetPause( CsC_AvObject::PAUSE_PATH, false );
				m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
				m_pParent->SetDefault_IdleAnimation();			

				assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
				CTamer* pTamer = (CTamer*)m_pParent;
				CDigimon* pDigimon = g_pCharMng->GetDigimon( pTamer->GetDigimonLink() );
				if( pDigimon )
				{
					pDigimon->SetPause( CsC_AvObject::PAUSE_PATH, false );
					pDigimon->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
					pDigimon->SetDefault_IdleAnimation();
					pDigimon->SetEnableObject( true );
				}
			}			
		}
		break;	
	case nSync::PCBang:
		{
			switch( m_pParent->GetLeafRTTI() )
			{
			case RTTI_TAMER:
			case RTTI_TAMER_USER:
				{
					CTamer* pTamer = (CTamer*)m_pParent;					
					if( pTamer )
					{
						pTamer->GetCharOption()->SetTitleMark();
					}
				}
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


void cCondition::_ApplyEtc()
{
	_Check_PCBangEffect();

	m_pParent->SetDefault_IdleAnimation();
}

void cCondition::_Check_PCBangEffect()
{
	switch( m_pParent->GetLeafRTTI() )
	{
	case RTTI_TAMER:
	case RTTI_TAMER_USER:
		break;
	default:
		return;
	}

	assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );
	CTamer* pTamer = (CTamer*)m_pParent;
	CsC_EffectProp* pProp = pTamer->GetProp_Effect();

	// 라이딩 일때
#ifndef GM_COSTUME
	if( IsCondition( nSync::Ride ) )
	{
		// 무조건 끄자		
		pProp->DeleteLoopEffect( CsC_EffectProp::LE_PCBANG );
	}
	// 라이딩 아닐시
	else
	{
		if( IsCondition( nSync::PCBang ) )
		{
			// 이펙트 아직 없다면 이펙트 추가
			if( pProp->IsLoopEffect( CsC_EffectProp::LE_PCBANG ) == false )
				pProp->AddLoopEffect( CsC_EffectProp::LE_PCBANG, "system\\PC_Tamer_Effect.nif" );
		}
		else
		{
			pProp->DeleteLoopEffect( CsC_EffectProp::LE_PCBANG );
		}
	}
#endif
}