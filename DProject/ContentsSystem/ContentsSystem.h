#pragma once

#include "ContentsStream.h"
#include <vector>
//-------------------------------------------------------------------------------------
template< typename EventType, typename EventReceiver >
class TEventRouter
{
public:
	TEventRouter(){};

	typedef EventType Event;
	typedef EventReceiver Receiver;
	typedef void (EventReceiver::*RoutedFunction)(Event const& Evt);
	typedef void (EventReceiver::*RoutedFunctionStream)(Event const& Evt, ContentsStream const& Stream);

public:
	typedef std::set< Receiver* > ReceiverContainer;
	typedef std::map< EventType, ReceiverContainer > ClassifiedContainer;

public:
	void Register(Event const& Evt, Receiver* pReceiver)
	{
		auto Rst = m_kClassified.insert( std::make_pair( Evt, ReceiverContainer() ) );
		Rst.first->second.insert( pReceiver );
	}

	void UnRegister(Event const& Evt, Receiver* pReceiver)
	{
		ClassifiedContainer::iterator it = m_kClassified.find( Evt );
		if( it != m_kClassified.end() )
		{
			ReceiverContainer& ReceiverContRef = it->second;
			ReceiverContainer::iterator recv_it = ReceiverContRef.find( pReceiver );
			if( recv_it != ReceiverContRef.end() )
				ReceiverContRef.erase( recv_it );

			if( ReceiverContRef.empty() )
				m_kClassified.erase( it );
		}
	}

	void UnRegisterAll(Receiver* pReceiver)
	{
		ClassifiedContainer::iterator it = m_kClassified.begin();
		while( it != m_kClassified.end() )
		{
			ReceiverContainer& ReceiverContRef = it->second;
			ReceiverContainer::iterator recv_it = ReceiverContRef.find( pReceiver );
			if( recv_it != ReceiverContRef.end() )
				ReceiverContRef.erase( recv_it );

			if( ReceiverContRef.empty() )
			{
				it = m_kClassified.erase( it );
				continue;
			}

			++it;
		}
	}

	void ClearAllRegistered(void)
	{
		m_kClassified.clear();
	}

protected:
	void RemoveReceiverInstance( Receiver* pReceiver )
	{
		if( pReceiver == NULL )
			return;

		ClassifiedContainer::iterator it = m_kClassified.begin();
		while( it != m_kClassified.end() )
		{
			ReceiverContainer& ReceiverContRef = it->second;
			ReceiverContainer::iterator recv_it = ReceiverContRef.find( pReceiver );
			if( recv_it != ReceiverContRef.end() )
				ReceiverContRef.erase( recv_it );

			if( ReceiverContRef.empty() )
			{
				it = m_kClassified.erase( it );
				continue;
			}

			++it;
		}
	}

	void Routed(Event const& Evt, RoutedFunction Function)
	{
		ClassifiedContainer::iterator it = m_kClassified.find( Evt );
		if( it != m_kClassified.end() )
		{
			ReceiverContainer& ReceiverContRef = it->second;
			std::vector<Receiver*> routedSnapshot;
			routedSnapshot.reserve( ReceiverContRef.size() );
			ReceiverContainer::iterator recv_it = ReceiverContRef.begin();
			while( recv_it != ReceiverContRef.end() )
			{
				routedSnapshot.push_back( *recv_it );
				++recv_it;
			}

			for( std::vector<Receiver*>::iterator snapshot_it = routedSnapshot.begin(); snapshot_it != routedSnapshot.end(); ++snapshot_it )
			{
				Receiver* pReceiver = *snapshot_it;
				if( pReceiver == NULL )
					continue;

				ClassifiedContainer::iterator verify_it = m_kClassified.find( Evt );
				if( verify_it == m_kClassified.end() )
					continue;

				ReceiverContainer& verifyRef = verify_it->second;
				if( verifyRef.find( pReceiver ) == verifyRef.end() )
					continue;

				( pReceiver->*Function )( Evt );
			}
		}
	}


