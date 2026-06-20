#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

struct sFTPInfo
{
	sFTPInfo() {};
	~sFTPInfo() {};

	void Reset()
	{
		s_Host.clear();		s_port = 0;		s_id.clear();		s_pw.clear();		s_dir.clear();		s_file.clear();
	}

	bool IsValid() const
	{
		if( s_Host.empty() )
			return false;
		if( 0 == s_port )
			return false;
		if( s_id.empty() )
			return false;
		if( s_file.empty() )
			return false;
		return true;
	}

	std::string GetString() const
	{
		std::string value;
		value = s_Host;
		value += ",";
		value += s_id;
		value += ",";
		value += s_dir;
		value += ",";
		value += s_file;
		return value;
	}	

	std::string s_Host;
	unsigned int s_port;
	std::string s_id;
	std::string s_pw;
	std::string s_dir;
	std::string s_file;
};

class CResurecChecker;

class cResourectIntegrityChecker
	: public IUIContentSubject< cResourectIntegrityChecker >
{
	enum eState
	{
		eWait = 0,
		eCheck_Start,
		eFinish,
	};

public:
	static int const IsContentsIdentity(void);

public:
	cResourectIntegrityChecker(void);
	virtual ~cResourectIntegrityChecker(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

	void		callback_Function( int const& nErrorCode );

	static void CALLBACK EventTimeoutProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime );

private:
	void		_CheckErrorCode();
	void		_ThreadStopLogSend( int const& nErrorType );

private:
	void		_StartResourceChecker( void* pData );
	void		_ResourChekTimeOut(void* pData);
	void		_ShowErrorMessage( int const& nErrorType );
	void		_SendErrorLog( int const& nErrorType, std::string const& msg );

private:
	CResurecChecker*		m_pResourceChecker;
	eState					m_nCheckState;
	int						m_nCheckError;
	sFTPInfo				m_DefaultUploadFtpInfo;
};
