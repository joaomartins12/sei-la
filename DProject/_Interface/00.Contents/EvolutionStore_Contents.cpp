#include "stdafx.h"
#include "EvolutionStore_Contents.h"


// bool cEvolutionStoreContents::sItemToDigimonData::IsEnableSend() const
// {
// 	if( m_bIsSenderData )
// 		return false; // 서버에 이미 패킷을 보낸 상태
// 
// 	if( m_wsMakeDigimonName.empty() ) // 만들려고하는 디지몬의 이름이 없음.
// 		return false;
// 
// 	if( 0 == m_dwMakeDigimonTbIDX )	// 만들려고하는 디지몬의 인덱스 없음.
// 		return false;		
// 
// 	return true;
// }

void cEvolutionStoreContents::sItemToDigimonData::ResetData()
{
	//m_bIsSenderData = false;
	m_wsMakeDigimonName.c_str();
	m_dwMakeDigimonTbIDX = 0;
}

void cEvolutionStoreContents::sItemToDigimonData::SetMakeDigimonTable( DWORD const& dwDigimonTbIDX )
{
	m_dwMakeDigimonTbIDX = dwDigimonTbIDX;
}

void cEvolutionStoreContents::sItemToDigimonData::SetMakeDigmonName(std::wstring const& wsName)
{
	m_wsMakeDigimonName = wsName;
}

// void cEvolutionStoreContents::sItemToDigimonData::SetSender()
// {
// 	m_bIsSenderData = true;
// }
// 
// bool cEvolutionStoreContents::sItemToDigimonData::IsSend() const
// {
// 	return m_bIsSenderData;
// }

//////////////////////////////////////////////////////////////////////////

// bool cEvolutionStoreContents::sDigimonToItemData::IsEnableSend() const
// {
// // 	if( m_bIsSenderData )
// // 		return false; // 서버에 이미 패킷을 보낸 상태
// 
// 	if( -1 == m_nTacticsSlotNum )
// 		return false;
// 
// 	if( m_sPassword.empty() )
// 		return false;		
// 
// 	return true;
// }

void cEvolutionStoreContents::sDigimonToItemData::ResetData()
{
	//m_bIsSenderData = false;
	m_sPassword.c_str();
	m_nTacticsSlotNum = -1;
}

void cEvolutionStoreContents::sDigimonToItemData::SetTacticsSlotNum( int const& nTacticsSlotNum )
{
	m_nTacticsSlotNum = nTacticsSlotNum;
}

void cEvolutionStoreContents::sDigimonToItemData::SetPassword( std::wstring const& sPassword )
{
	m_sPassword.clear();
	DmCS::StringFn::ToMB( sPassword, m_sPassword);
}

void cEvolutionStoreContents::sDigimonToItemData::SetPassword( std::string const& sPassword )
{
	m_sPassword = sPassword;
}

// void cEvolutionStoreContents::sDigimonToItemData::SetSender()
// {
// 	m_bIsSenderData = true;
// }
// 
// bool cEvolutionStoreContents::sDigimonToItemData::IsSend() const
// {
// 	return m_bIsSenderData;
// }

int cEvolutionStoreContents::sDigimonToItemData::GetTacticsSlotNum() const
{
	return m_nTacticsSlotNum;
}

