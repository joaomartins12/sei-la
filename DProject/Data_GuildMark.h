#pragma once 

#ifdef GUILD_MARK_DOWN

class CCharOption;
class cData_GuildMark : public NiMemObject
{
public:
	cData_GuildMark();

public:	
	void	Delete();
	void	Init();	
	void	Update();
	
	void	ResetCheck();
	void	MarkDownload( CCharOption* pOption );	

protected:	
	std::list < CCharOption* > m_ListGuild; //포인터만 들고 있음.. 
};

#endif