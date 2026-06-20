#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class ServerSelectContents
	: public IUIContentSubject< ServerSelectContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eRecieve_ServerList, // 서버목록 받기
	};

public:
	ServerSelectContents(void);
	virtual ~ServerSelectContents(void);

	virtual int const	GetContentsIdentity(void) const;
	virtual bool 		Initialize(void);
	virtual void 		UnInitialize(void);
	virtual bool 		IntraConnection(ContentsSystem& System);
	virtual void 		Update(float const& fElapsedTime);
	virtual void 		NotifyEvent(int const& iNotifiedEvt);
	virtual void 		MakeMainActorData(void);
	virtual void 		ClearMainActorData(void);

	void				ServerListClear();
	UINT				GetSelectedLastServerIdx();
	void				ConnectingServer( const int& nSelServer );
	void				gotoBack();

	void				RequestServerList();
	void				RecvServerList(void* pData);
	void				RecvServerSelectCluster(void * pData);
	void				ResetServerList();
	
	// [5/2/2016 hyun] ServerSelect 클래스에서 옮김
	void				CheckConnectAccountSever();
	bool				IsVisible() const;

	std::list<sServerInfo> const& GetServerList() const;
	std::string			GetServerImg(int const& nServerIdx) const;

	void				RenderBackgroundMap();
	void				UpdateBackgroundMap(float const& fAccumTime);

	void				Load3DBackgroundData();
	void				Remove3DBackgroundData();

private:
	sServerInfo const*	GetServerInfoFromIdx( int const& nIdx ) const;
	sServerInfo const*	GetNewServerInfo() const;
	sServerInfo const*	GetFirstServerInfo() const;
	sServerInfo const*	GetHaveManyCharacterServerInfo() const;

	bool				Load3DEffect(std::string const& loadFileName);

private:
	bool				m_bVisible; 
	CsNodeObj			m_Effect;
	CsEnableTimeSeq		m_RefreshTime;

	std::list<sServerInfo>	m_ServerInfoList;
};
