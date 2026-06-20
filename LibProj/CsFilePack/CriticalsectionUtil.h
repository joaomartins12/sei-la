#ifndef __CRITICALSECTIONUTIL_H__
#define __CRITICALSECTIONUTIL_H__


#pragma once

//////////////////////////////////////////////////////////////////////////
// 단순히 Criticalsection 을 클래스 로 묶은 별것 아닌 클래스
class CriticalsectionUtil
{
public:
	CriticalsectionUtil(int p_iSpinCount = 4000);
	virtual ~CriticalsectionUtil(void);

	BOOL TryEnter();
	virtual void Enter();
	virtual void Leave();

	CRITICAL_SECTION &GetCS() { return m_CS; }

	int GetSpinCount(){ return m_iSpinCount; }
private:

	CRITICAL_SECTION m_CS;
	int m_iSpinCount; 
};

//////////////////////////////////////////////////////////////////////////
// 단순히 Criticalsection 을 클래스 로 묶은 별것 아닌 클래스
class CSSpinLock : public CriticalsectionUtil
{
public:
	CSSpinLock(int p_iSpinCount = 4000);
	~CSSpinLock(void);

	void Enter();
};

//////////////////////////////////////////////////////////////////////////
// Criticalsection Enter Leave 를 내부 변수 선언만으로 자동화 함
class CSAutoLocker
{
public:
	CSAutoLocker( CriticalsectionUtil &p_refCsUtil );
	CSAutoLocker( CriticalsectionUtil *p_pCsUtil );
	~CSAutoLocker();

private:
	CSAutoLocker(){}

	void Enter();
	void Leave();

	CriticalsectionUtil *m_pCsUtil;

	BOOL m_bLock;
};

#define	AUTOLOCK( Y )	CSAutoLocker _autolock( Y )

#endif