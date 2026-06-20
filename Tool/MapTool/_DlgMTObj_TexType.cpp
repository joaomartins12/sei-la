// _DlgMTObj_TexType.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTObj_TexType.h"


// _DlgMTObj_TexType 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTObj_TexType, CDialog)

_DlgMTObj_TexType::_DlgMTObj_TexType(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTObj_TexType::IDD, pParent)
{

}

_DlgMTObj_TexType::~_DlgMTObj_TexType()
{
}

void _DlgMTObj_TexType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE0, m_combo_TextureType[ 0 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE1, m_combo_TextureType[ 1 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE2, m_combo_TextureType[ 2 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE3, m_combo_TextureType[ 3 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE4, m_combo_TextureType[ 4 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE5, m_combo_TextureType[ 5 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE6, m_combo_TextureType[ 6 ]);
	DDX_Control(pDX, IDC_COMBO_TEXTURE_TYPE7, m_combo_TextureType[ 7 ]);
}


BEGIN_MESSAGE_MAP(_DlgMTObj_TexType, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE0, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType0)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE1, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType1)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE2, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType2)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE3, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType3)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE4, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType4)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE5, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType5)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE6, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType6)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_TYPE7, &_DlgMTObj_TexType::OnCbnSelchangeComboTextureType7)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


//========================================================================================
//
//		정보 입력
//
//========================================================================================
void _DlgMTObj_TexType::__ResetTextureType()
{
	for( int i=0; i<8; ++i )
	{
		m_combo_TextureType[ i ].ResetContent();
		m_combo_TextureType[ i ].EnableWindow( FALSE );
	}

	sUSER_DEFINE* pUserDefine = m_pCsGBObject->GetUserDefine();
	// 텍스쳐 타입을 사용 하는가
	assert_cs( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_USE_TEXTYPE );
	
	// 그룹별로 셋팅
	sUDD_TexType* pUDD = (sUDD_TexType*)pUserDefine->GetData(sUDD_TexType::Type() );
	std::map< BYTE, sUDD_TexType::sTEXTURE_GROUP* >::iterator itTexGroup = pUDD->s_mapTextureGroup.begin();
	std::map< BYTE, nsCSFILE::sFileName_c* >::iterator itFileName_c;
	std::map< BYTE, nsCSFILE::sFileName_c* >* pMapFileName_c;
	TCHAR szCombo[ MAX_FILENAME ];
	for( ; itTexGroup != pUDD->s_mapTextureGroup.end(); ++itTexGroup )
	{
		m_combo_TextureType[ itTexGroup->first ].EnableWindow( TRUE );
		pMapFileName_c = &itTexGroup->second->s_mapFileName;
		itFileName_c = pMapFileName_c->begin();
		for( ; itFileName_c != pMapFileName_c->end(); ++itFileName_c )
		{
			M2W( szCombo, itFileName_c->second->s_cFileName, MAX_FILENAME );
			m_combo_TextureType[ itTexGroup->first ].InsertString( itFileName_c->first, szCombo );
		}
		// 첫번? 오브젝트의 텍스쳐 타입으로 설정
		m_combo_TextureType[ itTexGroup->first ].SetCurSel( m_pCsGBObject->GetInfo()->GetTextureTypeIndex( itTexGroup->first ) );
	}
}

//========================================================================================
//
//		컨트롤
//
//========================================================================================

// 콤보 박스 변경
#define SELECT_COMBO_TEXTURE_TYPE( index )		void _DlgMTObj_TexType::OnCbnSelchangeComboTextureType##index()\
{\
	LIST_OBJMODEL* plistModel = g_ObjectMng.GetListSelectModel();\
	LIST_OBJMODEL_IT it = plistModel->begin();\
	CsGBObject* pObject = NULL;\
	for( ; it != plistModel->end(); ++it )\
{\
	pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( (*it)->GetObjectInfo() );\
	pObject->SetTextureType( index, m_combo_TextureType[ index ].GetCurSel() );\
	CsGBObject::GlobalSetTextureType( (*it)->GetNiNode(), (*it)->GetObjectInfo(), pObject->GetUserDefine(), index, m_combo_TextureType[ index ].GetCurSel()  );\
}\
}\

SELECT_COMBO_TEXTURE_TYPE( 0 );
SELECT_COMBO_TEXTURE_TYPE( 1 );
SELECT_COMBO_TEXTURE_TYPE( 2 );
SELECT_COMBO_TEXTURE_TYPE( 3 );
SELECT_COMBO_TEXTURE_TYPE( 4 );
SELECT_COMBO_TEXTURE_TYPE( 5 );
SELECT_COMBO_TEXTURE_TYPE( 6 );
SELECT_COMBO_TEXTURE_TYPE( 7 );

void _DlgMTObj_TexType::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		m_pCsGBObject = GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetUniqCsGBObject();
		__ResetTextureType();
	}
}
