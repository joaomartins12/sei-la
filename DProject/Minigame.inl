#ifdef MINIGAME

inline cMinigame::sBAR_N*		cMinigame::GetGauge( ushort iIdx ) 
{ 
	return &m_sBar[ iIdx ]; 
}
inline float cMinigame::UpdateCurrReadyCount(void)
{
	uint tmp = GetTickCount();
	float ftime = (float)( m_sBar[ 0 ].m_nStartTick + 2999 ) - (float)tmp;// ( 미니게임 시작시간 + 3초 ) - 현재 시간

	return ftime*0.001f;	// 카운트다운 남은시간
}
inline ushort		cMinigame::GetCurrGaugeIdx( void ) 
{ 
	return m_CurrGuageIndex;
}
inline ushort		cMinigame::GetSuccessCnt(void) 
{ 
	return m_nSuccessCnt; 
}
inline void	cMinigame::SetStartGauge( bool bStartGaugeUpdate )
{
	m_bStartGaugeUpdate = bStartGaugeUpdate; 
}
inline void	cMinigame::SetServRecv( bool bRecv ) 
{
	m_bServerRecv = bRecv; 
}
inline void	cMinigame::SetCurrGaugeIdx( ushort nBarIdx ) 
{ 
	m_CurrGuageIndex = nBarIdx; 
}
inline void	cMinigame::SetSuccessCnt( ushort nCnt ) 
{ 
	m_nSuccessCnt = nCnt; 
}

inline void cMinigame::SetGaugeStartTime()
{
	m_sBar[ m_CurrGuageIndex ].m_nStartTick = GetTickCount();
}


#endif