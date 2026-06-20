

#pragma once


class cRideSource
{
public:
	cRideSource(void);
   ~cRideSource(void);

public:
	int m_nIDX;				// 디지몬 고유 번호

public:
	int m_nSpeed;			// cm/s
	int m_nIsNotDetected;	// 선공몹으로 부터 인식 아니됨

public:
	int m_nIsPartyRide;		// 파티원이 같이 탈 수 있는가?

	int m_nFpUpTime;		// fp 올라가는 시간 간격(second)
	int m_nFpUpAmount;		// fp amount

	int m_nFsUpTime;		// fs 올라가는 시간 간격(second)
	int m_nFsUpAmount;		// fs amount
	int m_nFsUpMaxAmount;	// fs up max amount by riding mode

	int m_nOpenItemType1;	// 열때 소모되는 아이템 종류
	int m_nOpenItemCount1;	// 소모되는 아이템 갯수

	int m_nOpenItemType2;
	int m_nOpenItemCount2;

	int m_nUseItemType;		// 사용시 소모될 아이템 종류
	int m_nUseItemCount;	// 사용시 소모될 아이템 개수



};