std::string cEvolutionStoreContents::sDigimonToItemData::GetPassword() const
{
	return m_sPassword;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cEvolutionStoreContents::sMaterialData::SetMaterialEnable()
{
	bEnable = IsMaterialEnable();	
}

// 모든 재료가 있는지 검사
bool cEvolutionStoreContents::sMaterialData::IsMaterialEnable() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	switch( eMaterialType )
	{
	case eItemMaterial:
		{
			cData_Inven const* pInven = g_pDataMng->GetInven();
			SAFE_POINTER_RETVAL( pInven, false );

			if( pInven->GetCount_Item_ID( dwMaterialIDX ) < nCount )
				return false;

			return true;
		}
		break;
	case eDigimonMaterial:
		{
			cData_Tactics* pTactics = g_pDataMng->GetTactics();
			SAFE_POINTER_RETVAL( pTactics, false );
			int nTacticsCount = pTactics->GetTacticsCount();
			for( int n = 0; n < nTacticsCount; ++n )
			{
				cData_PostLoad::sDATA const* pData = pTactics->GetTactics( n );
				SAFE_POINTER_CON( pData );
				if( pData->s_dwBaseDigimonID != dwMaterialIDX )
					continue;

				if( pData->s_nLevel < nCount )
					continue;

				return true;
			}
		}
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void cEvolutionStoreContents::sExtraExchageData::SetAllMaterialEnable()
{
	std::list<sMaterialData>::iterator forIT = MainNeedMaterial.begin();
	for( ; forIT != MainNeedMaterial.end(); ++forIT )
		(*forIT).SetMaterialEnable();
	
	forIT = SubNeedMaterial.begin();
	for( ; forIT != SubNeedMaterial.end(); ++forIT )
		(*forIT).SetMaterialEnable();
}

// 선택된 아이템 및 디지몬으로 교환이 가능하지 체크
bool cEvolutionStoreContents::sExtraExchageData::IsEnableTrade() const
{
	switch( usWayType )
	{
	case 1:// 모든 재료가 있어야지 교환 가능 
		{
			std::list<sMaterialData>::const_iterator forIT = MainNeedMaterial.begin();
			for( ; forIT != MainNeedMaterial.end(); ++forIT )
			{
				if( !(*forIT).IsMaterialEnable() )
				{
					switch( (*forIT).eMaterialType )
					{
					case sMaterialData::eItemMaterial:
						cPrintMsg::PrintMsg( 30650, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					case sMaterialData::eDigimonMaterial:
						cPrintMsg::PrintMsg( 30702, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					}
					return false;
				}
			}

			forIT = SubNeedMaterial.begin();
			for( ; forIT != SubNeedMaterial.end(); ++forIT )
			{
				if( !(*forIT).IsMaterialEnable() )
				{
					switch( (*forIT).eMaterialType )
					{
					case sMaterialData::eItemMaterial:
						cPrintMsg::PrintMsg( 30650, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					case sMaterialData::eDigimonMaterial:
						cPrintMsg::PrintMsg( 30702, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					}
					return false;
				}
			}

			return true;
		}
		break;
	case 2:// 각각 한개의 재료만 있어도 교환 가능
		{
			bool bMainEnable = false;
			std::list<sMaterialData>::const_iterator forIT = MainNeedMaterial.begin();
			for( ; forIT != MainNeedMaterial.end(); ++forIT )
			{
				if( !(*forIT).IsMaterialEnable() )
					continue;

				bMainEnable = true;
				break;
			}

			if( !bMainEnable )
			{
				forIT = MainNeedMaterial.begin();
				if( forIT != MainNeedMaterial.end() )
				{
					switch( (*forIT).eMaterialType )
					{
					case sMaterialData::eItemMaterial:
						cPrintMsg::PrintMsg( 30650, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					case sMaterialData::eDigimonMaterial:
						cPrintMsg::PrintMsg( 30702, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					}
				}
				return false;
			}

			bool bSubEnable = false;
			forIT = SubNeedMaterial.begin();
			for( ; forIT != SubNeedMaterial.end(); ++forIT )
			{
				if( !(*forIT).IsMaterialEnable() )
					continue;

				bSubEnable = true;
				break;
			}

			if( !bSubEnable )
			{
				forIT = SubNeedMaterial.begin();
				if( forIT != SubNeedMaterial.end() )
				{
					switch( (*forIT).eMaterialType )
					{
					case sMaterialData::eItemMaterial:
						cPrintMsg::PrintMsg( 30650, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					case sMaterialData::eDigimonMaterial:
						cPrintMsg::PrintMsg( 30702, const_cast<TCHAR*>((*forIT).wsObjectName.c_str()) );
						break;
					}
				}

				return false;
			}
			return true;
		}
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
cEvolutionStoreContents::cEvolutionStoreContents():m_dwTargetNpcIdx(0),m_pTarget(NULL)
{
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_EXTRA_EVOLUTION_STORE, this, &cEvolutionStoreContents::openEvolutionStoreWindow );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CLOSE_EXTRA_EVOLUTION_STORE, this, &cEvolutionStoreContents::closeEvolutionStoreWindow );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ASK_DIGIMON_TO_ITEM, this, &cEvolutionStoreContents::_MessageAsk_DigimonToItem );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ASK_ITEM_TO_DIGIMON, this, &cEvolutionStoreContents::_MessageAsk_ItemToDigimon );
	//GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_DIGIMON_TO_ITEM, this, &cEvolutionStoreContents::_SendDigimonToItem );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_ITEM_TO_DIGIMON, this, &cEvolutionStoreContents::_SendItemToDigimon );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_ITEM_TO_DIGIMON, this, &cEvolutionStoreContents::_Recv_ItemToDigimon );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_DIGIMON_TO_ITEM, this, &cEvolutionStoreContents::_Recv_DigimonToItem );
}

cEvolutionStoreContents::~cEvolutionStoreContents()
{
	GAME_EVENT_ST.DeleteEventAll(this);
}


int const cEvolutionStoreContents::IsContentsIdentity(void)
{
	return E_CT_EVOLUTION_STORE_CONTENTS;
}

int const cEvolutionStoreContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cEvolutionStoreContents::Initialize(void)
{
	return true;
}

void cEvolutionStoreContents::UnInitialize(void)
{

}

bool cEvolutionStoreContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void cEvolutionStoreContents::Update(float const& fElapsedTime)
{

}

void cEvolutionStoreContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void cEvolutionStoreContents::MakeMainActorData(void)
{

}

void cEvolutionStoreContents::ClearMainActorData(void)
{

}

void cEvolutionStoreContents::MakeWorldData(void)
{
	_ResetData();
}

void cEvolutionStoreContents::ClearWorldData(void)
{
	_ResetData();
}

void cEvolutionStoreContents::_ResetData()
{
	m_pTarget = NULL;
	m_dwTargetNpcIdx = 0;
	m_mapExchangeData.clear();
	m_ItemToDigimon.ResetData();
	m_DigimonToItem.ResetData();
}

void cEvolutionStoreContents::CloseWindow(void)
{
	_ResetData();

	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::CHANGE_ITEM_COUNT, this );

	bool bIsEnable = true;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );

	cCreateName::DeleteInstance( cBaseWindow::WT_SPIRIT_EVOLUTION, 0 );

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SPIRIT_EVOLUTION );
}

CsC_AvObject* cEvolutionStoreContents::GetTargetNpc()
{
	return m_pTarget;
}

void cEvolutionStoreContents::openEvolutionStoreWindow(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pGameIF );

	CsC_AvObject* pTargetObject = static_cast<CsC_AvObject*>(pData);
	if( pTargetObject->GetLeafRTTI() != RTTI_NPC )
		return;

	DWORD dwTargetObjIdx = pTargetObject->GetFTID();
	if( m_dwTargetNpcIdx == dwTargetObjIdx )
		return;

	m_dwTargetNpcIdx = dwTargetObjIdx;
	m_pTarget = pTargetObject;

	_LoadExchageData( m_dwTargetNpcIdx );
	_CheckAllEnableMaterial();

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CHANGE_ITEM_COUNT, this, &cEvolutionStoreContents::changeMyItemCount );
	
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_SPIRIT_EVOLUTION );
}

std::wstring cEvolutionStoreContents::getNpcTypeFromUITitleName()
{
	SAFE_POINTER_RETVAL(m_pTarget, L"");
	CNpc* pNpc = dynamic_cast<CNpc*>(m_pTarget);
	SAFE_POINTER_RETVAL(pNpc, L"");

	CsNpc* pFTNpc = pNpc->GetFTNpc();
	SAFE_POINTER_RETVAL(pFTNpc, L"");

	CsNpc::sINFO* pNpcFTInfo = pFTNpc->GetInfo();
	SAFE_POINTER_RETVAL(pNpcFTInfo, L"");

	switch( pNpcFTInfo->s_eType )
	{
	case nsCsNpcTable::NT_SPIRIT_EVO:			return UISTRING_TEXT( "SPRITI_EVOLUTION_WINDOW_TITLE" );
	case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:	return UISTRING_TEXT( "EXTRA_EVOLUTION_WINDOW_TITLE" );
	}
	return L"";
}


void cEvolutionStoreContents::closeEvolutionStoreWindow(void* pData)
{
	CloseWindow();
}

void cEvolutionStoreContents::_LoadExchageData( DWORD const& dwTargetNpcIDX )
{
	m_mapExchangeData.clear();

	SAFE_POINTER_RET( nsCsFileTable::g_pExtraEvolutionTBMng );
	TB::MAP_EXTRAEVOLUTION_DATA const* pExchageData = nsCsFileTable::g_pExtraEvolutionTBMng->GetExtraEvolData( dwTargetNpcIDX );
	SAFE_POINTER_RET( pExchageData );

	TB::MAP_EXTRAEVOLUTION_DATA_CIT it = pExchageData->begin();
	for( ; it != pExchageData->end(); ++it )
	{
		unsigned short nExchageType = it->first;// 
		std::list<sExtraExchageData> addList;
		TB::LIST_EXCHAGEDATA_CIT secIT = it->second.begin();
		for( ; secIT != it->second.end(); ++secIT )
		{
			sExtraExchageData addData;
			addData.usWayType			= (*secIT).usWayType;
			addData.dwObjectIDX			= (*secIT).dwObjectIDX;
			addData.dwNeedMoney			= (*secIT).dwNeedMoney;

			switch( nExchageType )
			{
			case TB::eExchangeType::eItemToDigion:// 아이템으로 디지몬 진화, 대상이 디지몬
				{
					addData.wsExchageObjectName = GetDigimonName((*secIT).dwObjectIDX);
					addData.sDigimonIconName	= GetDigimonIconFile((*secIT).dwObjectIDX);
				}
				
				break;
			case TB::eExchangeType::eDigionToItem:// 디지몬을 아이템으로 교환
				{
					addData.wsExchageObjectName = GetItemNameName((*secIT).dwObjectIDX);
				}
				break;
			}		

			TB::LIST_MATERIALDATA_CIT mainMIt = (*secIT).listNeedMainMaterial.begin();
			for( ; mainMIt != (*secIT).listNeedMainMaterial.end(); ++mainMIt )
			{
				sMaterialData mainMItem;
				mainMItem.dwMaterialIDX	= (*mainMIt).dwMaterialIDX;
				
				if( TB::eExchangeType::eDigionToItem == nExchageType )
				{
					mainMItem.eMaterialType = sMaterialData::eDigimonMaterial;
					mainMItem.wsObjectName = GetDigimonName( mainMItem.dwMaterialIDX );
					mainMItem.sDigimonIconName = GetDigimonIconFile(mainMItem.dwMaterialIDX);
					mainMItem.nCount		= (*secIT).nNeedMaterialValue;
				}
				else
				{
					mainMItem.eMaterialType = sMaterialData::eItemMaterial;
					mainMItem.wsObjectName = GetItemNameName( mainMItem.dwMaterialIDX );
					mainMItem.nCount		= (*mainMIt).nCount;
				}
				addData.MainNeedMaterial.push_back( mainMItem );
			}


			TB::LIST_MATERIALDATA_CIT subMIt = (*secIT).listNeedSubMaterial.begin();
			for( ; subMIt != (*secIT).listNeedSubMaterial.end(); ++subMIt )
			{
				// 테이블상에 디지몬인지 아이템인지 알수 있는 방법이 없기때문에
				// 보조 재료는 무조건 아이템이여야 함.
				sMaterialData subMItem;
 				subMItem.dwMaterialIDX	= (*subMIt).dwMaterialIDX;
// 				if( TB::eExchangeType::eDigionToItem == nExchageType )
// 				{
// 					subMItem.eMaterialType = sMaterialData::eDigimonMaterial;
// 					subMItem.wsObjectName = GetDigimonName( subMItem.dwMaterialIDX );
// 					subMItem.sDigimonIconName = GetDigimonIconFile(subMItem.dwMaterialIDX);
// 					subMItem.nCount		= (*secIT).nNeedMaterialValue;
// 				}
// 				else
// 				{
					subMItem.eMaterialType = sMaterialData::eItemMaterial;
					subMItem.wsObjectName = GetItemNameName( subMItem.dwMaterialIDX );
					subMItem.nCount		= (*subMIt).nCount;
//				}
				addData.SubNeedMaterial.push_back( subMItem );
			}

			addList.push_back( addData );
		}

		m_mapExchangeData.insert( std::make_pair( nExchageType, addList ) );
	}
}

std::wstring cEvolutionStoreContents::GetDigimonName( DWORD const& dwDigimonIDX )
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pDigimonMng, L"");
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonIDX );
	SAFE_POINTER_RETVAL(pDigimon, L"");
	CsDigimon::sINFO* pInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL(pInfo, L"");
	return pInfo->s_szName;
}

