#pragma once

class cTacticsObject : public CsC_AvObject
{	
	CsCRTTI_H

public:
	cTacticsObject(void);
	~cTacticsObject(void);

public:
	virtual void		Delete();
	virtual void		Init( UINT nFileTableID, NiPoint3 vPos, float fRot, float fStartTime, sCREATEINFO* pProp );

	virtual	void		Update( float fDeltaTime, bool bAnimation = true );	

	/**< 선행로드 인가? */
	virtual bool		_IsFastPartLoad() { return true; }	

			void		SetAniOption( DWORD dwAni, float fStartTime, float fEndTime );
			bool		IsEnd() { return ( m_fCurTime >= m_fEndTime ) ? true : false; }

protected:
	void	_FirstUpdate();
	void	_SetAniTime( float fTime );

protected:
	float				m_fStartTime;
	float				m_fCurTime;
	float				m_fEndTime;
	DWORD				m_dwAniID;

	bool				m_bUpdate;


};
