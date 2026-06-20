
#pragma once 

class cPreviewObj
{
public:
	enum ePREVIEW_OBJ_TYPE{ POT_NONE, POT_CHAR, POT_TEXTURE, POT_TERRAIN_OBJ };
protected:
	ePREVIEW_OBJ_TYPE		m_eType;

public:
	ePREVIEW_OBJ_TYPE		GetType(){ return m_eType; }


public:
	virtual void			Update( CsGBSubCamera* pCamera ) = 0;
	virtual void			Render( CsGBSubCamera* pCamera ) = 0;

	virtual bool			IsRender() = 0;
};