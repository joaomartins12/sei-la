
#include "stdafx.h"
#include "Data_QuickSlot.h"

cData_QuickSlot::cData_QuickSlot()
{
	m_pQuickSlot = NULL;
}

void cData_QuickSlot::SetChar( CsC_AvObject* pParent )
{
	if( pParent == NULL )
	{
		TCHAR szFileName[ Language::pLength::name ];
		memset( szFileName, 0, sizeof( szFileName ) );

		_tcscat_s( szFileName, Language::pLength::name, g_pCharMng->GetTamerUser()->GetCharOption()->GetName() );
		_tcscat_s( szFileName, Language::pLength::name,  _T("_A") );

		m_pQuickSlot = g_pResist->LoadCharQuickSlot( szFileName );
		if( m_pQuickSlot == NULL )
		{
			_InitSlot_Added( NULL );
		}
		g_pGameIF->GetQuickSlot( cQuickSlot::Q_ADDSLOT )->_InitSlot_AddQuick( m_pQuickSlot );

	}
	else
	{

		switch( pParent->GetLeafRTTI() )
		{
		case RTTI_TUTORIAL_TAMER:
		case RTTI_TAMER_USER:
			{
				TCHAR* szName = ( (CTamerUser*)pParent )->GetCharOption()->GetName();
				m_pQuickSlot = g_pResist->LoadCharQuickSlot( szName );
				if( m_pQuickSlot == NULL )
				{
					_InitSlot_Tamer( pParent );
				}
			}
			break; 
		case RTTI_TUTORIAL_DIGIMON:
		case RTTI_DIGIMON_USER:
			{
				TCHAR* szName = ( (CDigimonUser*)pParent )->GetCharOption()->GetName();
				m_pQuickSlot = g_pResist->LoadCharQuickSlot( szName );
				if( m_pQuickSlot == NULL )
				{
					m_pQuickSlot = csnew cResist::sCHAR_QUICKSLOT;
					m_pQuickSlot->Reset();				
				}

				if( m_pQuickSlot->s_QuickSlotChar[ ( (CDigimonUser*)pParent )->GetFTEvolCurObj()->m_nEvoSlot ].s_bInitial == false )
				{
					_InitSlot_Digimon( pParent );
				}
			}
			break;
		default:
			assert_cs( false );
		}
	}
}

void cData_QuickSlot::_InitSlot_Tamer( CsC_AvObject* pParent )
{
	CTamerUser* pTamer = (CTamerUser*)pParent;

	// 일단 테이머 스킬 연결
	m_pQuickSlot = csnew cResist::sCHAR_QUICKSLOT;
	m_pQuickSlot->Reset();

	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_bInitial = true;

	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 0 ].s_eType = cResist::SKILL;
	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 0 ].s_nID = 2;

	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 1 ].s_eType = cResist::SKILL;
	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 1 ].s_nID = 1;

	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 2 ].s_eType = cResist::SKILL;
	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 2 ].s_nID = 0;

	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 3 ].s_eType = cResist::SKILL;
	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 3 ].s_nID = 3;


	cSkill* pSkill = pTamer->GetSkillMng();	
	int nCount = CsMin( pSkill->GetSkillCount(), 6 );
	int nIndex = 4;
	for( int i=nIndex; i<nCount; ++i )
	{
		if( pSkill->GetFTSkill( i )->IsPasive() == true )
			continue;

		m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ nIndex ].s_eType = cResist::SKILL;
		m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ nIndex ].s_nID = i;
		++nIndex;
	}


	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 6 ].s_eType = cResist::ITEM;
	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 6 ].s_nID = 21001;

	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 7 ].s_eType = cResist::ITEM;
	m_pQuickSlot->s_QuickSlotChar[ 0 ].s_QuickSlotInfo[ 7 ].s_nID = 21501;

	g_pResist->SetCharQuickSlot( pTamer->GetCharOption()->GetName(), m_pQuickSlot );
}

