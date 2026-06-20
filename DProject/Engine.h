

#pragma once

#define MAX_GAUSSIANBLUR_SAMPLE			9

#include "_DeviceInfo\DeviceInfo.h"
//#include <ddraw.h>
#pragma comment(lib, "DDraw.lib")

bool	ResetDevice(bool bBeforeReset, void* pvData);

#define MAX_CHAR_RENDER_COUNT			8000

struct sDEVICE_STATE
{
	bool	s_bEnablePS_2_0;
	int		s_nMaxSimultaneousTextures;		// 한패쓰에 사용할수 있는 샘플러 갯수
	int		s_nMaxTextureBlendStage;		// 디바이스에 몇개의 텍스쳐 까지 들어가는가
	bool	s_bTRsw_Blend_Enalbe;
	bool	s_bDxVersion_9_0;
};

class CEngine : public NiMemObject
{
public:
	CEngine();
	virtual ~CEngine();
	

	
	void ConfirmShader(NiAVObject* pkObject);
	void RecursiveConfirmShader(NiAVObject* pkObject);


// 	void DrawText( int x, int y, DWORD color, UINT Align, TCHAR const* strText, ... );
// 	void DrawText_OutLine( int x, int y, DWORD color, UINT Align, TCHAR const* szStr );
// 	void DrawTextRender( int x, int y, DWORD color, TCHAR const* str, UINT Align  );
// 
// 	void DrawText( CsPoint pos, CsPoint size, DWORD color, UINT Align, TCHAR const* strText, ... );
// 	void DrawText_OutLine( CsPoint pos, CsPoint size, DWORD color, UINT Align, TCHAR const* szStr );
// 	void DrawTextRender( CsPoint pos, CsPoint size, DWORD color, UINT Align, TCHAR const* str );

	//==============================================================================
	//
	//		베이스
	//
	//==============================================================================
public:
	static void			GBInit();
	static bool			Init();	
	static void			ShutDown();

	bool				Create();
	void				Delete();
	bool				ResetDevice();

	bool				Update();
	void				Render();
//	void				UpdateInput();

	bool				UpdateDeltaTime();

	void				ResetDevice_Pre();
	void				ResetDevice_Post();


	//==============================================================================
	//
	//	Post Effect
	//
	//==============================================================================

public:
	enum ePOST_EFFECT{
		PE_NONE, PE_GAUSSIAN_BLUR, PE_DOF,
	};

	void					_UpdateMSAARenderTargetSurface();
protected:	
	ePOST_EFFECT					m_ePostEffect_Set;
	ePOST_EFFECT					m_ePostEffect_Start;

	bool							m_bEnablePostEffect;

	NiRenderedTexturePtr			m_pRenderTexture_Org;
	NiRenderTargetGroupPtr			m_pRT_Org;

protected:
	void					_ResetDevice_PostEffect( bool bBefore );
	void					_DeletePostEffect();
	void					_InitPostEffect();	
	bool					_CreateMSAARenderTargetSurface();

public:
	ePOST_EFFECT			GetPostEffect(){ return m_ePostEffect_Set; }
	void					SetPostEffect( ePOST_EFFECT pe );

	NiRenderTargetGroup*	GetCurrentRenderTargetGroup();
	NiRenderTargetGroup*	GetRenderTargetGroup( ePOST_EFFECT pe );

	bool					IsEnablePostEffect(){ return m_bEnablePostEffect; }

	//==============================================================================
	//		가우시안 블러
	//==============================================================================
protected:	
	NiRenderTargetGroupPtr			m_pRT_GB_Blur;
	NiScreenFillingRenderViewPtr	m_pScreen_GB;

	D3DXVECTOR4						m_vGaussianBlurVal;

	int								m_nGaussianBlurMSAA;	
	NiPoint2						m_GaussianOffset[ MAX_GAUSSIANBLUR_SAMPLE ];
	float							m_GaussianWeights[ MAX_GAUSSIANBLUR_SAMPLE ];
protected:
	void			_Delete_GaussianBlur();
	bool			_Create_GaussianBlur();
	void			_SetGaussianBlurParameter( float fX, float fY, int nPixelCount );
	float			_ComputeGaussianBlur( float n );

	void			_Render_PE_GB();

public:	
	D3DXVECTOR4*	GetGaussianBlurVal(){ return &m_vGaussianBlurVal; }
	void			SetGaussianBlurVal( float fOrgVal, float fBlurVal, float fFillter );

	//==============================================================================
	//		Depth Of Field
	//==============================================================================
protected:
	NiRenderTargetGroupPtr			m_pRT_DOF_Blur;
	NiRenderTargetGroupPtr			m_pRT_DOF_Depth;
	NiScreenFillingRenderViewPtr	m_pScreen_DOF;
	NiColorA						m_BackupBackGroundColor;

protected:
	void			_Delete_DOF();
	bool			_Create_DOF();

