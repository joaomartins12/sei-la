
#include "stdafx.h"
#include "04_PartObject.h"

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CsC_PartObject, RTTI_PARTOBJECT )
THREAD_MEMPOOL_CPP( CsC_PartObject )

//CsCRTTI_CPP( PARENT_CLASS, CsC_CardObject, RTTI_CARDOBJECT )
//THREAD_MEMPOOL_CPP( CsC_CardObject )


#ifdef PC_BANG_SERVICE
#define PCBANG_COSTUME_ITEM_ID			17101
#endif
CsC_PartObject::CsC_PartObject()
{
	m_bInitialLoadPart = false;
	m_cWorkingFolder[ 0 ] = NULL;

	// 파츠 정보 리셋
	for( int i=0; i<PART_INFO_COUNT; ++i )
		m_PartInfo[ i ].Reset();	

	for( int i=0; i<nsPART::MAX_CHANGE_PART_COUNT; ++i )
		m_SettingPart[ i ].Reset();
}

void CsC_PartObject::Delete()
{	
	PARENT_CLASS::Delete();
	
	m_bInitialLoadPart = false;
	assert_cs( GetRefLinkPart() == 0 );
	assert_cs( m_queueReadyPart.Empty() == true );
	/*std::vector< sLOAD_PART* >	vItem;
	m_queueReadyPart.Get( vItem );
	size_t nCount = vItem.size();
	for( size_t i=0; i<nCount; ++i )
	{
		if( vItem[ i ]->s_nPartIndex > -1 )
		{
			SAFE_DELETE( vItem[ i ]->s_pNiNode );
			if( vItem[ i ]->s_pRefLoad )
			{
				vItem[ i ]->s_pRefLoad->DecreaseRef();
				vItem[ i ]->s_pRefLoad = NULL;
			}
		}
		else
		{
			DecreaseRefLinkPart();
		}
		sLOAD_PART::DeleteInstance( vItem[ i ] );
	}*/

	m_cWorkingFolder[ 0 ] = NULL;

	// 파츠 정보 리셋
	for( int i=0; i<PART_INFO_COUNT; ++i )
		m_PartInfo[ i ].Reset();	

	for( int i=0; i<nsPART::MAX_CHANGE_PART_COUNT; ++i )
		m_SettingPart[ i ].Reset();
}

void CsC_PartObject::DeleteUpdate()
{
	PARENT_CLASS::DeleteUpdate();

	if( GetRefLinkPart() == 0 )
	{
		// GetRefLinkPart 찰나의 순간에 들어 올수도 있다
		assert_cs( GetRefOnThreadLoad()||GetRefLinkPart()||( m_bInitialLoadPart == true ) );
		return;
	}

	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	assert_cs( m_queueReadyPart.Empty() == false );

	sLOAD_PART* pReadyPart = NULL;
	while( m_queueReadyPart.Get( pReadyPart ) )
	{
		if( pReadyPart->s_nPartIndex > - 1 )
		{
			SAFE_DELETE( pReadyPart->s_pNiNode );
			if( pReadyPart->s_pRefLoad )
			{
				pReadyPart->s_pRefLoad->DecreaseRef();
				pReadyPart->s_pRefLoad = NULL;
			}
		}
		else
		{
			// -1 이라면 레퍼런스 감소로 놓겠다
			DecreaseRefLinkPart();
		}
		pReadyPart->s_pNiNode = NULL;
		pReadyPart->s_pRefLoad = NULL;
		sLOAD_PART::DeleteInstance( pReadyPart );		
	}
	m_bInitialLoadPart = true;
}

void CsC_PartObject::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	assert_cs( m_bInitialLoadPart == false );

	// 폴더 설정	
	std::string strPath = g_pModelDataMng->GetKfmPath( nFileTableID );
	nsCSFILE::GetFilePathOnly( m_cWorkingFolder, OBJECT_PATH_LEN, strPath.c_str(), true );	

	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );
}

void CsC_PartObject::Update( float fDeltaTime, bool bAnimation )
{
	_UpdateReadyPart();
	PARENT_CLASS::Update( fDeltaTime, bAnimation );
}

void CsC_PartObject::Render()
{
	PARENT_CLASS::Render();
}


void CsC_PartObject::SetScale( float fScale, bool bOrgScale )
{
	PARENT_CLASS::SetScale( fScale, bOrgScale );
	
	if( !IsProperty( CsC_Property::EFFECT ) )
		return;

	CsC_EffectProp* pEff = GetProp_Effect();
	SAFE_POINTER_RET( pEff );

	CsC_AvObject* pAura = pEff->GetLoopEffect( CsC_EffectProp::LE_AURA );
	if( pAura )
		pAura->SetScale( GetToolWidth() * 0.0334f );
}

//====================================================================================
//
//		Part
//
//====================================================================================

void CsC_PartObject::ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM /*=true*/ )
{
	//=======================================================================================
	//	기본 본 정보
	//=======================================================================================
	if( bIncludeBaseKFM == true )
	{
		assert_cs( GetFTID() != 0 );
		sTCUnitPart* pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::KFM_File, GetFTID() );
		pUint->s_Prop = m_CharProp;
		pUint->s_pLoadedObject = (CsC_Object*)this;

		if( _IsFastPartLoad() )
			pUint->SetLoadType( sTCUnit::eFast );		

		// 루트 정보
		
		std::string strPath = GetModelPath();
		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );
		assert_cs( pUint->s_dwLoadID != 0 );
		g_pThread->LoadChar( pUint );
	}

	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
	{
		m_SettingPart[ i ].s_nSettingID = pItemCodeArray[ i ].s_nFileTableID;
		m_SettingPart[ i ].s_nRemainTime = pItemCodeArray[ i ].s_nRemainTime;
	}
	

	//=======================================================================================
	//	파트 정보
	//=======================================================================================
	sTCUnitPart* pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::All_Part_File, GetFTID() );
	pUint->s_Prop = m_CharProp;
	pUint->s_pLoadedObject = (CsC_Object*)this;
	if( _IsFastPartLoad() )
		pUint->SetLoadType( sTCUnit::eFast );

	// 코스튬 상태가 아님
	if( pItemCodeArray[ nsPART::Costume ].s_nFileTableID == 0 )
	{
		// 로드시 필요한 초기화
		pUint->s_ePartType = sTCUnitPart::PT_PART;

		// 파트 정보
		for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )
		{
			pUint->s_PartInfo[ i ].s_nFileTableID = pItemCodeArray[ i ].s_nFileTableID;			
			GetFileName_FromID( pUint->s_PartInfo[ i ].s_cPartName, &pItemCodeArray[ i ] );

			if(0 != pItemCodeArray[ i ].s_nFileTableID)
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemCodeArray[ i ].s_nFileTableID )->GetInfo();
				int TamerModelIDX = GetFTID();

				if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S) )
				{
					pUint->s_PartInfo[ i ].s_nFileTableID = 0;
					sCHANGE_PART_INFO sFakeItemCode;
					sFakeItemCode.s_nFileTableID = 0;
					sFakeItemCode.s_nPartIndex = pItemCodeArray[ i ].s_nPartIndex;
					sFakeItemCode.s_nRemainTime = 0;
					
					GetFileName_FromID( pUint->s_PartInfo[ i ].s_cPartName, &sFakeItemCode );
				}
			}
		}
	}
	// 코스튬 상태
	else
	{
		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemCodeArray[ nsPART::Costume ].s_nFileTableID )->GetInfo();
		int TamerModelIDX = GetFTID();

