
#ifdef MINIGAME
inline	const ushort & MakeTacticsContents::GetStartBarTime(void) const 
{
	return iMinigame_StartBarTime;
}
inline	const ushort & MakeTacticsContents::GetClickResultNextBarIdx(void) const 
{
	return iMiniGame_ClickResult_NextBarIdx;
}
inline	const ushort & MakeTacticsContents::GetNextBarChargingTime(void) const
{
	return iMiniGame_NextBarChargingTime;
}
inline	const ushort & MakeTacticsContents::GetTimeOutNextBarTime(void) const
{
	return iMiniGame_TimeOut_NextBarTime;
}
inline	const uchar & MakeTacticsContents::GetSuccessCnt(void)const
{
	return iMiniGame_SuccessCnt;
}
inline	const bool & MakeTacticsContents::GetClickResult(void)const
{
	return bMiniGame_ClickResult;
}
inline const bool & MakeTacticsContents::GetMinigameComp(void)const
{
	return m_nIsGame;
}
inline const ushort & MakeTacticsContents::GetMinigameSuccessCnt(void)const
{
	return m_nSuccessCnt;
}
inline	void MakeTacticsContents::SetClickResult(const bool & iValue)
{
	bMiniGame_ClickResult = iValue;
}
inline	void MakeTacticsContents::SetStartBarChargingTime(const ushort& iValue)
{
	iMinigame_StartBarTime = iValue;
}
inline	void MakeTacticsContents::SetClickResultNextBarIdx(const ushort &iValue)
{
	iMiniGame_ClickResult_NextBarIdx = iValue;
}
inline	void MakeTacticsContents::SetNextBarChargingTime(const ushort &iValue)
{
	iMiniGame_NextBarChargingTime = iValue;
}
inline	void MakeTacticsContents::SetTimeOutNextBarTime(const ushort &iValue)
{
	iMiniGame_TimeOut_NextBarTime = iValue;
}
inline	void MakeTacticsContents::SetSuccessCnt(const uchar &iValue)
{
	iMiniGame_SuccessCnt = iValue;
}
inline void MakeTacticsContents::SetMinigameComp(const bool& bisGame)
{
	m_nIsGame = bisGame;
}
inline void MakeTacticsContents::SetMinigameSuccessCnt(const int& iSuccessCnt)
{
	m_nSuccessCnt = iSuccessCnt;
}
#endif