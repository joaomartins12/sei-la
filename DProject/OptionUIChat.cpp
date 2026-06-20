#include "StdAfx.h"
#include "OptionUIBase.h"
#include "OptionUIChat.h"

COptionUIChat::COptionUIChat(void)
{
	AddPath("CHAT");
}

COptionUIChat::~COptionUIChat(void)
{
	m_ClassPath.clear();
}

TiXmlElement* COptionUIChat::SetCommonAttribute(TiXmlElement* root)
{
	SAFE_POINTER_RETVAL(root, NULL);

	root->SetAttribute(UI_WINDOW_X,		m_UiData.Pos.x);
	root->SetAttribute(UI_WINDOW_Y,		m_UiData.Pos.y);
	root->SetAttribute(UI_WINDOW_EXPAND_LEVEL,	m_UiData.ExpandLevel);

	return root;
}

void COptionUIChat::GetCommonAttribute(TiXmlElement* root)
{
	SAFE_POINTER_RET(root);

	IntAttribute(root, UI_WINDOW_X,(int*) &m_UiData.Pos.x);
	IntAttribute(root, UI_WINDOW_Y,(int*) &m_UiData.Pos.y);
	IntAttribute(root, UI_WINDOW_EXPAND_LEVEL, &m_UiData.ExpandLevel);
}

const char* COptionUIChat::GetName()	const
{
	return m_UiData.Name.c_str();
}
void COptionUIChat::AddPath(string path)
{
	m_ClassPath.push_back(path);
}

vector<string> COptionUIChat::GetChatPath() const
{
	return m_ClassPath;
}

ST_CHAT_OPTION*	COptionUIChat::GetChatOption()
{
	return &m_UiData;
}

void COptionUIChat::SetChatOption(ST_CHAT_OPTION * pThis)
{
	SAFE_POINTER_RET(pThis);
	m_UiData = pThis;
}