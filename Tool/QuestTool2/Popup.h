

#pragma once

class cPopup : public NiMemObject
{
public:
	enum eTYPE{ JUMP, };

public:
	cPopup(){ Init(); }
	virtual ~cPopup(){ Delete(); }

public:
	void				Delete();
	void				Init();

	void				Render();

protected:
	void*				m_pSrcDataPointer;
	bool				m_bRenderPopup;

	CsPoint				m_ptPos;
	eTYPE				m_eType;

public:
	void				SetPopup( eTYPE type, void* pData, CsPoint pos );
};