#ifdef DO_SCALARS
#undef DO_SCALARS
#endif

#ifndef AUDIOFLOAT
#define AUDIOFLOAT F32
#endif

/*
#if defined(IS_XENON) || !defined(__RADXENON__)
#define __RADXENON__
#endif

#if defined(IS_PS3) || !defined(__RADPS3__)
#ifdef IS_PS3
#define __RADPS3__
#endif

#if defined(IS_X86) || !defined(__RADX86__)
#ifdef IS_X86
#define __RADX86__
#endif

#if defined(IS_32) || !defined(__RAD32__)
#ifdef IS_32
#define __RAD32__
#endif

#if defined(IS_WIN64) || !defined(__RAD64__)
#ifdef IS_WIN64
#define __RAD64__
#endif

#if defined(IS_WIIU) || !defined(__RADWIIU__)
#ifdef IS_WIIU
#define __RADWIIU__
#endif

#ifndef RADRESTRICT
#define RADRESTRICT MSSRESTRICT
#endif
*/

#if defined(CLAMP_FORCE_LE) && !defined(STORE_LE_SWAP16) // we might inherit this from mss.h
  #if defined(__RADLITTLEENDIAN__)
    #define STORE_LE_SWAP16( ptr, val )  { *((U16*)(ptr))=(U16)(val); }
  #elif defined(__RADWIIU__)
    #define STORE_LE_SWAP16( ptr, val ) (*(__bytereversed short *)(ptr) = (val))
  #endif
#endif



#if (defined(__RADX86__) && defined(__RAD32__) && defined(_MSC_VER) && !defined(__RADXENON__)) || defined(__RADPS4__)

// for sse intrinsics.
#if (defined(_MSC_VER) && !defined(__RADXENON__)) || defined(__RADPS4__)
#include <emmintrin.h>
#include <mmintrin.h>
#endif

#if defined( __RAD64__ )

// 64 uses sse2
#define ClampToS16 ClampToS16_sse2
#define ClampToS16AndInterleave ClampToS16AndInterleave_sse2

#else

#ifndef CLAMP_SSE2_CHECK
#define CLAMP_SSE2_CHECK CPU_can_use(CPU_SSE2)
#endif

#define DO_SCALARS
#define ClampToS16 ClampToS16_dispatch
#define ClampToS16AndInterleave ClampToS16AndInterleave_dispatch

static void ClampToS16AndInterleave_x86(S16* dest, F32* inp0, F32* inp1, F32 scale, U32 num);
static void ClampToS16AndInterleave_sse2(S16* dest, F32* inp0, F32* inp1, F32 scale, U32 num);
static void ClampToS16AndInterleave_dispatch(S16* dest, F32* inp0, F32* inp1, F32 scale, U32 num)
{
    if (CLAMP_SSE2_CHECK)
    {
        ClampToS16AndInterleave_sse2(dest, inp0, inp1, scale, num);
        return;
    }
    ClampToS16AndInterleave_x86(dest, inp0, inp1, scale, num);
}

static void ClampToS16_x86(S16* dest, F32* inp, F32 scale, U32 num);
static void ClampToS16_sse2(S16* dest, F32* inp, F32 scale, U32 num);
static void ClampToS16_dispatch(S16* dest, F32* inp, F32 scale, U32 num)
{
    if (CLAMP_SSE2_CHECK)
    {
        ClampToS16_sse2(dest, inp, scale, num);
        return;
    }
    ClampToS16_x86(dest, inp, scale, num);
}

#endif

