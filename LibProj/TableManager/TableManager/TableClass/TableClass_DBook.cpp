
#include "stdafx.h"
#include "TableClass_DBook.h"

BEGIN_NAME_TABLE


CDBookTable::CDBookTable()
{

}
CDBookTable::~CDBookTable()
{

}

void CDBookTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"DBookDeckData.bin", this, &CDBookTable::LoadData_Deck);
	AddLoadTableEvent(L"DBookCompositionData.bin", this, &CDBookTable::LoadData_Compsition);
	AddLoadTableEvent(L"DBookOptionData.bin", this, &CDBookTable::LoadData_Option);
	AddLoadTableEvent(L"DBookExceptionData.bin", this, &CDBookTable::LoadData_Exception);
}

void CDBookTable::ReleaseTable()
{
	m_kDecks.clear();
	m_kDBookOption.clear();
	m_kException.clear();
}

bool CDBookTable::LoadData_Deck(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nUse = 0;
		pReader.pop( nUse );
		if( nUse )
		{
			DWORD dwGroupID = 0;
			pReader.pop( dwGroupID );

			sDBookDeck deck;
			
			do 
			{
				sDBookDeck::sCondition condi;
				pReader.pop( condi.s_nCondition );
				if( 0 == condi.s_nCondition )
					break;

				pReader.pop( condi.s_nAT_Type );
				pReader.pop( condi.s_nOption );
				pReader.pop( condi.s_nVal );
				pReader.pop( condi.s_nProb );
				pReader.pop( condi.s_nTime );
				deck.s_listConditions.push_back( condi );
			} while ( 1 );

			m_kDecks.insert( std::make_pair( dwGroupID, deck ) );
			pReader.pop( dwGroupID );
		}

		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CDBookTable::LoadData_Compsition(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwGroupID = 0;
		pReader.pop( dwGroupID );
		if( 0 == dwGroupID )
			break;

		DeckCont::iterator it = m_kDecks.find( dwGroupID );
		if( it != m_kDecks.end() )
		{
			sDBookDeck::sComposition comp;	
			pReader.pop( comp.s_dwBaseDIdx );
			pReader.pop( comp.s_nEvolslot );
			pReader.pop( comp.s_dwDestDIdx );
			it->second.s_listCompositions.push_back( comp );
		}

		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CDBookTable::LoadData_Option(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nNum = 0;
		pReader.pop( nNum );
		if( 0 == nNum )
			break;
		sDBookOption opt;
		pReader.pop( opt.s_dwIconNum );
		m_kDBookOption.insert( std::make_pair( nNum, opt ) );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CDBookTable::LoadData_Exception(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwExceptionDigimonIdx = 0;
		pReader.pop( dwExceptionDigimonIdx );
		if( 0 == dwExceptionDigimonIdx )
			break;
		m_kException.push_back( dwExceptionDigimonIdx );
		bSuccess = pReader.MoveNext();
	}
	return true;
}
END_NAME_TABLE