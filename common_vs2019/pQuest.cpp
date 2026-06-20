


#include "pQuest.h"


//#define _DebugQuest	// 테스트가 필요한 경우


using namespace nQuest;

cSource::cSource(void) :
		m_pRequire(null),
		m_pPreSupply(null),
		m_pComplete(null),
		m_pResult(null),
		m_nRequire(0),
		m_nPreSupply(0),
		m_nComplete(0),
		m_nResult(0),
		m_bIsReset(false)
{


}


cSource::~cSource(void)
{

	xdelete_array(m_pRequire);
	xdelete_array(m_pPreSupply);
	xdelete_array(m_pComplete);
	xdelete_array(m_pResult);
}


void cSource::InitRequire(uint nNum)
{
	m_nRequire = nNum;
	if(!m_nRequire)
	{
		return;
	}

	m_pRequire = xnew nQuest::sRequire[m_nRequire];
}

void cSource::InitPreSupply(uint nNum)
{
	m_nPreSupply = nNum;
	if(!m_nPreSupply)
	{
		return;
	}

	m_pPreSupply = xnew nQuest::sPreSupply[m_nPreSupply];
}


void cSource::InitComplete(uint nNum)
{
	m_nComplete = nNum;
	if(!m_nComplete)
	{
		return;
	}

	m_pComplete = xnew nQuest::sComplete[m_nComplete];
}

void cSource::InitResult(uint nNum)
{
	m_nResult = nNum;
	if(!m_nResult)
	{
		return;
	}

	m_pResult = xnew nQuest::sResult[m_nResult];
}






cInfo::cInfo(void)
{
	memset(this, 0, sizeof(cInfo));

}


cInfo::~cInfo(void)
{


}


void cInfo::OutputDebugInfo(void)
{


	DBG("begin");


//	for(int i=1; i<nLimit::Quest; i++)
	for(int i=1; i< (nLimit::Quest * nLimit::MaxQuestSetCount); i++)
	{
		switch(Check(i))
		{
			case nQuest::Complete	: DBG("C : %d", i);	break;
			case nQuest::Execute	: DBG("E : %d", i);	break;
			case nQuest::None		: break;
			default					: DBG("N : %d", i);	break;
		}
	}

	DBG("end...");


}


void cInfo::on(uint nQuestIDX)
{
	-- nQuestIDX;
	uint nBitIDX = nQuestIDX % 32;
	uint nArrIDX = nQuestIDX / 32;

	m_nComplete[nArrIDX] |= (1<<nBitIDX);
}


void cInfo::off(uint nQuestIDX)
{
	-- nQuestIDX;
	uint nBitIDX = nQuestIDX % 32;
	uint nArrIDX = nQuestIDX / 32;

	m_nComplete[nArrIDX] &= ~(1<<nBitIDX);
}


uint cInfo::Check(uint nQuestIDX)
{
	if(nQuestIDX == 0)
	{
		return nQuest::Complete; // 0인 경우 아이템 사용/이용 시 체크를 편하게 하기 위함
	}


//	if(nQuestIDX >= nLimit::Quest)
	if(nQuestIDX >= (nLimit::Quest * nLimit::MaxQuestSetCount))
	{
//		DBG("nQuestIDX:%d < %d\n", nQuestIDX, nLimit::Quest);
		//DBG("nQuestIDX:%d < %d\n", nQuestIDX, (nLimit::Quest * nLimit::MaxQuestSetCount));
		return nQuest::Error;
	}

	uint x = nQuestIDX - 1;
	uint nBitIDX = x % 32;
	uint nArrIDX = x / 32;

	if(m_nComplete[nArrIDX] & (1<<nBitIDX))
	{
		return nQuest::Complete; // 완료
	}

	for(int i=0; i<nQuest::MaxExecute; i++)
	{
		if(m_pExecute[i].m_nIDX == nQuestIDX)
		{
			return nQuest::Execute; // 수행중
		}
	}

	return nQuest::None; // 봉인된 상태
}


/*
nQuest::sExeInfo *cInfo::GetExeInfo(int nQuestIDX)
{
	for(int i=0; i<nQuest::MaxExecute; i++)
	{
		if(m_pExecute[i].m_nIDX == nQuestIDX)
		{
			return &m_pExecute[i];
		}
	}

	return null;
}
//*/