void cData_QuickSlot::_InitSlot_Digimon( CsC_AvObject* pParent )
{
	CDigimonUser* pDigimon = (CDigimonUser*)pParent;

	cSkill* pSkill = pDigimon->GetSkillMng();	
	int nCount = pSkill->GetSkillCount();
	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pInfo;

	m_pQuickSlot->s_QuickSlotChar[ pDigimon->GetFTEvolCurObj()->m_nEvoSlot ].s_bInitial = true;
	for( int i=0; i<nCount; ++i )
	{
		pInfo = &m_pQuickSlot->s_QuickSlotChar[ pDigimon->GetFTEvolCurObj()->m_nEvoSlot ].s_QuickSlotInfo[ i ];
		pInfo->s_eType = cResist::SKILL;
		pInfo->s_nID = i;
	}
	m_pQuickSlot->s_QuickSlotChar[ pDigimon->GetFTEvolCurObj()->m_nEvoSlot ].s_QuickSlotInfo[ 6 ].s_eType = cResist::ITEM;
	m_pQuickSlot->s_QuickSlotChar[ pDigimon->GetFTEvolCurObj()->m_nEvoSlot ].s_QuickSlotInfo[ 6 ].s_nID = 21108;
	m_pQuickSlot->s_QuickSlotChar[ pDigimon->GetFTEvolCurObj()->m_nEvoSlot ].s_QuickSlotInfo[ 7 ].s_eType = cResist::ITEM;
	m_pQuickSlot->s_QuickSlotChar[ pDigimon->GetFTEvolCurObj()->m_nEvoSlot ].s_QuickSlotInfo[ 7 ].s_nID = 21608;

	g_pResist->SetCharQuickSlot( pDigimon->GetCharOption()->GetName(), m_pQuickSlot );
}

void cData_QuickSlot::QuickSlotUse( CsC_AvObject* pParent, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSlotIndex )
{
	SAFE_POINTER_RET( pData );
	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pInfo = &pData->s_QuickSlotInfo[ nSlotIndex ];

	switch( pInfo->s_eType )
	{
	case cResist::ITEM:
		{
			// 아이템을 커서로 들어올리면 리턴
			//CURSOR_ST.SetIcon( CURSOR_ICON::CI_INVEN, nInvenIndex, nItemCount, &m_IFIcon[ i ] );
			// 커서의 아이템과 단축키의 아이템이 인덱스가 같으면 리턴
			
			if( CURSOR_ST.IsNotUseItem(CURSOR_ICON::CI_INVEN, pInfo->s_nID ) ){return;}

			////////////////////////////////////////////////////////////////////////// 아이템 사용이 불가능한 경우의 스킬 메모리 예외 처리.	#4162
			CsItem*	pITEM = nsCsFileTable::g_pItemMng->GetItem( static_cast<int>(pInfo->s_nID) );
			SAFE_POINTER_RET(pITEM);

			CsItem::sINFO* pFTInfo = pITEM->GetInfo();
			SAFE_POINTER_RET(pFTInfo);

			USHORT	ItemType	= pFTInfo->s_nType_L;		// 아이템 타입.

			SAFE_POINTER_RET(g_pDataMng);
			cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
			SAFE_POINTER_RET(pServerSync);

			if( !pServerSync->IsEmptyRefCount(cData_ServerSync::CHANGE_PARTNER) ||		// 파트너 교체
				!pServerSync->IsEmptyRefCount(cData_ServerSync::CHANGE_PARTNER_ING)	||	// 파트너 교체중
				!pServerSync->IsEmptyRefCount(cData_ServerSync::CHANGE_KFM)	||			// 모델링 교체
				!pServerSync->IsEmptyRefCount(cData_ServerSync::CHANGE_KFM_ING) )		// 모델링 교체중
			{
				bool	bNotUseItem = false;
				// 공격형 스킬 중복 검사.
				if( ItemType == nItem::DigimonSkillChipATK)
					bNotUseItem = true;

				// 방어형 스킬 중복 검사.
				if( ItemType == nItem::DigimonSkillChipDEF)
					bNotUseItem = true;

				// 보조형 스킬 중복 검사.
				if( ItemType == nItem::DigimonSkillChipAST)
					bNotUseItem = true;

				if(bNotUseItem)
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}
			}

			//////////////////////////////////////////////////////////////////////////

			cData_Inven* pInven = g_pDataMng->GetInven();
			int nSlot = pInven->GetFirstSlot_Item_ID( pInfo->s_nID );
			assert_cs( nSlot != cData_Inven::INVALIDE_INVEN_INDEX );
			g_pDataMng->SendItemUse( TO_INVEN_SID( nSlot ) );
		}
		break;
	case cResist::SKILL:
		{
			if( pParent->GetLeafRTTI() == RTTI_TAMER_USER ||
				pParent->GetLeafRTTI() == RTTI_TUTORIAL_TAMER )
			{
				( (CTamerUser*)pParent )->UseSkill( pInfo->s_nID );
			}
			else
			{
				//assert_cs( pParent->GetLeafRTTI() == RTTI_DIGIMON_USER );
				( (CDigimonUser*)pParent )->UseSkill( pInfo->s_nID );
			}
		}
		break;
	case cResist::NONE:
		break;
	default:
		assert_cs( false );
	}
}

