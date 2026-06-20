
#pragma once 

class CsGBObjOpt : public NiMemObject
{
public:
	CsGBObjOpt();
	virtual ~CsGBObjOpt();


public:
	enum eOPT_TYPE
	{
		OPT_DARKMAP,
	};

protected:
	CsGBObject*			m_pParentObj;
	sUSER_DEFINE*		m_pParentUserDefine;

public:	
	static void			DeleteOption( CsGBObjOpt* pDeleteOpt );

public:
	virtual eOPT_TYPE	GetType() = 0;

	virtual void		Delete();
	
	virtual void		ResetOption(){}

	virtual void		Init( CsGBObject* pParentObj );

	virtual void		SetData( char* pData, UINT& uiOffset ){}
	virtual UINT		GetSaveSize(){ return 0; }
	virtual void		Save( char* pData, UINT& uiOffset ){}
	

public:
	//CsGBObject*			GetParent(){ return m_pParentObj; }	
};