
#include "StdAfx.h"

#include "_Interface/Game/ChatContents.h"

#include "PlayerUIOptionDocument.h"

CPlayerUIOptionDocument::CPlayerUIOptionDocument()
{	
}

CPlayerUIOptionDocument::~CPlayerUIOptionDocument()
{
	RemoveOptionData();
}
void CPlayerUIOptionDocument::CreateOptionData()
{
	if(!m_OptionChatUI.empty())
		return;

	m_OptionChatUI.insert(make_pair(XML_CHAT_MAIN,		new CFileUIChatting()));	// 순서를 바꿔도 키값순으로 정렬된다.
	m_OptionChatUI.insert(make_pair(XML_CHAT_SYSTEM,	new CFileUISystem()));
	m_OptionChatUI.insert(make_pair(XML_CHAT_BATTLELOG,	new CFileUIBattleLog()));
	m_OptionChatUI.insert(make_pair(XML_CHAT_CONTENTS,	new CFileUIContents()));
}

void CPlayerUIOptionDocument::RemoveOptionData()
{
	for(MAP_CAHT_UI_ITOR iter = m_OptionChatUI.begin(); iter != m_OptionChatUI.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	m_OptionChatUI.clear();
}
void CPlayerUIOptionDocument::LoadToPlayerUIRecord(TiXmlDocument*	pDoc, string parent)
{
	CreateOptionData();

	// UI 초기화
	if(pDoc == NULL)
	{
		for(MAP_CAHT_UI_ITOR	iter = m_OptionChatUI.begin(); iter != m_OptionChatUI.end(); ++iter)
			iter->second->InitDefault();
	}
	// 검색
	else if(pDoc && !parent.empty() )
	{
		TiXmlHandle handle(pDoc);       // 검색을 위하여 핸들 생성

		for(MAP_CAHT_UI_ITOR	iter = m_OptionChatUI.begin(); iter != m_OptionChatUI.end(); ++iter)
		{
			COptionUIChat*	pChatUI = iter->second;
			pChatUI->InitDefault();
			TiXmlHandle findHandle = handle.FirstChild(parent.c_str());

			COptionUIChat::VEC_STRING	VecPath = pChatUI->GetChatPath();
			COptionUIChat::VEC_STRING_ITER viter = VecPath.begin();
			for(viter; viter != VecPath.end(); ++viter)	//검색 위치 지정
			{
				string	Str = *viter;
				findHandle = findHandle.FirstChild(Str.c_str());
				if(findHandle.ToNode()==NULL)
					break;
			}

			if(findHandle.ToNode()==NULL)
				continue;

			TiXmlElement* XmlElem = findHandle.ToElement();
			if(XmlElem)
				pChatUI->Read(XmlElem);				
		}
	}
}
void CPlayerUIOptionDocument::SaveToPlayerUIRecord(TiXmlDocument*	pDoc, string parent)
{
	SAFE_POINTER_RET(pDoc);	

	TiXmlHandle handle(pDoc);

	TiXmlElement* root = new TiXmlElement(parent.c_str());
	SAFE_POINTER_RET(root);
	pDoc->LinkEndChild(root);

	// 저장 데이터 설정.
	for(MAP_CAHT_UI_ITOR	iter = m_OptionChatUI.begin(); iter != m_OptionChatUI.end(); ++iter)
	{
		COptionUIChat*	pOptChat = iter->second;

		TiXmlHandle findHandle = handle.FirstChild(parent.c_str());

		TiXmlElement* child = root;
		int MaxSize = pOptChat->GetChatPath().size();
		for(int i=0; i<MaxSize; ++i)	//검색 위치 지정
		{
			string	Str = pOptChat->GetChatPath().at(i);

			findHandle = findHandle.FirstChild(Str.c_str());

			if(findHandle.ToNode()==NULL)
			{
				TiXmlElement* Elem = new TiXmlElement(Str.c_str());
				child = child->LinkEndChild(Elem)->ToElement();
			}
			else if( findHandle.ToElement() != NULL)
				child = findHandle.ToElement();

			if(MaxSize-1==i)
			{
				pOptChat->Write(child);
			}			
		}		
	}

	RemoveOptionData();
}

void CPlayerUIOptionDocument::SetUIOption(int type, ST_CHAT_OPTION*	pThis)
{
	if(m_OptionChatUI.empty())
		return;

	MAP_CAHT_UI_ITOR	iter = m_OptionChatUI.find(type);
	if(iter == m_OptionChatUI.end())
		return;

	SAFE_POINTER_RET(pThis);
	iter->second->SetChatOption(pThis);
}

COptionUIChat*	CPlayerUIOptionDocument::GetUIChatClass(int type, bool bLoad)
{
	if(m_OptionChatUI.empty())
		return NULL;

	MAP_CAHT_UI_ITOR	iter = m_OptionChatUI.find(type);
	if(iter == m_OptionChatUI.end())
		return NULL;

	COptionUIChat*	pUiChat = iter->second;

	SAFE_POINTER_RETVAL(pUiChat, NULL);

	if(!bLoad)
		pUiChat->InitDefault();

	return pUiChat;
}

ST_CHAT_OPTION*	CPlayerUIOptionDocument::GetUIOption(int type, bool bLoad)
{
	if(m_OptionChatUI.empty())
		return NULL;

	MAP_CAHT_UI_ITOR	iter = m_OptionChatUI.find(type);
	if(iter == m_OptionChatUI.end())
		return NULL;

	COptionUIChat*	pUiChat = iter->second;

	SAFE_POINTER_RETVAL(pUiChat, NULL);

	if(!bLoad)
		pUiChat->InitDefault();

	return pUiChat->GetChatOption();
}
INT	CPlayerUIOptionDocument::GetUIOptionValue(int type, eOPTION_UI_VALUE_TYPE const OptID, bool	bLoad)
{
	ST_CHAT_OPTION*	pUiOpt = GetUIOption(type, bLoad);
	SAFE_POINTER_RETVAL(pUiOpt, 0);
	switch (OptID)
	{
	case eOPTION_UI_POS_X:
		return pUiOpt->Pos.x;

	case eOPTION_UI_POS_Y:
		return pUiOpt->Pos.y;

	case eOPTION_UI_EXPAND_LEVEL:
		return pUiOpt->ExpandLevel;

	case eOPTION_UI_SHOW_ETC:
		return pUiOpt->AddShow ? 1 : 0;
	}

	return 0;
}