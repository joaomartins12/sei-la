
#pragma once

class cData_MonsterIcon : public NiMemObject
{
public:

	void		Delete();
	void		Init();
	void		InitNetOff();

public:
	NiSourceTexturePtr				m_pMonsterIcon;
	NiSourceTexturePtr				m_pMonsterIcon2;

protected:
	std::map< DWORD, CsAchieve* >	m_mapComp;
	std::list< CsAchieve* >			m_listProc;
	DWORD							m_nDispTitle;
};
