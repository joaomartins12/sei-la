
#pragma once



/**
 * \ingroup     :	CsGBChar
 * \date        :	2009-11-06
 * \author      :	song
 *
 * \remarks 
 *	케릭터 클래스의 가장 상위 클래스 
 *
 */
class CsC_Object : public NiMemObject
{
	CsCRTTI_H

public:
	enum { INAVLIDE_FT_ID = ( DWORD )-1, /**< 존재하지 않는 FTID */ };

public:
	CsC_Object(){ m_nIDX = 0; m_nFileTableID = 0; m_pRefLoad = NULL; }
	/**< 생성자 - 초기화 */

	//====================================================================================
	//		Virtual
	//====================================================================================
public:
	virtual void		Delete(){ m_nIDX = 0; m_nFileTableID = 0; if( m_pRefLoad != NULL ){ m_pRefLoad->DecreaseRef(); m_pRefLoad = NULL; } }
	/**< 모든 값 초기화 - 쓰레드 참조 감소 */
	virtual void		PreDelete(){}

	virtual bool		IsDelete(){ return ( m_RefLinkObject.GetRef() == 0 )&&( m_RefOnThreadLoad.GetRef() == 0 )&&( m_RefLinkPart.GetRef() == 0 ); }
	/**< 제거 가능한가? ( 현재 쓰레드에서 요청한것이 있는가 ) */

	virtual bool		IsEnableThreadLoad( bool bKFM ){ return ( m_RefLinkObject.GetRef() == 0 )&&( m_RefLinkPart.GetRef() == 0 ); }

	/**
	 * \author      :	      song
	 * \remarks 
	 * 진화시에 KFM 변경시 사용
	 * \param nFTID 파일 테이블 ID
	 */
	virtual void		ChangeKFM( UINT nFTID ){ m_nFileTableID = nFTID; assert_cs( m_pRefLoad ); m_pRefLoad->DecreaseRef(); m_pRefLoad = NULL; }

	/**
	 * \author      :	      song
	 * \remarks 
	 * 파트너몬 변경 시에 사용
	 * \param nFTID 파일테이블 ID
	 * \param szName 변경 되는 케릭터의 이름
	 */
	virtual void		ChangeModel( UINT nFTID, LPCTSTR szName ){ m_nFileTableID = nFTID; assert_cs( m_pRefLoad ); m_pRefLoad->DecreaseRef(); m_pRefLoad = NULL; }
	virtual void		ThreadCallBack_LoadKFM( sLOAD_KFM* pLoad ){ assert_cs( m_pRefLoad == NULL ); m_pRefLoad = pLoad->s_pRefLoad; }
	/**< 쓰레드에서 KFM 로드 완료시 호출 */
	virtual void		ThreadCallBack_LoadNIF( sLOAD_NIF* pLoad ){ assert_cs( m_pRefLoad == NULL ); m_pRefLoad = pLoad->s_pRefLoad; }
	/**< 쓰레드에서 NIF 로드 완료시 호출 */
	virtual void		ThreadCallBack_LoadCompleate( int unitType, int nAddRefCount );
	/**< 쓰레드에 요청한 모든로드 완료시 호출 */	

	//====================================================================================
	//		Load  RefCont	- 임의로 호출 금지		( 로드된 레퍼런스 카운트 )
	//====================================================================================
private:
	/**< 파일에서 참조하는 레퍼런스 카운트 ( 총 몇개의 오브젝트가 로드 되어 있는 상태인가 ) */
	sLOAD_THREAD_REF*	m_pRefLoad;	

	//====================================================================================
	//		Thread RefCont			- 임의로 호출 금지 ( 쓰레드에 요청한 레퍼런스카운트
	//====================================================================================
private:
	CThreadSafeRef		m_RefLinkObject;
	CThreadSafeRef		m_RefLinkPart;
	CThreadSafeRef		m_RefOnThreadLoad;
	
	/**< 쓰레드에 요청한 레퍼런스 카운트 */

public:	
	virtual void		IncreaseRefOnThreadLoad(){ m_RefOnThreadLoad.IncreaseRef(); }
	/**< 쓰레드에 요청 갯수 증가 */
	inline void			DecreaseRefOnThreadLoad(){ assert_cs( m_RefOnThreadLoad.GetRef() > 0 ); m_RefOnThreadLoad.DecreaseRef(); }
	/**< 쓰레드에 요청 갯수 감소 */
	int					GetRefOnThreadLoad(){ return m_RefOnThreadLoad.GetRef(); }
	/**< 쓰레드에 요청한 갯수 */

	int					__GetRefLoadCnt() { return m_pRefLoad->GetRef(); }

	//virtual void		IncreaseRefLinkObject(){ m_RefLinkObject.IncreaseRef(); }
	void				AddRefLinkObject( int nAdd ){ m_RefLinkObject.AddRef( nAdd ); }
	inline void			DecreaseRefLinkObject(){ assert_cs( m_RefLinkObject.GetRef() > 0 ); m_RefLinkObject.DecreaseRef(); }
	int					GetRefLinkObject(){ return m_RefLinkObject.GetRef(); }

	//virtual void		IncreaseRefLinkPart(){ m_RefLinkPart.IncreaseRef(); }
	void				AddRefLinkPart( int nAdd ){ m_RefLinkPart.AddRef( nAdd ); }
	inline void			DecreaseRefLinkPart(){ assert_cs( m_RefLinkPart.GetRef() > 0 ); m_RefLinkPart.DecreaseRef(); }
	int					GetRefLinkPart(){ return m_RefLinkPart.GetRef(); }


	//====================================================================================
	//		Initialize
	//====================================================================================
public:
	/**
	 * \author      :	      song
	 * \remarks 
	 * 초기화
	 * \param nIDX 서버에서 통신하는 IDX, 대부분 0~4095 사이 (UniqID 랑은 틀리다 )
	 * \param nFileTableID 파일 테이블 ID
	 */
	void				Init( unsigned int nIDX, unsigned int nFileTableID ){ 
		m_nIDX = nIDX; m_nFileTableID = nFileTableID;
		assert_cs( m_RefOnThreadLoad.GetRef() == 0 );
		assert_cs( m_RefLinkObject.GetRef() == 0 );
		assert_cs( m_RefLinkPart.GetRef() == 0 );
	}	

	//====================================================================================
	//		Load
	//====================================================================================
protected:
	virtual void		_LoadedFile( int nCallRTTI ){}

	//====================================================================================
	//		ID
	//====================================================================================
public:	
	UINT GetIDX(void)  const { return m_nIDX; }
	/**< IDX 리턴 */
	UINT GetFTID(void) const { return m_nFileTableID; }
	/**< 파일테이블ID리턴 */

protected:	
	UINT				m_nIDX;	
	/**< 서버에서 통신하는 IDX, 대부분 0~4095 사이 (UniqID 랑은 틀리다 ) */
	UINT				m_nFileTableID;	
	/**< 파일테이블 ID */
};