// #ifdef PC_BANG_SERVICE
// 		if( pFTItem->s_dwItemID == PCBANG_COSTUME_ITEM_ID )
// 		{
// 			return;
// 		}
// 		else
// #endif
		//  세이버용 의상이면.. && 세이버...//  어드벤처용 의상이면.. && 어드벤쳐면.. // 공용이면
		// 테이머 타입과 맞지않는 아이템을 입었다.
// 		if(false == ((2 == pFTItem->s_nType_S &&  CODE_IKUTO >= TamerModelIDX) || 
// 			(3 == pFTItem->s_nType_S &&  CODE_TAICHI <= TamerModelIDX) ||
// 			(1 == pFTItem->s_nType_S &&  CODE_TAKERU >= TamerModelIDX)))
		if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S) )
		{
			// 로드시 필요한 초기화
			pUint->s_ePartType = sTCUnitPart::PT_PART;

			// 파트 정보
			for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )
			{
				pUint->s_PartInfo[ i ].s_nFileTableID = 0;		

				sCHANGE_PART_INFO sFakeItemCode;
				sFakeItemCode.s_nFileTableID = 0;
				sFakeItemCode.s_nPartIndex = pItemCodeArray[ i ].s_nPartIndex;
				sFakeItemCode.s_nRemainTime = 0;

				GetFileName_FromID( pUint->s_PartInfo[ i ].s_cPartName, &sFakeItemCode );
			}
		}
 		else if( GetFileName_FromID( pUint->s_PartInfo[ 0 ].s_cPartName, &pItemCodeArray[ nsPART::Costume ] ) == true )
		{
			// 신규 테이머 추가 시 어드벤쳐에 속하지만 코스튬 미작업 테이머일 경우 처리
			// 코스튬 파일 존재 유무로 확인
			char pPath[ OBJECT_PATH_LEN ];
			sprintf_s( pPath, OBJECT_PATH_LEN, "%s\\%s", GetWorkingFolder(), pUint->s_PartInfo[0].s_cPartName );

			if( !CsFPS::CsFPSystem::IsExist( 0, pPath ) )
			{
				// 로드시 필요한 초기화
				pUint->s_ePartType = sTCUnitPart::PT_PART;

				// 파트 정보
				for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )
				{
					pUint->s_PartInfo[ i ].s_nFileTableID = 0;		

					sCHANGE_PART_INFO sFakeItemCode;
					sFakeItemCode.s_nFileTableID = 0;
					sFakeItemCode.s_nPartIndex = pItemCodeArray[ i ].s_nPartIndex;
					sFakeItemCode.s_nRemainTime = 0;

					GetFileName_FromID( pUint->s_PartInfo[ i ].s_cPartName, &sFakeItemCode );
				}
			}
			else
			{
				// 로드시 필요한 초기화		
				pUint->s_ePartType = sTCUnitPart::PT_COSTUME_PUTON;
				// 코스튬 정보
				pUint->s_PartInfo[ 0 ].s_nFileTableID = pItemCodeArray[ nsPART::Costume ].s_nFileTableID;		
			}
		}
		else
		{
			sTCUnitPart::DeleteInstance( pUint );
			pItemCodeArray[ nsPART::Costume ].s_nFileTableID = 0;			
			CsC_PartObject::ThreadLoad_Part( pItemCodeArray, false );
			return;
		}
	}

	// 어태치 정보
	for( int i=nsPART::MAX_CHANGE_PART_COUNT; i<nsPART::MAX_ATTACH_COUNT; ++i )
	{
		if( pItemCodeArray[ i ].s_nFileTableID != 0 )
		{
			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemCodeArray[ i ].s_nFileTableID )->GetInfo();
			int TamerModelIDX = GetFTID();

			if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S) )
			{
				break;
			}

			if( GetFileName_FromID( pUint->s_AttachInfo[ i-nsPART::MAX_CHANGE_PART_COUNT ].s_cPartName, &pItemCodeArray[ i ] ) == true )
			{
				assert_cs( pUint->s_ePartType != sTCUnitPart::NONE_DATA );
				pUint->s_ePartType = (sTCUnitPart::ePART_TYPE)( pUint->s_ePartType | sTCUnitPart::AT_ATTACH_PUTON );
				pUint->s_AttachInfo[ i-nsPART::MAX_CHANGE_PART_COUNT ].s_nFileTableID = pItemCodeArray[ i ].s_nFileTableID;
			}
			else
			{
				pUint->s_AttachInfo[ i-nsPART::MAX_CHANGE_PART_COUNT ].s_nFileTableID = 0;
			}
		}
	}

	// 쓰레드 로드
	assert_cs( pUint != NULL );	
	g_pThread->LoadChar( pUint );
	
}

void CsC_PartObject::DeletePart( int nPartIndex )
{
	sTCUnitPart* pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::Part_Delete, GetFTID() );
	pUint->s_pLoadedObject = (CsC_Object*)this;
	pUint->s_nPartIndex = nPartIndex;
	g_pThread->LoadChar( pUint );
}

void CsC_PartObject::ToOriginalPart( UINT nFileTableID )
{
	ChangeKFM( nFileTableID );

	sCHANGE_PART_INFO cpi[ nsPART::MAX_TOTAL_COUNT ];
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
	{
		cpi[ i ].s_nFileTableID = m_SettingPart[ i ].s_nSettingID;
		cpi[ i ].s_nRemainTime = m_SettingPart[ i ].s_nRemainTime;
		cpi[ i ].s_nPartIndex = i;
	}
	ThreadLoad_Part( cpi );
}

