#include "StdAfx.h"
#include "OptionUIBase.h"
#include "FileUISystem.h"

#include "_Interface/Game/ChatBase.h"

CFileUISystem::CFileUISystem(void)
{
	AddPath("SYSTEM");
}

CFileUISystem::~CFileUISystem(void)
{
}

void CFileUISystem::Write(TiXmlElement* root)
{
	SetCommonAttribute(root);
}

bool CFileUISystem::Read(TiXmlElement* root)
{
	GetCommonAttribute(root);
	return true;
}

void CFileUISystem::InitDefault()
{
	ChatBase::stExpandData	Expand;
	m_UiData.ExpandLevel = 0;
	m_UiData.Pos = CsPoint( 2, g_nScreenHeight -CHAT_SIZE_HEIGHT*2 -50 -Expand.GetHeight(2) );
}