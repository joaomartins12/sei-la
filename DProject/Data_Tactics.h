
#pragma once 

class cData_Tactics
{
public:
	enum eTACTICS_INDEX{ INVALIDE_TACTICS_INDEX = INT_MAX, };
public:
	cData_Tactics();

public:
	void	Delete();
	void	Init();
	void	InitNetOff();

	// 용병
protected:
#ifdef CROSSWARS_SYSTEM
	cData_PostLoad::sDATA		m_TempTactics;
#endif

	int							m_nTacticsCount;
	cData_PostLoad::sDATA		m_Tactics[ nLimit::Memory ];

public:
	void						SetTacticsCount( int nCount ){ m_nTacticsCount = nCount; }
	int							GetTacticsCount() const{ return m_nTacticsCount; }

	cData_PostLoad::sDATA*		GetEmptyTactics();
	void						LinkTactics();
	void						AddTactics( cData_PostLoad::sDATA* pSrcTactics, int nTacticsIndex );
	void						DeleteTactics( int nTacticsIndex );
#ifdef CROSSWARS_SYSTEM
	cData_PostLoad::sDATA*		GetTempTactics(){ return &m_TempTactics; }
	void						DeleteTempTactics()	{ m_TempTactics.Delete(); }

	void						ChangeCrossToTactics( CsC_AvObject* pSrc, int nDestTacticsIndex );
#endif
	void						ChangeTactics( CsC_AvObject* pSrc, int nDestTacticsIndex );
	void						ChangeStatus(cData_PostLoad::sDATA*	pData, CsC_AvObject* pSrc);	// 용병 슬롯 정보를 최신정보로 갱신 해준다.	// 기존의 경우 DigimonUser만 서버에서 최신정보로 처리하기 때문에 미처리.

	cData_PostLoad::sDATA*		GetTactics( int nTacticsIndex );
	bool						IsTactics( int nTacticsIndex ) const;
	cData_PostLoad::sDATA*		GetJoGressMercenaries( DWORD const& dwOpenedJogressID, USHORT nMercenary, int* arrMercenaries, int& nSlotIndex, int& nGetError);	
	cData_PostLoad::sDATA*		GetJoGressMercenaries( DWORD const& dwOpenedJogressID, USHORT nMercenary, int* arrMercenaries, int& nSlotIndex, int& nGetError, int* pIndex );
	cData_PostLoad::sDATA*		GetXJogressMercenaries( DWORD const& dwOpenedJogressID, USHORT	nMercenary, int* arrMercenaries, int& nSlotIndex, int& nGetError );

	bool						IsSlotOpenedJoGressMercenaries( DWORD const& dwOpenedJogressID, cData_PostLoad::sDATA* pSrcMercenary );
	int							GetMercenaryID( DWORD dwMercenaryID );

	// 신규 변화용
protected:
	cData_PostLoad::sDATA		m_NextData;


protected:
	int				m_nOpenSlotCnt;	// 오픈된 소지 용병슬롯 수

public:
	void			_SetOpenSlot( int nCnt );
	int				_GetOpenSlot();


	static bool		Check_EnableChange();
};