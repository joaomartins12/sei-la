
#pragma once 

class CsMapCharLight
{
public:
	typedef	std::list< CsMapCharLight* >				LIST;
	typedef	std::list< CsMapCharLight* >::iterator		LIST_IT;

public:
	CsMapCharLight(){ m_pInfo = NULL; }

	// ===== Info
public:
	struct sINFO_BASE
	{
		DWORD		s_dwMapID;

		bool		s_bChar;
		bool		s_bShadow;

		int			s_nType;
		bool		s_bEnableLight;
		float		s_fDiffuse[ 3 ];
		float		s_fAmbient[ 3 ];
		float		s_fSpecular[ 3 ];
		float		s_fPower;
	};

	struct sINFO_DIR : public sINFO_BASE
	{
		float		s_fRot[ 3 ][ 3 ];
		float		s_fPos[ 3 ];
		float		s_fScale;
	};

	struct sINFO_POINT : public sINFO_BASE
	{
		float		s_Pos[ 3 ];
		float		s_C;
		float		s_L;
		float		s_Q;
		float		s_Range;
	};

protected:
	sINFO_BASE*		m_pInfo;
public:
	sINFO_BASE*		GetInfo(){ return m_pInfo; }

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO_BASE* pSrcInfo );
};