//void CsC_PartObject::ThreadCallBack_PreLoadAttachPart( sLOAD_PART* pPart, int nPartIndex )
//{
//	assert_cs( IsLoad() == false );
//
//	m_PartInfo[ nPartIndex ].ChangePart( pPart->s_nFileTableID );
//	m_PartInfo[ nPartIndex ].SetRefLoad( pPart->s_pRefLoad );
//	_SetPart( pPart->s_pNiNode, nPartIndex );
//}

void CsC_PartObject::ThreadCallBack_AttachPart( sLOAD_PART* pPart )
{
	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	m_queueReadyPart.Put( pPart );

	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
	pDecreaseRefPart->s_nPartIndex = -1;
	m_queueReadyPart.Put( pDecreaseRefPart );
}

void CsC_PartObject::ThreadCallBack_AttachPart( std::queue< sLOAD_PART* >* pPartQueue )
{	
	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	assert_cs( pPartQueue->size() > 0 );
	while( pPartQueue->size() )
	{
		m_queueReadyPart.Put( pPartQueue->front() );
		pPartQueue->pop();
	}

	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
	pDecreaseRefPart->s_nPartIndex = -1;
	m_queueReadyPart.Put( pDecreaseRefPart );
}

void CsC_PartObject::ChangePart( sCHANGE_PART_INFO* pPartInfo )
{
	// 보여지는 부위여야만 한다
	if( pPartInfo->s_nPartIndex >= PART_INFO_COUNT )
		return;

	sTCUnitPart* pUint = NULL;
	if( pPartInfo->s_nPartIndex < nsPART::MAX_TOTAL_COUNT )
	{
		m_SettingPart[ pPartInfo->s_nPartIndex ].s_nSettingID = pPartInfo->s_nFileTableID;
		m_SettingPart[ pPartInfo->s_nPartIndex ].s_nRemainTime = pPartInfo->s_nRemainTime;
	}

	// 장비 변경 쪽
	if( pPartInfo->s_nPartIndex < nsPART::MAX_CHANGE_PART_COUNT )
	{
		// 코스튬 아닐때
		if( pPartInfo->s_nPartIndex != nsPART::Costume )
		{
			// 코스츔 상태면서 장비일때는 패스
			if( ( m_SettingPart[ nsPART::Costume ].s_nSettingID )&&( pPartInfo->s_nPartIndex < nsPART::MAX_APPLY_STATE_COUNT ) )
			{
				m_SettingPart[ pPartInfo->s_nPartIndex ].s_nSettingID = pPartInfo->s_nFileTableID;
				m_SettingPart[ pPartInfo->s_nPartIndex ].s_nRemainTime = pPartInfo->s_nRemainTime;
				char cPartName[ OBJECT_NAME_LEN ];
				sCHANGE_PART_INFO* pSettingInfo = csnew sCHANGE_PART_INFO;
				pSettingInfo->s_nFileTableID = m_SettingPart[ nsPART::Costume ].s_nSettingID;
				pSettingInfo->s_nRemainTime = m_SettingPart[ nsPART::Costume ].s_nRemainTime;
				pSettingInfo->s_nPartIndex = nsPART::Costume;
				GetFileName_FromID( cPartName, pSettingInfo );
				if( strcmp( cPartName, "NoneCostume.nif" ) != 0 )
				{
					return;
				}
			}	
			
			pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::One_Part_File, GetFTID() );
			pUint->s_pLoadedObject = (CsC_Object*)this;

			// 로드시 필요한 초기화
			pUint->s_ePartType = sTCUnitPart::PT_PART;
			pUint->s_nPartIndex = pPartInfo->s_nPartIndex;
			pUint->s_PartInfo[ 0 ].s_nFileTableID = pPartInfo->s_nFileTableID;
			GetFileName_FromID( pUint->s_PartInfo[ 0 ].s_cPartName, pPartInfo );

			if(0 != pPartInfo->s_nFileTableID)
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo();
				int TamerModelIDX = GetFTID();

				if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S) )
				{					
					pUint->s_PartInfo[ 0 ].s_nFileTableID = 0;

					sCHANGE_PART_INFO Fakecp;
					Fakecp.s_nFileTableID = 0;
					Fakecp.s_nPartIndex = pPartInfo->s_nPartIndex;
					Fakecp.s_nRemainTime = 0;

					GetFileName_FromID( pUint->s_PartInfo[ 0 ].s_cPartName, &Fakecp );
				}
			}
		}
		// 코스튬
		else
		{
			m_SettingPart[ nsPART::Costume ].s_nSettingID = pPartInfo->s_nFileTableID;
			m_SettingPart[ nsPART::Costume ].s_nRemainTime = pPartInfo->s_nRemainTime;

			pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::One_Part_File, GetFTID() );
			pUint->s_pLoadedObject = (CsC_Object*)this;

			// 코스츔 추가 - 기존 파츠 제거는 쓰레드 구분에서
			bool bDelCostume = true;
			if( pPartInfo->s_nFileTableID != 0 )
			{				
				if( GetFileName_FromID( pUint->s_PartInfo[ 0 ].s_cPartName, pPartInfo ) == true )
				{
					bDelCostume = false;
					// 로드시 필요한 초기화
					pUint->s_ePartType = sTCUnitPart::PT_COSTUME_PUTON;
					pUint->s_PartInfo[ 0 ].s_nFileTableID = pPartInfo->s_nFileTableID;
				}
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo();
				int TamerModelIDX = GetFTID();

				// 신규 테이머 추가 시 어드벤쳐에 속하지만 코스튬 미작업 테이머일 경우 처리
				// 코스튬 파일 존재 유무로 확인
				char pPath[ OBJECT_PATH_LEN ];
				sprintf_s( pPath, OBJECT_PATH_LEN, "%s\\%s", GetWorkingFolder(), pUint->s_PartInfo[0].s_cPartName );

				if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S)
					|| !CsFPS::CsFPSystem::IsExist( 0, pPath ) )
				{
					bDelCostume = true;
				}
			}

			// 코스츔 제거 - 기존 파츠 추가 ( 코스츔 제거는 쓰레드 구문에서 )
			if( bDelCostume == true )
			{
				pUint->s_ePartType = sTCUnitPart::PT_COSTUME_PUTOFF;
				// 파트 정보
				for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )
				{
					pUint->s_PartInfo[ i ].s_nFileTableID = m_SettingPart[ i ].s_nSettingID;					

					sCHANGE_PART_INFO cp;
					cp.s_nFileTableID = m_SettingPart[ i ].s_nSettingID;
					cp.s_nPartIndex = i;
					cp.s_nRemainTime = m_SettingPart[ i ].s_nRemainTime;
					GetFileName_FromID( pUint->s_PartInfo[ i ].s_cPartName, &cp );

					if(0 != m_SettingPart[ i ].s_nSettingID)
					{
						CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pUint->s_PartInfo[ i ].s_nFileTableID )->GetInfo();
						int TamerModelIDX = GetFTID();

						if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S) )
						{
							pUint->s_PartInfo[ i ].s_nFileTableID = 0;

							sCHANGE_PART_INFO cp;
							cp.s_nFileTableID = 0;
							cp.s_nPartIndex = i;
							cp.s_nRemainTime = 0;
							GetFileName_FromID( pUint->s_PartInfo[ i ].s_cPartName, &cp );

						}
					}
				}
			}
		}
	}
	// 어태치쪽
	else
	{
		// 로드시 필요한 초기화
		if( ( pPartInfo->s_nPartIndex < nsPART::MAX_ATTACH_COUNT )&&( pPartInfo->s_nFileTableID == 0 ) )
		{
			DeletePart( pPartInfo->s_nPartIndex );
			return;
		}
		else if( pPartInfo->s_nPartIndex < nsPART::MAX_ATTACH_COUNT )
		{
			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo();
			int TamerModelIDX = GetFTID();

			if( false == g_pModelDataMng->CostumeCategory(TamerModelIDX, pFTItem->s_nType_S) )
			{
				DeletePart( pPartInfo->s_nPartIndex );
				return;
			}			
			pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::One_Part_File, GetFTID() );
			if( GetFileName_FromID( pUint->s_PartInfo[ 0 ].s_cPartName, pPartInfo ) == true )
			{				
				pUint->s_pLoadedObject = (CsC_Object*)this;
				pUint->s_nPartIndex = pPartInfo->s_nPartIndex;			
				pUint->s_ePartType = sTCUnitPart::AT_ATTACH_PUTON;

				pUint->s_PartInfo[ 0 ].s_nFileTableID = pPartInfo->s_nFileTableID;
			}
			else
			{
				sTCUnit::DeleteInstance( pUint );
				if( ( pPartInfo->s_nPartIndex < nsPART::MAX_ATTACH_COUNT )&&( IsPart( pPartInfo->s_nPartIndex ) ) )
				{
					DeletePart( pPartInfo->s_nPartIndex );
				}
				return;
			}
		}
		else if( pPartInfo->s_nPartIndex < PART_INFO_COUNT)
		{
			//오라템일때
			if ( pPartInfo->s_nPartIndex == nsPART::EquipAura )
			{		
				m_SettingPart[ pPartInfo->s_nPartIndex ].s_nSettingID = pPartInfo->s_nFileTableID;
				m_SettingPart[ pPartInfo->s_nPartIndex ].s_nRemainTime = pPartInfo->s_nRemainTime;
				return;
			}
#ifdef SDM_TAMER_XGUAGE_20180628
			if( pPartInfo->s_nPartIndex == nsPART::XAI )
			{
				m_SettingPart[ pPartInfo->s_nPartIndex ].s_nSettingID = pPartInfo->s_nFileTableID;
				m_SettingPart[ pPartInfo->s_nPartIndex ].s_nRemainTime = pPartInfo->s_nRemainTime;
				return;
			}
#endif
			// 반지		
			// 귀걸이	=> 현재 팻으로 쓰므로 무시하자
			if( ( pPartInfo->s_nPartIndex != nsPART::Ring )&&( pPartInfo->s_nPartIndex != nsPART::Necklace )&&
				( pPartInfo->s_nPartIndex != nsPART::Earring ) 
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
				&& ( pPartInfo->s_nPartIndex != nsPART::Bracelet ) 
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
				&& ( pPartInfo->s_nPartIndex != nsPART::Digivice )
#endif
				)
			{
				pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::One_Part_File, GetFTID() );
				bool bSuccess = GetFileName_FromID( pUint->s_PartInfo[ 0 ].s_cPartName, pPartInfo );
				assert_cs( bSuccess == true );

				pUint->s_pLoadedObject = (CsC_Object*)this;
				pUint->s_nPartIndex = pPartInfo->s_nPartIndex;
				pUint->s_PartInfo[ 0 ].s_nFileTableID = pPartInfo->s_nFileTableID;
				pUint->s_ePartType = sTCUnitPart::AT_ATTACH_PUTON;
			}			
		}
	}

	// 쓰레드 로드
	if( pUint != NULL )
	{
		// 우선순위 로드 인가??
		if( _IsFastPartLoad() )
			pUint->SetLoadType( sTCUnit::eFast );

		g_pThread->LoadChar( pUint );
	}
}

