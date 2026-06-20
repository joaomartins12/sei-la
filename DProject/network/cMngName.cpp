


#include "StdAfx.h"
#include "cMngName.h"


cMngName::cMngName(void)
{


}


cMngName::~cMngName(void)
{


}


void cMngName::Load(void)
{


}


void cMngName::Save(void)
{


}


char *cMngName::GetName(uint idx)
{
	auto pName = m_mName.find(idx);
	if(pName->second)
	{
		return pName->second->m_szName;
	}

	m_qReqName.push(idx);
	return null;
}


void cMngName::SendReqNameList(void)
{
	cPacket p;


}