bool cInfo::Begin(uint nQuestIDX)
{
#ifdef _DebugQuest
	LOG("IDX:%d", nQuestIDX);
#endif //_DebugQuest

	switch(Check(nQuestIDX))
	{
	case nQuest::None		: break;
	case nQuest::Error		:
	case nQuest::Execute	:
	case nQuest::Complete	: return false;
	}

	for(int i=0; i<nQuest::MaxExecute; i++)
	{
		if(m_pExecute[i].m_nIDX == 0)
		{
			m_pExecute[i].m_nIDX = (u2)nQuestIDX;
			memset(m_pExecute[i].m_nCondition, 0, nQuest::MaxCondition);
			return true;
		}
	}

	DBG("퀘스트 오류, 이미 수행할 수 있는 퀘스트 수 를 모두 채웠습니다.");
	return false;
}


bool cInfo::Cancel(uint nQuestIDX)
{
#ifdef _DebugQuest
	LOG("IDX:%d", nQuestIDX);
#endif //_DebugQuest

	for(int i=0; i<nQuest::MaxExecute; i++)
	{
		if(m_pExecute[i].m_nIDX == nQuestIDX)
		{
			memset(&m_pExecute[i], 0, sizeof(sExeInfo));
			return true;
		}
	}

	return false;
}


uint cInfo::CheckAchieve(uint nQuestIDX )
{
	xassert(nQuestIDX, "퀘스트 번호 0" );

//	xassert2(nQuestIDX < nLimit::Quest, "퀘스트 번호(%d) 오류 max:%d", nQuestIDX, nLimit::Quest);
	xassert2(nQuestIDX < (nLimit::Quest * nLimit::MaxQuestSetCount), "퀘스트 번호(%d) 오류 max:%d", nQuestIDX, (nLimit::Quest * nLimit::MaxQuestSetCount));

	uint x = nQuestIDX - 1;
	uint nBitIDX = x % 32;
	uint nArrIDX = x / 32;

	return ( m_nComplete[nArrIDX] & (1<<nBitIDX) ) ? nQuest::Complete : nQuest::None;
}


bool cInfo::CompleteAchievement(uint nQuestIDX)
{
#ifdef _DebugQuest
	LOG("IDX:%d", nQuestIDX);
#endif //_DebugQuest

	switch(Check(nQuestIDX))
	{
	case nQuest::None		: break;
	case nQuest::Execute	:
	case nQuest::Error		:
	case nQuest::Complete	: return false;
	}

	on(nQuestIDX);
	return true;
}


bool cInfo::Complete(uint nQuestIDX)
{
#ifdef _DebugQuest
	LOG("IDX:%d", nQuestIDX);
#endif //_DebugQuest

	switch(Check(nQuestIDX))
	{
	case nQuest::Execute	: break;
	case nQuest::None		:
	case nQuest::Error		:
	case nQuest::Complete	: return false;
	}

	for(int i=0; i<nQuest::MaxExecute; i++)
	{
		if(m_pExecute[i].m_nIDX == nQuestIDX)
		{
			on(nQuestIDX);
			memset(&m_pExecute[i], 0, sizeof(sExeInfo));
			return true;
		}
	}

	return false;
}


bool cInfo::CheatComplete(uint nQuestIDX)
{
#ifdef _DebugQuest
	LOG("IDX:%d", nQuestIDX);
#endif // _DebugQuest

	switch(Check(nQuestIDX))
	{
	case nQuest::Error		:
	case nQuest::Complete	: break;
	case nQuest::Execute	: Cancel(nQuestIDX);
	case nQuest::None		: on(nQuestIDX);		return true;
	}

	OutputDebugInfo();

	return false;
}

bool cInfo::CheatClear(uint nQuestIDX)
{
#ifdef _DebugQuest
	LOG("IDX:%d", nQuestIDX);
#endif //_DebugQuest

	switch(Check(nQuestIDX))
	{
	case nQuest::None		:
	case nQuest::Error		: break;
	case nQuest::Execute	: Cancel(nQuestIDX);	return true;
	case nQuest::Complete	: off(nQuestIDX);		return true;
	}

	return false;
}






