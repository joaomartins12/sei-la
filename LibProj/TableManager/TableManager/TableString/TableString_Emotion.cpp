
#include "stdafx.h"
#include "TableString_Emotion.h"

BEGIN_NAME_TABLE


CSTable_Emotion::CSTable_Emotion()
{

}
CSTable_Emotion::~CSTable_Emotion()
{

}

void CSTable_Emotion::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"TamerEmotion_Str.bin", this, &CSTable_Emotion::ReadItemData );
}

void CSTable_Emotion::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Emotion::ItemCont const& CSTable_Emotion::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Emotion::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nKey = 0;
		pReader.pop( nKey );
		
		std::list<std::wstring> command;
		
		do 
		{
			std::wstring comm;
			pReader.pop( comm );
			if( comm.empty() )
				break;

			command.push_back( comm );
		} while ( 1 );		

		m_kItems.insert( make_pair( nKey, command ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE