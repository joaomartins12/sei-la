
#pragma once 

class CsImageInterpolation
{
public:
	CsImageInterpolation( BYTE* pInData, int nSrcWidth, int nSrcHeight, int nBytePerPixel, bool bAutoDelete = true );
	~CsImageInterpolation();

protected:
	BYTE*		m_pInData;
	BYTE*		m_pOutData;
	int			m_nSrcWidth;
	int			m_nSrcHeight;
	int			m_nBytePerPixel;

	bool		m_bAutoDelete;

public:
	void		Delete();
	BYTE*		Liner( int nOutWidth, int nOutHeight );
};