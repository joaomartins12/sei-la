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

#ifndef NIMAXSHADER_H
#define NIMAXSHADER_H

#include "NiMAX.h"

#include "MiResource.h"
#include "MiUtils.h"

#define NISHADER_CLASSID Class_ID(0x670a77d0, 0x23ab5c7f)
#define NISHADER_CLASSID2 Class_ID(0x670a77d1, 0x23ab5c80)

#define NI_MAX_SHADER_VERSION 6
#define NI_DEFAULT_SHADER_NAME "Default Shader"
#define NI_DEFAULT_SHADER_TECHNIQUE_NAME "Best Available for Hardware"
#define NI_DEFAULT_SHADER_TECHNIQUE_INDEX 0
#define NI_SHADER_CUST_ATTRIB_NAME "NiShaderCustAttribs"

extern ClassDesc* GetGamebryoShaderDesc();

// Paramblock2 name
enum { shader_params, texture_export_params };
// Paramblock2 parameter list
enum 
{ 
    sk_amb, sk_diff, sk_spec, sk_emit, sk_shininess, sk_alpha, sk_test_ref,
    sk_srcBlend, sk_destBlend, sk_testmode, sk_alphatest_enable, 
    sk_applyMode, sk_srcVertexMode, sk_lightingMode,
    sk_vertexColors_enable, sk_specular_enable, sk_no_sorter, sk_dither,
    // removing ps2 params would cause loading to fail for legacy files
    sk_l_base,   sk_l_dark,   sk_l_detail,   sk_l_decal1,   sk_l_decal2,   
    sk_l_glow, 
    // allowing sk_k to be 24 causes the .MAX file to be unloadable
    sk_k_base=100, sk_k_dark,   sk_k_detail,   sk_k_decal1,   sk_k_decal2,
    sk_k_glow, 
    sk_lon_base, sk_lon_dark, sk_lon_detail, sk_lon_decal1, sk_lon_decal2, 
    sk_lon_glow, 
    sk_kon_base, sk_kon_dark, sk_kon_detail, sk_kon_decal1, sk_kon_decal2,
    sk_kon_glow, 
    sk_alphaMode, sk_ps2_dark_rgb, sk_ps2_detail_rgb, sk_bump_magnitude, 
    sk_luma_offset, sk_luma_scale,
    sk_base_export, sk_dark_export, sk_detail_export, sk_decal1_export, 
    sk_decal2_export, sk_gloss_export, sk_glow_export, 
    sk_shader_name, sk_shader_viewer_technique, sk_shader_export_technique,
    sk_use_normal_maps, sk_normal_map_technique, sk_l_gloss, sk_lon_gloss,
    sk_k_gloss, sk_kon_gloss, sk_parallax_offset,
    sk_num_params
};

//---------------------------------------------------------------------------
// Definition of our UI and map parameters for the shader
//---------------------------------------------------------------------------

// Number of Map Buttons on our UI and number of texmaps
#define NMBUTS 1
#define SHADER_NTEXMAPS 10
// Channels used by this shader
#define S_BASE      0
#define S_DARK      1
#define S_DETAIL    2
#define S_DECAL     3
#define S_NORMALMAP 4
#define S_PARALLAX  5
#define S_BUMP      6
#define S_GLOSS     7
#define S_GLOW      8
#define S_PARTICLE_COLOR 9
#define S_PARTICLE_OPACITY 10
#define S_REFLECTION 11
#define S_DECAL2    12

// Texture Channel number --> IDC resource ID
static int texMButtonsIDC[] = {
    IDC_MAPON_DIFF
};
        