// x64 can't build inline asm
#ifndef __RAD64__
static void ClampToS16_x86( S16* dest,
                            F32* inp,
                            F32 scale,
                            U32 num )
{
  U32 buf1,buf2;
  __asm {
    push ecx
    mov ecx,[num]
    fld dword ptr [scale]
    mov esi,[inp]
    shr ecx,1
    mov edi,[dest]
  loop1:
    fld dword ptr [esi]
    fld dword ptr [esi+4]
    fmul ST,ST(2)
    fxch ST(1)
    fmul ST,ST(2)
    fistp dword ptr [buf1]
    fistp dword ptr [buf2]
    add esi,8
    mov eax,[buf1]
    mov edx,[buf2]
    cmp eax,32767
    jg over1
    cmp eax,-32768
    jl under1
    mov [edi],ax
    cmp edx,32767
    jg over2
    cmp edx,-32768
    jl under2
    mov [edi+2],dx
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   over1:
    mov word ptr [edi],32767
    cmp edx,32767
    jg over2
    cmp edx,-32768
    jl under2
    mov [edi+2],dx
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   under1:
    mov word ptr [edi],-32768
    cmp edx,32767
    jg over2
    cmp edx,-32768
    jl under2
    mov [edi+2],dx
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   over2:
    mov word ptr [edi+2],32767
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   under2:
    mov word ptr [edi+2],-32768
    add edi,4
    dec ecx
    jnz loop1

   done:
    pop ecx
    fistp dword ptr [buf1]
  }
}

static void ClampToS16AndInterleave_x86( S16* dest,
                                         F32* inp0,
                                         F32* inp1,
                                         F32 scale,
                                         U32 num )
{
  U32 buf1,buf2;
  __asm {
    push ecx
    push ebx
    mov ecx,[num]
    fld dword ptr [scale]
    mov esi,[inp0]
    mov ebx,[inp1]
    sub ebx,esi
    mov edi,[dest]
  loop1:
    fld dword ptr [esi]
    fld dword ptr [esi+ebx]
    fmul ST,ST(2)
    fxch ST(1)
    fmul ST,ST(2)
    fistp dword ptr [buf1]
    fistp dword ptr [buf2]
    add esi,4
    mov eax,[buf1]
    mov edx,[buf2]
    cmp eax,32767
    jg over1
    cmp eax,-32768
    jl under1
    mov [edi],ax
    cmp edx,32767
    jg over2
    cmp edx,-32768
    jl under2
    mov [edi+2],dx
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   over1:
    mov word ptr [edi],32767
    cmp edx,32767
    jg over2
    cmp edx,-32768
    jl under2
    mov [edi+2],dx
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   under1:
    mov word ptr [edi],-32768
    cmp edx,32767
    jg over2
    cmp edx,-32768
    jl under2
    mov [edi+2],dx
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   over2:
    mov word ptr [edi+2],32767
    add edi,4
    dec ecx
    jnz loop1
    jmp done

   under2:
    mov word ptr [edi+2],-32768
    add edi,4
    dec ecx
    jnz loop1

   done:
    pop ebx
    pop ecx
    fistp dword ptr [buf1]
  }
}

#endif

static void ClampToS16_sse2( S16* dest,
                             F32* inp,
                             F32 scale,
                             U32 num )
{
    AUDIOFLOAT * RADRESTRICT f;
    S16* c;
    U32 i;

    __m128 MultVec = _mm_set_ps(scale, scale, scale, scale);

    radassert(!(num%8));
    //radassert(!((UINTa)dest & 0xF));
    //radassert(!((UINTa)inp & 0xF));

    f=inp;
    c = (S16*)dest;
    for (i = (num/8); i; i--)
    {
#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
        __m128 LoadVector = _mm_loadu_ps(f);
        __m128 LoadVector2 = _mm_loadu_ps(f + 4);
#else
        __m128 LoadVector = _mm_load_ps(f);
        __m128 LoadVector2 = _mm_load_ps(f + 4);
#endif
        __m128i LoadVectorInt;
        __m128i LoadVector2Int;

        LoadVector = _mm_mul_ps(LoadVector, MultVec);
        LoadVector2 = _mm_mul_ps(LoadVector2, MultVec);

        LoadVectorInt = _mm_cvtps_epi32(LoadVector);
        LoadVector2Int = _mm_cvtps_epi32(LoadVector2);

        LoadVectorInt = _mm_packs_epi32(LoadVectorInt, LoadVector2Int);

        _mm_store_si128((__m128i*)c, LoadVectorInt);

        f+=8;
        c+=8;
    }
}

