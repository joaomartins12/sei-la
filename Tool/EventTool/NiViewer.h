

#pragma once 

class NiViewer : public CView
{
protected: // serialization에서만 만들어집니다.
	NiViewer();
	DECLARE_DYNCREATE(NiViewer)


public:
	void		_Init(){}
	void		_Update();	

protected:
	void		_SetEnumAxisRotation( NiColor emittance );
	NiAVObject* _PickAxisToRenderAxis( NiAVObject* pPickAxis );

	enum eAXIS_ROTATION{ AR_Z, AR_Y, AR_X };
	eAXIS_ROTATION	m_eAR;
	CPoint		m_ptPos;
	bool		m_bClickAxis;

protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};