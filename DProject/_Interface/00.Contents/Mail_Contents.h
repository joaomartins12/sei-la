#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptCashshop;

class MailContents
	: public IUIContentSubject< MailContents >
{
public:
	struct sMAILINFO
	{
		typedef std::pair<DWORD,unsigned int>				PAIR_ITEM_DATA;

		typedef std::list<PAIR_ITEM_DATA>					MAIL_ITEMS;
		typedef std::list<PAIR_ITEM_DATA>::iterator			MAIL_ITEMS_IT;
		typedef std::list<PAIR_ITEM_DATA>::const_iterator	MAIL_ITEMS_CIT;

		sMAILINFO():s_nSendTimeTS(0),s_nMailType(0)
		{}

		~sMAILINFO() {}

		void			Reset();

		void			SetSenderName( std::wstring const& wsSender );
		void			SetMessage( std::wstring const& wsMessage );
		void			SetSendTime( uint const& time );
		void			SetMailType( u2 const& mailType );


		std::wstring	GetSenderName() const;
		std::wstring	GetMessage() const;
		uint			GetSendTime() const;
		MAIL_ITEMS const& GetMailItems() const;
		u2				GetMailType() const;

	private:
		std::wstring	s_szSenderName;
		std::wstring	s_szMessage;
		uint			s_nSendTimeTS;
		u2				s_nMailType;

		MAIL_ITEMS		s_Items;
	};

public:
	static int const IsContentsIdentity(void);

public:
	MailContents(void);
	virtual ~MailContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);


public:
	void				OpenMailWindow();
	sMAILINFO const&	GetMailInfo() const;
	void				SendMailConfirm();
	void				GetCashItemData(DWORD const& dwProductIDX, std::wstring & wsProductName, LIST_PACKAGE_ITEMS & listData);

private:
	void				Ntf_User_Message(void* pData);

private:
	sMAILINFO						m_listMail;
	TOwnershipGuard<AdaptCashshop>	m_pCashshopContents;
};