static void ClampToS16AndInterleave_sse2( S16* dest,
                                          F32* inp0,
                                          F32* inp1,
                                          F32 scale,
                                          U32 num )
{
    AUDIOFLOAT * RADRESTRICT f;
    S32* c;
    U32 i;
    SINTa diff = ( (char*) inp1 ) - ( (char*) inp0 );

    __m128 MultVec = _mm_set_ps(scale, scale, scale, scale);

    radassert(!(num%4));
    //radassert(!((UINTa)dest & 0xF));
    //radassert(!((UINTa)inp0 & 0xF));

    f=inp0;
    c = (S32*)dest;
    for (i = (num/4); i; i--)
    {
#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
        __m128 LoadVector = _mm_loadu_ps(f);
        __m128 LoadVector2 = _mm_loadu_ps( (F32*)(((char*)f) + diff ) );
#else
        __m128 LoadVector = _mm_load_ps(f);
        __m128 LoadVector2 = _mm_load_ps( (F32*)(((char*)f) + diff ) );
#endif
        __m128i LoadVectorInt;
        __m128i LoadVector2Int;
        __m128i PackedOne;

        // [ x, y, z, w ]
        LoadVector = _mm_mul_ps(LoadVector, MultVec);
        LoadVector2 = _mm_mul_ps(LoadVector2, MultVec);

        // [ x, y, z, w ]
        LoadVectorInt = _mm_cvtps_epi32(LoadVector);
        LoadVector2Int = _mm_cvtps_epi32(LoadVector2);

        // [ x1, y1, z1, w1, x2, y2, z2, w2 ]
        PackedOne = _mm_packs_epi32(LoadVectorInt, LoadVector2Int);

        // [ x2, y2, z2, w2, x1, y1, z1, w1 ]
        LoadVector2Int = _mm_packs_epi32(LoadVector2Int, LoadVectorInt);

        // [ x1, x2, y1, y2, z1, z2, w1, w2 ]
        LoadVectorInt = _mm_unpacklo_epi16(PackedOne, LoadVector2Int);
        
        _mm_store_si128((__m128i*)c, LoadVectorInt);

        f+=4;
        c+=4;
    }

}

#elif defined( __RADSPU__)

static void ClampToS16( S16* dest,
                        F32* inp,
                        F32 scale,
                        U32 num )
{
  vector float * RADRESTRICT f4;
  vector signed short * RADRESTRICT c4;
  U32 i;
  vector float scale4;

  scale4 = spu_splats(scale);

  f4=(vector float*)inp;
  c4=(vector signed short*)dest;
  for (i = num/8; i; i--) 
  {
    vector float s41;
    vector signed int i41;
    vector float s42;
    vector signed int i42;

    vector signed short h4;

    s41 = *f4++;
    s42 = *f4++;
    s41 = s41 * scale4;
    s42 = s42 * scale4;
    i41 = spu_convts( s41, 16);
    i42 = spu_convts( s42, 16);

#ifdef CLAMP_FORCE_LE
    h4 = (vec_short8)spu_shuffle(i41, i42, ((vec_uchar16){ 1,  0,  5,  4,  9,  8, 13, 12,
                                                          17, 16, 21, 20, 25, 24, 29, 28}));
#else
    h4 = (vec_short8)spu_shuffle(i41, i42, ((vec_uchar16){ 0,  1,  4,  5,  8,  9, 12, 13,
                                                          16, 17, 20, 21, 24, 25, 28, 29}));
#endif

     *c4++ = h4;
   }
}

