
#pragma once 

#define CHAR_DEST_ALPHA_TIME		0.8f			// 변화하는데 걸리는 시간

class CsC_AlphaProp : public CsC_Property
{
	THREAD_MEMPOOL_H( CsC_AlphaProp )

public:
	enum eALPHA_TYPE{ AT_STOP, AT_KEEP, AT_ALPHA_INCREASE, AT_ALPHA_DECREASE };

	//================================================================================
	//		전역
	//================================================================================

	//================================================================================
	//		지역
	//================================================================================
protected:
	eALPHA_TYPE						m_eAlphaType;

	float							m_fDestAlphaTime;
	float							m_fCurAlphaValue;
	bool							m_bUseDistAlpha;

	bool							m_bHideDistOut;				// 하이드 거리 바깥에 존재하는가

public:
	CsC_AlphaProp();
	~CsC_AlphaProp();

	virtual void			PreDelete();

	virtual void			ChangeKFM();
	void					Init( CsC_AvObject* pParent, eALPHA_TYPE eAlphaType, float fInitAlphaValue );
	virtual void			CheckModelComp();

	void					Update( float fDeltaTime );

	void					SetUseDistAlpha( bool bUse ){ m_bUseDistAlpha = bUse; }
	bool					IsUseDistAlpha(){ return m_bUseDistAlpha; }
	void					SetType( eALPHA_TYPE type ){ m_eAlphaType = type; }
	eALPHA_TYPE				GetType(){ return m_eAlphaType; }
	void					SetCurAlphaValue( float fAlphaValue );
	void					SetCurAlphaValue( eALPHA_TYPE type, float fAlphaValue );
	float					GetCurAlphaValue(){ return m_fCurAlphaValue; }

	void					SetDestAlphaTime( float fDestTime ){ m_fDestAlphaTime = fDestTime; }

	bool					IsHideDistOut(){ return m_bHideDistOut; }

protected:
	void					_UpdateDist( float fDeltaTime );
	void					_UpdateType( float fDeltaTime );

protected:
	void					_SetMaterialAlpha( float fAlpha );
	
};