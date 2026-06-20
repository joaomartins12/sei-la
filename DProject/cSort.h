#pragma once

//////////////////////////////////////////////////////////////////////////
// 아이템 소트 관련 유틸 클래스.
//////////////////////////////////////////////////////////////////////////

class cSort
{
public:
	cSort(void);
	~cSort(void);

	virtual	bool	Update();
	virtual	void	Start();	// 나중에 추가 작업..
	//virtual	void	Send();
	//virtual	void	Next();
	//virtual void	Fail();
	//virtual void	Release();
	//CsCoolTimeSeq	GetCoolTime();

private:
	//	CsCoolTimeSeq	m_CoolTimeSeq;	// 소트 재사용 시간

	float			m_TotalTime;	// 전체 걸린 시간
	int				m_TotalCount;	// 전체 소트한 갯수
	float			m_OneTime;	// 1회 소트에 걸리는 시간
	int				m_Count;		// 현재 소트한 횟수

};
