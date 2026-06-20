
#pragma once 

class CsGBObjOptFactor : public NiMemObject
{
	CSGBMEMPOOL_H( CsGBObjOptFactor );

protected:
	std::map< DWORD, CsGBObjOpt* >		m_mapOption;

public:
	CsGBObjOptFactor();
	~CsGBObjOptFactor();

	void			Delete();

	CsGBObjOpt*		InsertOption( CsGBObjOpt::eOPT_TYPE eType, CsGBObject* pParentObject );

	void			ResetOption();


	// Option
	inline CsGBObjOpt*	GetOption( CsGBObjOpt::eOPT_TYPE eType );
	inline bool			IsOption( CsGBObjOpt::eOPT_TYPE eType );


	// Save
	UINT			GetSaveSize();
	void			Save( char* pData, UINT& uiOffset );
};

#include "CsGBObjOptFactor.inl"

