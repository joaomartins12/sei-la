
#include "StdAfx.h"
#include "Mail_Contents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptCashshop.h"

//////////////////////////////////////////////////////////////////////////

void MailContents::sMAILINFO::Reset()
{
	s_szSenderName.clear();
	s_szMessage.clear();
	s_nSendTimeTS = 0;
	s_nMailType = 0;
}

void MailContents::sMAILINFO::SetSenderName( std::wstring const& wsSender )
{
	s_szSenderName = wsSender;
}

void MailContents::sMAILINFO::SetMessage( std::wstring const& wsMessage )
{
	s_szMessage.clear();
	if( wsMessage.empty() )
		return;

	s_szMessage = wsMessage;
}

void MailContents::sMAILINFO::SetSendTime( uint const& time )
{
	s_nSendTimeTS = time;
}

void MailContents::sMAILINFO::SetMailType( u2 const& mailType )
{
	s_nMailType = mailType;
}


std::wstring MailContents::sMAILINFO::GetSenderName() const
{
	return s_szSenderName;
}

std::wstring MailContents::sMAILINFO::GetMessage() const
{
	return s_szMessage;
}

uint MailContents::sMAILINFO::GetSendTime() const
{
	return s_nSendTimeTS;
}

u2 MailContents::sMAILINFO::GetMailType() const
{
	return s_nMailType;
}

MailContents::sMAILINFO::MAIL_ITEMS const& MailContents::sMAILINFO::GetMailItems() const
{
	return s_Items;
}

//////////////////////////////////////////////////////////////////////////

int const MailContents::IsContentsIdentity(void)
{
	return E_CT_MAIL_SYSTEM;
}

MailContents::MailContents(void) : m_pCashshopContents(NULL)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_USER_MESSAGE, this, &MailContents::Ntf_User_Message );	
}

MailContents::~MailContents(void)
{
	m_pCashshopContents = 0;
	GAME_EVENT_ST.DeleteEventAll(this);
}

int const MailContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool MailContents::Initialize(void)
{

	return true;
}

void MailContents::UnInitialize(void)
{
}

bool MailContents::IntraConnection(ContentsSystem& System)
{ 
	m_pCashshopContents = new AdaptCashshop;
	if( m_pCashshopContents && !m_pCashshopContents->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_NEW_CASHSHOP ) ) )
		return false;

	return true; 
}

void MailContents::Update(float const& fElapsedTime)
{
}

void MailContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void MailContents::MakeMainActorData(void)
{
}

void MailContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

MailContents::sMAILINFO const& MailContents::GetMailInfo() const
{
	return m_listMail;
}

void MailContents::OpenMailWindow()
{
	if( g_pGameIF->GetAlime() )
		g_pGameIF->GetAlime()->Release();

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIL ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAIL );
}

void MailContents::SendMailConfirm()
{
	switch( m_listMail.GetMailType() )
	{
	case mail::eAccount:
		if( net::game )
			net::game->SendUserMessageConfirm();
		break;
	case mail::eTamer:
		if( net::game )
			net::game->SendTamerMessageConfirm();
		break;
	}

	m_listMail.Reset();
}

//////////////////////////////////////////////////////////////////////////
void MailContents::Ntf_User_Message(void* pData)
{
	GS2C_NTF_USER_MESSAGE* pRecv = static_cast<GS2C_NTF_USER_MESSAGE*>(pData);

	if( !pRecv->m_szMessage.empty() )
	{
		if( pRecv->m_szMessage[0] == '`' && pRecv->m_szMessage[1] == '1' )
		{
			if( net::game )
				net::game->SendTamerMessageConfirm();

			if( g_pGameIF->GetAlime() )
				g_pGameIF->GetAlime()->SetAlime( 310, cAlime::EVENT, 0 );
			return;
		}
	}

	m_listMail.Reset();
	m_listMail.SetSendTime( pRecv->m_nSendTimeTS );
	m_listMail.SetSenderName( pRecv->m_szSenderName );
	m_listMail.SetMessage( pRecv->m_szMessage );
	m_listMail.SetMailType( pRecv->m_nMailType );

	if( g_pGameIF->GetAlime() )
		g_pGameIF->GetAlime()->SetAlime( 310, cAlime::MAIL, 0 );

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIL_BUTTON ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAIL_BUTTON );
}

void MailContents::GetCashItemData( DWORD const& dwProductIDX, std::wstring & wsProductName, LIST_PACKAGE_ITEMS & listData )
{
	listData.clear();
	wsProductName.clear();

	SAFE_POINTER_RET( m_pCashshopContents );

	m_pCashshopContents->GetAllIncludePackageItemsFromProductIDX( dwProductIDX, wsProductName, listData );
}