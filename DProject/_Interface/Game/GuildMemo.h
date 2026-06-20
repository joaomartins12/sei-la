#pragma once

class cGuildMemo : public cBaseWindow
{
public:
	cGuildMemo(){}

public:
	enum eMEMOSTAT { WRITE = 0, READ, };
	eMEMOSTAT		m_eStat;

	virtual eWINDOW_TYPE	GetWindowType(){ return WT_GUILD_MEMO; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			ResetMap(){ Close(false); }

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual bool			OnEscapeKey() { return false; }
	virtual void			OnMoveWindow();

	virtual void			OnRButtonUp( CsPoint pos );
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnMouseOutWindow();
protected:
	cSprite*			m_pSprite[ 9 ];	// BG
	cSprite*			m_pInputBox;	// 메모 입력칸 표시
	cButton*			m_pOKBtn;		// 저장 버튼
	cButton*			m_pCancelBtn;	// 취소 버튼
	cEditBox*			m_pEditMemo;	// 메모 입력
	std::wstring		m_szMemoInput;	// 에디트박스에서 입력한 메모 스트링
	cStringList			m_strMemo;		// 화면에 보여줄 메모 스트링

	void				OnEditBox( TCHAR const* szMemoInput );
};