std::wstring cEvolutionStoreContents::GetItemNameName( DWORD const& dwItemIDX )
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, L"");
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemIDX );
	SAFE_POINTER_RETVAL(pItem, L"");
	CsItem::sINFO* pInfo = pItem->GetInfo();
	SAFE_POINTER_RETVAL(pInfo, L"");
	return pInfo->s_szName;
}

std::string cEvolutionStoreContents::GetDigimonIconFile( DWORD const& dwDigimonIDX )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, "" );
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );

	DWORD dwDigimonModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( dwDigimonIDX );
	return g_pModelDataMng->GetSmallModelIconFile( dwDigimonModelID );
}

DWORD cEvolutionStoreContents::GetItemIconIndex( DWORD const& dwItemIDX )
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, 0);
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemIDX );
	SAFE_POINTER_RETVAL(pItem, 0);
	CsItem::sINFO* pInfo = pItem->GetInfo();
	SAFE_POINTER_RETVAL(pInfo, 0);
	return pInfo->s_nIcon;
}

cEvolutionStoreContents::sExtraExchageData const* cEvolutionStoreContents::GetExchageData( unsigned short const& nExchageType, DWORD const& dwObjectIDX, unsigned short const& usWayType)
{
	std::list<sExtraExchageData> const* pData = GetExchageTargetList( nExchageType );
	SAFE_POINTER_RETVAL( pData, NULL );

	std::list<sExtraExchageData>::const_iterator it = pData->begin();
	for( ; it != pData->end(); ++it )
	{
		if( (*it).dwObjectIDX != dwObjectIDX )
			continue;

		if( (*it).usWayType != usWayType )
			continue;
		
		return &(*it);
	}

	return NULL;
}

