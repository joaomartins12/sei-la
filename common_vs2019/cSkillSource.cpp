


#include "nlib/base.h"
#include "pParameter.h"
#include "cSkillSource.h"


cSkillSource::cSkillSource(void)
{
	memset(m_nAttribute, 0, sizeof(m_nAttribute));
	memset(m_szName, 0, sizeof(m_szName));
}


cSkillSource::~cSkillSource(void)
{


}


//bool cSkillSource::IsExpBuff(void)
//{
//	if(0x000000FE & get(nSkill::BuffBit))
//	{
//		return true;
//	}
//	return false;
//}
//
//
//bool cSkillSource::IsExpBuffParty(void)
//{
//	if(0x0000FE00 & get(nSkill::BuffBit))
//	{
//		return true;
//	}
//	return false;
//}
//
//bool cSkillSource::IsExpBuffExt(void)
//{
//	if(0xFF000000 & get(nSkill::BuffBit))
//	{
//		return true;
//	}
//	return false;
//}

//
uint cSkillSource::get(uint nAttr)
{
	xstop2(nAttr<nSkill::MaxAttribute, "(attr:%d) nSkill::MaxAttribute:%d", nAttr, nSkill::MaxAttribute);
	return m_nAttribute[nAttr];
}

//int cSkillSource::get(uint nAttr)
//{
//	xstop2(nAttr<nSkill::MaxAttribute, "(attr:%d) nSkill::MaxAttribute:%d", nAttr, nSkill::MaxAttribute);
//	return m_nAttribute[nAttr];
//}

void cSkillSource::set(uint nAttr, uint nVal)
{
	xstop3(nAttr<nSkill::MaxAttribute, "(attr:%d, val:%d) nSkill::MaxAttribute:%d", nAttr, nVal, nSkill::MaxAttribute);
	m_nAttribute[nAttr] = nVal;
}


int cSkillSource::GetReachedTime(int nDistance) // tick
{
	int nTick = get(nSkill::ReadyTime);

	int nSpeed = get(nSkill::Speed);
	int nAcceleration = get(nSkill::Acceleration);

	nTick += nSkill::GetReachedTime(nDistance, nSpeed, nAcceleration);


	return nTick; // ms
}



