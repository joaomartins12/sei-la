
#pragma once 

#define MAX_SCO_KIND		16

class CsGBCell : public NiMemObject
{
public:
	CsGBCell();
	~CsGBCell();


	struct sINFO
	{
		NiTransform		s_trObject;
		NiTransform		s_trParent;
	};

	struct sOBJECT : public NiMemObject
	{
		sOBJECT():s_fAniTime(0.0f), s_fAniSpeed(0.0f), s_fRemainCheckTime(0.0f)
		{}

		sINFO			s_Info;
		float			s_fAniTime;
		float			s_fAniSpeed;
		float			s_fRemainCheckTime;
	};

public:
	static NiMatrix3	m_ConstantMatrix;

public:
	void			Delete();
	void			Init( NiPoint3 ptCenter, float fRadius );

	void			Update( float fElapsedTime );
	bool			Culling( float fViewerHeight );
	void			Render( int nBaseIndex, bool& bFirst );

	sINFO*			Resist( int nBaseIndex, NiTransform* trParent, NiTransform* trObject, bool bAnimation );

protected:	
	NiPoint3		m_ptCenter;
	float			m_fRadius;

public:
	int				GetSize( int nBaseIndex ){ return m_vpObject.IsExistElement( nBaseIndex ) ? m_vpObject[ nBaseIndex ]->s_vpElement.Size() : 0; }
	sINFO*			GetInfo( int nBaseIndex, int nObjectIndex ){ return &m_vpObject[ nBaseIndex ]->s_vpElement[ nObjectIndex ]->s_Info; }
	void			DeleteBaseObject( int nBaseIndex );
	void			DeleteObject( int nBaseIndex, int nObjectIndex );
	void			DeleteObject( int nBaseIndex, NiTransform* pTrParent );


	// 오브젝트
protected:
	struct sOBJECT_ARRAY : public NiMemObject
	{
		CsVectorPB< sOBJECT* >		s_vpElement;
	};
	CsVectorPB< sOBJECT_ARRAY* >	m_vpObject;

	// 익스트라 데이타
public:
	UINT			GetExtraDataSize();
	void			SaveExtraData( char* pData, UINT& nOffset );
	void			LoadExtraData( char* pData, UINT& nOffset );
};