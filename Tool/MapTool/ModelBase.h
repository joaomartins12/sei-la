

#pragma once

class cModel_Base
{
public:
	cModel_Base();
protected:
	NiActorManager*		m_pAM;
	float				m_fAniTime;
	DWORD				m_dwModelKey;

public:
	virtual void		Delete();	
	virtual void		Update( float fElapsedTime, bool bUpdateControl );
	
	DWORD				GetModelKey(){ return m_dwModelKey; }
	NiActorManager*		Clone(){ return m_pAM->Clone(); }
	void				ResetAni(){ m_fAniTime = 0.0f; }

	NiActorManager*		GetActorManager(){ return m_pAM; }
	NiNode*				GetNiNode(){ return (NiNode*)m_pAM->GetNIFRoot(); }
	NiTransform			GetTransform(){ return GetNiNode()->GetWorldTransform(); }
	NiNode*				GetCollision(){ return (NiNode*)GetNiNode()->GetAt( GetNiNode()->GetChildCount() - 1 ); }	
protected:
	void				_Init( DWORD dwModelKey );
};