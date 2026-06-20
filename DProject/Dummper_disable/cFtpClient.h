


#pragma once

#include <WinInet.h>

class cFtpClient
{
public:
	cFtpClient(void);
   ~cFtpClient(void);

public:
	bool Init(void);
	bool Connect(char const *szSvrName, UINT const &nSvrPort, char const *szUserID, char const *szUserPass, bool bIsPassive=false);
	void Close(void);

public:
	bool GetFile(char const*szRemoteFile, char *szLocalFile);
	bool GetFile(std::string const& szFileName, std::string& szSaveData);
	bool PutFile(char const *szRemoteFile, char const*szLocalFile);

public:
	void DelFile(char const*szRemoteFile);

public:
	bool SetCurDir(char const*szRemoteDirectory);
	bool GetCurDir(char *szRemoteDirectory);

public:
	void NewDirectory(char const*szRemoteDirectory);
	void DelDirectory(char const*szRemoteDirectory);


public:
	virtual void OnStart(void) { };
	virtual void OnReceive(void) { };
	virtual void OnCompleted(void) { };


private:
	HINTERNET m_hInternet;
	HINTERNET m_hFtp;

private:
	bool m_bIsPassive;

private:
	void Command(char const*cmd);
	void MakeLocalPath(char *path);

};