void cEvolutionStoreContents::_CheckAllEnableMaterial()
{
	std::map<int, std::list<sExtraExchageData>>::iterator it = m_mapExchangeData.begin();
	for( ; it != m_mapExchangeData.end(); ++it )
	{
		std::list<sExtraExchageData>::iterator subIT = it->second.begin();
		for( ; subIT != it->second.end(); ++subIT )
			(*subIT).SetAllMaterialEnable();		
	}
}

std::list<cEvolutionStoreContents::sExtraExchageData> const* cEvolutionStoreContents::GetExchageTargetList( unsigned short const& nExchageType ) const
{
	std::map<int, std::list<sExtraExchageData>>::const_iterator it = m_mapExchangeData.find( nExchageType );
	if( it == m_mapExchangeData.end() )
		return NULL;

	return &(it->second);
}

void cEvolutionStoreContents::SendExtraEvolution( unsigned short const& nExchageType, unsigned short const& usWayType, DWORD const& dwObjectIDX )
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	sExtraExchageData const* pExchangeData = GetExchageData( nExchageType, dwObjectIDX, usWayType );
	if( NULL == pExchangeData )
		return; // 데이터가 없다.

	// 돈 체크
	if( pExchangeData->dwNeedMoney > pInven->GetMoney().GetInt() )
	{
		cPrintMsg::PrintMsg( 11003 );
		return;
	}

	if( !pExchangeData->IsEnableTrade() )
		return;// 부족한 아이템 정보 출력 해 쥐야 함.

	//cPrintMsg::PrintMsg( 30650, nsCsFileTable::g_pItemMng->GetItem( m_pSelDispDigimon->s_nReqItem[i] )->GetInfo()->s_szName );

	std::wstring wsItemName = pExchangeData->wsExchageObjectName;
	switch( nExchageType )
	{
	case TB::eExchangeType::eItemToDigion:// 아이템으로 디지몬 진화
		{
 			cPrintMsg::PrintMsg( 30703, const_cast<TCHAR*>(wsItemName.c_str()), const_cast<TCHAR*>(wsItemName.c_str()) );
  			cMessageBox* pMsgBox = cMessageBox::GetMessageBox( 30703 );
 			if( pMsgBox )
			{
 				pMsgBox->SetValue1( 1 );
				pMsgBox->SetValue2( dwObjectIDX );// 만들려는 디지몬의 테이블 인덱스 
			}
		}
		break;
	case TB::eExchangeType::eDigionToItem:// 디지몬을 아이템으로 교환
		{
			// 중복되는 디지몬이 있는지 검사
			if( pExchangeData->MainNeedMaterial.empty() )
				return;

			std::list<sMaterialData>::const_iterator it = pExchangeData->MainNeedMaterial.begin();
			if( IsTacticsOverlapDigimon( (*it).dwMaterialIDX, (*it).nCount ) )
			{
				ContentsStream kTmp;
				kTmp << (*it).dwMaterialIDX, (*it).nCount;
				Notify( eOpenOverlapSelectUI, kTmp );
			}
			else
			{
				int nTacticsSlotNum = GetTacticsSlotNum( (*it).dwMaterialIDX, (*it).nCount );
				if( -1 == nTacticsSlotNum )
					return;

				cPrintMsg::PrintMsg( 30703, const_cast<TCHAR*>(wsItemName.c_str()), const_cast<TCHAR*>(wsItemName.c_str()));
				cMessageBox* pMsgBox = cMessageBox::GetMessageBox( 30703 );	// 디지몬에서 디지몬으로
				if( pMsgBox )
				{
					pMsgBox->SetValue1( 3 );
					pMsgBox->SetValue2( nTacticsSlotNum );
				}
			}
		}
		break;
	}
}

