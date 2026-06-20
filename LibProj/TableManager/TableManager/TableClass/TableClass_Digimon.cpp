
#include "stdafx.h"
#include "TableClass_Digimon.h"

BEGIN_NAME_TABLE


CDigimonTable::CDigimonTable()
{

}
CDigimonTable::~CDigimonTable()
{

}

void CDigimonTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"DigimonListData.bin", this, &CDigimonTable::_LoadData_Digimon );
	AddChangeStringEvent( ESTRING_TABLE_DIGIMON,  this, &CDigimonTable::_InsertString );
}

void CDigimonTable::ReleaseTable()
{
	m_kDigimons.clear();
}

bool CDigimonTable::_InsertString(ITbImpl const* pData)
{
	CSTable_Digimon const* pDString = static_cast<CSTable_Digimon const*>( pData );
	DigimonCont::iterator it = m_kDigimons.begin();
	for( ; it != m_kDigimons.end(); ++it )
		it->second.s_szName = pDString->GetName( it->first );
	return true;
}

bool CDigimonTable::_LoadData_Digimon(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwDigimonIDx = 0;
		pReader.pop( dwDigimonIDx );
		if( 0 == dwDigimonIDx )
			break;

		sDigimonData Ddata;
		pReader.pop( Ddata.s_dwModelID );
		pReader.pop( Ddata.s_nDigimonRank );
		pReader.pop( Ddata.s_eEvolutionType );
		pReader.pop( Ddata.s_eAttributeType );
		pReader.pop( Ddata.s_eFamilyType[0] );
		pReader.pop( Ddata.s_eFamilyType[1] );
		pReader.pop( Ddata.s_eFamilyType[2] );
		pReader.pop( Ddata.s_eBaseNatureType );
		pReader.pop( Ddata.s_eBaseNatureTypes[0] );
		pReader.pop( Ddata.s_eBaseNatureTypes[1] );
		pReader.pop( Ddata.s_eBaseNatureTypes[2] );
		pReader.pop( Ddata.s_dwBaseLevel );

		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_HP );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_DS );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_DefPower );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_Evasion );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_MoveSpeed );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_CriticalRate );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_AttPower );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_AttSpeed );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_AttRange );
		pReader.pop( Ddata.s_sBaseStat.s_nBaseStat_HitRate );

		pReader.pop( Ddata.s_nDigimonType );
		pReader.pop( Ddata.s_dwCharSize );

		for( int n = 0; n < 4; ++n )
		{
			sDigimonData::sSKILL skillInfo;
			pReader.pop( skillInfo.s_dwID );
			if( 0 == skillInfo.s_dwID )
				break;

			pReader.pop( skillInfo.s_nReqPrevSkillLevel );
			Ddata.s_Skill.insert( std::make_pair( n, skillInfo) );
		}

		m_kDigimons.insert( std::make_pair( dwDigimonIDx, Ddata ) );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE