#include "StdAfx.h"
#include "OptionUIBase.h"
#include "OptionUIChat.h"
#include "FileUIChatting.h"

#include "_Interface/Game/ChatBase.h"
//---------------------------------------------------------------------------
//	채팅창 설정
CFileUIChatting::CFileUIChatting(void)
{
	AddPath("MAIN");
}

CFileUIChatting::~CFileUIChatting(void)
{
}

void CFileUIChatting::Write(TiXmlElement* root)
{
	TiXmlElement* pElement = SetCommonAttribute(root);
	pElement->SetAttribute(UI_SHOW_ETC,	m_UiData.AddShow? 1:0);
}

bool CFileUIChatting::Read(TiXmlElement* root)
{
	GetCommonAttribute(root);
	BoolAttribute(root, UI_SHOW_ETC, &m_UiData.AddShow);
	return true;
}

void CFileUIChatting::InitDefault()
{
	ChatBase::stExpandData	Expand;
	m_UiData.ExpandLevel = 2;
	m_UiData.Pos = CsPoint( 2, g_nScreenHeight -CHAT_SIZE_HEIGHT -50 -Expand.GetHeight(2) );
	m_UiData.AddShow = false;
}