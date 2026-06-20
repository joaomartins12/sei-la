

#include "cMapRegion.h"

///#include "nlib/cFileData.h"


cMapRegion::cMapRegion(void)
{


}


cMapRegion::~cMapRegion(void)
{


}


bool cMapRegion::IsOpened(int nMapNo)
{
	xstop2(nMapNo<nLimit::Map, "nMapNo:%d limit:%d", nMapNo, nLimit::Map);

	if(m_nOpenRegion[nMapNo] & 0x80)
	{
		return true;
	}
	// 20151012 Log를 남기는 이유가 없어서 주석 처리 -> 남기는 이유 자체가 병맛임
	// SetOpenMap()에서 IsOpened()호출 false가 떨어져야 정상인데 로그를 남김, 남기고 싶으면 호출하는 쪽에서...
	//LOG("해당 맵(%d)이 오픈되지 않은 상태에서 region이 열려있음(%d)",nMapNo,m_nOpenRegion[nMapNo]);
	//xstop1(!m_nOpenRegion[nMapNo], "해당 맵(%d)이 오픈되지 않은 상태에서 region이 열려있음", nMapNo);
	return false;
}


bool cMapRegion::IsOpened(int nMapNo, int nRegionNo)
{
	xstop2(nMapNo<nLimit::Map, "nMapNo:%d limit:%d", nMapNo, nLimit::Map);

//	xstop1(m_nOpenRegion[nMapNo]&0x80, "맵(%d)이 오픈되지 않은 상태에서 region이 열려있음", nMapNo);
	if(!(m_nOpenRegion[nMapNo] & 0x80))
	{
		return false;
	}

//	xstop2(nRegionNo<nLimit::Region, "nRegionNo:%d limit:%d", nRegionNo, nLimit::Region);
	if(nRegionNo >= nLimit::Region)
	{
		return false;
	}

	if(m_nOpenRegion[nMapNo] & (0x01<<nRegionNo))
	{
		return true;
	}

	return false;
}


bool cMapRegion::SetOpenMap(int nMapNo)
{
	xstop2(nMapNo<nLimit::Map, "nMapNo:%d limit:%d", nMapNo, nLimit::Map);
	if(IsOpened(nMapNo))
	{
		// 이미 열려있음
		return false;
	}

	m_nOpenRegion[nMapNo] = 0x80; // 해당 맵을 오픈
	return true;
}


bool cMapRegion::SetOpenRegion(int nMapNo, int nRegionNo)
{
	xstop2(nMapNo<nLimit::Map, "nMapNo:%d limit:%d", nMapNo, nLimit::Map);
	xstop2(nRegionNo<nLimit::Region, "nRegionNo:%d limit:%d", nRegionNo, nLimit::Region);
	if(IsOpened(nMapNo, nRegionNo))
	{
		// 이미 열려있음
		return false;
	}

	m_nOpenRegion[nMapNo] |= (0x01<<nRegionNo);
	return true;
}







