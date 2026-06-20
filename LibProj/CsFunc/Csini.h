


#pragma once 


class Csini
{
public:
	Csini( char* cFileName )
	{ 
		m_cFileName = cFileName; 
		memset( &m_cBuffer, 0, MAX_PATH );
	}

protected:
	char		m_cBuffer[ MAX_PATH ];
	char*		m_cFileName;

public:
	bool		IsFile();

	char*		GetStr( char const* cApp, char const* cKey );
	TCHAR*		GetStrW( char const* cApp, char const* cKey, TCHAR* szData );
	int			GetInt( char const* cApp, char const* cKey );
	
	void		WriteInt( char const* cApp, char const* cKey, int nValue );
};