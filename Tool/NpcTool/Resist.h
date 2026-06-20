
#pragma once 

class cResist
{
public:
	#define			RESIST_VERSION		0x00010001
	struct sWINDOW_INFO
	{
		void Reset(){
			s_ptMainPos.x = 0;
			s_ptMainPos.y = 0;
			s_ptViewPos.x = 0;
			s_ptViewPos.y = 730;
		}
		POINT	s_ptMainPos;
		POINT	s_ptViewPos;
	};
	sWINDOW_INFO		m_WindowInfo;
	sWINDOW_INFO*		GetWindowInfo(){ return &m_WindowInfo; }

	struct sSTR_VIEWER
	{
		void Reset(){
			s_bViewer = true;
			s_nViewerWidth = 1024;
		}
		bool		s_bViewer;
		int			s_nViewerWidth;
	};
	sSTR_VIEWER			m_StrViewer;
	sSTR_VIEWER*		GetStrViewer(){ return &m_StrViewer; }

protected:
	void			_Reset();

public:
	void			Load();
	void			Save();
};

extern cResist g_Resist;