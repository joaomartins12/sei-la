//-----------------------------------------------------------------------------
// 	                              GbDxDiagInfo.h   
//-----------------------------------------------------------------------------
#pragma once

#include <dxdiag.h>
#include <list>

struct SystemInformation 
{
	SystemInformation( void )
	{
		strcpy_s( m_szProcessorEnglish, 256, "UnKnowned" ) ;
		strcpy_s( m_szPhysicalMemoryEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szCSDVersion, 256, "UnKnowned" ) ;
		strcpy_s( m_szDirectXVersionEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szDirectXVersionLongEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szOSEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szOSExEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szOSExLongEnglish, 256,"UnKnowned" ) ;
	} ;

	char m_szProcessorEnglish[256] ;
	char m_szPhysicalMemoryEnglish[256] ;
	char m_szCSDVersion[256] ;
	char m_szDirectXVersionEnglish[256] ;
	char m_szDirectXVersionLongEnglish[256] ;
	char m_szOSEnglish[256] ;
	char m_szOSExEnglish[256] ;
	char m_szOSExLongEnglish[256] ;
} ;

struct DispalyInformation 
{
	DispalyInformation( void )
	{
		strcpy_s( m_szDescription, 256,"UnKnowned" ) ;
		strcpy_s( m_szManufacturer, 256,"UnKnowned" ) ;
		strcpy_s( m_szChipType, 256,"UnKnowned" ) ;
		strcpy_s( m_szDACType, 256,"UnKnowned" ) ;
		strcpy_s( m_szRevision,"UnKnowned" ) ;
		strcpy_s( m_szDisplayMemoryLocalized, 256,"UnKnowned" ) ;
		strcpy_s( m_szDisplayMemoryEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szDisplayModeLocalized, 256,"UnKnowned" ) ;
		strcpy_s( m_szDisplayModeEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szMonitorName, 256,"UnKnowned" ) ;
		strcpy_s( m_szMonitorMaxRes, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverName, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverVersion, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverAttributes, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverLanguageEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverLanguageLocalized, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverDateEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szDriverDateLocalized, 256,"UnKnowned" ) ;
		strcpy_s( m_szMiniVdd, 256,"UnKnowned" ) ;
		strcpy_s( m_szMiniVddDateLocalized, 256,"UnKnowned" ) ;
		strcpy_s( m_szMiniVddDateEnglish, 256,"UnKnowned" ) ;
		strcpy_s( m_szVdd, 256,"UnKnowned" ) ;
	} ;

	char m_szDescription[256] ;
	char m_szManufacturer[256] ;
	char m_szChipType[256] ;
	char m_szDACType[256] ;
	char m_szRevision[256] ;
	char m_szDisplayMemoryLocalized[256] ;
	char m_szDisplayMemoryEnglish[256] ;
	char m_szDisplayModeLocalized[256] ;
	char m_szDisplayModeEnglish[256] ;
	DWORD m_dwWidth ;
	DWORD m_dwHeight ;
	DWORD m_dwBpp ;
	DWORD m_dwRefreshRate ;
	char m_szMonitorName[256] ;
	char m_szMonitorMaxRes[256] ;
	char m_szDriverName[256] ;
	char m_szDriverVersion[256] ;
	char m_szDriverAttributes[256] ;
	char m_szDriverLanguageEnglish[256] ;
	char m_szDriverLanguageLocalized[256] ;
	char m_szDriverDateEnglish[256] ;
	char m_szDriverDateLocalized[256] ;
	LONG  m_lDriverSize ;
	char m_szMiniVdd[256] ;
	char m_szMiniVddDateLocalized[256] ;
	char m_szMiniVddDateEnglish[256] ;
	LONG  m_lMiniVddSize ;
	char m_szVdd[256] ;
} ;

class GbDxDiagInfo
{
public:
	// constructor and destructor
	GbDxDiagInfo( void ) ;
	~GbDxDiagInfo( void );

	// initialize and destroy
	void Initialize( void ) ;
	void Destroy( void ) ;

	// query system value
	void QueryDxDiagValue( void ) ;

	// inline 
	SystemInformation* GetSystemInfo( void ) ;
	std::list<DispalyInformation>* GetDisplayInfo( void ) ;

	void Write( char* szFile );

private:

	// initialized
	bool						m_bInited ;

	// DxDiag object
	IDxDiagProvider*		m_pkDxProvider ;
	IDxDiagContainer*		m_pkDxRoot ;

	// hardware information
	SystemInformation	m_kSystemInformation ;	
	std::list<DispalyInformation> m_listDisplayInformation ;

};

//-----------------------------------------------------------------------------
inline SystemInformation* GbDxDiagInfo::GetSystemInfo( void ) 
{
	return &m_kSystemInformation ;
}

//-----------------------------------------------------------------------------
inline std::list<DispalyInformation>* GbDxDiagInfo::GetDisplayInfo( void ) 
{
	return &m_listDisplayInformation ;
}


class DxResolutionInfo : public NiMemObject
{
protected:
	std::list< D3DDISPLAYMODE* >	m_list;
	std::map< NiDX9Renderer::FramebufferMode, bool >		m_mapMultisample;

public:
	static void			GlobalInit();
	static void			GlobalShotDown();

	void				Init();
	void				Delete();

	int					GetResolutionCount(){ return (int)m_list.size(); }
	D3DDISPLAYMODE*		GetResolution( int nIndex );
	D3DDISPLAYMODE*		GetResolution( int nWidth, int nHeight );
	int					FindResolutionIndex( int nWidth, int nHeight );

	bool				IsMultisample( NiDX9Renderer::FramebufferMode fm ){ return ( m_mapMultisample.find( fm ) != m_mapMultisample.end() ); }
};

extern DxResolutionInfo*		g_pResolutionInfo;



