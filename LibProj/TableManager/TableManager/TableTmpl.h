//---------------------------------------------------------------------------
//
// 파일명 : TableImp.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

BEGIN_NAME_TABLE

//////////////////////////////////////////////////////////////////////////

class ITbImpl : public GMemObject
{
public:
	virtual ~ITbImpl() {};

public:
	virtual void InitTable(int const& nServiceType) = 0;
	virtual void ReleaseTable() = 0;
};

//////////////////////////////////////////////////////////////////////////

class IEventHandlerBase : public GMemObject
{
public:
	virtual ~IEventHandlerBase() {};
	bool OnExecute( DataStream & pArgument )				{ return OnEvent(pArgument); }
	//bool OnExecute( SaveStream & pArgument )				{ return OnEvent(pArgument); }
	bool OnExecute( ITbImpl const* pArgument )				{ return OnEvent(pArgument); }

	virtual std::wstring GetFile() const = 0;
	virtual int GetSTableIdx() const = 0;

private:
	virtual bool OnEvent(DataStream&) = 0;
	//virtual bool OnEvent(SaveStream&) = 0;
	virtual bool OnEvent(ITbImpl const*) = 0;
};


template <class T>
class TableEventHandler : public IEventHandlerBase
{
public:
	typedef bool (T::*MemberFunc1)(DataStream&);	
	//typedef bool (T::*MemberFunc2)(SaveStream&);	
	typedef bool (T::*MemberFunc3)(ITbImpl const*);

	TableEventHandler(T* instance, std::wstring const& wsFile, MemberFunc1 p_pFunc1)
		:  _instance( instance ),m_fileName(wsFile), m_fpEvent1( p_pFunc1 ), /*m_fpEvent2( NULL ),*/m_fpEvent3(NULL),m_nSTableIdx(0)
	{}

// 	TableEventHandler(T* instance, std::wstring const& wsFile, MemberFunc2 p_pFunc2)
// 		:  _instance( instance ),m_fileName(wsFile), m_fpEvent1( NULL ), m_fpEvent2( p_pFunc2 ),m_fpEvent3(NULL),m_nSTableIdx(0)
// 	{}

	TableEventHandler(T* instance, int const& nStIdx, MemberFunc3 p_pFunc3)
		:  _instance( instance ), m_fpEvent1( NULL ), /*m_fpEvent2( NULL ),*/m_fpEvent3(p_pFunc3),m_nSTableIdx(nStIdx)
	{}


	virtual ~TableEventHandler(void)
	{}		

	std::wstring GetFile() const override { return m_fileName; };
	int GetSTableIdx() const { return m_nSTableIdx; };

private:
	virtual bool OnEvent(DataStream& pArgument) override
	{
		if( _instance )
			return (_instance->*m_fpEvent1)(pArgument);
		return false;
	}

// 	virtual bool OnEvent(SaveStream& pArgument) override
// 	{
// 		if( _instance )
// 			return (_instance->*m_fpEvent2)(pArgument);
// 		return false;
// 	}

	virtual bool OnEvent(ITbImpl const* pArgument) override
	{
		if( _instance )
			return (_instance->*m_fpEvent3)(pArgument);
		return false;
	}

private:
	T*				_instance;			// 호출자 포인터 
	MemberFunc1		m_fpEvent1;			// 함수 포인터
//	MemberFunc2		m_fpEvent2;			// 함수 포인터
	MemberFunc3		m_fpEvent3;			// 함수 포인터
	std::wstring	m_fileName;
	int				m_nSTableIdx;
};

//-------------------------------------------------------------------------

class ITableEvent : public ITbImpl
{
	friend class CTableMgr;
public:
	typedef std::list<IEventHandlerBase*>	EventCont;

	virtual ~ITableEvent()
	{
		DeleteAllEvent();
	}

protected:
	template <class T>
	void AddLoadTableEvent( std::wstring const& strLoadFile, T* obj, bool (T::*memFn)(DataStream&) )
	{
		TableEventHandler<T> * p_EventHandler = GNew TableEventHandler<T>(obj, strLoadFile, memFn);
		if( NULL == p_EventHandler )
			return; // new 실패

		m_LoadTableEvent.push_back( p_EventHandler );
	}

// 	template <class T>
// 	void AddSaveTableEvent( std::wstring const& strLoadFile, T* obj, bool (T::*memFn)(SaveStream&) )
// 	{
// 		TableEventHandler<T> * p_EventHandler = GNew TableEventHandler<T>(obj,strLoadFile,memFn);
// 		if( NULL == p_EventHandler )
// 			return; // new 실패
// 
// 		m_SaveTableEvent.push_back( p_EventHandler );
// 	}

	virtual void DeleteAllEvent()
	{
		EventCont::iterator it = m_LoadTableEvent.begin();
		for( ; it != m_LoadTableEvent.end(); ++it )
			GDelete *it;
		m_LoadTableEvent.clear();

// 		it = m_SaveTableEvent.begin();
// 		for( ; it != m_SaveTableEvent.end(); ++it )
// 			GDelete *it;
// 
// 		m_SaveTableEvent.clear();
	}

private:
//	EventCont	m_SaveTableEvent;
	EventCont	m_LoadTableEvent;
};

//////////////////////////////////////////////////////////////////////////

class IDataTableEvent : public ITableEvent
{
	friend class CTableMgr;

public:
	virtual ~IDataTableEvent()
	{
		DeleteAllEvent();
	}

protected:
	template<class T>
	void AddChangeStringEvent( int nType, T* obj, bool (T::*memFn)(ITbImpl const*) )
	{
		TableEventHandler<T> * p_EventHandler = GNew TableEventHandler<T>(obj,nType,memFn);
		if( NULL == p_EventHandler )
			return; // new 실패

		m_ChangeStringEvent.push_back( p_EventHandler );
	}

	virtual void DeleteAllEvent()
	{
		ITableEvent::DeleteAllEvent();

		EventCont::iterator it = m_ChangeStringEvent.begin();
		for( ; it != m_ChangeStringEvent.end(); ++it )
			GDelete *it;

		m_ChangeStringEvent.clear();
	}


private:
	EventCont	m_ChangeStringEvent;
};

//////////////////////////////////////////////////////////////////////////
END_NAME_TABLE
//---------------------------------------------------------------------------