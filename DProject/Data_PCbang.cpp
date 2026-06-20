#include "stdafx.h"
#include "Data_PCbang.h"

cData_PCBang::cData_PCBang()
{	
	m_bCert = false;
	m_dwCostume = 0;
	m_dwPartyExp = 0;
}


void cData_PCBang::SetPcBang( bool bCert )
{
	m_bCert = bCert; 
} 
bool cData_PCBang::GetPcBang()
{ 
	return m_bCert;
}