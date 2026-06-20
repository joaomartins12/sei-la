#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class CShutDownContents
	: public IUIContentSubject< CShutDownContents >
{
public:
	struct sPlayerTimeData
	{
		int		m_nShutDown_Year;	// 서버에서 받은 유저 생년
		int		m_nShutDown_Month;	// 서버에서 받은 유저 생월
		int		m_nShutDown_Day;	// 서버에서 받은 유저 생일

		void	SetData( char const* pData );
	};
private:
	// 강제적 셧다운 데이터
	struct sCompulsoryShutdown
	{
		sCompulsoryShutdown():m_bShutDown_Check(false)
		{};

		bool m_bShutDown_Check;	// 만 15세미만 강제 셧다운 체크
		void Init();
		void Check(sPlayerTimeData const& PlayerData );
		bool Update_Check( int nLastCheckHour );
	};

	// 선택적 셧다운 데이터
	struct sSelectionShutdown
	{
		bool			m_bSelectionShutDown_Check;//만 18세 미만 선택적 셧다운 체크
		sShutdownWeek	m_sSelectionShutDownData;

		sSelectionShutdown():m_bSelectionShutDown_Check(false)
		{};

		void SetData( sShutdownWeek pData);
		void Check(sPlayerTimeData const& PlayerData);
		bool Update_Check( tm const& unSerTime );
		time_t GetShutDownStartTime( tm const& currentTime );
	};

public:
	static int const IsContentsIdentity(void);

public:
	CShutDownContents(void);
	virtual ~CShutDownContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	static void CALLBACK EventTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime );

private:
	void		RecvShutDownData(void* pData);
	void		RecvWeekShutDownData(void* pData);

private:
	sCompulsoryShutdown	m_sCompulsoryShutDown;
	sSelectionShutdown	m_sSelectionShutDown;
	sPlayerTimeData		m_sPlayerTimeData;
};
