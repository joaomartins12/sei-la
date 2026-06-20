#pragma once

class CsMapNpc
{
public:
	struct sINFO
	{
		sINFO*		Init( DWORD dwMapID, DWORD dwNpcID ){
			s_dwMapID = dwMapID;
			s_dwNpcID = dwNpcID;
			s_nInitPosX = s_nInitPosY = 0;			
			s_fRotation = 0.0f;
			return this;
		}
		DWORD		s_dwNpcID;
		DWORD		s_dwMapID;
		int			s_nInitPosX;
		int			s_nInitPosY;

		float		s_fRotation;
	};

protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }


public:
	void			Delete(){}	
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};


