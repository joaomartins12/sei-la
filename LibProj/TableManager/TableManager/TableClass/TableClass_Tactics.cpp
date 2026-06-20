
#include "stdafx.h"
#include "TableClass_Tactics.h"

BEGIN_NAME_TABLE

// 
// CTacticsTable::CTacticsTable()
// {
// 
// }
// CTacticsTable::~CTacticsTable()
// {
// 
// }
// 
// void CTacticsTable::InitTable(int const& nServiceType)
// {
// 	AddLoadTableEvent( L"Evo_Digitama.bin", this, &CTacticsTable::Load_EvoDigitama );
// 	AddLoadTableEvent( L"Evo_Explain.bin", this, &CTacticsTable::Load_EvoExplain );
// 	AddLoadTableEvent( L"Enchant_Item_Info.bin", this, &CTacticsTable::Load_EnchantItem );
// 	AddLoadTableEvent( L"Transcend_Info.bin", this, &CTacticsTable::Load_TranscendInfo );
// 	AddLoadTableEvent( L"Transcend_Evo.bin", this, &CTacticsTable::Load_TranscendEvo );
// 	AddLoadTableEvent( L"Transcend_Value.bin", this, &CTacticsTable::Load_TranscendValue );
// 	AddLoadTableEvent( L"Transcend_Value_EX.bin", this, &CTacticsTable::Load_TranscendValueEx );
// }
// 
// void CTacticsTable::ReleaseTable()
// {
// 	m_mapTactics.clear();
// 	m_mapTacticsExplain.clear();
// 	m_mapEnchantItemInfo.clear();
// 	m_mapEnchantStatInfo.clear();
// 	m_lTacticsTransInfo.clear();
// 	m_lTacticsTransEvo.clear();
// 	m_mapExpDigimonGroup.clear();
// }
// 
// bool CTacticsTable::Load_EvoDigitama(DataStream& pReader)
// {
// 	bool bSuccess = pReader.MoveFirst();
// 	while( bSuccess )
// 	{
// 		DWORD dwDigitamaIdx = 0;
// 		pReader.pop( dwDigitamaIdx );
// 		if( 0 == dwDigitamaIdx )
// 			break;
// 
// 		DWORD dwDigimonIdx = 0;
// 		pReader.pop( dwDigimonIdx );
// 		if( 0 == dwDigimonIdx )
// 			break;
// 
// 		for(int n = 0; n < 5; ++n)
// 		{
// 			pReader.pop( dwDigimonIdx );
// 		}
// 		
// 
// 		bSuccess = pReader.MoveNext();
// 	}
// }
// 
// bool CTacticsTable::Load_EvoExplain(DataStream& pReader)
// {
// 
// }
// 
// bool CTacticsTable::Load_EnchantItem(DataStream& pReader)
// {
// 
// }
// 
// bool CTacticsTable::Load_TranscendInfo(DataStream& pReader)
// {
// 
// }
// 
// bool CTacticsTable::Load_TranscendEvo(DataStream& pReader)
// {
// 
// }
// 
// bool CTacticsTable::Load_TranscendValue(DataStream& pReader)
// {
// 
// }
// 
// bool CTacticsTable::Load_TranscendValueEx(DataStream& pReader)
// {
// 
// }
END_NAME_TABLE