#pragma once 

class cData_PCBang : public NiMemObject
{
public:
	cData_PCBang();		

public:
	void	SetPcBang( bool bCert );
	bool	GetPcBang();

	void	SetCostume( DWORD dwCostume ) { m_dwCostume = dwCostume; } 
	DWORD	GetCostume() { return m_dwCostume; }	

	cItemInfo*		GetDataArray(){ return m_pInven; }
	cItemInfo*		GetData( int nIndex ){ assert_cs( nIndex < nLimit::PcbangInven ); return &m_pInven[ nIndex ]; }
	
protected:	
	bool				m_bCert;
	DWORD				m_dwCostume;
	DWORD				m_dwPartyExp;
	cItemInfo			m_pInven[nLimit::PcbangInven];
	

};