
#pragma once 

BEGIN_NAME_TABLE

struct sAchieveData
{
	sAchieveData():s_nQuestID(0),s_nIcon(0),s_nPoint(0),s_bDisplay(false)
		,s_nGroup(0),s_nSubGroup(0),s_nBuffCode(0)
	{}
	
	DWORD		s_nQuestID;
	DWORD		s_nIcon;
	int			s_nPoint;
	bool		s_bDisplay;
	int			s_nGroup;
	int			s_nSubGroup;
	int			s_nBuffCode;	// น๖วม ฤฺตๅ

	std::wstring s_szName;
	std::wstring s_szComment;
	std::wstring s_szTitle;
};

struct sAchieveTitle
{
	std::wstring name;
	std::map<int,std::wstring>	m_mapSubTitle;	
};
END_NAME_TABLE