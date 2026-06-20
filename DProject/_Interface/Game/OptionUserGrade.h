#pragma once 

class cOptionUserGrade : public cOptionBase
{
public:
	cOptionUserGrade(){}
public:
	  virtual eWINDOW_TYPE	GetWindowType(){ return WT_OPTION_USERGRADE; }
	  virtual void			Destroy();
	  virtual void			DeleteResource();
	  virtual void			Create( int nValue = 0 );

	  virtual void			Update(float const& fDeltaTime);
	  virtual eMU_TYPE		Update_ForMouse();
	  virtual void			Render();

	  virtual void			ResetDevice(); 
	  virtual void			ResetMap(){ Close( false ); }
	  virtual bool			OnEscapeKey(){ _OnCancel(); return true; }

protected: 

	cStringList			m_StringList;
	cSprite*			m_pLogo1;
	cSprite*			m_pLogo2;

protected:
	void				_UpdateDeltaTime();
};