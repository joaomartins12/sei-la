#pragma once 

#ifdef MONSTER_ICON

class CCharOption;
class cData_MonsterIcon : public NiMemObject
{
public:
	cData_MonsterIcon(){};

public:	
	void	Delete();
	void	Init();	
	//void	Update();

	//void	ResetCheck();
protected:
	DWORD	m_nDispIcon;

public:
	NiSourceTexturePtr				m_pMonIcon;

	DWORD							GetDispIcon(){ return m_nDispIcon; }

protected:	
	std::list < CCharOption* > m_ListMonIcon; //포인터만 들고 있음.. 
};

#endif