void CsC_PartObject::ChangePart_ByTool( sCHANGE_PART_INFO* pPartInfo )
{	
}

void CsC_PartObject::_UpdateReadyPart()
{
	if( GetRefLinkPart() == 0 )
		return;

	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
	assert_cs( m_queueReadyPart.Empty() == false );
	bool bPartChange = false;

	sLOAD_PART* pReadyPart = NULL;
	while( m_queueReadyPart.Get( pReadyPart ) )
	{
		if( pReadyPart->s_nPartIndex > - 1 )
		{
			// 현재 연결된 KFM 과 ID가 같을 시에만
			if( pReadyPart->s_dwLoadID == m_dwLoadID )
			{
				// 파트 추가
				if( pReadyPart->s_pNiNode )
				{
					m_PartInfo[ pReadyPart->s_nPartIndex ].SetRefLoad( pReadyPart->s_pRefLoad );
					_SetPart( pReadyPart->s_pNiNode, pReadyPart->s_nPartIndex );
				}
				// 파트 제거
				else
				{
					m_Node.m_pNiNode->DetachChildAt( pReadyPart->s_nPartIndex + nsPART::PART_NODE_CONSTANT );
					m_PartInfo[ pReadyPart->s_nPartIndex ].Reset();
				}
				bPartChange = true;
			}			
		}
		else
		{
			// -1 이라면 레퍼런스 감소로 놓겠다
			DecreaseRefLinkPart();
			PostLinkObject();
		}
		pReadyPart->s_pNiNode = NULL;
		pReadyPart->s_pRefLoad = NULL;
		sLOAD_PART::DeleteInstance( pReadyPart );		
	}

	if( bPartChange == true )
	{
		m_Node.ResetNiObject( m_CharProp.s_eInsertGeomType );
		m_Node.ActiveShader();
	}
	m_bInitialLoadPart = true;
}