int cEvolutionStoreContents::GetTacticsSlotNum( DWORD const& dwDigimonTbIDX, int const& nLv )
{
	SAFE_POINTER_RETVAL( g_pDataMng, -1 );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, -1 );
	int nCount = pTactics->GetTacticsCount();
	for( int n = 0; n < nCount; ++n )
	{
		cData_PostLoad::sDATA const* pData = pTactics->GetTactics( n );
		SAFE_POINTER_CON( pData );
		if( pData->s_dwBaseDigimonID != dwDigimonTbIDX )
			continue;

		if( pData->s_nLevel < nLv )
			continue;

		return n;
	}
	return -1;
}

bool cEvolutionStoreContents::IsTacticsOverlapDigimon(DWORD const& dwDigimonTBIdx, int const& nLv)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, false );
	int nCount = pTactics->GetTacticsCount();
	int nOverlapCount = 0;
	for( int n = 0; n < nCount; ++n )
	{
		cData_PostLoad::sDATA const* pData = pTactics->GetTactics( n );
		SAFE_POINTER_CON( pData );
		if( pData->s_dwBaseDigimonID != dwDigimonTBIdx )
			continue;

		if( pData->s_nLevel < nLv )
			continue;

		nOverlapCount++;
	}

	if( nOverlapCount == 1 )
		return false;

	return true;
}