static void ClampToS16AndInterleave( S16* dest,
                                     F32* inp0,
                                     F32* inp1,
                                     F32 scale,
                                     U32 num )
{
  vector float * RADRESTRICT f41;
  vector float * RADRESTRICT f42;
  vector signed short * RADRESTRICT c4;
  U32 i;
  vector float scale4;

  scale4 = spu_splats(scale);

  f41=(vector float*)inp0;
  f42=(vector float*)inp1;
  c4=(vector signed short*)dest;
  for (i = num/4; i; i--) 
  {
    vector float s41;
    vector signed int i41;
    vector float s42;
    vector signed int i42;

    vector signed short h4;

    s41 = *f41++;
    s42 = *f42++;
    s41 = s41 * scale4;
    s42 = s42 * scale4;
    i41 = spu_convts( s41, 16);
    i42 = spu_convts( s42, 16);

#ifdef CLAMP_FORCE_LE
    h4 = (vec_short8)spu_shuffle(i41, i42, ((vec_uchar16){ 1,  0, 17, 16,  5,  4, 21, 20,
                                                           9,  8, 25, 24, 13, 12, 29, 28}));
#else
    h4 = (vec_short8)spu_shuffle(i41, i42, ((vec_uchar16){ 0,  1, 16, 17,  4,  5, 20, 21,
                                                           8,  9, 24, 25, 12, 13, 28, 29}));
#endif

    *c4++ = h4;
   }

}

#elif defined(__RADPS3__)

