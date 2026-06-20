
#include "stdafx.h"
#include "TableClass_Item.h"

BEGIN_NAME_TABLE


CItemTable::CItemTable()
{

}
CItemTable::~CItemTable()
{

}

void CItemTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"ItemData.bin", this, &CItemTable::ReadItemData );
	AddLoadTableEvent( L"QuestItemData.bin", this, &CItemTable::ReadItemData );
	AddLoadTableEvent( L"DigioAuraEffect.bin", this, &CItemTable::ReadItemData_AuraEffect);
	AddChangeStringEvent( ESTRING_TABLE_ITEM, this, &CItemTable::InsertString );
}

void CItemTable::ReleaseTable()
{
	m_kItems.clear();
}

bool CItemTable::InsertString(ITbImpl const* pData)
{
	CSTable_Item const* pString = static_cast<CSTable_Item const*>(pData);
	ItemCont::iterator it = m_kItems.begin();
	for( ; it != m_kItems.end(); ++it )
	{
		CSTable_Item::sItemString const* pStr = pString->GetItemString(it->first);
		SAFE_POINTER_CON( pStr );
		it->second.s_szName			= pStr->name;
		it->second.s_szComment		= pStr->comment;
		it->second.s_szTypeComment	= pStr->typeComm;
	}
	return true;
}	

bool CItemTable::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		sItemData itemdata;
		pReader.pop( itemdata.s_dwItemID );
		pReader.pop( itemdata.s_nIcon );
		pReader.pop( itemdata.s_cNif );
		pReader.pop( itemdata.s_dwSkill );
		pReader.pop( itemdata.s_nSocketCount );
		pReader.pop( itemdata.s_btApplyRateMin );
		pReader.pop( itemdata.s_btApplyRateMax );
		pReader.pop( itemdata.s_btApplyElement );
		pReader.pop( itemdata.s_nClass );
		int nType = 0;
		pReader.pop( nType );
		pReader.pop( itemdata.s_nTypeValue );
		pReader.pop( itemdata.s_btCodeTag );
		int nOldType = 0;
		pReader.pop( nOldType );
		if( 0 == nType )
			nType = nOldType * 100;

		itemdata.s_nSection = nType;

		pReader.pop( itemdata.s_nSellType );
		pReader.pop( itemdata.s_nUseMode );
		pReader.pop( itemdata.s_nUseTimeGroup );
		pReader.pop( itemdata.s_nOverlap );
		pReader.pop( itemdata.s_nTamerReqMinLevel );
		pReader.pop( itemdata.s_nTamerReqMaxLevel );
		pReader.pop( itemdata.s_nDigimonReqMinLevel );
		pReader.pop( itemdata.s_nDigimonReqMaxLevel );
		pReader.pop( itemdata.s_nPossess );

		pReader.pop( itemdata.s_nEquipSeries );
		pReader.pop( itemdata.s_nUseCharacter );
		pReader.pop( itemdata.s_nDrop );
		pReader.pop( itemdata.s_dwDigiCorePrice );
		pReader.pop( itemdata.s_nEventItemType );
		pReader.pop( itemdata.s_dwEventItemPrice );
		pReader.pop( itemdata.s_dwScanPrice );
		pReader.pop( itemdata.s_dwSale );
		if( itemdata.s_dwSale == 0 && itemdata.s_dwItemID / 10000 != 9 )
			itemdata.s_dwSale = (DWORD)( itemdata.s_dwScanPrice*0.1f );

		pReader.pop( itemdata.s_cModel_Nif );
		pReader.pop( itemdata.s_cModel_Effect );
		pReader.pop( itemdata.s_bModel_Loop );
		pReader.pop( itemdata.s_bModel_Shader );
		pReader.pop( itemdata.s_dwSoundID );
		pReader.pop( itemdata.s_nBelonging );
		pReader.pop( itemdata.s_nQuest1 );
		pReader.pop( itemdata.s_nQuest2 );
		pReader.pop( itemdata.s_nQuest3 );

		pReader.pop( itemdata.s_nQuestRequire );
		pReader.pop( itemdata.s_nDigiviceSkillSlot );
		pReader.pop( itemdata.s_nDigiviceChipsetSlot );
		pReader.pop( itemdata.s_btUseTimeType );
		pReader.pop( itemdata.s_nUseTime_Min );
		pReader.pop( itemdata.s_bUseTimeType );
		pReader.pop( itemdata.s_nUseBattle );
		pReader.pop( itemdata.s_nDoNotUseType );
		
		ItemCont::_Pairib rib = m_kItems.insert( std::make_pair( itemdata.s_dwItemID, itemdata ) );
		if( !rib.second )
			_tprintf( _T("Error : Seam ItemCode : %d \n"), itemdata.s_dwItemID);
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CItemTable::ReadItemData_AuraEffect(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		DWORD dwItemIdx = 0;
		pReader.pop(dwItemIdx);
		if (0 == dwItemIdx )
			continue;
		std::wstring effectFile;
		pReader.pop(effectFile);
		m_kEffect.insert(std::make_pair(dwItemIdx, effectFile));
		bSuccess = pReader.MoveNext();
	}
	return true;
}
END_NAME_TABLE