void cData_QuickSlot::CheckItemCount_AllUser()
{
	CheckItemCount( g_pCharMng->GetTamerUser()->GetQuickSlot() );

	CDigimonUser* pDigimon;
	for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )
	{
		pDigimon = g_pCharMng->GetDigimonUser( i );
		if( pDigimon != NULL )
		{
			CheckItemCount( pDigimon->GetQuickSlot() );
		}		
	}

#ifdef USE_CENTER_BAR
	cCenterbar::CheckAllSlot();
#endif	
	CheckItemCount( g_pGameIF->GetQuickSlot( cQuickSlot::Q_ADDSLOT )->GetQuickSlot() );
}

void cData_QuickSlot::CheckAllSlot( CsC_AvObject* pParent, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSkillCount )
{
	SAFE_POINTER_RET( pData );
	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pInfo;
	if( pParent == NULL )
	{
		for( int i = 0 ; i < nLimit::QuickSlot ; ++i )
		{
			pInfo = &pData->s_QuickSlotInfo[ i ];
			switch( pInfo->s_eType )
			{
			case cResist::NONE:
				break;
			case cResist::ITEM:
				{
					// 인벤에 존재 하지 않는 아이템은 리셋
					pInfo->s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pInfo->s_nID );
					pInfo->s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pInfo->s_nID );
					if( pInfo->s_nCount == 0 )
						pInfo->Reset();
				}
				break;
			case cResist::SKILL:
				{
					// 스킬 인덱싱이 넘어 간거는 리셋
					if( pInfo->s_nID >= (uint)nSkillCount )
					{
						pInfo->Reset();
					}
					// 제한 레벨이 아직 안된거는 리셋
					else
					{
						int nLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();

						cSkill* pSkillMng = g_pCharMng->GetTamerUser()->GetSkillMng();

						if( nLevel < pSkillMng->GetFTSkill( pInfo->s_nID )->GetInfo()->s_nLimitLevel )
						{
							pInfo->Reset();
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}
	else
	{
		for( int i=0; i<nLimit::QuickSlot; ++i )
		{
			pInfo = &pData->s_QuickSlotInfo[ i ];
			switch( pInfo->s_eType )
			{
			case cResist::NONE:
				break;
			case cResist::ITEM:
				{
					// 인벤에 존재 하지 않는 아이템은 리셋
					pInfo->s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pInfo->s_nID );
					pInfo->s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pInfo->s_nID );
					if( pInfo->s_nCount == 0 )
						pInfo->Reset();
				}
				break;
			case cResist::SKILL:
				{
					// 스킬 인덱싱이 넘어 간거는 리셋
					if( pInfo->s_nID >= (uint)nSkillCount )
					{
						pInfo->Reset();
					}
					// 제한 레벨이 아직 안된거는 리셋
					else
					{
						int nLevel = pParent->GetBaseStat()->GetLevel();

						cSkill* pSkillMng = NULL;
						switch( pParent->GetLeafRTTI() )
						{
						case RTTI_TUTORIAL_DIGIMON:
						case RTTI_DIGIMON:
						case RTTI_DIGIMON_USER:
							pSkillMng = ( (CDigimon*)pParent )->GetSkillMng();
							break;
						case RTTI_TUTORIAL_TAMER:
						case RTTI_TAMER:				
						case RTTI_TAMER_USER:
							pSkillMng = ( (CTamer*)pParent )->GetSkillMng();
							break;
						default:
							assert_cs( false );
						}
						if( nLevel < pSkillMng->GetFTSkill( pInfo->s_nID )->GetInfo()->s_nLimitLevel )
						{
							pInfo->Reset();
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}
}

void cData_QuickSlot::CheckItemCount( cResist::sCHAR_QUICKSLOT::sCHAR* pData )
{
	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pInfo;
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		pInfo = &pData->s_QuickSlotInfo[ i ];
		switch( pInfo->s_eType )
		{
		case cResist::ITEM:
			{
				pInfo->s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pInfo->s_nID );
				pInfo->s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pInfo->s_nID );
				if( pInfo->s_nCount == 0 )
					pInfo->Reset();
			}
			break;
		case cResist::NONE:		break;
		case cResist::SKILL:	break;
		default:				assert_cs( false );
		}
	}
}


// 리턴값 0 이면 - 커서 존속 시키자
// 리턴값 1 이면 - 커서 교체
// 리턴값 2 이면 - 커서 제거
int cData_QuickSlot::InvenToQuickSlot( int nInvenIndex, int nDestParentIndex, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSlotIndex )
{
	int nReturn = 0;

	cItemInfo* pItem = g_pDataMng->GetInven()->GetData( nInvenIndex );
	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType )->GetInfo();

	// 퀵슬롯 장착 가능한 물품인지 확인
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Spirit:
	case nItem::UseQuest:
	case nItem::Capsule:		// 캡슐
	case nItem::Potion:			// 포션, 카드, 소모품
	case nItem::Card:
	case nItem::Consume:
	case nItem::Buff:
	case nItem::SKILL_Card:
	case nItem::Fire_Work:
#ifdef CROSSWARS_SYSTEM
	case nItem::CodeCrown:
#endif
#ifdef DEBUFF_ITEM
	case nItem::Debuff:
#endif
	// 퀵슬롯 등록 타입 추가
	case nItem::Portal:
	case nItem::Cash_ChangeDName:
	case nItem::Cash_ChangeTName:
	case nItem::Cash_ResetSkillPoint:
	case nItem::Cash_ExtraInven:
	case nItem::Cash_ExtraWarehouse:
	case nItem::Cash_JumpBuster:
	case nItem::DigimonSlotInc:
	case nItem::Cash_DigimonCareSlot:	
	case nItem::ScanUse_Item:
	case nItem::Item_TamerSKillConsume:
	case nItem::PersonStore:
#ifdef CROSSWARS_SYSTEM
	case nItem::Cash_CrossInvenSlot:
#endif

#ifdef LEVELUP_EFFECT_LOOP
	case nItem::NoticeItem:
#endif

	// 스킬 등록 아이템 큇슬롯에 등록 가능하도록...
	case nItem::DigimonSkillChipAST:
	case nItem::DigimonSkillChipATK:
	case nItem::DigimonSkillChipDEF:
	case nItem::AntiX:
		break;
	default:
		{
			cPrintMsg::PrintMsg( 11014 );
		}
		return nReturn;
	}

	nReturn = 2;
	// 대상이 비여 있는지 체크
	// 비여 있지 않다면 커서아이콘에 셋팅
	if( pData->s_QuickSlotInfo[ nSlotIndex ].s_eType != cResist::NONE )
	{
		CURSOR_ST.SetIconSubInfo( nDestParentIndex, pData->s_QuickSlotInfo[ nSlotIndex ].s_eType, pData->s_QuickSlotInfo[ nSlotIndex ].s_nID );
		nReturn = 1;
	}

	pData->s_QuickSlotInfo[ nSlotIndex ].s_eType = cResist::ITEM;
	pData->s_QuickSlotInfo[ nSlotIndex ].s_nID = pItem->m_nType;
	pData->s_QuickSlotInfo[ nSlotIndex ].s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pItem->m_nType );
	pData->s_QuickSlotInfo[ nSlotIndex ].s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pItem->m_nType );
	assert_cs( pData->s_QuickSlotInfo[ nSlotIndex ].s_nCount > 0 );
	return nReturn;
}

