
#include "Mail_Define.h"

//////////////////////////////////////////////////////////////////////////
// Mail 
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_USER_MESSAGE
{
public:
	GS2C_NTF_USER_MESSAGE()	:m_nSendTimeTS(0),m_nMailType(mail::eAccount)
	{}

public:	
	std::wstring	m_szSenderName;
	std::wstring	m_szMessage;
	uint			m_nSendTimeTS;
	u2				m_nMailType;
};