static void ClampToS16( S16* dest,
                        F32* inp,
                        F32 scale,
                        U32 num )
{
    AUDIOFLOAT * RADRESTRICT f;
    S16* c;
    U32 i;

    vector float MultVec = {scale, scale, scale, scale};
    vector float ZeroVector = {0};

#ifdef CLAMP_FORCE_LE
    const vector unsigned int LEPermVector = {0x01000302, 0x05040706, 0x09080b0a, 0x0d0c0f0e};
#endif

    radassert(!(num%8));
    radassert(!((unsigned int)dest & 0xF));

#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    if ((UINTa)inp & 0xF)
    {
        // Unaligned load.
        f=inp;
        c = (S16*)dest;
        for (i = (num/8); i; i--)
        {
            vector float LoadVector_a = vec_lvlx(0, f);
            vector float LoadVector_b = vec_lvrx(16, f);
            vector float LoadVector2_a = vec_lvlx(16, f);
            vector float LoadVector2_b = vec_lvrx(32, f);
            vector float LoadVector = vec_or(LoadVector_a, LoadVector_b);
            vector float LoadVector2 = vec_or(LoadVector2_a, LoadVector2_b);

            LoadVector = vec_madd(LoadVector, MultVec, ZeroVector);
            LoadVector2 = vec_madd(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = (vector float)vec_cts(LoadVector2, 0);
            LoadVector = (vector float)vec_cts(LoadVector, 0);

            LoadVector = (vector float)vec_packs((vector int)LoadVector, (vector int)LoadVector2);

#ifdef CLAMP_FORCE_LE
            // Permute to little endian.
            LoadVector = vec_perm(LoadVector, LoadVector, (vector unsigned char)LEPermVector);
#endif
                
            vec_st(LoadVector, 0, (vector float*)c);

            f+=8;
            c+=8;
        }
    }
    else
    {
#endif
        // Aligned load.
        radassert(!((unsigned int)inp & 0xF));

        f=inp;
        c = (S16*)dest;
        for (i = (num/8); i; i--)
        {
            vector float LoadVector = vec_ld(0, f);
            vector float LoadVector2 = vec_ld(16, f);

            LoadVector = vec_madd(LoadVector, MultVec, ZeroVector);
            LoadVector2 = vec_madd(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = (vector float)vec_cts(LoadVector2, 0);
            LoadVector = (vector float)vec_cts(LoadVector, 0);

            LoadVector = (vector float)vec_packs((vector int)LoadVector, (vector int)LoadVector2);

#ifdef CLAMP_FORCE_LE
            // Permute to little endian.
            LoadVector = vec_perm(LoadVector, LoadVector, (vector unsigned char)LEPermVector);
#endif
                
            vec_st(LoadVector, 0, (vector float*)c);

            f+=8;
            c+=8;
        }
#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    }
#endif
}

static void ClampToS16AndInterleave( S16* dest,
                                     F32* inp0,
                                     F32* inp1,
                                     F32 scale,
                                     U32 num )
{
    AUDIOFLOAT * RADRESTRICT f;
    AUDIOFLOAT * RADRESTRICT f2;
    S32* c;
    U32 i;

    vector float MultVec = {scale, scale, scale, scale};
    vector float ZeroVector = {0};
#ifdef CLAMP_FORCE_LE
    const vector unsigned char PermConst = {
        1, 0,
        9, 8,
        3, 2,
        11, 10,
        5, 4,
        13, 12,
        7, 6,
        15, 14};
#else
    const vector unsigned char PermConst = {
        0, 1,
        8, 9,
        2, 3,
        10, 11,
        4, 5,
        12, 13,
        6, 7,
        14, 15};
#endif
    radassert(!(num%4));
    radassert(!((unsigned int)dest & 0xF));

#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    if (((UINTa)inp0 & 0xF) || ((UINTa)inp1 & 0xF))
    {
        // Unaligned source.
        f=inp0;
        f2=inp1;
        c = (S32*)dest;
        for (i = (num/4); i; i--)
        {
            vector float LoadVector_a = vec_lvlx(0, f);
            vector float LoadVector_b = vec_lvrx(16, f);
            vector float LoadVector2_a = vec_lvlx(0, f2);
            vector float LoadVector2_b = vec_lvrx(16, f2);
            vector float LoadVector = vec_or(LoadVector_a, LoadVector_b);
            vector float LoadVector2 = vec_or(LoadVector2_a, LoadVector2_b);

            LoadVector = vec_madd(LoadVector, MultVec, ZeroVector);
            LoadVector2 = vec_madd(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = (vector float)vec_cts(LoadVector2, 0);
            LoadVector = (vector float)vec_cts(LoadVector, 0);

            LoadVector = (vector float)vec_packs((vector int)LoadVector, (vector int)LoadVector2);
            LoadVector = vec_perm(LoadVector, LoadVector, PermConst);

            vec_st(LoadVector, 0, (vector float*)c);

            f+=4;
            f2+=4;
            c+=4;
        }
    }
    else
    {
#endif
        radassert(!((UINTa)inp0 & 0xF));
        radassert(!((UINTa)inp1 & 0xF));

        f=inp0;
        f2=inp1;
        c = (S32*)dest;
        for (i = (num/4); i; i--)
        {
            vector float LoadVector = vec_ld(0, f);
            vector float LoadVector2 = vec_ld(0, f2);

            LoadVector = vec_madd(LoadVector, MultVec, ZeroVector);
            LoadVector2 = vec_madd(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = (vector float)vec_cts(LoadVector2, 0);
            LoadVector = (vector float)vec_cts(LoadVector, 0);

            LoadVector = (vector float)vec_packs((vector int)LoadVector, (vector int)LoadVector2);
            LoadVector = vec_perm(LoadVector, LoadVector, PermConst);

            vec_st(LoadVector, 0, (vector float*)c);

            f+=4;
            f2+=4;
            c+=4;
        }
#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    }
#endif
}
#elif defined(__RADXENON__)

#define radfsel(dest,cond,cmp1,cmp2) dest = (F32)__fsel(cond,cmp1,cmp2)

static void ClampToS16( S16* dest,
                        F32* inp,
                        F32 scale,
                        U32 num )
{
    AUDIOFLOAT * RADRESTRICT f;
    S16* c;
    U32 i;

    __vector4 MultVec = {scale, scale, scale, scale};
    __vector4 ZeroVector = {0};

#ifdef CLAMP_FORCE_LE
    static RAD_ALIGN(U32, LEPermSource[4], 16) = {0x01000302, 0x05040706, 0x09080b0a, 0x0d0c0f0e};
    __vector4 LEPermVector = __lvx(LEPermSource, 0);
#endif

    radassert(!(num%8));
    radassert(!((unsigned int)dest & 0xF)); // only check alignment if there is work to be done.

#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    if ((unsigned int)inp & 0xF)
    {
        // Unaligned copy.
        f=inp;
        c = (S16*)dest;
        for (i = (num/8); i; i--)
        {
            __vector4 LoadVector_a = __lvlx(f, 0);
            __vector4 LoadVector_b = __lvrx(f, 16);
            __vector4 LoadVector2_a = __lvlx(f, 16);
            __vector4 LoadVector2_b = __lvrx(f, 32);
            __vector4 LoadVector = __vor(LoadVector_a, LoadVector_b);
            __vector4 LoadVector2 = __vor(LoadVector2_a, LoadVector2_b);

            LoadVector = __vmaddfp(LoadVector, MultVec, ZeroVector);
            LoadVector2 = __vmaddfp(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = __vctsxs(LoadVector2, 0);
            LoadVector = __vctsxs(LoadVector, 0);

            LoadVector = __vpkswss(LoadVector, LoadVector2);

#ifdef CLAMP_FORCE_LE
            // Permute to little endian.
            LoadVector = __vperm(LoadVector, LoadVector, LEPermVector);
#endif

            __stvx(LoadVector, c, 0);

            f+=8;
            c+=8;
        }
    }
    else
    {
#endif
        // Aligned copy.
        //radassert(!((unsigned int)inp & 0xF));
        f=inp;
        c = (S16*)dest;
        for (i = (num/8); i; i--)
        {
            __vector4 LoadVector = __lvx(f, 0);
            __vector4 LoadVector2 = __lvx(f, 16);

            LoadVector = __vmaddfp(LoadVector, MultVec, ZeroVector);
            LoadVector2 = __vmaddfp(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = __vctsxs(LoadVector2, 0);
            LoadVector = __vctsxs(LoadVector, 0);

            LoadVector = __vpkswss(LoadVector, LoadVector2);

    #ifdef CLAMP_FORCE_LE
            // Permute to little endian.
            LoadVector = __vperm(LoadVector, LoadVector, LEPermVector);
    #endif

            __stvx(LoadVector, c, 0);

            f+=8;
            c+=8;
        }
#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    } // end aligned.
#endif
}

static void ClampToS16AndInterleave( S16* dest,
                                     F32* inp0,
                                     F32* inp1,
                                     F32 scale,
                                     U32 num )
{
    AUDIOFLOAT * RADRESTRICT f;
    AUDIOFLOAT * RADRESTRICT f2;
    S32 * RADRESTRICT c;
    U32 i;

    __vector4 MultVec = {scale, scale, scale, scale};
    __vector4 ZeroVector = {0};

#ifdef CLAMP_FORCE_LE
    const int __declspec(align(16)) PermConst[4] = { 
        (1 << 24) + (0 << 16) + (9 << 8) + (8),
        (3 << 24) + (2 << 16) + (11 << 8) + 10,
        (5 << 24) + (4 << 16) + (13 << 8) + 12,
        (7 << 24) + (6 << 16) + (15 << 8) + 14};
#else
    const int __declspec(align(16)) PermConst[4] = { 
        (0 << 24) + (1 << 16) + (8 << 8) + (9),
        (2 << 24) + (3 << 16) + (10 << 8) + 11,
        (4 << 24) + (5 << 16) + (12 << 8) + 13,
        (6 << 24) + (7 << 16) + (14 << 8) + 15};
#endif

    radassert(!(num%4));
    radassert(!((unsigned int)dest & 0xF));

#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    if (((unsigned int)inp0 & 0xF) || ((unsigned int)inp1 & 0xF))
    {
        // Unaligned
        f=inp0;
        f2=inp1;
        c = (S32*)dest;
        for (i = (num/4); i; i--)
        {
            __vector4 LoadVector_a = __lvlx(f, 0);
            __vector4 LoadVector_b = __lvrx(f, 16);
            __vector4 LoadVector2_a = __lvlx(f2, 0);
            __vector4 LoadVector2_b = __lvrx(f2, 16);
            __vector4 LoadVector = __vor(LoadVector_a, LoadVector_b);
            __vector4 LoadVector2 = __vor(LoadVector2_a, LoadVector2_b);

            LoadVector = __vmaddfp(LoadVector, MultVec, ZeroVector);
            LoadVector2 = __vmaddfp(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = __vctsxs(LoadVector2, 0);
            LoadVector = __vctsxs(LoadVector, 0);

            LoadVector = __vpkswss(LoadVector, LoadVector2);
            LoadVector = __vperm(LoadVector, LoadVector, *(__vector4*)PermConst);

            __stvx(LoadVector, c, 0);

            f+=4;
            f2+=4;
            c+=4;
        }
    }
    else
    {
#endif
        //radassert(!((unsigned int)inp0 & 0xF));
        //radassert(!((unsigned int)inp1 & 0xF));

        f=inp0;
        f2=inp1;
        c = (S32*)dest;
        for (i = (num/4); i; i--)
        {
            __vector4 LoadVector = __lvx(f, 0);
            __vector4 LoadVector2 = __lvx(f2, 0);

            LoadVector = __vmaddfp(LoadVector, MultVec, ZeroVector);
            LoadVector2 = __vmaddfp(LoadVector2, MultVec, ZeroVector);

            LoadVector2 = __vctsxs(LoadVector2, 0);
            LoadVector = __vctsxs(LoadVector, 0);

            LoadVector = __vpkswss(LoadVector, LoadVector2);
            LoadVector = __vperm(LoadVector, LoadVector, *(__vector4*)PermConst);

            __stvx(LoadVector, c, 0);

            f+=4;
            f2+=4;
            c+=4;
        }
#ifdef CLAMP_SUPPORT_UNALIGNED_SOURCE
    } // end if aligned.
#endif
}

#else

// Use the c versions if we have no speedy version.
#define ClampToS16AndInterleave ClampToS16AndInterleave_c
#define ClampToS16 ClampToS16_c
#define DO_SCALARS

#endif

#ifdef DO_SCALARS

static void ClampToS16_c( S16* dest,
                          F32* inp,
                          F32 scale,
                          U32 num )
{
  AUDIOFLOAT* f;
  S16* c;
  U32 i;

  f=inp;
  c=(S16*)dest;
  for (i = num; i; i--) {
    S32 v=(S32)(*f * ((AUDIOFLOAT)scale));
    S16 v16 = (v>=32767)?(S16)32767:((v<=-32768)?(S16)-32768:(S16)v);
#ifdef CLAMP_FORCE_LE
    STORE_LE_SWAP16(c, v16);
#else
    *c = v16;
#endif
    c++;
    f++;
  }
}

static void ClampToS16AndInterleave_c( S16* dest,
                                       F32* inp0,
                                       F32* inp1,
                                       F32 scale,
                                       U32 num )
{
  AUDIOFLOAT * f;
  AUDIOFLOAT * f2;

  S16 * c;
  U32 i;

  f = inp0;
  f2 = inp1;
  c = (S16*) dest;
  for ( i = num ; i-- ; )
  {
    S32 v;
    S16 v16;

    v = (S32) ( f[ 0 ] * scale );
    v16 = ( v >= 32767 ) ? (S16) 32767 : ( ( v <= -32768 ) ? (S16) -32768 : (S16) v );
#ifdef CLAMP_FORCE_LE
    STORE_LE_SWAP16(c, v16);
#else
    *c = v16;
#endif
    c++;
    v = (S32) ( f2[ 0 ] * scale );
    v16 = ( v >= 32767 ) ? (S16) 32767 : ( ( v <= -32768 ) ? (S16) -32768 : (S16) v );
#ifdef CLAMP_FORCE_LE
    STORE_LE_SWAP16(c, v16);
#else
    *c = v16;
#endif
    c++;
    ++f;
    ++f2;
  }
}

#endif
