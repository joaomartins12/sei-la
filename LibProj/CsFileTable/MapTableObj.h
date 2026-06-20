
#pragma once 

class CsMapTableObj 
{
public:
	CsMapTableObj();

public:
	typedef	std::list< CsMapTableObj* >					LIST;
	typedef	std::list< CsMapTableObj* >::iterator		LIST_IT;

	// ===== Info
public:
	struct sINFO
	{
		int			s_nDataSize;		
	};

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	void			SaveBin( FILE* fp );
	void			LoadBin( FILE* fp );
	void			LoadFilePack( int nHandle );
	
protected:
	sINFO			m_Info;
	char*			m_pData;
public:
	sINFO*			GetInfo(){ return &m_Info; }

	void			AllocData();
	char*			GetData(){ return m_pData; }
};