	void			_Render_PE_DOF();

public:
	void			Begin_PE_DOF_Depth();

	//==============================================================================
	//
	//		렌더러
	//
	//==============================================================================
public:
	bool				m_bD3D10;
	CsGBDX9RendererPtr	m_spRenderer;	
	NiDX9Renderer::FrameBufferFormat	m_nFrameBufferFormat;
	NiDX9Renderer::DepthStencilFormat	m_nDepthBufferFormat;
	
	//NiCamera*			m_pCamera;
	bool				m_bScreenSpace;
	bool				m_bOffScreen;
	sDEVICE_STATE		m_DeviceCaps;

	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	D3DXMATRIX			m_matViewProj;
	D3DVIEWPORT9		m_ViewPort;

private:
	NiDX9Renderer::RecreateStatus m_eDeviceStatus;

public:
	D3DXMATRIX*			GetD3DView(){ return &m_matView; }
	D3DXMATRIX*			GetD3DProj(){ return &m_matProj; }
	D3DXMATRIX*			GetD3DViewProj(){ return &m_matViewProj; }
	D3DVIEWPORT9*		GetD3DViewPort(){ return &m_ViewPort; }
	NiPoint3			Cal3D_To_2D( NiPoint3 vPos );

	// static
	static NiDX9Renderer*		GetDX9Renderer();
	static LPDIRECT3DDEVICE9	GetD3DDevice();
	static LPDIRECT3D9			GetD3D();

protected:
	bool				_CreateRender( bool bD3D10 = FALSE );
	bool				_CheckDeviceCaps();

public:	
	bool				StartRender( ePOST_EFFECT PostEffectStart );
	bool				EndRender();	

	void				Change_ShaderRender_Render();
	void				Change_Render_ShaderRender();

	void				ScreenSpace();
	void				ResetRendererCamera(){ m_spRenderer->SetCameraData( CAMERA_ST.GetCameraObj() ); m_bScreenSpace = false; }
	void				ResetRenderer();

	void				Clear();
	void				SetClearColor(NiColorA p_ClearColor);
	bool				BeginFrame();
	bool				BeginUsingRenderTargetGroup(NiRenderTargetGroup* pGroup, NiRenderer::ClearFlags p_eFlags);
	bool				BeginUsingCurrentRenderTargetGroup(NiRenderer::ClearFlags p_eFlags);
	bool				BeginDefaultRenderTarget(NiRenderer::ClearFlags p_eFlags);
	bool				EndRenderTarget();
	bool				EndFrame();
	bool				DisplayFrame();
	bool				BeginOffScreenFrame();
	bool				EndOffScreenFrame();

	//==============================================================================
	//
	//		쉐이더
	//
	//==============================================================================
protected:
	void				_CreateShaderSystem();
	bool				_RunShaderParsers(const char* pcShaderDir);
	bool				_RunShaderLibraries(const char* pcShaderDir);

public:
	static bool ShaderCallback_LibraryClassCreate(	const char* pcLibFile, 
													NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
													bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
	static bool FXLibraryClassCreate(				const char* pcLibFile,
													NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
													bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);

	static unsigned int ShaderCallback_RunParser(	const char* pcLibFile, 
													NiRenderer* pkRenderer, const char* pcDirectory, 
													bool bRecurseSubFolders);

	static unsigned int ShaderCallback_Errors(		const char* pcError, 
													NiShaderError eError, bool bRecoverable);
	//==============================================================================
	//
	//		인풋
	//
	//==============================================================================
public:
	CInput* m_pInput;

	//==============================================================================
	//
	//		프레임
	//
	//==============================================================================
public:
	CFrame			m_Frame;

	//==============================================================================
	//
	//		인터페이스
	//
	//==============================================================================
protected:
//	IDirectDraw*		m_lpDD;
public:
	int					m_nBackupScreenWidth;
	int					m_nBackupScreenHeight;
	int					m_nOldScreenWidth;
	int					m_nOldScreenHeight;	
	bool				m_bUseToggleWindow;
public:
	void				ToggleWindow( bool bFullScreen, bool bForceToggle = false );
	void				ChangeResolution();
	void				ChangeResolutionMode( bool bFullMode, int nWidth, int nHeight );
	
	//==============================================================================
	//
	//		폰트
	//
	//==============================================================================
public:
	CFont			m_FontSystem;
	CFont			m_FontText;
//	LPD3DXFONT		m_pD3DXFont;

public:
	bool			CreateFont();
	void			DeleteFont();

	//==============================================================================
	//
	//		카메라
	//
	//==============================================================================
protected:
	void			_DeleteCamera();
public:
	void			CreateCamera();


	//==============================================================================
	//
	//		D3DXLINE
	//
	//==============================================================================
public:
 	LPD3DXLINE		m_pD3DXLine;
 	void			CreateLine();
 	void			DeleteLine();
	

};

extern CEngine*		g_pEngine;


