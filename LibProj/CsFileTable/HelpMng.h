
#pragma once 

#include "Help.h"

class CsHelpMng
{
public:
	CsHelpMng();

public:
	void			Delete();
	bool			Init();

	void			Save();
	bool			Load();
	void			ToEng();

protected:
	void			_LoadFilePack();

protected:
	UINT					m_nTitleUniqID;	
	CsVectorPB< CsHelp* >	m_vpData;

	CsVectorPB< CsHelp* >	m_vpCheck_TLevel;
	CsVectorPB< CsHelp* >	m_vpCheck_PartnerDLevel;
	CsVectorPB< CsHelp* >	m_vpCheck_MapArrive;

public:
	bool					IsTitleID( int nUniqID );
	CsHelp*					GetData( int nUniqID );
	CsVectorPB< CsHelp* >*	GetData(){ return &m_vpData; }
	CsVectorPB< CsHelp* >*	GetCheck_TLevel(){ return &m_vpCheck_TLevel; }
	CsVectorPB< CsHelp* >*	GetCheck_PartnerDLevel(){ return &m_vpCheck_PartnerDLevel; }
	CsVectorPB< CsHelp* >*	GetCheck_MapArrive(){ return &m_vpCheck_MapArrive; }

	CsHelp*					AddData();
	void					DelData( CsHelp* pDel );
	UINT					GetUniqID(){ return ++m_nTitleUniqID; }

	void					SwapUp( CsHelp* pSrc );
	void					SwapDown( CsHelp* pSrc );
};
