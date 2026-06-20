


#pragma once 

class CsMapList
{

public:
	CsMapList();
	~CsMapList();

	typedef	std::map< DWORD, CsMapList* >				MAP;
	typedef	std::map< DWORD, CsMapList* >::iterator		MAP_IT;

	// ===== Info
public:
	struct sINFO
	{
		sINFO();
		~sINFO();

		void		Init();

		USHORT			s_nFatigue_DeBuff;
		USHORT			s_nMapRegionID;						// 서버와의 통신에서 쓸 MapID
		USHORT			s_nFatigue_Type;
		USHORT			s_nFatigue_StartTime;
		int				s_nHeight;
		std::string		s_cMapName;
		SHORT			s_nCamera_Max_Level;
		USHORT			s_nFatigue_AddTime;
		std::string		s_cMapPath;
		bool			s_bXgConsumeType;
		int				s_nWidth;
		std::string		s_cBGSound;
		std::wstring	s_szMapDiscript;
		DWORD			s_dwResurrectionMapID;
		DWORD			s_dwMapID;
		bool			s_bBattleTagUse;
		std::wstring	s_szMapDiscript_Eng;
		SHORT			s_nFatigue_AddPoint;
	};

public:
	void			Init( sINFO* pSrcInfo );
	void			Delete();
	sINFO*			GetInfo(){ return &m_Info; }

protected:
	sINFO			m_Info;
};