void CsC_PartObject::_SetPart( NiNode* pPart, int nPartIndex )
{
	// 알아서 셋팅 된다.
	//// 본정보 제거 - 쓰레드에서 지우면 깨진다~ 왜? 나도 몰러 ㅡ.,ㅡ;
	//NiAVObject* pDetach = NULL;
	//int nChildCount = pPart->GetChildCount();
	//assert_csm2( nChildCount == 2, _T( "ModelID = %d, PartIndex = %d" ), GetFTID(), nPartIndex );
	//for( int i=0; i<nChildCount; ++i )
	//{
	//	pDetach = (NiNode*)pPart->GetAt( i );		
	//	if( strcmp( pDetach->GetName(), "Bip01" ) == 0 )
	//	{
	//		break;
	//	}
	//	// 하이드 된 오브젝트가 잇는가~ 경고
	//	assert_cs( pDetach->GetAppCulled() == false );
	//}
	//if( pDetach )
	//{
	//	pDetach->SetSelectiveUpdate( false );
	//	pDetach->SetSelectiveUpdateTransforms( false );
	//	pDetach->SetSelectiveUpdatePropertyControllers( false );
	//	pDetach->SetSelectiveUpdateRigid( true );
	//}

#ifndef _DEBUG
	assert_csm3( NiGetCollisionData( pPart ) == NULL, _T( "ModelID = %d, ItemID = %d, PartIndex = %d" ), GetFTID(), m_PartInfo[nPartIndex].s_nFileTableID, nPartIndex );
#endif

	//========================================================================================
	// 새 메쉬에 본의 포인터 연결
	//========================================================================================
	// 본이 아닌 지오메트리를 찾는다
	std::list< NiAVObject* > list;
	nsCSGBFUNC::GetSkinGeometyList( &list, pPart );

#ifdef _GIVE
	if( list.empty() == true )
		return;
#else
	assert_cs( list.size() );
#endif
	
	NiGeometry* pGeom = NULL;
	std::list< NiAVObject* >::iterator it = list.begin();
	std::list< NiAVObject* >::iterator itEnd = list.end();
	for( ; it!=itEnd; ++it )
	{
		pGeom = (NiGeometry*)( *it );

		assert_csm3( pGeom, _T( "피직or스킨되어있는 메쉬를 찾을수 없음\n ModelID = %d, ItemID = %d, PartIndex = %d" ), GetFTID(), m_PartInfo[nPartIndex].s_nFileTableID, nPartIndex );
		NiSkinInstance* pSkin = pGeom->GetSkinInstance();
		assert_cs( pSkin != NULL );
		int nBoneCount = pSkin->GetSkinData()->GetBoneCount();
		NiAVObject* const* pBones = pSkin->GetBones();
		NiAVObject* pBoneObject;
		for( int i=0; i<nBoneCount; ++i )
		{
			// 이름으로 본 찾아서
			pBoneObject = m_Node.m_pNiNode->GetObjectByName( pBones[ i ]->GetName() );
			if( pBoneObject != NULL )
			{
				// 포인터 연결
				pSkin->SetBone( i, pBoneObject );
			}
		}
	}	

	// 장비 아이템일경우
	if( nPartIndex < nsPART::MAX_CHANGE_PART_COUNT )
	{
		// 코스츔 변경이면서 코스츔 테이블 인덱스가 0이 아니라면 - 코스츔 장착~ 파츠 제거
		if( ( nPartIndex == nsPART::Costume )&&( m_SettingPart[ nsPART::Costume ].s_nSettingID != 0 ) )
		{
			for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )
			{
				m_Node.m_pNiNode->DetachChildAt( i + nsPART::PART_NODE_CONSTANT );
				m_PartInfo[ i ].Reset();
			}
		}
		else
		{
			m_Node.m_pNiNode->DetachChildAt( nsPART::Costume + nsPART::PART_NODE_CONSTANT );
			m_PartInfo[ nsPART::Costume ].Reset();			
		}
	}
	//오라일 경우
	if(nPartIndex == nsPART::EquipAura)
	{
		
		//오라 아이템 차고 있는 상태에서 오라템 착용할 경우
		
		if( ( nPartIndex == nsPART::EquipAura )&&( m_SettingPart[ nsPART::EquipAura ].s_nSettingID != 0 ) )
		{
			m_Node.m_pNiNode->DetachChildAt( nsPART::EquipAura + nsPART::PART_NODE_CONSTANT );
			m_PartInfo[ nsPART::EquipAura ].Reset();
			
			for( int i=0; i<nsPART::MAX_APPLY_STATE_COUNT; ++i )
			{
				m_Node.m_pNiNode->DetachChildAt( i + nsPART::PART_NODE_CONSTANT );
				m_PartInfo[ i ].Reset();
			}
		}
		else
		{
			m_Node.m_pNiNode->DetachChildAt( nsPART::EquipAura + nsPART::PART_NODE_CONSTANT );
			m_PartInfo[ nsPART::EquipAura ].Reset();		
		}
		
	}

	//========================================================================================
	// 부모에 붙인다 - 기존에 있는것이 리턴되서 나온다 ( 스마트 포인터라서 알아서 제거 )
	//========================================================================================
	NiAVObjectPtr pCheck = m_Node.m_pNiNode->SetAt( nPartIndex + nsPART::PART_NODE_CONSTANT, pPart );

	CsGBVisible::GetPlag( m_dwVisiblePlag, m_Node.m_pNiNode );
}


//====================================================================================
//
//		Path
//
//====================================================================================
void CsC_PartObject::DeletePath()
{
	PARENT_CLASS::DeletePath();
}


