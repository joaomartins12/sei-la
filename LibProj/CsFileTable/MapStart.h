
#pragma once 

class CsMapStart 
{
public:
	typedef	std::list< CsMapStart* >				LIST;
	typedef	std::list< CsMapStart* >::iterator		LIST_IT;

	// ===== Info
public:
	struct sINFO
	{
		void		Init( DWORD dwMapID ){
			s_dwMapID = dwMapID;
			s_nCenterX = s_nCenterY = 0;
			s_nRadius = 500;
			s_nType = 0;
		}

		DWORD		s_dwMapID;
		int			s_nCenterX;
		int			s_nCenterY;
		int			s_nRadius;
		BYTE		s_nType;
	};
protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );
};

