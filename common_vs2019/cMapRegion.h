

#pragma once

#include "pLimit.h"

// 0~6 : 맵의 특정 영역
//   7 : 해당 맵이 오픈되었는지 유무

class cMapRegion
{
public:
	cMapRegion(void);
   ~cMapRegion(void);

   // nOrderedMapNo == (dm_base.xls->맵정보->생성순서)
   // nRegionNo == (map.xls->region->map_idx 에서 맵별 순서(위에서부터))
public:
	bool IsOpened(int nOrderedMapNo);	// 해당 맵이 오픈되어 있는지
	bool IsOpened(int nOrderedMapNo, int nRegionNo); // 해당 맵의 해당 지역이 오픈되어 있는지

	bool SetOpenMap(int nOrderedMapNo);			// 이미 열려있다면 return false
	bool SetOpenRegion(int nOrderedMapNo, int nRegionNo);	// 이미 열려있다면 return false

public:
	u1 m_nOpenRegion[nLimit::Map];


};