bool CsC_PartObject::GetFileName_FromID( char* pOut, sCHANGE_PART_INFO* pPartInfo )
{
	switch( pPartInfo->s_nPartIndex )
	{
	case nsCONDITION_PART::Barcode:
		// 일반
		if( pPartInfo->s_nFileTableID == 1 )
			strcpy_s( pOut, OBJECT_NAME_LEN, "digi_clock.nif" );
		// 한정판
		else
			strcpy_s( pOut, OBJECT_NAME_LEN, "digi_clock2.nif" );
		return true;
	}

	assert_csm1( ( pPartInfo->s_nFileTableID == 0 ) || ( nsCsFileTable::g_pItemMng->IsItem( pPartInfo->s_nFileTableID ) == true ),  _T( "테이블에 존재하지않는 장비 착용 %d" ), pPartInfo->s_nFileTableID );
/************************************************************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************************************************************/
//파츠 비교에서 Type의 enum값이 29 다음이 31(오라)이어서 계산과정에 21을 빼는 부분은 오라만 따로 계산... 추후 수정 필요함																											   //
// 	if( ( pPartInfo->s_nFileTableID == 0 ) || (nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L != 31))//오라템이 아닐 때 chu8820														   //
// 	{																																																															   //
// 		assert_cs( ( pPartInfo->s_nFileTableID == 0 ) || ( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L - 21 == pPartInfo->s_nPartIndex ) );//원본은 이 한줄					   //
// 	}																																																															   //
// 	else																												//오라템일때 chu8820																												   //
// 	{																																																															   //
// 		assert_cs( ( pPartInfo->s_nFileTableID == 0 ) || ( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L - 22 == pPartInfo->s_nPartIndex ) );//오라 추가 chu8820				   //
// 	}																																																															   //
	if( pPartInfo->s_nFileTableID != 0 )																																																					   //
	{																																																															   //
		if( nsCsFileTable::g_pItemMng->IsItem( pPartInfo->s_nFileTableID ) == false )																																							   //
		{																																																														   //
			assert_csm2( false, _T( "FT = %d, nPart = %d" ), pPartInfo->s_nFileTableID, pPartInfo->s_nPartIndex );																															   //
			pPartInfo->s_nFileTableID = 0;																																																				   //
		}

		// 디지오라일 때
		if( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L == 31) 
		{
			if( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L - 22 != pPartInfo->s_nPartIndex )
			{
				assert_csm2( false, _T( "FT = %d, nPart = %d" ), pPartInfo->s_nFileTableID, pPartInfo->s_nPartIndex );
				pPartInfo->s_nFileTableID = 0;
			}
		}
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
		// 디지바이스일 때
		else if( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L == 53 )
		{
			if( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L - 40 != pPartInfo->s_nPartIndex )
			{
				assert_csm2( false, _T( "FT = %d, nPart = %d" ), pPartInfo->s_nFileTableID, pPartInfo->s_nPartIndex );
				pPartInfo->s_nFileTableID = 0;
			}
		}
#endif
		else
		{
			if( nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_nType_L - 21 != pPartInfo->s_nPartIndex )
			{
				assert_csm2( false, _T( "FT = %d, nPart = %d" ), pPartInfo->s_nFileTableID, pPartInfo->s_nPartIndex );
				pPartInfo->s_nFileTableID = 0;
			}
		}																																																														   //
	}						

/************************************************************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************************************************************/
	if( pPartInfo->s_nFileTableID == 0 )
	{
		switch( pPartInfo->s_nPartIndex )
		{
		case nsPART::Head:					strcpy_s( pOut, OBJECT_NAME_LEN, "Head.nif" );			return true;
		case nsPART::Coat:					strcpy_s( pOut, OBJECT_NAME_LEN, "Up.nif" );			return true;
		case nsPART::Glove:					strcpy_s( pOut, OBJECT_NAME_LEN, "Hand.nif" );			return true;
		case nsPART::Pants:					strcpy_s( pOut, OBJECT_NAME_LEN, "Down.nif" );			return true;
		case nsPART::Shoes:					strcpy_s( pOut, OBJECT_NAME_LEN, "Shoes.nif" );			return true;
		case nsPART::EquipAura:				return true;//chu8820
#ifdef SDM_TAMER_XGUAGE_20180628
		case nsPART::XAI:				return true;//chu8820
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
		case nsPART::Digivice:			return true;
#endif
		default:							assert_cs( false );
		}
	}

	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo();
/************************************************************************/
//오라 파츠 비교부분에서 Type의 enum값이 29 다음이 31(오라)이어서 계산과정에 21을 빼는 부분은 오라만 따로 계산... 추후 수정 필요함
// 	if(pFTItem->s_nType_L != 31)//오라가 아닐 때chu8820
// 	{
// 	assert_cs( pFTItem->s_nType_L - 21 == pPartInfo->s_nPartIndex );
// 	}else							  //오라일 때
// 	{
// 	assert_cs( pFTItem->s_nType_L - 22 == pPartInfo->s_nPartIndex );
// 	}
/************************************************************************/
	// 기간제 아이템일 경우에
	if( pFTItem->s_nUseTime_Min > 0 )
	{
		// 타입이 2일 경우에만
		if( pFTItem->s_btUseTimeType == 2 )
		{
			// 기간이 끝난 아이템일 경우에
			if( ( pPartInfo->s_nRemainTime == (DWORD)-1 )||( pPartInfo->s_nRemainTime <= _GetTimeTS() ) )
			{
				switch( pPartInfo->s_nPartIndex )
				{
				case nsPART::Head:					strcpy_s( pOut, OBJECT_NAME_LEN, "Head.nif" );			return true;
				case nsPART::Coat:					strcpy_s( pOut, OBJECT_NAME_LEN, "Up.nif" );			return true;
				case nsPART::Glove:					strcpy_s( pOut, OBJECT_NAME_LEN, "Hand.nif" );			return true;
				case nsPART::Pants:					strcpy_s( pOut, OBJECT_NAME_LEN, "Down.nif" );			return true;
				case nsPART::Shoes:					strcpy_s( pOut, OBJECT_NAME_LEN, "Shoes.nif" );			return true;	
				case nsPART::EquipAura:				return true; //chu8820
#ifdef SDM_TAMER_XGUAGE_20180628
				case nsPART::XAI:				return true;//chu8820
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
				case nsPART::Digivice:			return true;
#endif
				default:							return false;
				}
			}
		}
	}
	sprintf_s( pOut, OBJECT_NAME_LEN, "%s.nif", nsCsFileTable::g_pItemMng->GetItem( pPartInfo->s_nFileTableID )->GetInfo()->s_cNif );
	return true;
}



