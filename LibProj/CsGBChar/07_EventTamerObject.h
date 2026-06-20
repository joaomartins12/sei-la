#pragma once

class CsC_EventTamerObject : public CsC_PartObject
{
	CsCRTTI_H
	THREAD_MEMPOOL_H( CsC_EventTamerObject )

public:
	CsC_EventTamerObject(void);
	~CsC_EventTamerObject(void);

public:
	virtual void		Delete();
	virtual void		Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp, sCHANGE_PART_INFO* pPartInfo );

	virtual	void		Update( float fDeltaTime, bool bAnimation = true );

	virtual	float		GetAttackRange() { return 0.0f; }
	virtual TCHAR*		GetName() { return m_szName; }
	virtual NiColor			GetNameColor(){ return NiColor( 1.0f, 0.878f, 0.153f ); }

	/**< 선행로드 인가? */
	virtual bool		_IsFastPartLoad() { return true; }

	void	SetName( TCHAR* szName ) { _tcscpy_s( m_szName, 32, szName ); }	
	void	SetScale( float fScale );

public:
	UINT				m_EventIdx;		//이벤트 번호

	enum eEVENTOBJECT_STATE { OBJECT_STOP, OBJECT_NORMAL, OBJECT_MOVE, OBJECT_END };
	eEVENTOBJECT_STATE			m_eObjectState; //이벤트 대상 상태	

protected:
	TCHAR						m_szName[ 32 ]; //캐릭터 이름

	//====================================================================================
	//		부가 정보
	//====================================================================================
protected:
	virtual UINT		_GetTimeTS(){ return 0; }	

};