void cEvolutionStoreContents::DigimonToItem_SelectOverlapDigimon( int const& nSlotNum, DWORD const& dwDigimonTBIDX )
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	int nEmptyCnt = pInven->GetEmptySlotCount();	// 인벤토리 빈슬롯 체크
	if( nEmptyCnt == 0 )
	{
		cPrintMsg::PrintMsg( 11015 );	// 인벤토리에 여유 공간이 부족합니다.
		CloseOverlapDigimonSelectUI();
		return;
	}

	cPrintMsg::PrintMsg( 30701 );
	cMessageBox* pMsgBox = cMessageBox::GetMessageBox( 30701 );	// 디지몬에서 디지몬으로
	if( NULL == pMsgBox )
	{
		CloseOverlapDigimonSelectUI();
		return;
	}

	pMsgBox->SetValue1( nSlotNum );
	CloseOverlapDigimonSelectUI();
}

void cEvolutionStoreContents::changeMyItemCount(void* pData)
{
	_CheckAllEnableMaterial();
	Notify( eUpdateNeedItemCount );
}

void cEvolutionStoreContents::ClosePasswordInput()
{
	m_DigimonToItem.ResetData();
	Notify( eClosePasswordInputUI );
}

void cEvolutionStoreContents::CloseOverlapDigimonSelectUI()
{
	Notify( eCloseOverlapSelectUI );
}

