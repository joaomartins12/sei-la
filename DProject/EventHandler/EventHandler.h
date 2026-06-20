//---------------------------------------------------------------------------
// 파일명 : EventHandler.h
//---------------------------------------------------------------------------

#pragma once

class EventHandlerBase
{
public:
	virtual ~EventHandlerBase() {};
	bool exec(void* pArgument)
	{
		__try
		{
			OnEvent(pArgument);
			return true;
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}
	}

private:
	virtual void OnEvent(void*) = 0;
};

template <class T>
class EventHandler : public EventHandlerBase, public NiMemObject
{
public:
	typedef void (T::*MemberFunc)(void*);

	EventHandler(T* instance, MemberFunc p_pFunc)
		:  _instance( instance ), m_fpEvent( p_pFunc )
	{}

	virtual ~EventHandler(void)
	{}		

	bool CompareCaller(T* pCaller)
	{
		return ( _instance == pCaller )?( true ):( false );
	}

	bool operator == (const EventHandler<T>& a) const
	{
		return ( (_instance == a._instance) && (m_fpEvent == a.m_fpEvent) )? true : false;
	}

private:
	virtual void OnEvent(void* pArgument)
	{
		if(_instance)
			(_instance->*m_fpEvent)(pArgument);
	}

private:
	T*			_instance;			// 호출자 포인터 
	MemberFunc	m_fpEvent;			// 함수 포인터
};


template <class T>
class UIEventHandler : public EventHandlerBase, public NiMemObject
{
public:
	typedef void (T::*MemberFunc)(void*,void*);

	UIEventHandler(T* instance, MemberFunc p_pFunc)
		:  _instance( instance ), m_fpEvent( p_pFunc ),m_pSender(NULL)
	{}

	virtual ~UIEventHandler(void)
	{}		

	bool CompareCaller(T* pCaller)
	{
		return ( _instance == pCaller )?( true ):( false );
	}

	void SetSender(void* pSender)
	{
		m_pSender = pSender;
	}

	bool operator == (const EventHandler<T>& a) const
	{
		return ( (_instance == a._instance) && (m_fpEvent == a.m_fpEvent) )? true : false;
	}

private:
	virtual void OnEvent(void* pArgument)
	{
		if(_instance)
			(_instance->*m_fpEvent)(m_pSender,pArgument);
	}

private:
	T*			_instance;			// 호출자 포인터 
	void*		m_pSender;
	MemberFunc	m_fpEvent;			// 함수 포인터
};
