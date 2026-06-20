#include "StdAfx.h"
#include "cSort.h"


#define	SEND_TIME 0.1f
cSort::cSort(void) : m_TotalTime(0.0f), m_TotalCount(0), m_OneTime(0.0f), m_Count(0)

//m_CoolTimeSeq(30.0f)	// 미사용 값.
{
}

cSort::~cSort(void)
{
}
bool cSort::Update()
{
	m_TotalTime += g_fDeltaTime;
	m_OneTime += g_fDeltaTime;
	if(m_OneTime >= SEND_TIME)	// 0.1초 마다 1회씩만 보내도록 한다.
	{
		m_TotalCount++;
		m_OneTime = 0.0f;
		m_Count = 0;
		return true;
	}

	return false;
}

//CsCoolTimeSeq cSort::GetCoolTime()
//{
//	return	m_CoolTimeSeq;
//}
void	cSort::Start()
{
	m_TotalTime = 0.0f;
	m_TotalCount= 0;
	m_OneTime = 0.0f;
	m_Count = 0;
}
//void	cSort::Send();
//void	cSort::Next();
//void	cSort::Fail();
//void	cSort::Release();