	void Routed(Event const& Evt, ContentsStream const& Stream, RoutedFunctionStream Function)
	{
		ClassifiedContainer::iterator it = m_kClassified.find( Evt );
		if( it != m_kClassified.end() )
		{
			ReceiverContainer& ReceiverContRef = it->second;
			std::vector<Receiver*> routedSnapshot;
			routedSnapshot.reserve( ReceiverContRef.size() );
			ReceiverContainer::iterator recv_it = ReceiverContRef.begin();
			while( recv_it != ReceiverContRef.end() )
			{
				routedSnapshot.push_back( *recv_it );
				++recv_it;
			}

			for( std::vector<Receiver*>::iterator snapshot_it = routedSnapshot.begin(); snapshot_it != routedSnapshot.end(); ++snapshot_it )
			{
				Receiver* pReceiver = *snapshot_it;
				if( pReceiver == NULL )
					continue;

				ClassifiedContainer::iterator verify_it = m_kClassified.find( Evt );
				if( verify_it == m_kClassified.end() )
					continue;

				ReceiverContainer& verifyRef = verify_it->second;
				if( verifyRef.find( pReceiver ) == verifyRef.end() )
					continue;

				( pReceiver->*Function )( Evt, Stream );
				Stream.SetRp(0);
			}
		}
	}

protected:
	ClassifiedContainer m_kClassified;
};
//-------------------------------------------------------------------------------------
template< typename T >
class TReferenceGuard
{
public:
	typedef T ReferenceType;

public:
	TReferenceGuard(void) 
		: ReferencePointee( 0 ) 
	{}
	
	TReferenceGuard(T* pReference) 
		: ReferencePointee( pReference ) 
	{}

	TReferenceGuard(TReferenceGuard< T > const& Guard) 
		: ReferencePointee( Guard.ReferencePointee ) 
	{}

	~TReferenceGuard(void)
	{}
	
	void operator=(T* pReference) 
	{ 
		ReferencePointee = pReference; 
	}

	void operator=(TReferenceGuard< T > const& Guard) 
	{
		ReferencePointee = Guard.ReferencePointee; 
	}

	operator bool(void) const 
	{ 
		return ( ReferencePointee )?( true ):( false ); 
	}
	
	T* operator->(void) const 
	{ 
		return ReferencePointee; 
	}

private:
	T* ReferencePointee;
};
//-------------------------------------------------------------------------------------
template< typename T >
class TOwnershipGuard
{
public:
	typedef T OwnedType;

public:
	TOwnershipGuard(void) 
		: OwnedPointee( 0 ) 
	{}
	
	TOwnershipGuard(T* pOwned) 
		: OwnedPointee( pOwned ) 
	{}

	~TOwnershipGuard(void)
	{
		SAFE_DELETE( OwnedPointee );
	}

	void operator=(T* pOwned)
	{
		SAFE_DELETE( OwnedPointee );
		OwnedPointee = pOwned; 
	}

	operator bool(void) const 
	{ 
		return ( OwnedPointee )?( true ):( false ); 
	}
	
	T* operator->(void) const 
	{ 
		return OwnedPointee;
	}

private:
	T* OwnedPointee;
};
//-------------------------------------------------------------------------------------
template< typename T >
class TCreateGuard
{
public:
	typedef T InstanceType;

public:
	TCreateGuard(T* pInstance) 
		: InstancePointee( pInstance ) 
		, bFailed( true )
	{}

	~TCreateGuard(void)
	{
		if( bFailed )
			SAFE_DELETE( InstancePointee );
	}
	
	operator bool(void) const 
	{ 
		return ( InstancePointee )?( true ):( false ); 
	}
	
	T* operator->(void) const 
	{ 
		return InstancePointee; 
	}

	T* Succeeded(void) const
	{
		bFailed = false;
		return InstancePointee;
	}

private:
	TCreateGuard(TCreateGuard const& rhs);
	TCreateGuard& operator=(TCreateGuard const& rhs);

private:
	T* InstancePointee;
	mutable bool bFailed;
};
//-------------------------------------------------------------------------------------
class ContentsSystem;
class UIContentsEventRounter;
class IUIContentBase
{
	friend ContentsSystem;

public:
	typedef TReferenceGuard< UIContentsEventRounter > ProtectedEventRouterRef;
	typedef short CResult;

// public:
// 	static bool CResultCheck(CResult cResult, bool bNotifyMessage = true);
// 
// 	static bool CAddChatLogMessage(std::wstring const& wstrMessage);
// 	static bool CShowWarningMessage(std::wstring const& wstrMessage);
// 	static bool CShowNoticeMessage(std::wstring const& wstrMessage);
// 	static bool CShowImpactNoticeMessage(std::wstring const& wstrMessage);
// 	static bool CSendMessage(int iSendType, std::wstring const& wstrMessage);
// 	static bool CAddChatGuildSystemMessage(std::wstring const& wstrMessage);
// 	static bool CAddChatPartySystemMessage(std::wstring const& wstrMessage);
// 	static bool CPopupDialogMessage( GUI::GUIPage* pPage, GUI::WindowEvent* pWE, std::wstring const& wstrMessage );

public:
	virtual int const GetContentsIdentity(void) const = 0;
	virtual bool Initialize(void) = 0;
	virtual void UnInitialize(void) = 0;
	virtual bool IntraConnection(ContentsSystem& System) = 0;
	virtual void Update(float const& fElapsedTime) = 0;
	virtual void Render(float const& fElapsedTime){}
	virtual void NotifyEvent(int const& iNotifiedEvt) = 0;
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream){}
	virtual void MakeMainActorData(void) = 0;
	virtual void ClearMainActorData(void) = 0;
	virtual void MakeWorldData(void){}
	virtual void ClearWorldData(void){}
	virtual void OnOverride(int const& p_iNextFlowID){}
	virtual void OnResume(int const& p_nBeforFlowID){}
	virtual void EnterContents(void){}
	virtual void ExitContents(void){}

	IUIContentBase(void){}
	virtual ~IUIContentBase(void){}

