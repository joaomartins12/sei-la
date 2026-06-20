#include "StdAfx.h"
#include "ContentsSystem.h"
#include "ContentsSystemDef.h"

//#include "../HotKeyEventMng.h"
// bool IUIContentBase::CResultCheck(CResult cResult, bool bNotifyMessage)
// {
// 	Nt::ResultChecker::CONT_STRING StrCont;
// 	if( RC_SUCCESS == Nt::ResultChecker::ResultDoCheck( cResult, StrCont ) )
// 		return true;
// 
// 	if( bNotifyMessage )
// 		Nt::ResultChecker::NotifyMessage( StrCont, ( GUIMGR_STPTR )?( GUIMGR_STPTR->GetCurrentPage() ):( NULL ) );
// 
// 	return false;
// }
// 
// bool IUIContentBase::CPopupDialogMessage( GUI::GUIPage* pPage, GUI::WindowEvent* pWE, std::wstring const& wstrMessage )
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::PopUpOKDialog( (pPage)? (pPage) : (GUIMGR_STPTR->GetCurrentPage()), (pWE)?pWE:NULL, 4, 5, 256, 128, wstrMessage );
// 	return true;
// }
// 
// bool IUIContentBase::CAddChatLogMessage(std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::AddChatLogMessage( wstrMessage );
// 	return true;
// }
// 
// bool IUIContentBase::CShowWarningMessage(std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::ShowWarningMessage( wstrMessage );
// 	return true;
// }
// 
// bool IUIContentBase::CShowNoticeMessage(std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::ShowNoticeMessage( wstrMessage );
// 	return true;
// }
// 
// bool IUIContentBase::CShowImpactNoticeMessage(std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::ShowImpactSystemMessage( wstrMessage );
// 	return true;
// }
// 
// bool IUIContentBase::CSendMessage(int iSendType, std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::SendMessage( iSendType, wstrMessage );
// 	return true;
// }
// 
// 
// bool IUIContentBase::CAddChatGuildSystemMessage(std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::AddGuildSystemMessage( wstrMessage );
// 	return true;
// }
// 
// bool IUIContentBase::CAddChatPartySystemMessage(std::wstring const& wstrMessage)
// {
// 	if( wstrMessage.empty() )
// 		return false;
// 
// 	Nt::ResultChecker::AddPartySystemMessage(wstrMessage );
// 	return true;
// }

IUIContentBase::ProtectedEventRouterRef IUIContentBase::ms_EventRouter = 0;

UIContentsEventRounter::UIContentsEventRounter(void)
{
}

UIContentsEventRounter::~UIContentsEventRounter(void)
{
}

ContentsSystem::ContentsSystem(void)
{
	IUIContentBase::ms_EventRouter = &m_kEventRouter;
}

ContentsSystem::~ContentsSystem(void)
{
	IUIContentBase::ms_EventRouter = 0;

	RemoveAll();
}

void ContentsSystem::BuildContents(void)
{
	for( int i = 0; i < E_CT_END; ++i )
	{
		RegistContents( i );
		//ECFLOG( "===== Contents Load(%d)\n",i );
	}
}

void ContentsSystem::RegistContents(int const& iContentsType)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.find( iContentsType );
	if( ct_itr == m_kContents.end() )
	{
		auto Result = m_kContents.insert( std::make_pair( iContentsType, ( IUIContentBase* )NULL ) );
		if( Result.second )
		{
			Result.first->second = UIContentsContructor( iContentsType );
			if( Result.first->second )
				Result.first->second->Initialize();
			else
			{
				m_kContents.erase( Result.first );
				//ECERROR( "===== Contents Init Fail(%d)\n",iContentsType );
			}
		}
	}
}

void ContentsSystem::IntraConnection(void)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->IntraConnection( *this );

		++ct_itr;
	}
}

bool ContentsSystem::InitializeContents(int const& iContentsType)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.find( iContentsType );
	if( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			return ct_itr->second->Initialize();
	}

	return false;
}

void ContentsSystem::EnterContents(int const& iContentsType)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.find( iContentsType );
	if( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->EnterContents();
	}
}

void ContentsSystem::ExitContents(int const& iContentsType)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.find( iContentsType );
	if( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->ExitContents();
	}
}

IUIContentBase* ContentsSystem::GetContents(int const& iContentsType)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.find( iContentsType );
	if( ct_itr != m_kContents.end() )
		return ct_itr->second;

	return 0;
}

void ContentsSystem::MakeMainActorData(void)
{
// 	if( HOTKEYEVENTMNG_STPTR )
// 		HOTKEYEVENTMNG_STPTR->SetHotKeyEventBlock(true);
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->MakeMainActorData();
		++ct_itr;
	}
}

void ContentsSystem::ClearMainActorData(void)
{
// 	if( HOTKEYEVENTMNG_STPTR )
// 		HOTKEYEVENTMNG_STPTR->SetHotKeyEventBlock(true);
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->ClearMainActorData();
		++ct_itr;
	}
}

void ContentsSystem::MakeWorldData(void)
{
// 	if( HOTKEYEVENTMNG_STPTR )
// 		HOTKEYEVENTMNG_STPTR->SetHotKeyEventBlock(false);
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->MakeWorldData();
		++ct_itr;
	}
}

void ContentsSystem::ClearWorldData(void)
{
// 	if( HOTKEYEVENTMNG_STPTR )
// 		HOTKEYEVENTMNG_STPTR->SetHotKeyEventBlock(true);
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->ClearWorldData();
		++ct_itr;
	}
}

void ContentsSystem::OnOverride(int const& p_iNextFlowID)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->OnOverride(p_iNextFlowID);
		++ct_itr;
	}
}

void ContentsSystem::OnResume(int const& p_nBeforFlowID)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->OnResume(p_nBeforFlowID);
		++ct_itr;
	}
}

void ContentsSystem::RemoveContents(int const& iContentsType)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.find( iContentsType );
	if( ct_itr == m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->UnInitialize();
		SAFE_DELETE( ct_itr->second );
		m_kContents.erase( ct_itr );
	}
}

void ContentsSystem::RemoveAll(void)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->UnInitialize();
		SAFE_DELETE( ct_itr->second );
		ct_itr = m_kContents.erase( ct_itr );
	}
}

void ContentsSystem::PrepareDestroy(void)
{
	m_kEventRouter.Notify( CONTENTS_EVENT::EEvt_PrepareDestroy );
}

void ContentsSystem::Update(float const& fElapsedTime)
{
	CONTENTS_CONTAINER::iterator ct_itr = m_kContents.begin();
	while( ct_itr != m_kContents.end() )
	{
		if( ct_itr->second )
			ct_itr->second->Update( fElapsedTime );
		++ct_itr;
	}
}

void ContentsSystem::Render(float const& fElapsedTime)
{
	while( !m_vecRenderNeedContents.empty() )
	{
		IUIContentBase *pContents = GetContents( m_vecRenderNeedContents.front() );
		if( pContents )
			pContents->Render( fElapsedTime );
		m_vecRenderNeedContents.pop();
	}
}

void ContentsSystem::AddRenderNeed( int const& iContentsType )
{
	m_vecRenderNeedContents.push( iContentsType );
}