void cEvolutionStoreContents::_MessageAsk_DigimonToItem(void* pData)
{
	SAFE_POINTER_RET( pData );

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	int nEmptyCnt = pInven->GetEmptySlotCount();	// 인벤토리 빈슬롯 체크
	if( nEmptyCnt == 0 )
	{
		cPrintMsg::PrintMsg( 11015 );	// 인벤토리에 여유 공간이 부족합니다.
		return;
	}

	int nTacticsSlotNum = *static_cast<int*>(pData);

	m_DigimonToItem.SetTacticsSlotNum( nTacticsSlotNum );

	Notify( eOpenPasswordInputUI );
}

// 디지몬을 아이템으로 
void cEvolutionStoreContents::SendDigimonToItem( std::wstring const& wsPassword)
{
	SAFE_POINTER_RET( net::game );

	if( wsPassword.empty() )
		return;

// 	if( m_DigimonToItem.IsSend() )
// 		return;

	m_DigimonToItem.SetPassword( wsPassword );
	

	net::game->SendSpirit_DigimonToSpirit( (u1)m_DigimonToItem.GetTacticsSlotNum() + 1, m_DigimonToItem.GetPassword().c_str(), m_dwTargetNpcIdx );

	//m_DigimonToItem.SetSender();
	ClosePasswordInput();
}


void cEvolutionStoreContents::_MessageAsk_ItemToDigimon(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RET( pTactics );

	if( NULL == pTactics->GetEmptyTactics() )		// 용병 슬롯 여유 공간 체크
	{
		cPrintMsg::PrintMsg( 20007 );	// 용병 슬롯에 빈 공간이 없습니다.
		return;
	}

	int nTradObjectID = *static_cast<int*>(pData);

	cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_SPIRIT_EVOLUTION, 0, CsPoint( -1, -1 ) );
	if( pCName )
		pCName->SetValue1( nTradObjectID );
}
// 아이템을 디지몬으로
void cEvolutionStoreContents::_SendItemToDigimon(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( net::game );

	if( 0 == m_dwTargetNpcIdx )
		return;

	std::pair<int,std::wstring> * pSendData = static_cast<std::pair<int,std::wstring> *>(pData);

	m_ItemToDigimon.SetMakeDigimonTable( pSendData->first );
	m_ItemToDigimon.SetMakeDigmonName( pSendData->second );
	
	net::game->SendSpirit_ItemToDigimon( pSendData->first, pSendData->second, m_dwTargetNpcIdx );
}

void cEvolutionStoreContents::_Recv_ItemToDigimon(void* pData)
{
	m_ItemToDigimon.ResetData();
	
	_CheckAllEnableMaterial();	
	Notify( eUpdateNeedItemCount );
}

void cEvolutionStoreContents::_Recv_DigimonToItem(void* pData)
{
	m_DigimonToItem.ResetData();

	_CheckAllEnableMaterial();
	Notify( eUpdateNeedItemCount );
}

// 디지몬 슬롯 번호와 아이콘 파일 이름을 얻어옴.
void cEvolutionStoreContents::GetTacticsDigimonSlotInfo( DWORD const& dwFindDigimonTBIDX, int const& nLv, std::list<std::pair<int,std::string>>& slotInfo )
{
	slotInfo.clear();
	SAFE_POINTER_RET( g_pDataMng );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RET( pTactics );
	int nCount = pTactics->GetTacticsCount();
	for( int n = 0; n < nCount; ++n )
	{
		cData_PostLoad::sDATA const* pData = pTactics->GetTactics( n );
		SAFE_POINTER_CON( pData );
		if( pData->s_dwBaseDigimonID != dwFindDigimonTBIDX )
			continue;

		if( pData->s_nLevel < nLv )
			continue;

		std::pair<int,std::string> addItem;
		addItem.first = n;
		addItem.second = GetDigimonIconFile( pData->s_dwBaseDigimonID );
		slotInfo.push_back( addItem );
	}
}