protected:
	ProtectedEventRouterRef const& EventRouter(void){ return ms_EventRouter; }

private:
	static ProtectedEventRouterRef ms_EventRouter;
};

//-------------------------------------------------------------------------------------
class UIContentsEventRounter
	: public TEventRouter< int, IUIContentBase >
{
public:
	UIContentsEventRounter(void);
	virtual ~UIContentsEventRounter(void);

	void Notify(Event const& Evt)
	{ 
		Routed( Evt, &IUIContentBase::NotifyEvent ); 
	}

	void NotifyStream(Event const& Evt, ContentsStream const& kStream)
	{ 
		Routed( Evt, kStream, &IUIContentBase::NotifyEventStream ); 
	}
};
//-------------------------------------------------------------------------------------
template< typename T > class IUIContentSubject;
//-------------------------------------------------------------------------------------
template< typename SYSTEM >
class IUIContentObserver
{
	friend IUIContentSubject< SYSTEM >;

public:
	typedef SYSTEM SystemType;
	typedef TReferenceGuard< SystemType > ProtectedSystemRef;

public:
	virtual ~IUIContentObserver(void){}

protected:
	virtual void Notify(int const& iNotifiedEvt, ContentsStream const& kStream) = 0;

	ProtectedSystemRef const& GetSystem(void) const { return m_System; }
	void SetSystem(SystemType* pSystem){ m_System = pSystem; }
	void SetSystem(ProtectedSystemRef System){ m_System = System; }

private:
	ProtectedSystemRef m_System;
};
//-------------------------------------------------------------------------------------
template< typename T >
class IUIContentSubject 
	: public IUIContentBase
	, public TEventRouter< int, IUIContentObserver< T > >
{
public:
	typedef Receiver ObserverType;

public:
	static ContentsStream EmptyStream;

public:
	IUIContentSubject(void){}
	virtual ~IUIContentSubject(void){}

	void Notify(Event const& Evt, ContentsStream const& Stream = EmptyStream)
	{
		Routed( Evt, Stream, &IUIContentObserver< T >::Notify );
	}

	void NotifyContents(Event const& Evt)
	{
		if( EventRouter() )
			EventRouter()->Notify( Evt );
	}

	void NotifyContentsAndStream(Event const& Evt, ContentsStream const& Stream = EmptyStream)
	{
		if( EventRouter() )
			EventRouter()->NotifyStream( Evt, Stream );
	}
};

template< typename T >
ContentsStream IUIContentSubject< T >::EmptyStream;
//-------------------------------------------------------------------------------------
class ContentsSystem
	: public CSingleton< ContentsSystem >
{
public:
	static IUIContentBase* UIContentsContructor(int const& iContentsType);

public:
	typedef std::map< int, IUIContentBase* > CONTENTS_CONTAINER;

public:
	void BuildContents(void);
	void RegistContents(int const& iContentsType);
	void IntraConnection(void);
	bool InitializeContents(int const& iContentsType);
	void MakeMainActorData(void);
	void ClearMainActorData(void);
	void MakeWorldData(void);
	void ClearWorldData(void);
	void OnOverride(int const& p_iNextFlowID);
	void OnResume(int const& p_nBeforFlowID);
	IUIContentBase* GetContents(int const& iContentsType);
	void RemoveContents(int const& iContentsType);
	void RemoveAll(void);
	void PrepareDestroy(void);
	void EnterContents(int const& iContentsType);
	void ExitContents(int const& iContentsType);

	template< typename T >
	T* GetContents(int const& iContentsType)
	{ 
		return dynamic_cast< T* >( GetContents( iContentsType ) ); 
	}

	void AddRenderNeed( int const& iContentsType );
	
	void Update(float const& fElapsedTime);
	void Render(float const& fElapsedTime);

	ContentsSystem(void);
	virtual ~ContentsSystem(void);

private:
	CONTENTS_CONTAINER m_kContents;
	UIContentsEventRounter m_kEventRouter;
	std::queue<int> m_vecRenderNeedContents;
};

#define	CONTENTSSYSTEM_PTR	ContentsSystem::GetSingletonPtr()
