#pragma once

class cData_TamerIcon : public NiMemObject
{
public:
	cData_TamerIcon();
	~cData_TamerIcon();

	void		Delete();
	void		Init();
	void		InitNetOff();

public:
	NiSourceTexturePtr				m_pMatchIcon;	// 마스터즈매칭 팀 아이콘
	//NiSourceTexturePtr				m_pTamerIcon;

// protected:
// 	DWORD							m_nDispTitle;
};
