

#pragma once

class cScenePreview
{
protected:
	enum	ePREVIEW_MODE{	PM_WORLD, PM_CHAR, };	
	ePREVIEW_MODE			m_ePreviewMode;
public:
	cScenePreview();
	~cScenePreview();

	// ====== Delete
public:
	void					Delete();	

	// ====== Initialition
public:
	void					Init();
	void					Reset();
protected:
	void					_InitCamera();	

	// ====== Update
public:
	void					Update();
	// ====== Render
public:
	void					Render();

	// ====== Mouse
public:
	virtual void			OnLButtonDown( CPoint pt );
	virtual void			OnLButtonUp( CPoint pt );
	virtual void			OnRButtonDown( CPoint pt ); 
	virtual void			OnRButtonUp( CPoint pt );
	virtual void			OnMouseMove( CPoint pt );
	virtual void			OnMouseWheel( short zDelta );

	// ====== Key
public:
	void					KeyEscape();
};