int cData_QuickSlot::SkillToQuickSlot( int nSrcParentIndex, int nSkillIndex, int nDestParentIndex, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSlotIndex )
{
	int nReturn = 0;

	if( nSrcParentIndex != nDestParentIndex )
	{
		if(	nDestParentIndex != cQuickSlot::Q_ADDSLOT 
			&& nSrcParentIndex != cQuickSlot::Q_ADDSLOT )
		{
			// 같은 부모의 스킬이 아니다. 리턴
			cPrintMsg::PrintMsg( 11014 );
			return nReturn;
		}
	}

	if( ( nSrcParentIndex == cQuickSlot::Q_DIGIMON && nDestParentIndex == cQuickSlot::Q_ADDSLOT ) 
		|| ( nSrcParentIndex == cQuickSlot::Q_ADDSLOT && nDestParentIndex == cQuickSlot::Q_DIGIMON ) 
		)
	{
		cPrintMsg::PrintMsg( 11014 );
		return nReturn;
	}

	nReturn = 2;

	// 대상이 비여 있는지 체크
	// 비여 있지 않다면 커서아이콘에 셋팅
	if( pData->s_QuickSlotInfo[ nSlotIndex ].s_eType != cResist::NONE )
	{
		CURSOR_ST.SetIconSubInfo( nDestParentIndex, pData->s_QuickSlotInfo[ nSlotIndex ].s_eType, pData->s_QuickSlotInfo[ nSlotIndex ].s_nID );
		nReturn = 1;
	}

	pData->s_QuickSlotInfo[ nSlotIndex ].s_eType = cResist::SKILL;
	pData->s_QuickSlotInfo[ nSlotIndex ].s_nID = nSkillIndex;

	return nReturn;
}

