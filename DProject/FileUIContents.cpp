#include "StdAfx.h"

#include "OptionUIBase.h"
#include "FileUIContents.h"

#define ENABLE_CONTENTS_NORMAL		"NORMAL"
#define ENABLE_CONTENTS_PARTY		"PARTY"
#define ENABLE_CONTENTS_GUILD		"GUILD"
#define ENABLE_CONTENTS_WHISPER		"WHISPER"
#define ENABLE_CONTENTS_SHOUT		"SHOUT"
#define ENABLE_CONTENTS_MEGAPHONE	"MEGAPHONE"

CFileUIContents::CFileUIContents(void)
{
	//AddPath("Contents");
}

CFileUIContents::~CFileUIContents(void)
{
}

void CFileUIContents::Write(TiXmlElement* root)
{
	SAFE_POINTER_RET(root);
	
	ChatContents* pChatCon = (ChatContents*)CONTENTSSYSTEM_PTR->GetContents(E_CT_CHATTING_STANDARDIZATION);
	for(int i=NS_CHAT::NORMAL; i<NS_CHAT::MAXCOUNT; ++i)
		m_bChatEnable.On[i] = pChatCon->_IsChatEnable((NS_CHAT::TYPE)i);

	root->SetAttribute(ENABLE_CONTENTS_NORMAL,		m_bChatEnable.On[NS_CHAT::NORMAL]);
	root->SetAttribute(ENABLE_CONTENTS_PARTY,		m_bChatEnable.On[NS_CHAT::PARTY]);
	root->SetAttribute(ENABLE_CONTENTS_GUILD,		m_bChatEnable.On[NS_CHAT::GUILD]);
	root->SetAttribute(ENABLE_CONTENTS_WHISPER,		m_bChatEnable.On[NS_CHAT::WHISPER]);
	root->SetAttribute(ENABLE_CONTENTS_SHOUT,		m_bChatEnable.On[NS_CHAT::SHOUT]);
#ifdef MEGAPHONE_BAN
	root->SetAttribute(ENABLE_CONTENTS_MEGAPHONE,	m_bChatEnable.On[NS_CHAT::MEGAPHONE]);
#endif
}

bool CFileUIContents::Read(TiXmlElement* root)
{
	BoolAttribute(root, ENABLE_CONTENTS_NORMAL,		&m_bChatEnable.On[NS_CHAT::NORMAL], true);
	BoolAttribute(root, ENABLE_CONTENTS_PARTY,		&m_bChatEnable.On[NS_CHAT::PARTY], true);
	BoolAttribute(root, ENABLE_CONTENTS_GUILD,		&m_bChatEnable.On[NS_CHAT::GUILD], true);
	BoolAttribute(root, ENABLE_CONTENTS_WHISPER,	&m_bChatEnable.On[NS_CHAT::WHISPER], true);
	BoolAttribute(root, ENABLE_CONTENTS_SHOUT,		&m_bChatEnable.On[NS_CHAT::SHOUT], true);
#ifdef MEGAPHONE_BAN
	BoolAttribute(root, ENABLE_CONTENTS_MEGAPHONE,	&m_bChatEnable.On[NS_CHAT::MEGAPHONE], true);
#endif

	return true;
}

void CFileUIContents::InitDefault()
{
	for(int i=NS_CHAT::NORMAL; i<NS_CHAT::MAXCOUNT; ++i)
		m_bChatEnable.On[i] = true;
}