// Map Button --> Texture Map number
static int texmapFromMBut[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
// the diffuse button maps to the base texture


// Channel Name array
static int texNameIDS[STD2_NMAX_TEXMAPS] = {
    IDS_NI_BASE,        IDS_NI_DARK,        IDS_NI_DETAIL,      
    IDS_NI_DECAL,       IDS_NI_NORMAL,      IDS_NI_PARALLAX,
    IDS_NI_BUMP,        IDS_NI_GLOSS,       IDS_NI_GLOW,        
    IDS_NI_PCOLOR,      IDS_NI_POPAC,       IDS_NI_REFLECTION,  
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,        
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,
};  

// Channel Name array (INTERNAL NAMES)
// These must correspond to internal MAX channel names for any
// channels you want to be copied when switching shaders 
// (see StdMtl2::ShuffleTexMaps).
static int intertexNameIDS[STD2_NMAX_TEXMAPS] = {
    IDS_NI_BASE_I,      IDS_NI_DARK_I,      IDS_NI_DETAIL_I,    
    IDS_NI_DECAL_I,     IDS_NI_BUMP_I,      IDS_NI_PARALLAX,
    IDS_NI_BUMP,        IDS_NI_GLOSS_I,     IDS_NI_GLOW_I,  
    IDS_NI_PCOLOR,      IDS_NI_POPAC,       IDS_NI_REFLECTION,  
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,        
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,
    IDS_NI_NONE,        IDS_NI_NONE,        IDS_NI_NONE,
};  

// Type of map channels supported
static int chanType[STD2_NMAX_TEXMAPS] = {
    CLR_CHANNEL,         CLR_CHANNEL,         CLR_CHANNEL,  
    CLR_CHANNEL,         CLR_CHANNEL,         CLR_CHANNEL, 
    CLR_CHANNEL,         CLR_CHANNEL,         CLR_CHANNEL,
    CLR_CHANNEL,         CLR_CHANNEL,         CLR_CHANNEL, 
    UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
    UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL,
    UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL,
    UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL
};  


// Which channels in our shader map to a channel in the StdMat channel IDs?
//
// The following are the StdMat channel IDs:
//
// 0   --   ambient
// 1   --   diffuse             -> base 0
// 2   --   specular
// 3   --   shininess
// 4   --   shininess strength  -> gloss 5
// 5   --   self-illumination   -> dark 1
// 6   --   opacity
// 7   --   filter color
// 8   --   bump                -> bump 4
// 9   --   reflection          -> reflection 9
// 10  --   refraction 
// 11  --   displacement
//
// The following are the custom channel and the standard channel they 
// map to 
//
// 0   --   base        -> diffuse -> 1
// 1   --   dark map    -> self-illum -> 5
// 2   --   detail 
// 3   --   decals
// 4   --   bump        -> bump -> 8
// 5   --   gloss map   -> shininess strength -> 4
// 6   --   glow map 
// 7   --   particle color
// 8   --   particle opacity
// 9   --   reflection  -> reflection -> 9

static int stdIDToChannel[N_ID_CHANNELS] = 
{
    -1,     // 1, // base
    0,      // 5, // dark
    -1,     //-1, // detail
    -1,     //-1, // decals
    4,      // 8, // bump
    1,      // 4, // gloss
    -1,     //-1, // glow
    -1,     //-1, // particle color
    -1,     //-1, // particle opacity
    -1,     // 9, // reflection
    -1,     //-1,
    -1      //-1
 };
  
//---------------------------------------------------------------------------
// Class definition for the shader itself
//---------------------------------------------------------------------------

#define SHADER_PARAMS (STD_PARAM_SELFILLUM | STD_PARAM_SELFILLUM_CLR | \
                STD_PARAM_DIFFUSE_CLR | STD_PARAM_SPECULAR_CLR | \
                STD_PARAM_SELFILLUM_CLR_ON)

class NiMAXShaderDialog;
class NiShaderDesc;

class NiMAXShader : public Shader 
{
    friend class NiMAXShaderDialog;
    friend class NiMAXShaderCB;
public:
    enum TextureExportSetting
    {
        NI_GLOBAL,
        NI_IMAGE_DEFAULTS, 
        NI_HIGH_COLOR, 
        NI_COMPRESSED,
        NI_PALETTIZED,
        NI_NUM_EXPORT_SETTINGS
    };

    enum NBT_Method
    {
        MAX_NBT_NONE = 0,
        MAX_NBT_NDL,
        MAX_NBT_ATI,
        MAX_NBT_MAX,
        MAX_NBT_COUNT
    };

protected:

    // The parameter block
    IParamBlock2    *m_pblock; 

    Interval         m_validInterval;
    TimeValue        m_curTime;


    // Pointer to the dialog handler
    NiMAXShaderDialog *m_pDlg;


    // Storage for our parameters
    Color           m_ambient;
    Color           m_diffuse;
    Color           m_specular;
    Color           m_emittance;
    Color           m_reflection;

    float           m_fShininess;
    float           m_fBumpMagnitude;
    float           m_fLumaOffset;
    float           m_fLumaScale;
    float           m_fParallaxOffset;

    float           m_fAlpha;
    unsigned char   m_ucTestRef;
    int             m_iAlphaMode;
    int             m_iSrcBlend;
    int             m_iDestBlend;
    int             m_iAlphaTestMode;
    bool            m_bAlphaTestOn;

    int             m_iApplyMode;
    int             m_iSrcVertexMode;
    int             m_iLightingMode;

    bool            m_bVertexColorsOn;
    bool            m_bSpecularOn;
    bool            m_bNoSorter;
    bool            m_bDither;

    Color           m_dark;
    Color           m_detail;
    Color           m_decal;
    Color           m_bump;
    Color           m_gloss;
    Color           m_glow;
    Color           m_particle_color;
    Color           m_particle_opacity;

public:

    NiMAXShader();

    // Main plugin identity support
    Class_ID ClassID();
    SClass_ID SuperClassID();
    TSTR GetName();
    void GetClassName(TSTR& s);
    void DeleteThis();

    // Tell MAX what standard parameters that we support
    ULONG SupportStdParams();

    // copy std params, for switching shaders
    void CopyStdParams( Shader* pFrom );
    void ConvertParamBlk(ParamBlockDescID *oldPBDesc, 
                         int oldCount, 
                         IParamBlock *oldPB );

    // Texture map channel support
    long nTexChannelsSupported();
    TSTR GetTexChannelName( long nTex );
    TSTR GetTexChannelInternalName( long nTex );
    long ChannelType( long nChan );
    long StdIDToChannel( long stdID );

    // Find out if we have a key at time value t
    BOOL KeyAtTime(int id,TimeValue t);

    // Get the requirements for this material (supersampling, etc)
    ULONG GetRequirements( int subMtlNum );

    // Support for the dialog UI
#if MAX_RELEASE < 4000
    ShaderParamDlg* CreateParamDialog(HWND hOldRollup, HWND hwMtlEdit, 
        IMtlParams *imp, StdMat2* theMtl, int rollupOpen );
    ShaderParamDlg* GetParamDlg();
    void SetParamDlg( ShaderParamDlg* NiNewDlg );
#else
    ShaderParamDlg* CreateParamDialog(HWND hOldRollup, HWND hwMtlEdit, 
        IMtlParams *imp, StdMat2* theMtl, int rollupOpen, int );
    ShaderParamDlg* GetParamDlg(int);
    void SetParamDlg( ShaderParamDlg* NiNewDlg, int n=0 );
#endif


    // Animatables and References
    int NumSubs();
    Animatable* SubAnim(int i);
    TSTR SubAnimName(int i);
    int SubNumToRefNum(int subNum);