int cData_QuickSlot::QuickSlotToQuickSlot( int nSrcParentIndex, int nSrcType, int nSrcID,
											int nDestParentIndex, cResist::sCHAR_QUICKSLOT::sCHAR* pDestData, int nDestSlotIndex )
{
	int nReturn = 0;
	// 스킬이라면 같은 인덱스 인지 체크
	if( nSrcType == cResist::SKILL )
	{
		//디지몬 슬롯 -> 추가슬롯으로 혹은 반대로 스킬 옮길때
		if( ( nSrcParentIndex == cQuickSlot::Q_DIGIMON && nDestParentIndex == cQuickSlot::Q_ADDSLOT ) 
			|| ( nSrcParentIndex == cQuickSlot::Q_ADDSLOT && nDestParentIndex == cQuickSlot::Q_DIGIMON ) 
			)
		{
			cPrintMsg::PrintMsg( 11014 );
			return nReturn;
		}

		if( nSrcParentIndex != nDestParentIndex )
		{
			if(	nDestParentIndex != cQuickSlot::Q_ADDSLOT 
				&& nSrcParentIndex != cQuickSlot::Q_ADDSLOT )
			{
				// 같은 부모의 스킬이 아니다. 리턴
				cPrintMsg::PrintMsg( 11014 );
				return nReturn;
			}
		}
	}

	nReturn = 2;	
	// 대상이 비여 있는지 체크
	// 비여 있지 않다면 커서아이콘에 셋팅
	if( pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_eType != cResist::NONE )
	{
		CURSOR_ST.SetIconSubInfo( nDestParentIndex, pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_eType, pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_nID );
		nReturn = 1;
	}

	// 데이터 집어 넣자
	pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_eType = (cResist::eTYPE)nSrcType;
	pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_nID = nSrcID;
	switch( nSrcType )
	{
	case cResist::ITEM:
		pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( nSrcID );
		pDestData->s_QuickSlotInfo[ nDestSlotIndex ].s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( nSrcID );
		break;
	case cResist::SKILL:
		break;
	default:
		assert_cs( false );
	}

	return nReturn;
}

void cData_QuickSlot::_InitSlot_Added( CsC_AvObject* pParent )
{
	// 일단 테이머 스킬 연결
	m_pQuickSlot = csnew cResist::sCHAR_QUICKSLOT;
	m_pQuickSlot->Reset();
	m_pQuickSlot->s_QuickSlotChar[ nLimit::EvoUnit ].s_bInitial = true;

	for( int i = 0 ; i < nLimit::QuickSlot ; i++ )
	{
		m_pQuickSlot->s_QuickSlotChar[ nLimit::EvoUnit ].s_QuickSlotInfo[ i ].s_eType = cResist::NONE;
		m_pQuickSlot->s_QuickSlotChar[ nLimit::EvoUnit ].s_QuickSlotInfo[ i ].s_nID = (uint)-1;
	}
	
	TCHAR szFileName[ Language::pLength::name ];
	memset( szFileName, 0, sizeof( szFileName ) );

	_tcscat_s( szFileName, Language::pLength::name, g_pCharMng->GetTamerUser()->GetCharOption()->GetName() );
	_tcscat_s( szFileName, Language::pLength::name,  _T("_A") );

	g_pResist->SetCharQuickSlot( szFileName , m_pQuickSlot );
}
