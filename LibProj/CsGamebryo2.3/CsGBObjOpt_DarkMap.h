

#pragma once 


class CsGBObjOpt_DarkMap : public CsGBObjOpt
{
	CSGBMEMPOOL_H( CsGBObjOpt_DarkMap );

public:
	CsGBObjOpt_DarkMap();
	~CsGBObjOpt_DarkMap();

public:
	static CsGBObjOpt*	GetInstance( CsGBObject* pParentObj );

protected:
	NiPoint2*			m_pDMObjectUV;
	UINT				m_uiDMObjectUVCount;
	BYTE*				m_pDMObjectPixel;
	UINT				m_uiDMObjectPixelCount;	

public:
	virtual eOPT_TYPE	GetType(){ return OPT_DARKMAP; } 

	virtual void		Delete();

	virtual void		ResetOption();

	virtual void		SetData( char* pData, UINT& uiOffset );
	virtual UINT		GetSaveSize();
	virtual void		Save( char* pData, UINT& uiOffset );

public:
	void				CalDMObjectData();
	void				ResetCalDMObjectData();
protected:
	void				_ApplyDMObjectInfo();
	void				_ApplyDMObjectInfo( NiNode* pNiNode, UINT& uiUVOffset, NiSourceTexturePtr pNiTexture );

	void				_CalDMObjectPixel();
	void				_CalDMObjectUVCount( NiNode* pNiNode );
	void				_CalDMObjectUV( NiNode* pNiNode, UINT& uiUVOffset );
};