    int NumRefs();
    RefTargetHandle GetReference(int i);
    void SetReference(int i, RefTargetHandle rtarg);
    void NotifyChanged();
#if MAX_RELEASE <= 8000
    RefTargetHandle Clone(RemapDir &remap = NoRemap());
#else
    RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
#endif
    RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget,
        PartID& partID, RefMessage message );


    // Direct ParamBlock2 access
    int NumParamBlocks();
    IParamBlock2* GetParamBlock(int i);
    IParamBlock2* GetParamBlockByID(BlockID id);


    // Shader state methods
    void Update(TimeValue t, Interval& valid);
    void Reset();


    // Standard load / save
    IOResult Save(ISave *isave);
    IOResult Load(ILoad *iload);



    // ----------------------------------------------------------
    // The following methods are implimented in Eval.cpp

    // Fill the IllumParams with our data
    void GetIllumParams( ShadeContext &sc, IllumParams &ip );

    // Shader specific implimentations
    void Illum(ShadeContext &sc, IllumParams &ip);
    void AffectReflection(ShadeContext &sc, IllumParams &ip, Color &rcol);

    void CombineComponents( ShadeContext &sc, IllumParams& ip );

    // Metal support
    BOOL IsMetal();



    // ----------------------------------------------------------
    // The following transactions are implimented in GetSet.cpp

    // AMBIENT
    void SetAmbientClr(Color c, TimeValue t);
    Color GetAmbientClr(int mtlNum, BOOL backFace);     
    Color GetAmbientClr(TimeValue t);       

    // DIFFUSE
    void SetDiffuseClr(Color c, TimeValue t);
    Color GetDiffuseClr(int mtlNum, BOOL backFace);     
    Color GetDiffuseClr(TimeValue t);       

    // SPECULAR
    void SetSpecularClr(Color c, TimeValue t);
    Color GetSpecularClr(int mtlNum, BOOL backFace);
    Color GetSpecularClr(TimeValue t);

    // EMITTANCE
    void SetEmittance(Color c, TimeValue t);
    Color GetEmittance(int mtlNum, BOOL backFace);      
    Color GetEmittance(TimeValue t);        

    // SHINIESS
    void SetShininess(float fShininess, TimeValue t);
    float GetShininess(int mtlNum, BOOL backFace);      
    float GetShininess(TimeValue t);        

    // LUMA OFFSET
    void SetLumaOffset(float fLumaOffset);
    float GetLumaOffset();

    // LUMA SCALE
    void SetLumaScale(float fLumaScale);
    float GetLumaScale();

    // BUMP MAP MAGNITUDE
    void SetBumpMagnitude(float fBumpMagnitude);
    float GetBumpMagnitude();

    // PARALLAX OFFSET
    void SetParallaxOffset(float fParallaxOffset);
    float GetParallaxOffset();

    // ALPHA
    void SetAlpha(float fAlpha, TimeValue t);
    float GetAlpha(int mtlNum, BOOL backFace);      
    float GetAlpha(TimeValue t);        

    // TEST_REF
    void SetTestRef(unsigned char uc, TimeValue t);
    unsigned char GetTestRef(int mtlNum, BOOL backFace);
    unsigned char GetTestRef(TimeValue t);

    // SRC_BLEND
    void SetSrcBlend(int i, TimeValue t);
    int GetSrcBlend(int mtlNum, BOOL backFace);
    int GetSrcBlend(TimeValue t);

    // DEST_BLEND
    void SetDestBlend(int i, TimeValue t);
    int GetDestBlend(int mtlNum, BOOL backFace);
    int GetDestBlend(TimeValue t);

    // ALPHA_MODE
    void SetAlphaMode(int i, TimeValue t);
    int GetAlphaMode(int mtlNum, BOOL backFace);
    int GetAlphaMode(TimeValue t);

    // ALPHA_TEST_MODE
    void SetAlphaTestMode(int i, TimeValue t);
    int GetAlphaTestMode(int mtlNum, BOOL backFace);
    int GetAlphaTestMode(TimeValue t);

    // ALPHA_TEST_ENABLE
    void SetAlphaTestOn(int i, TimeValue t);
    int GetAlphaTestOn(int mtlNum, BOOL backFace);
    int GetAlphaTestOn(TimeValue t);
    
    // APPLY_MODE
    void SetTextureApplyMode(int i, TimeValue t);
    int GetTextureApplyMode(int mtlNum, BOOL backFace);
    int GetTextureApplyMode(TimeValue t);

    // SRC_VERTEX_MODE
    void SetSourceVertexMode(int i, TimeValue t);
    int GetSourceVertexMode(int mtlNum, BOOL backFace);
    int GetSourceVertexMode(TimeValue t);

    // LIGHTING_MODE
    void SetLightingMode(int i, TimeValue t);
    int GetLightingMode(int mtlNum, BOOL backFace);
    int GetLightingMode(TimeValue t);

    // Vertex Colors Enable
    void SetVertexColorsOn(int i, TimeValue t);
    int GetVertexColorsOn(int mtlNum, BOOL backFace);
    int GetVertexColorsOn(TimeValue t);

    // Specular Enable
    void SetSpecularOn(int i, TimeValue t);
    int GetSpecularOn(int mtlNum, BOOL backFace);
    int GetSpecularOn(TimeValue t);

    // NoSorter flag
    void SetNoSorter(int i, TimeValue t);
    int GetNoSorter(int mtlNum, BOOL backFace);
    int GetNoSorter(TimeValue t);

    // Dither
    void SetDither(int i, TimeValue t);
    int GetDither(int mtlNum, BOOL backFace);
    int GetDither(TimeValue t);


    // Standard locks not supported (Diffuse+Specular)/(Ambient+Diffuse)
    void SetLockDS(BOOL lock);
    BOOL GetLockDS();
    void SetLockAD(BOOL lock);
    BOOL GetLockAD();
    void SetLockADTex(BOOL lock);
    BOOL GetLockADTex();

    void SetSpecularLevel(float v, TimeValue t);        
    float GetSpecularLevel(int mtlNum, BOOL backFace);
    float GetSpecularLevel(TimeValue t);

    // SELFILLUM
    void SetSelfIllum(float v, TimeValue t);
    float GetSelfIllum(int mtlNum=0, BOOL backFace=FALSE);
    float GetSelfIllum(TimeValue t);    

    void SetSelfIllumClrOn( BOOL on );
    BOOL IsSelfIllumClrOn(int mtlNum, BOOL backFace);
    BOOL IsSelfIllumClrOn();
    
    void SetSelfIllumClr(Color c, TimeValue t);
    Color GetSelfIllumClr(int mtlNum=0, BOOL backFace=FALSE);
    Color GetSelfIllumClr(TimeValue t);     

    // SOFTEN
    void SetSoftenLevel(float v, TimeValue t);
    float GetSoftenLevel(int mtlNum=0, BOOL backFace=FALSE);
    float GetSoftenLevel(TimeValue t);

    void SetGlossiness(float v, TimeValue t);
    float GetGlossiness(int mtlNum, BOOL backFace);
    float GetGlossiness( TimeValue t);

    TextureExportSetting GetTextureExportSetting(int map);
    // For binding values to a specific limit
    float Bound( float fX, float fMin = 0.0f, float fMax = 1.0f );
    Color Bound( Color& c );

    NiString GetShaderName();
    void SetShaderName(NiString strWhichShader);
    unsigned int GetViewerShaderTechnique();
    unsigned int GetExportShaderTechnique();
    void SetViewerShaderTechnique(unsigned int uiWhichTechnique);
    void SetExportShaderTechnique(unsigned int uiWhichTechnique);

    BOOL GetUseNormalMap();
    void SetUseNormalMap(BOOL bUse);
    NBT_Method GetNormalMapTechnique();
    void SetNormalMapTechnique(NBT_Method eWhichTechnique);

    void UpdateShaderCustAttrib();
    void RemoveShaderCustAttrib();
    void AddRedefineShaderCustAttribCallback();

    static NiString GetAttributeDefinition(const char* pcShaderName);
    static NiString BuildAttributeDefinition(NiShaderDesc* pkDesc, 
          const char* pcShaderName);
    static unsigned int ConvertNiMAXShaderToTechnique(unsigned int 
        uiWhichTechnique);

    // Only valid if this is the current material in the material browser
    NiString GetMtlName();
    NiStdMat * GetMtl();

private:
    static bool ms_bAddedRedefinitionCallback;
    NiString GetMtlNameContainingThisShader(Mtl* pkMtl);
    NiStdMat * GetMtlContainingThisShader(Mtl* pkMtl);

};

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXShader.inl"

//---------------------------------------------------------------------------

#endif