//====================================================================================
//
//		Card 용 파트 오브젝트
//
//====================================================================================
// 
// 
// CsC_CardObject::CsC_CardObject()
// {
// 	m_bInitialLoadPart = false;
// 	m_cWorkingFolder[ 0 ] = NULL;
// 
// 	// 파츠 정보 리셋
// 	for( int i=0; i<nsCard::MAX_TOTAL_COUNT; ++i )
// 		m_PartInfo[ i ].Reset();
// }
// 
// void CsC_CardObject::Delete()
// {	
// 	PARENT_CLASS::Delete();
// 	
// 	m_bInitialLoadPart = false;
// 	assert_cs( GetRefLinkPart() == 0 );
// 	assert_cs( m_queueReadyPart.Empty() == true );
// 
// 
// 	m_cWorkingFolder[ 0 ] = NULL;
// 
// 	// 파츠 정보 리셋
// 	for( int i=0; i<nsCard::MAX_TOTAL_COUNT; ++i )
// 		m_PartInfo[ i ].Reset();
// }
// 
// void CsC_CardObject::DeleteUpdate()
// {
// 	PARENT_CLASS::DeleteUpdate();
// 
// 	if( GetRefLinkPart() == 0 )
// 	{
// 		// GetRefLinkPart 찰나의 순간에 들어 올수도 있다
// 		assert_cs( GetRefOnThreadLoad()||GetRefLinkPart()||( m_bInitialLoadPart == true ) );
// 		return;
// 	}
// 
// 	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
// 	assert_cs( m_queueReadyPart.Empty() == false );
// 
// 	sLOAD_PART* pReadyPart = NULL;
// 	while( m_queueReadyPart.Get( pReadyPart ) )
// 	{
// 		if( pReadyPart->s_nPartIndex > - 1 )
// 		{
// 			SAFE_DELETE( pReadyPart->s_pNiNode );
// 			if( pReadyPart->s_pRefLoad )
// 			{
// 				pReadyPart->s_pRefLoad->DecreaseRef();
// 				pReadyPart->s_pRefLoad = NULL;
// 			}
// 		}
// 		else
// 		{
// 			// -1 이라면 레퍼런스 감소로 놓겠다
// 			DecreaseRefLinkPart();
// 		}
// 		pReadyPart->s_pNiNode = NULL;
// 		pReadyPart->s_pRefLoad = NULL;
// 		sLOAD_PART::DeleteInstance( pReadyPart );		
// 	}
// 	m_bInitialLoadPart = true;
// }
// 
// void CsC_CardObject::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
// {
// 	assert_cs( m_bInitialLoadPart == false );
// 
// 	// 폴더 설정	
// 	std::string strPath = GetModelPath();
// 	nsCSFILE::GetFilePathOnly( m_cWorkingFolder, OBJECT_PATH_LEN, strPath.c_str(), true );	
// 
// 	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );
// }
// 
// void CsC_CardObject::Update( float fDeltaTime, bool bAnimation )
// {
// 	_UpdateReadyPart();
// 	PARENT_CLASS::Update( fDeltaTime, bAnimation );
// }
// 
// void CsC_CardObject::Render()
// {
// 	PARENT_CLASS::Render();
// }
// 
// 
// void CsC_CardObject::ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM /*=true*/ )
// {
// 	//=======================================================================================
// 	//	기본 본 정보
// 	//=======================================================================================
// 	if( bIncludeBaseKFM == true )
// 	{
// 		assert_cs( GetFTID() != 0 );
// 		sTCardPart* pUint = (sTCardPart*)sTCUnit::NewInstance( sTCUnit::KFM_File, GetFTID() );
// 		pUint->s_Prop = m_CharProp;
// 		pUint->s_pLoadedObject = (CsC_Object*)this;
// 
// 		if( _IsFastPartLoad() )
// 			pUint->SetLoadType( sTCUnit::eFast );		
// 
// 		// 루트 정보
// 		std::string strPath = GetModelPath();
// 		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );
// 		assert_cs( pUint->s_dwLoadID != 0 );
// 		g_pThread->LoadChar( pUint );
// 	}
// 
// 
// 
// 	//=======================================================================================
// 	//	파트 정보
// 	//=======================================================================================
// 	sTCardPart* pUint = (sTCardPart*)sTCUnit::NewInstance( sTCUnit::CARD_File, GetFTID() );
// 	pUint->s_Prop = m_CharProp;
// 	pUint->s_pLoadedObject = (CsC_Object*)this;
// 	if( _IsFastPartLoad() )
// 		pUint->SetLoadType( sTCUnit::eFast );
// 
// 	pUint->s_ePartType = sTCardPart::PT_PART;
// 	// 파트 정보
// 	for( int i=0; i<nsCard::MAX_TOTAL_COUNT; ++i )
// 	{
// 
// 		strcpy_s( pUint->s_PartInfo[ i ].s_cPartPath, OBJECT_PATH_LEN, pItemCodeArray[ i ].s_cPartPath );
// 	
// 		//pUint->s_PartInfo[ i ].m_pPixelData =  pItemCodeArray[ i ].m_pPixelData;		
// 	}
// 
// 	
// 	// 쓰레드 로드
// 	assert_cs( pUint != NULL );	
// 	g_pThread->LoadChar( pUint );
// 
// 	
// }
// 
// void CsC_CardObject::DeletePart( int nPartIndex )
// {
// 	sTCUnitPart* pUint = (sTCUnitPart*)sTCUnit::NewInstance( sTCUnit::Part_Delete, GetFTID() );
// 	pUint->s_pLoadedObject = (CsC_Object*)this;
// 	pUint->s_nPartIndex = nPartIndex;
// 	g_pThread->LoadChar( pUint );
// }
// 
// void CsC_CardObject::ThreadCallBack_AttachPart( sLOAD_PART* pPart )
// {
// 	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
// 	m_queueReadyPart.Put( pPart );
// 
// 	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
// 	pDecreaseRefPart->s_nPartIndex = -1;
// 	m_queueReadyPart.Put( pDecreaseRefPart );
// }
// 
// void CsC_CardObject::ThreadCallBack_AttachPart( std::queue< sLOAD_PART* >* pPartQueue )
// {	
// 	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
// 	assert_cs( pPartQueue->size() > 0 );
// 	while( pPartQueue->size() )
// 	{
// 		m_queueReadyPart.Put( pPartQueue->front() );
// 		pPartQueue->pop();
// 	}
// 
// 	sLOAD_PART* pDecreaseRefPart = sLOAD_PART::NewInstance();
// 	pDecreaseRefPart->s_nPartIndex = -1;
// 	m_queueReadyPart.Put( pDecreaseRefPart );
// }
// 
// 
// void CsC_CardObject::_UpdateReadyPart()
// {
// 	if( GetRefLinkPart() == 0 )
// 		return;
// 
// 	_SYNCHRONIZE_THREAD_(&s_CS_UpdateReadyPart);
// 	assert_cs( m_queueReadyPart.Empty() == false );
// 	bool bPartChange = false;
// 
// 	sLOAD_PART* pReadyPart = NULL;
// 	while( m_queueReadyPart.Get( pReadyPart ) )
// 	{
// 		if( pReadyPart->s_nPartIndex > - 1 )
// 		{
// 			// 현재 연결된 KFM 과 ID가 같을 시에만
// 			if( pReadyPart->s_dwLoadID == m_dwLoadID )
// 			{
// 				// 파트 추가
// 				if( pReadyPart->s_pNiNode )
// 				{
// 					m_PartInfo[ pReadyPart->s_nPartIndex ].SetRefLoad( pReadyPart->s_pRefLoad );
// 					_SetPart( pReadyPart->s_pNiNode, pReadyPart->s_nPartIndex );
// 				}
// 				// 파트 제거
// 				else
// 				{
// 					m_Node.m_pNiNode->DetachChildAt( pReadyPart->s_nPartIndex + nsPART::PART_NODE_CONSTANT );
// 					m_PartInfo[ pReadyPart->s_nPartIndex ].Reset();
// 				}
// 				bPartChange = true;
// 			}			
// 		}
// 		else
// 		{
// 			// -1 이라면 레퍼런스 감소로 놓겠다
// 			DecreaseRefLinkPart();
// 			PostLinkObject();
// 		}
// 		pReadyPart->s_pNiNode = NULL;
// 		pReadyPart->s_pRefLoad = NULL;
// 		sLOAD_PART::DeleteInstance( pReadyPart );		
// 	}
// 
// 	if( bPartChange == true )
// 	{
// 		m_Node.ResetNiObject( m_CharProp.s_eInsertGeomType );
// 		m_Node.ActiveShader();
// 	}
// 	m_bInitialLoadPart = true;
// }
// 
// void CsC_CardObject::_SetPart( NiNode* pPart, int nPartIndex )
// {
// 
// 
// #ifndef _DEBUG
// 	assert_csm3( NiGetCollisionData( pPart ) == NULL, _T( "ModelID = %d, ItemID = %d, PartIndex = %d" ), GetFTID(), m_PartInfo[nPartIndex].s_nFileTableID, nPartIndex );
// #endif
// 
// 	//========================================================================================
// 	// 새 메쉬에 본의 포인터 연결
// 	//========================================================================================
// 	// 본이 아닌 지오메트리를 찾는다
// 	std::list< NiAVObject* > list;
// 	nsCSGBFUNC::GetSkinGeometyList( &list, pPart );
// 
// #ifdef _GIVE
// 	if( list.empty() == true )
// 		return;
// #else
// 	//assert_cs( list.size() );
// 
// 	if( list.empty() == true )
// 		return;
// #endif
// 	
// 	NiGeometry* pGeom = NULL;
// 	std::list< NiAVObject* >::iterator it = list.begin();
// 	std::list< NiAVObject* >::iterator itEnd = list.end();
// 	for( ; it!=itEnd; ++it )
// 	{
// 		pGeom = (NiGeometry*)( *it );
// 
// 		assert_csm3( pGeom, _T( "피직or스킨되어있는 메쉬를 찾을수 없음\n ModelID = %d, ItemID = %d, PartIndex = %d" ), GetFTID(), m_PartInfo[nPartIndex].s_nFileTableID, nPartIndex );
// 		NiSkinInstance* pSkin = pGeom->GetSkinInstance();
// 		assert_cs( pSkin != NULL );
// 		int nBoneCount = pSkin->GetSkinData()->GetBoneCount();
// 		NiAVObject* const* pBones = pSkin->GetBones();
// 		NiAVObject* pBoneObject;
// 		for( int i=0; i<nBoneCount; ++i )
// 		{
// 			// 이름으로 본 찾아서
// 			pBoneObject = m_Node.m_pNiNode->GetObjectByName( pBones[ i ]->GetName() );
// 			if( pBoneObject != NULL )
// 			{
// 				// 포인터 연결
// 				pSkin->SetBone( i, pBoneObject );
// 			}
// 		}
// 	}	
// 	
// 	//========================================================================================
// 	// 부모에 붙인다 - 기존에 있는것이 리턴되서 나온다 ( 스마트 포인터라서 알아서 제거 )
// 	//========================================================================================
// 	NiAVObjectPtr pCheck = m_Node.m_pNiNode->SetAt( nPartIndex + nsPART::PART_NODE_CONSTANT, pPart );
// 
// 	CsGBVisible::GetPlag( m_dwVisiblePlag, m_Node.m_pNiNode );
// }
// 
// 
// //====================================================================================
// //
// //		Path
// //
// //====================================================================================
// void CsC_CardObject::DeletePath()
// {
// 	PARENT_CLASS::DeletePath();
// }
// 
// 
// bool CsC_CardObject::GetFileName_FromID( char* pOut, sCHANGE_PART_INFO* pPartInfo )
// {	
// 
// 	/*
// 	if( pPartInfo->s_nFileTableID != 0 )
// 	{
// 		//
// 		//nsCsFileTable::g_pMaster_CardMng->GetMasterCard( pPartInfo->s_nFileTableID )->Path
// 	}
// 	*/
// 	/*
// 	if( pPartInfo->s_nFileTableID == 0 )
// 	{
// 		switch( pPartInfo->s_nPartIndex )
// 		{
// 		case nsCard::FP:					strcpy_s( pOut, OBJECT_NAME_LEN, "FrontPlate_Normal.nif" );		return true;
// 		case nsCard::BP:					strcpy_s( pOut, OBJECT_NAME_LEN, "BackPlate_Normal.nif" );		return true;
// 		case nsCard::BE:					strcpy_s( pOut, OBJECT_NAME_LEN, "BackEff.nif" );				return true;
// 		case nsCard::MFD:					strcpy_s( pOut, OBJECT_NAME_LEN, "MainFrame_Devimon.nif" );		return true;
// 		case nsCard::DSCR:					strcpy_s( pOut, OBJECT_NAME_LEN, "Description.nif" );			return true;
// 		case nsCard::GR:					strcpy_s( pOut, OBJECT_NAME_LEN, "Grade_Child.nif" );			return true;
// 		case nsCard::NM:					strcpy_s( pOut, OBJECT_NAME_LEN, "Name.nif" );					return true;
// 		case nsCard::TD:					strcpy_s( pOut, OBJECT_NAME_LEN, "Type_Virus.nif" );			return true;
// 		case nsCard::TE:					strcpy_s( pOut, OBJECT_NAME_LEN, "Attribute_Fire.nif" );		return true;
// 		case nsCard::FE:					strcpy_s( pOut, OBJECT_NAME_LEN, "FrontEff.nif" );				return true;
// 
// 		default:							assert_cs( false );
// 		}
// 	}
// 	*/
// 
// 	return true;
// }
#undef		PARENT_CLASS

