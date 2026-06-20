
#pragma once 

class CsWeather 
{
public:
	~CsWeather(){ Delete(); }

	// ===== Info
public:
	enum eW_TYPE{ WT_NONE, WT_SNOW, WT_RAIN, };
	struct sELEMENT
	{
		sELEMENT(){ s_nTime_Min = 30; s_nFatigue = 0; s_Type = WT_NONE; s_nLevel = 0; }
		int			s_nTime_Min;
		int			s_nFatigue;
		BYTE		s_Type;
		BYTE		s_nLevel;		
	};

	struct sINFO
	{
		sINFO(){
			s_dwMapID = 0;
			s_nCenterX = s_nCenterY = 0;
			s_nRadius = 5000;
			s_nElementCount = 0;
		}

		DWORD		s_dwMapID;
		int			s_nCenterX;
		int			s_nCenterY;
		int			s_nRadius;
		int			s_nElementCount;
		UINT		s_nTotalLoopMin;
	};

public:
	typedef	std::list< CsWeather* >					LIST;
	typedef	std::list< CsWeather* >::iterator		LIST_IT;
	typedef std::list< sELEMENT* >					LISTELE;
	typedef	std::list< sELEMENT* >::iterator		LISTELE_IT;


protected:
	LISTELE					m_listElement;
	sINFO					m_Info;
public:
	sINFO*					GetInfo(){ return &m_Info; }

	sELEMENT*				AddElement( sELEMENT* pSrc );
	void					DelElement( sELEMENT* pSrc );
	LISTELE*				GetElementList(){ return &m_listElement; }

	sELEMENT*				UpdateElement( UINT nCurTime );

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );
};

