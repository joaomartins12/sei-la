
#pragma once 

#include <math.h>

struct CsPoint : public tagPOINT
{
	CsPoint(){ x = 0; y = 0; }
	CsPoint( int X, int Y ){ x = X; y = Y; }

	int			Sum(){ return x + y; }
	int			Mul(){ return x * y; }
	float		Length(){ return (float)sqrt( (double)( x*x + y*y ) ); }

	void		operator=( CsPoint d ){ x = d.x; y = d.y; }
	bool		operator==( CsPoint d ){ return (x == d.x)&&(y == d.y); }
	bool		operator!=( CsPoint d ){ return (x != d.x)||(y != d.y); }
	void		operator-=( CsPoint d ){ x -= d.x; y -= d.y; }
	void		operator+=( CsPoint d ){ x += d.x; y += d.y; }
	CsPoint		operator-( CsPoint d ){ return CsPoint( x-d.x, y-d.y ); }
	CsPoint		operator+( CsPoint d ){ return CsPoint( x+d.x, y+d.y ); }	

	CsPoint		operator*( float f ){ return CsPoint( (int)(x*f), (int)(y*f) ); }
	CsPoint		operator/( float f ){ return CsPoint( (int)(x/f), (int)(y/f) ); }
	void		operator*=( float f ){ x = (int)(x*f); y = (int)(y*f); }

	
	static const CsPoint		ZERO;
};

struct CsSIZE : public tagSIZE
{
	// Constructors
public:
	// uninitialized rectangle
	CsSIZE(){}
	CsSIZE( int x, int y ){ cx = x; cy = y; }
	CsSIZE( CsPoint point ){ cx = point.x; cy = point.y; }

	// Additional Operations
	void		operator=(const CsSIZE& src){ memcpy( this, &src, sizeof( CsSIZE ) ); }
	bool		operator==(const CsSIZE& src){ return (cx == src.cx)&&(cy == src.cy); }
	bool		operator!=(const CsSIZE& src ){ return (cx != src.cx)||(cy != src.cy); }

	CsSIZE		operator*( float f ){ return CsSIZE( (int)(cx*f), (int)(cy*f) ); }
	void		operator*=( float f ){ cx = (int)(cx*f); cy = (int)(cy*f); }
};

struct CsRect : public tagRECT
{
	// Constructors
public:
	// uninitialized rectangle
	CsRect(){}
	CsRect(int l, int t, int r, int b){ left = l; top = t; right = r; bottom = b; }
	CsRect( CsRect* lpSrcRect){ memcpy( this, lpSrcRect, sizeof( CsRect ) ); }
	CsRect( CsPoint point, CsSIZE size ){ left = point.x; top = point.y; right = left+size.cx; bottom = top+size.cy; }
	CsRect( CsPoint topLeft, CsPoint bottomRight){ left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }

	int Width() const { return right - left; }
	int Height() const { return bottom - top; }	
	CsPoint CenterPoint() const { return CsPoint( (left+right)/2, (top+bottom)/2); }
	CsPoint GetPos() const { return CsPoint( left, top ); }
	CsPoint GetSize() const { return CsPoint( Width(), Height() ); }
	// returns TRUE if point is within rectangle
	BOOL PtInRect( CsPoint point ){ return ::PtInRect( this, point ); }

	// set rectangle from left, top, right, and bottom
	void SetRect( int l, int t, int r, int b ){ left = l; top = t; right = r; bottom = b; }
	void SetRect( CsPoint topLeft, CsPoint bottomRight ){ left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }
	void SetRect( CsPoint point, CsSIZE size ){ left = point.x; top = point.y; right = left+size.cx; bottom = top+size.cy; }	

	BOOL IntersectRect( CsRect* lpRect1, CsRect* lpRect2 ){ return ::IntersectRect( this, lpRect1, lpRect2 ); }

	// Additional Operations
	void operator=(const CsRect& srcRect){ memcpy( this, &srcRect, sizeof( CsRect ) ); }
	BOOL operator==(const CsRect& rect){ return ::EqualRect( this, &rect ); }
	BOOL operator!=(const CsRect& rect){ return !::EqualRect(this, &rect); }
	void operator+=(CsPoint point){ ::OffsetRect(this, point.x, point.y); }
	void operator+=(CsSIZE size){ ::OffsetRect(this, size.cx, size.cy); }
	void operator-=(CsPoint point){ ::OffsetRect(this, -point.x, -point.y); }
	void operator-=(CsSIZE size){ ::OffsetRect(this, -size.cx, -size.cy); }

	CsRect operator+(CsPoint point){ CsRect rc( this ); ::OffsetRect( &rc, point.x, point.y ); return rc; }
	CsRect operator-(CsPoint point){ CsRect rc( this ); ::OffsetRect( &rc, -point.x, -point.y ); return rc; }	
	CsRect operator+(CsSIZE size){ CsRect rc( this ); ::OffsetRect( &rc, size.cx, size.cy ); return rc; }
	CsRect operator-(CsSIZE size){ CsRect rc( this ); ::OffsetRect( &rc, -size.cx, -size.cy ); return rc; }

	static const CsRect		ZERO;
};

