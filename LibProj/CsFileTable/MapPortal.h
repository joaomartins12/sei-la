
#pragma once 

class CsMapPortal
{
public:
	typedef	std::map< DWORD, CsMapPortal* >					MAP;
	typedef	std::map< DWORD, CsMapPortal* >::iterator		MAP_IT;

public:
	enum ePORTAL_TYPE{ OBJECT, NPC, };
	enum ePORTAL_DGTYPE{ NORMAL, DG, };
	// ===== Info
public:
	struct sINFO
	{
		void		Init(){
			s_dwPortalType = s_dwPortalID = s_dwSrcMapID = s_dwDestMapID = 0;
			s_nSrcTargetX = s_nSrcTargetY = s_nDestTargetX = s_nDestTargetY = 0;
			s_nSrcRadius = s_nDestRadius = 500;
			s_dwUniqObjectID = 0;
			s_nViewTargetX = s_nViewTargetY = 0;
		}
		DWORD		s_dwPortalID;
		DWORD		s_dwPortalType;
		DWORD		s_dwSrcMapID;
		int			s_nSrcTargetX;
		int			s_nSrcTargetY;
		int			s_nSrcRadius;

		DWORD		s_dwDestMapID;
		int			s_nDestTargetX;
		int			s_nDestTargetY;
		int			s_nDestRadius;

		ePORTAL_TYPE	s_ePortalType;
		DWORD			s_dwUniqObjectID;
		int				s_nPortalKindIndex;

		int			s_nViewTargetX;
		int			s_nViewTargetY;
	};

protected:
	sNPC_TYPE_PORTAL::sPORTAL*		m_pLinkNpcType;
	sINFO							m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }
	void						SetLinkNpcType( sNPC_TYPE_PORTAL::sPORTAL* pLink ){ m_pLinkNpcType = pLink; }
	sNPC_TYPE_PORTAL::sPORTAL*	GetLinkNpcType(){ return m_pLinkNpcType; }

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );
};
	
