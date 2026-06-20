
#pragma once 

#include "Reward.h"

class CsRewardMng
{
public:
	typedef	std::map< int, CsReward::MAP>				MAP_REWARD;
	typedef	std::map< int, CsReward::MAP>::iterator		MAP_REWARD_IT;

	void			Delete();
	bool			Init( char* cPath );

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	bool			IsReward( int nServerIdx, int nID );
	CsReward*		GetReward( int nServerIdx, int nID );
	CsReward::MAP*	GetReward( int nServerIdx );

public:
	void			SaveBin( char* cPath );	

private:
	void			_LoadExcelData( char* pTabName, int nIdx );

protected:
	MAP_REWARD		m_mapReward;
};
