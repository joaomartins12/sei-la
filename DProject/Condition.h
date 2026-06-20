
#pragma once

class cCondition : public NiMemObject
{
public:
	cCondition();

protected:
	uint						m_nCondition;
	uint						m_nConditionBackup;	
	CsC_AvObject*				m_pParent;

public:
	void			Delete();
	void			Init( CsC_AvObject* pParent );

	void			SetCondition( uint nCondition );
	void			AddCondition( uint nCondition ){ SetCondition( m_nCondition | nCondition ); }
	void			ReleaseCondition( uint nCondition ){ SetCondition( m_nCondition & (~nCondition) ); }	
	
	bool			IsCondition( nSync::eCondition condition ){ return ( ( m_nCondition & condition ) != 0 ); }
	bool			IsCondition( uint nSrc, nSync::eCondition condition ){ return ( (nSrc & condition) != 0 ); }
	bool			IsSameCondition( uint nSrc1, uint nSrc2, nSync::eCondition condition ){ return ( IsCondition( nSrc1, condition ) == IsCondition( nSrc2, condition ) ); }

protected:
	void			_ApplyCondition( uint nCondition, nSync::eCondition cd );
	void			_RemoveCondition( uint nCondition, nSync::eCondition cd );
	void			_AddCondition( uint nCondition, nSync::eCondition cd );

	//===========================================================================================================
	//		Etc
	//===========================================================================================================
protected:
	void			_ApplyEtc();
	void			_Check_PCBangEffect();
};