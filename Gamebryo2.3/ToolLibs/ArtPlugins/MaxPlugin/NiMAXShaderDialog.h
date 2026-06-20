// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIMAXSHADERDIALOG
#define NIMAXSHADERDIALOG

#include "NiMAXShader.h"
#include "NiMAXPerTextureDialog.h"

#define SHADER_NCOLBOX 4
#define SHADER_NORMALMAPTECH_COUNT 4
//class NiMAXPerTextureDialog;

class NiMAXShaderDialog : public ShaderParamDlg 
{
public:
    
    NiMAXShader*    m_pShader;
    NiMAXPerTextureDialog* m_pTextureExportDlg;
    StdMat2*        m_pMtl;
    HPALETTE        m_hOldPal;
    HWND            m_hwmEdit;
    IMtlParams*     m_pMtlPar;
    HWND            m_hRollup;
    TimeValue       m_curTime;
    BOOL            m_valid;
    BOOL            m_isActive;

    TexDADMgr       m_dadMgr;

    NiMAXShaderDialog( HWND hwMtlEdit, IMtlParams *pParams ); 
    ~NiMAXShaderDialog(); 

    int FindSubTexFromHWND(HWND hw);

    Class_ID ClassID();
    void DeleteThis();

    // Set shader or material host from incoming values
    void SetThing(ReferenceTarget *m);
    void SetThings( StdMat2* theMtl, Shader* theShader );

    // Get the shader and material host
    ReferenceTarget* GetThing();
    Shader* GetShader();
    
    void SetTime(TimeValue t);

    // Is there a keyframe at this time?
    BOOL KeyAtCurTime(int id);

    void ActivateDlg( BOOL dlgOn );
    HWND GetHWnd();

    // Get notified of changes, and update dialog controls
    void NotifyChanged();
    void LoadDialog(BOOL draw);
    void ReloadDialog();

    // Update the various components of the dialog
    void UpdateDialog( ParamID paramId );
    void UpdateMtlDisplay();
    void UpdateOpacity();

    // Start editing a color swatch
    void SelectEditColor(int i);

    BOOL PanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ); 

private:
    static const char *ms_aTestModes[NiAlphaProperty::TEST_MAX_MODES];
    static const char *ms_aAlphaModes[NiAlphaProperty::ALPHA_MAX_MODES];
    static const char *ms_aApplyModes[NiTexturingProperty::APPLY_MAX_MODES];
    static const char *
        ms_aVertexModes[NiVertexColorProperty::SOURCE_NUM_MODES];
    static const char *
        ms_aLightingModes[NiVertexColorProperty::LIGHTING_NUM_MODES];

    static const char *NiMAXShaderDialog::ms_aNormalMapTechniques[
        SHADER_NORMALMAPTECH_COUNT];

    ICustButton* m_texMBut[NMBUTS];

    IColorSwatch *m_colorSwatch[SHADER_NCOLBOX];

    ISpinnerControl *m_pShininessSpinner;
    ISpinnerControl *m_pAlphaSpinner;
    ISpinnerControl *m_pParallaxOffsetSpinner;
    ISpinnerControl *m_pBumpMagnitudeSpinner;
    ISpinnerControl *m_pTestRefSpinner2;

    void UpdateColSwatches();
    void UpdateMapButtons();

    static int ColorIDCToIndex(int idc);

};

// MAX does _not_ want this routine to be a member of GamebryoShaderDlg
BOOL CALLBACK GamebryoShaderDlgProc(HWND hwndDlg, 
                                      UINT msg, 
                                      WPARAM wParam, 
                                      LPARAM lParam);

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXShaderDialog.inl"

//---------------------------------------------------------------------------

#endif
