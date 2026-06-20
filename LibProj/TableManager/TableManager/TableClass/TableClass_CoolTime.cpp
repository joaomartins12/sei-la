
#include "stdafx.h"
#include "TableClass_CoolTime.h"

BEGIN_NAME_TABLE


CCoolTimeTable::CCoolTimeTable()
{

}
CCoolTimeTable::~CCoolTimeTable()
{

}

void CCoolTimeTable::InitTable(int const& nServiceType)
{
	//AddLoadTableEvent(L"CoolTime.bin", this, &CCoolTimeTable::LoadData);
}

void CCoolTimeTable::ReleaseTable()
{
	//m_kItems.clear();
}

bool CCoolTimeTable::LoadData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE