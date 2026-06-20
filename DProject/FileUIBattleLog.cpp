#include "StdAfx.h"
#include "OptionUIBase.h"
#include "FileUIBattleLog.h"

#include "_Interface/Game/ChatBase.h"

CFileUIBattleLog::CFileUIBattleLog(void)
{
	AddPath("BATTLE_LOG");
}

CFileUIBattleLog::~CFileUIBattleLog(void)
{
}

void CFileUIBattleLog::Write(TiXmlElement* root)
{
	SetCommonAttribute(root);
}

bool CFileUIBattleLog::Read(TiXmlElement* root)
{
	GetCommonAttribute(root);
	return true;
}

void CFileUIBattleLog::InitDefault()
{
	ChatBase::stExpandData	Expand;
	m_UiData.ExpandLevel = 0;
	m_UiData.Pos = CsPoint( 2, g_nScreenHeight -CHAT_SIZE_HEIGHT*3 -50 -Expand.GetHeight(2) );
}