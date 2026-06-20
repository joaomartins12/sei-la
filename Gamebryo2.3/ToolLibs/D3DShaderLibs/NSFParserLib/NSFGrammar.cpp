/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse NSFParserparse
#define yylex   NSFParserlex
#define yyerror NSFParsererror
#define yylval  NSFParserlval
#define yychar  NSFParserchar
#define yydebug NSFParserdebug
#define yynerrs NSFParsernerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EOLN = 258,
     PATH = 259,
     L_ACCOLADE = 260,
     R_ACCOLADE = 261,
     L_PARENTHESE = 262,
     R_PARENTHESE = 263,
     L_BRACKET = 264,
     R_BRACKET = 265,
     L_ANGLEBRACKET = 266,
     R_ANGLEBRACKET = 267,
     OR = 268,
     ASSIGN = 269,
     COMMA = 270,
     UNDERSCORE = 271,
     ASTERIK = 272,
     FORWARDSLASH = 273,
     PLUS = 274,
     MINUS = 275,
     N_HEX = 276,
     N_FLOAT = 277,
     N_INT = 278,
     N_STRING = 279,
     N_QUOTE = 280,
     N_BOOL = 281,
     N_VERSION = 282,
     NSFSHADER = 283,
     ARTIST = 284,
     HIDDEN = 285,
     SAVE = 286,
     ATTRIBUTES = 287,
     GLOBALATTRIBUTES = 288,
     ATTRIB = 289,
     ATTRIB_BOOL = 290,
     ATTRIB_STRING = 291,
     ATTRIB_UINT = 292,
     ATTRIB_FLOAT = 293,
     ATTRIB_POINT2 = 294,
     ATTRIB_POINT3 = 295,
     ATTRIB_POINT4 = 296,
     ATTRIB_MATRIX3 = 297,
     ATTRIB_TRANSFORM = 298,
     ATTRIB_COLOR = 299,
     ATTRIB_TEXTURE = 300,
     PACKINGDEF = 301,
     PD_STREAM = 302,
     PD_FIXEDFUNCTION = 303,
     PDP_POSITION = 304,
     PDP_BLENDWEIGHTS = 305,
     PDP_BLENDINDICES = 306,
     PDP_NORMAL = 307,
     PDP_POINTSIZE = 308,
     PDP_COLOR = 309,
     PDP_COLOR2 = 310,
     PDP_TEXCOORD0 = 311,
     PDP_TEXCOORD1 = 312,
     PDP_TEXCOORD2 = 313,
     PDP_TEXCOORD3 = 314,
     PDP_TEXCOORD4 = 315,
     PDP_TEXCOORD5 = 316,
     PDP_TEXCOORD6 = 317,
     PDP_TEXCOORD7 = 318,
     PDP_POSITION2 = 319,
     PDP_NORMAL2 = 320,
     PDP_TANGENT = 321,
     PDP_BINORMAL = 322,
     PDP_EXTRADATA = 323,
     PDT_FLOAT1 = 324,
     PDT_FLOAT2 = 325,
     PDT_FLOAT3 = 326,
     PDT_FLOAT4 = 327,
     PDT_UBYTECOLOR = 328,
     PDT_SHORT1 = 329,
     PDT_SHORT2 = 330,
     PDT_SHORT3 = 331,
     PDT_SHORT4 = 332,
     PDT_UBYTE4 = 333,
     PDT_NORMSHORT1 = 334,
     PDT_NORMSHORT2 = 335,
     PDT_NORMSHORT3 = 336,
     PDT_NORMSHORT4 = 337,
     PDT_NORMPACKED3 = 338,
     PDT_PBYTE1 = 339,
     PDT_PBYTE2 = 340,
     PDT_PBYTE3 = 341,
     PDT_PBYTE4 = 342,
     PDT_FLOAT2H = 343,
     PDT_NORMUBYTE4 = 344,
     PDT_NORMUSHORT2 = 345,
     PDT_NORMUSHORT4 = 346,
     PDT_UDEC3 = 347,
     PDT_NORMDEC3 = 348,
     PDT_FLOAT16_2 = 349,
     PDT_FLOAT16_4 = 350,
     PDTESS_DEFAULT = 351,
     PDTESS_PARTIALU = 352,
     PDTESS_PARTIALV = 353,
     PDTESS_CROSSUV = 354,
     PDTESS_UV = 355,
     PDTESS_LOOKUP = 356,
     PDTESS_LOOKUPPRESAMPLED = 357,
     PDU_POSITION = 358,
     PDU_BLENDWEIGHT = 359,
     PDU_BLENDINDICES = 360,
     PDU_NORMAL = 361,
     PDU_PSIZE = 362,
     PDU_TEXCOORD = 363,
     PDU_TANGENT = 364,
     PDU_BINORMAL = 365,
     PDU_TESSFACTOR = 366,
     PDU_POSITIONT = 367,
     PDU_COLOR = 368,
     PDU_FOG = 369,
     PDU_DEPTH = 370,
     PDU_SAMPLE = 371,
     RENDERSTATES = 372,
     CMDEFINED = 373,
     CMATTRIBUTE = 374,
     CMCONSTANT = 375,
     CMGLOBAL = 376,
     CMOPERATOR = 377,
     VSCONSTANTMAP = 378,
     VSPROGRAM = 379,
     GSCONSTANTMAP = 380,
     GSPROGRAM = 381,
     PSCONSTANTMAP = 382,
     PSPROGRAM = 383,
     PROGRAM = 384,
     ENTRYPOINT = 385,
     SHADERTARGET = 386,
     SOFTWAREVP = 387,
     BONEMATRIX3 = 388,
     BONEMATRIX4 = 389,
     SKINBONEMATRIX3 = 390,
     SKINBONEMATRIX4 = 391,
     REQUIREMENTS = 392,
     VSVERSION = 393,
     GSVERSION = 394,
     PSVERSION = 395,
     USERVERSION = 396,
     PLATFORM = 397,
     BONESPERPARTITION = 398,
     BINORMALTANGENTMETHOD = 399,
     BINORMALTANGENTUVSOURCE = 400,
     NBTMETHOD_NONE = 401,
     NBTMETHOD_NI = 402,
     NBTMETHOD_MAX = 403,
     NBTMETHOD_ATI = 404,
     USERDEFINEDDATA = 405,
     IMPLEMENTATION = 406,
     CLASSNAME = 407,
     PASS = 408,
     STAGE = 409,
     TSS_TEXTURE = 410,
     TSS_COLOROP = 411,
     TSS_COLORARG0 = 412,
     TSS_COLORARG1 = 413,
     TSS_COLORARG2 = 414,
     TSS_ALPHAOP = 415,
     TSS_ALPHAARG0 = 416,
     TSS_ALPHAARG1 = 417,
     TSS_ALPHAARG2 = 418,
     TSS_RESULTARG = 419,
     TSS_CONSTANT_DEPRECATED = 420,
     TSS_BUMPENVMAT00 = 421,
     TSS_BUMPENVMAT01 = 422,
     TSS_BUMPENVMAT10 = 423,
     TSS_BUMPENVMAT11 = 424,
     TSS_BUMPENVLSCALE = 425,
     TSS_BUMPENVLOFFSET = 426,
     TSS_TEXCOORDINDEX = 427,
     TSS_TEXTURETRANSFORMFLAGS = 428,
     TSS_TEXTRANSMATRIX = 429,
     TTFF_DISABLE = 430,
     TTFF_COUNT1 = 431,
     TTFF_COUNT2 = 432,
     TTFF_COUNT3 = 433,
     TTFF_COUNT4 = 434,
     TTFF_PROJECTED = 435,
     PROJECTED = 436,
     USEMAPINDEX = 437,
     INVERSE = 438,
     TRANSPOSE = 439,
     TTSRC_GLOBAL = 440,
     TTSRC_CONSTANT = 441,
     TT_WORLD_PARALLEL = 442,
     TT_WORLD_PERSPECTIVE = 443,
     TT_WORLD_SPHERE_MAP = 444,
     TT_CAMERA_SPHERE_MAP = 445,
     TT_SPECULAR_CUBE_MAP = 446,
     TT_DIFFUSE_CUBE_MAP = 447,
     TCI_PASSTHRU = 448,
     TCI_CAMERASPACENORMAL = 449,
     TCI_CAMERASPACEPOSITION = 450,
     TCI_CAMERASPACEREFLECT = 451,
     TCI_SPHEREMAP = 452,
     TOP_DISABLE = 453,
     TOP_SELECTARG1 = 454,
     TOP_SELECTARG2 = 455,
     TOP_MODULATE = 456,
     TOP_MODULATE2X = 457,
     TOP_MODULATE4X = 458,
     TOP_ADD = 459,
     TOP_ADDSIGNED = 460,
     TOP_ADDSIGNED2X = 461,
     TOP_SUBTRACT = 462,
     TOP_ADDSMOOTH = 463,
     TOP_BLENDDIFFUSEALPHA = 464,
     TOP_BLENDTEXTUREALPHA = 465,
     TOP_BLENDFACTORALPHA = 466,
     TOP_BLENDTEXTUREALPHAPM = 467,
     TOP_BLENDCURRENTALPHA = 468,
     TOP_PREMODULATE = 469,
     TOP_MODULATEALPHA_ADDCOLOR = 470,
     TOP_MODULATECOLOR_ADDALPHA = 471,
     TOP_MODULATEINVALPHA_ADDCOLOR = 472,
     TOP_MODULATEINVCOLOR_ADDALPHA = 473,
     TOP_BUMPENVMAP = 474,
     TOP_BUMPENVMAPLUMINANCE = 475,
     TOP_DOTPRODUCT3 = 476,
     TOP_MULTIPLYADD = 477,
     TOP_LERP = 478,
     TA_CURRENT = 479,
     TA_DIFFUSE = 480,
     TA_SELECTMASK = 481,
     TA_SPECULAR = 482,
     TA_TEMP = 483,
     TA_TEXTURE = 484,
     TA_TFACTOR = 485,
     TA_ALPHAREPLICATE = 486,
     TA_COMPLEMENT = 487,
     NTM_BASE = 488,
     NTM_DARK = 489,
     NTM_DETAIL = 490,
     NTM_GLOSS = 491,
     NTM_GLOW = 492,
     NTM_BUMP = 493,
     NTM_NORMAL = 494,
     NTM_PARALLAX = 495,
     NTM_DECAL = 496,
     SAMPLER = 497,
     TSAMP_ADDRESSU = 498,
     TSAMP_ADDRESSV = 499,
     TSAMP_ADDRESSW = 500,
     TSAMP_BORDERCOLOR = 501,
     TSAMP_MAGFILTER = 502,
     TSAMP_MINFILTER = 503,
     TSAMP_MIPFILTER = 504,
     TSAMP_MIPMAPLODBIAS = 505,
     TSAMP_MAXMIPLEVEL = 506,
     TSAMP_MAXANISOTROPY = 507,
     TSAMP_SRGBTEXTURE = 508,
     TSAMP_ELEMENTINDEX = 509,
     TSAMP_DMAPOFFSET = 510,
     TSAMP_ALPHAKILL_DEPRECATED = 511,
     TSAMP_COLORKEYOP_DEPRECATED = 512,
     TSAMP_COLORSIGN_DEPRECATED = 513,
     TSAMP_COLORKEYCOLOR_DEPRECATED = 514,
     TADDR_WRAP = 515,
     TADDR_MIRROR = 516,
     TADDR_CLAMP = 517,
     TADDR_BORDER = 518,
     TADDR_MIRRORONCE = 519,
     TADDR_CLAMPTOEDGE_DEPRECATED = 520,
     TEXF_NONE = 521,
     TEXF_POINT = 522,
     TEXF_LINEAR = 523,
     TEXF_ANISOTROPIC = 524,
     TEXF_PYRAMIDALQUAD = 525,
     TEXF_GAUSSIANQUAD = 526,
     TEXF_FLATCUBIC_DEPRECATED = 527,
     TEXF_GAUSSIANCUBIC_DEPRECATED = 528,
     TEXF_QUINCUNX_DEPRECATED = 529,
     TEXF_MAX_DEPRECATED = 530,
     TAK_DISABLE_DEPRECATED = 531,
     TAK_ENABLE_DEPRECATED = 532,
     TCKOP_DISABLE_DEPRECATED = 533,
     TCKOP_ALPHA_DEPRECATED = 534,
     TCKOP_RGBA_DEPRECATED = 535,
     TCKOP_KILL_DEPRECATED = 536,
     TEXTURE = 537,
     TEXTURE_SOURCE = 538,
     OBJECTS = 539,
     EFFECT_GENERALLIGHT = 540,
     EFFECT_POINTLIGHT = 541,
     EFFECT_DIRECTIONALLIGHT = 542,
     EFFECT_SPOTLIGHT = 543,
     EFFECT_SHADOWPOINTLIGHT = 544,
     EFFECT_SHADOWDIRECTIONALLIGHT = 545,
     EFFECT_SHADOWSPOTLIGHT = 546,
     EFFECT = 547,
     EFFECT_ENVIRONMENTMAP = 548,
     EFFECT_PROJECTEDSHADOWMAP = 549,
     EFFECT_PROJECTEDLIGHTMAP = 550,
     EFFECT_FOGMAP = 551,
     USEMAPVALUE = 552,
     CMOBJECT = 553
   };
#endif
/* Tokens.  */
#define EOLN 258
#define PATH 259
#define L_ACCOLADE 260
#define R_ACCOLADE 261
#define L_PARENTHESE 262
#define R_PARENTHESE 263
#define L_BRACKET 264
#define R_BRACKET 265
#define L_ANGLEBRACKET 266
#define R_ANGLEBRACKET 267
#define OR 268
#define ASSIGN 269
#define COMMA 270
#define UNDERSCORE 271
#define ASTERIK 272
#define FORWARDSLASH 273
#define PLUS 274
#define MINUS 275
#define N_HEX 276
#define N_FLOAT 277
#define N_INT 278
#define N_STRING 279
#define N_QUOTE 280
#define N_BOOL 281
#define N_VERSION 282
#define NSFSHADER 283
#define ARTIST 284
#define HIDDEN 285
#define SAVE 286
#define ATTRIBUTES 287
#define GLOBALATTRIBUTES 288
#define ATTRIB 289
#define ATTRIB_BOOL 290
#define ATTRIB_STRING 291
#define ATTRIB_UINT 292
#define ATTRIB_FLOAT 293
#define ATTRIB_POINT2 294
#define ATTRIB_POINT3 295
#define ATTRIB_POINT4 296
#define ATTRIB_MATRIX3 297
#define ATTRIB_TRANSFORM 298
#define ATTRIB_COLOR 299
#define ATTRIB_TEXTURE 300
#define PACKINGDEF 301
#define PD_STREAM 302
#define PD_FIXEDFUNCTION 303
#define PDP_POSITION 304
#define PDP_BLENDWEIGHTS 305
#define PDP_BLENDINDICES 306
#define PDP_NORMAL 307
#define PDP_POINTSIZE 308
#define PDP_COLOR 309
#define PDP_COLOR2 310
#define PDP_TEXCOORD0 311
#define PDP_TEXCOORD1 312
#define PDP_TEXCOORD2 313
#define PDP_TEXCOORD3 314
#define PDP_TEXCOORD4 315
#define PDP_TEXCOORD5 316
#define PDP_TEXCOORD6 317
#define PDP_TEXCOORD7 318
#define PDP_POSITION2 319
#define PDP_NORMAL2 320
#define PDP_TANGENT 321
#define PDP_BINORMAL 322
#define PDP_EXTRADATA 323
#define PDT_FLOAT1 324
#define PDT_FLOAT2 325
#define PDT_FLOAT3 326
#define PDT_FLOAT4 327
#define PDT_UBYTECOLOR 328
#define PDT_SHORT1 329
#define PDT_SHORT2 330
#define PDT_SHORT3 331
#define PDT_SHORT4 332
#define PDT_UBYTE4 333
#define PDT_NORMSHORT1 334
#define PDT_NORMSHORT2 335
#define PDT_NORMSHORT3 336
#define PDT_NORMSHORT4 337
#define PDT_NORMPACKED3 338
#define PDT_PBYTE1 339
#define PDT_PBYTE2 340
#define PDT_PBYTE3 341
#define PDT_PBYTE4 342
#define PDT_FLOAT2H 343
#define PDT_NORMUBYTE4 344
#define PDT_NORMUSHORT2 345
#define PDT_NORMUSHORT4 346
#define PDT_UDEC3 347
#define PDT_NORMDEC3 348
#define PDT_FLOAT16_2 349
#define PDT_FLOAT16_4 350
#define PDTESS_DEFAULT 351
#define PDTESS_PARTIALU 352
#define PDTESS_PARTIALV 353
#define PDTESS_CROSSUV 354
#define PDTESS_UV 355
#define PDTESS_LOOKUP 356
#define PDTESS_LOOKUPPRESAMPLED 357
#define PDU_POSITION 358
#define PDU_BLENDWEIGHT 359
#define PDU_BLENDINDICES 360
#define PDU_NORMAL 361
#define PDU_PSIZE 362
#define PDU_TEXCOORD 363
#define PDU_TANGENT 364
#define PDU_BINORMAL 365
#define PDU_TESSFACTOR 366
#define PDU_POSITIONT 367
#define PDU_COLOR 368
#define PDU_FOG 369
#define PDU_DEPTH 370
#define PDU_SAMPLE 371
#define RENDERSTATES 372
#define CMDEFINED 373
#define CMATTRIBUTE 374
#define CMCONSTANT 375
#define CMGLOBAL 376
#define CMOPERATOR 377
#define VSCONSTANTMAP 378
#define VSPROGRAM 379
#define GSCONSTANTMAP 380
#define GSPROGRAM 381
#define PSCONSTANTMAP 382
#define PSPROGRAM 383
#define PROGRAM 384
#define ENTRYPOINT 385
#define SHADERTARGET 386
#define SOFTWAREVP 387
#define BONEMATRIX3 388
#define BONEMATRIX4 389
#define SKINBONEMATRIX3 390
#define SKINBONEMATRIX4 391
#define REQUIREMENTS 392
#define VSVERSION 393
#define GSVERSION 394
#define PSVERSION 395
#define USERVERSION 396
#define PLATFORM 397
#define BONESPERPARTITION 398
#define BINORMALTANGENTMETHOD 399
#define BINORMALTANGENTUVSOURCE 400
#define NBTMETHOD_NONE 401
#define NBTMETHOD_NI 402
#define NBTMETHOD_MAX 403
#define NBTMETHOD_ATI 404
#define USERDEFINEDDATA 405
#define IMPLEMENTATION 406
#define CLASSNAME 407
#define PASS 408
#define STAGE 409
#define TSS_TEXTURE 410
#define TSS_COLOROP 411
#define TSS_COLORARG0 412
#define TSS_COLORARG1 413
#define TSS_COLORARG2 414
#define TSS_ALPHAOP 415
#define TSS_ALPHAARG0 416
#define TSS_ALPHAARG1 417
#define TSS_ALPHAARG2 418
#define TSS_RESULTARG 419
#define TSS_CONSTANT_DEPRECATED 420
#define TSS_BUMPENVMAT00 421
#define TSS_BUMPENVMAT01 422
#define TSS_BUMPENVMAT10 423
#define TSS_BUMPENVMAT11 424
#define TSS_BUMPENVLSCALE 425
#define TSS_BUMPENVLOFFSET 426
#define TSS_TEXCOORDINDEX 427
#define TSS_TEXTURETRANSFORMFLAGS 428
#define TSS_TEXTRANSMATRIX 429
#define TTFF_DISABLE 430
#define TTFF_COUNT1 431
#define TTFF_COUNT2 432
#define TTFF_COUNT3 433
#define TTFF_COUNT4 434
#define TTFF_PROJECTED 435
#define PROJECTED 436
#define USEMAPINDEX 437
#define INVERSE 438
#define TRANSPOSE 439
#define TTSRC_GLOBAL 440
#define TTSRC_CONSTANT 441
#define TT_WORLD_PARALLEL 442
#define TT_WORLD_PERSPECTIVE 443
#define TT_WORLD_SPHERE_MAP 444
#define TT_CAMERA_SPHERE_MAP 445
#define TT_SPECULAR_CUBE_MAP 446
#define TT_DIFFUSE_CUBE_MAP 447
#define TCI_PASSTHRU 448
#define TCI_CAMERASPACENORMAL 449
#define TCI_CAMERASPACEPOSITION 450
#define TCI_CAMERASPACEREFLECT 451
#define TCI_SPHEREMAP 452
#define TOP_DISABLE 453
#define TOP_SELECTARG1 454
#define TOP_SELECTARG2 455
#define TOP_MODULATE 456
#define TOP_MODULATE2X 457
#define TOP_MODULATE4X 458
#define TOP_ADD 459
#define TOP_ADDSIGNED 460
#define TOP_ADDSIGNED2X 461
#define TOP_SUBTRACT 462
#define TOP_ADDSMOOTH 463
#define TOP_BLENDDIFFUSEALPHA 464
#define TOP_BLENDTEXTUREALPHA 465
#define TOP_BLENDFACTORALPHA 466
#define TOP_BLENDTEXTUREALPHAPM 467
#define TOP_BLENDCURRENTALPHA 468
#define TOP_PREMODULATE 469
#define TOP_MODULATEALPHA_ADDCOLOR 470
#define TOP_MODULATECOLOR_ADDALPHA 471
#define TOP_MODULATEINVALPHA_ADDCOLOR 472
#define TOP_MODULATEINVCOLOR_ADDALPHA 473
#define TOP_BUMPENVMAP 474
#define TOP_BUMPENVMAPLUMINANCE 475
#define TOP_DOTPRODUCT3 476
#define TOP_MULTIPLYADD 477
#define TOP_LERP 478
#define TA_CURRENT 479
#define TA_DIFFUSE 480
#define TA_SELECTMASK 481
#define TA_SPECULAR 482
#define TA_TEMP 483
#define TA_TEXTURE 484
#define TA_TFACTOR 485
#define TA_ALPHAREPLICATE 486
#define TA_COMPLEMENT 487
#define NTM_BASE 488
#define NTM_DARK 489
#define NTM_DETAIL 490
#define NTM_GLOSS 491
#define NTM_GLOW 492
#define NTM_BUMP 493
#define NTM_NORMAL 494
#define NTM_PARALLAX 495
#define NTM_DECAL 496
#define SAMPLER 497
#define TSAMP_ADDRESSU 498
#define TSAMP_ADDRESSV 499
#define TSAMP_ADDRESSW 500
#define TSAMP_BORDERCOLOR 501
#define TSAMP_MAGFILTER 502
#define TSAMP_MINFILTER 503
#define TSAMP_MIPFILTER 504
#define TSAMP_MIPMAPLODBIAS 505
#define TSAMP_MAXMIPLEVEL 506
#define TSAMP_MAXANISOTROPY 507
#define TSAMP_SRGBTEXTURE 508
#define TSAMP_ELEMENTINDEX 509
#define TSAMP_DMAPOFFSET 510
#define TSAMP_ALPHAKILL_DEPRECATED 511
#define TSAMP_COLORKEYOP_DEPRECATED 512
#define TSAMP_COLORSIGN_DEPRECATED 513
#define TSAMP_COLORKEYCOLOR_DEPRECATED 514
#define TADDR_WRAP 515
#define TADDR_MIRROR 516
#define TADDR_CLAMP 517
#define TADDR_BORDER 518
#define TADDR_MIRRORONCE 519
#define TADDR_CLAMPTOEDGE_DEPRECATED 520
#define TEXF_NONE 521
#define TEXF_POINT 522
#define TEXF_LINEAR 523
#define TEXF_ANISOTROPIC 524
#define TEXF_PYRAMIDALQUAD 525
#define TEXF_GAUSSIANQUAD 526
#define TEXF_FLATCUBIC_DEPRECATED 527
#define TEXF_GAUSSIANCUBIC_DEPRECATED 528
#define TEXF_QUINCUNX_DEPRECATED 529
#define TEXF_MAX_DEPRECATED 530
#define TAK_DISABLE_DEPRECATED 531
#define TAK_ENABLE_DEPRECATED 532
#define TCKOP_DISABLE_DEPRECATED 533
#define TCKOP_ALPHA_DEPRECATED 534
#define TCKOP_RGBA_DEPRECATED 535
#define TCKOP_KILL_DEPRECATED 536
#define TEXTURE 537
#define TEXTURE_SOURCE 538
#define OBJECTS 539
#define EFFECT_GENERALLIGHT 540
#define EFFECT_POINTLIGHT 541
#define EFFECT_DIRECTIONALLIGHT 542
#define EFFECT_SPOTLIGHT 543
#define EFFECT_SHADOWPOINTLIGHT 544
#define EFFECT_SHADOWDIRECTIONALLIGHT 545
#define EFFECT_SHADOWSPOTLIGHT 546
#define EFFECT 547
#define EFFECT_ENVIRONMENTMAP 548
#define EFFECT_PROJECTEDSHADOWMAP 549
#define EFFECT_PROJECTEDLIGHTMAP 550
#define EFFECT_FOGMAP 551
#define USEMAPVALUE 552
#define CMOBJECT 553




/* Copy the first part of user declarations.  */


#if defined(_XENON)
    #include <xtl.h>
    #include <malloc.h>
    #include <NiD3DDefines.h>
    #include <NiD3DTextureStage.h>
#elif defined (WIN32)
    #include "windows.h"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include <malloc.h>
    #include <NiD3DDefines.h>
    #include <NiD3DTextureStage.h>
#elif defined (_PS3)

    // NOTE!!!: flex or bison inserts some incorrect GCC specific ifdef code
    // that must be hand-corrected after running the batch files
    // Add:
    //   !defined(_PS3) &&  
    // into the instance of #if defined (__GNUC_MINOR__) ...

    #include <ctype.h>    
    #include <NiPS3TextureSampler.h>
    
    class NiD3DShaderDeclaration
    {
    public:
        enum ShaderParameterTesselator
        {
            SPTESS_DEFAULT,
            SPTESS_PARTIALU,
            SPTESS_PARTIALV,
            SPTESS_CROSSUV,
            SPTESS_UV,
            SPTESS_LOOKUP,
            SPTESS_LOOKUPPRESAMPLED,
            SPTESS_COUNT
        };
    };
    class NiD3DTextureStage : public NiPS3TextureSampler
    {
    public:
        enum TSTextureTransformFlags
        {
            TSTTF_IGNORE                = 0x00000000,
            TSTTF_GLOBAL                = 0x10000000,
            TSTTF_CONSTANT              = 0x20000000,
            TSTTF_SOURCE_MASK           = 0xf0000000,
            TSTTF_NI_NO_CALC            = 0x00000000,
            TSTTF_NI_WORLD_PARALLEL     = 0x00000001,
            TSTTF_NI_WORLD_PERSPECTIVE  = 0x00000002,
            TSTTF_NI_WORLD_SPHERE_MAP   = 0x00000003,
            TSTTF_NI_CAMERA_SPHERE_MAP  = 0x00000004,
            TSTTF_NI_SPECULAR_CUBE_MAP  = 0x00000005,
            TSTTF_NI_DIFFUSE_CUBE_MAP   = 0x00000006
        };
    };
        
#endif  //#if defined(_XENON)
    #include "NSFTextFile.h"
    #include "NSFParsedShader.h"

    #include <NiTArray.h>
    #include <NiShaderFactory.h>
    
    #include <NSBStateGroup.h>
    #include <NSBRenderStates.h>
    #include <NSBStageAndSamplerStates.h>

    #include <NSBShader.h>
    #include <NSBAttributeDesc.h>
    #include <NSBAttributeTable.h>
    #include <NSBObjectTable.h>
    #include <NSBPackingDef.h>
    #include <NSBImplementation.h>
    #include <NSBStateGroup.h>
    #include <NSBConstantMap.h>
    #include <NSBPass.h>
    #include <NSBTextureStage.h>
    #include <NSBTexture.h>
    
    unsigned int F2DW(float fValue) {return *((unsigned int*)&fValue);}
    
    #include <NSBUserDefinedDataSet.h>
    
    
    void NSFParsererror2 (char *s);
    void NSFParsererror  (char *s);
    int  yylex    (void);

    NSFTextFile* g_pkFile = 0;
    
    extern int NSFParserGetLineNumber();

//    #define _ENABLE_DEBUG_STRING_OUT_
    char g_szDSO[1024];
    bool g_bFirstDSOFileAccess = true;
    int g_iDSOIndent = 0;
    FILE* g_pfDSOFile = 0;
    void DebugStringOut(char* pszOut, bool bIndent = true);

    #define YYDEBUG                 1
    #define NSFParsererror_VERBOSE  1
    #define YYERROR_VERBOSE         1

    #define ERR_INVALID_ENUM    "Invalid enumerant"
    #define ERR_INVALID_COMMAND "Invalid command in block"

    // Gamebryo
        
    #define FLOAT_ARRAY_SIZE        64
    #define MAX_QUOTE_LENGTH    6 * 1024

    NiTPrimitiveArray<float>* g_afValues;
    
    unsigned int ResetFloatValueArray(void);
    unsigned int AddFloatToValueArray(float fValue);
    
    void ResetFloatRangeArrays(void);
    unsigned int AddFloatToLowArray(float fValue);
    unsigned int AddFloatToHighArray(float fValue);
    
    // Ranges
    bool g_bRanged;
    unsigned int g_uiLow, g_uiHigh;
    unsigned int g_uiLowFloatValues;
    float g_afLowValues[FLOAT_ARRAY_SIZE];
    unsigned int g_uiHighFloatValues;
    float g_afHighValues[FLOAT_ARRAY_SIZE];
    
    typedef enum
    {
        CONSTMAP_VERTEX = 0,
        CONSTMAP_PIXEL,
        CONSTMAP_GEOMETRY
    } NiConstantMapMode;
    
    NiConstantMapMode g_eConstMapMode;
    unsigned int g_uiCurrentPlatforms;
    bool g_bConstantMapPlatformBlock = false;
    
    void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
        unsigned int uiIndex, const char* pcName, const char* pcDebugString);
    unsigned int DecodeAttribTypeString(char* pszAttribType);
    unsigned int DecodePlatformString(char* pszPlatform);
    bool AddAttributeToConstantMap(char* pszName, 
        unsigned int uiRegisterStart, unsigned int uiRegisterCount,
        unsigned int uiExtraNumber, bool bIsGlobal);
    bool SetupOperatorEntry(char* pszName, int iRegStart, int iRegCount, 
        char* pszEntry1, int iOperation, char* pszEntry2, bool bInverse, 
        bool bTranspose);
    NiShaderAttributeDesc::AttributeType DetermineOperatorResult(
        int iOperation, NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultDivide(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultAdd(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);

    NiTPointerList<NSFParsedShader*> g_kParsedShaderList;
    NSFParsedShader* g_pkCurrShader = 0;

    // Binary Shader
    NSBShader* g_pkCurrNSBShader = 0;
    // Attribute Table
    bool g_bGlobalAttributes = false;
    NSBAttributeTable* g_pkCurrAttribTable = 0;
    // Object Table
    NSBObjectTable* g_pkCurrObjectTable = 0;
    // Packing Definition
    unsigned int g_uiCurrPDStream = 0;    
    bool g_bCurrPDFixedFunction = false;
    NSBPackingDef* g_pkCurrPackingDef = 0;
    // Requirements
    NSBRequirements* g_pkCurrRequirements = 0;
    // Implementation
    unsigned int g_uiCurrImplementation = 0;
    NSBImplementation* g_pkCurrImplementation = 0;
    // RenderState Group
    NSBStateGroup* g_pkCurrRSGroup = 0;
    // ConstantMap
    unsigned int g_uiCurrImplemVSConstantMap = 0;
    unsigned int g_uiCurrImplemGSConstantMap = 0;
    unsigned int g_uiCurrImplemPSConstantMap = 0;
    unsigned int g_uiCurrPassVSConstantMap = 0;
    unsigned int g_uiCurrPassGSConstantMap = 0;
    unsigned int g_uiCurrPassPSConstantMap = 0;
    NSBConstantMap* g_pkCurrConstantMap = 0;
    // Pass
    unsigned int g_uiCurrPassIndex = 0;
    NSBPass* g_pkCurrPass = 0;
    // TextureStage
    NSBTextureStage* g_pkCurrTextureStage = 0;
    unsigned int g_uiCurrTextureSlot = 0;
    NSBTexture* g_pkCurrTexture = 0;
    
    bool g_bCurrStateValid = false;
    unsigned int g_uiCurrStateState = 0;
    unsigned int g_uiCurrStateValue = 0;
    bool g_bUseMapValue = false;

    NSBUserDefinedDataSet* g_pkCurrUDDataSet = 0;
    NSBUserDefinedDataBlock* g_pkCurrUDDataBlock = 0;
    


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
    float fval;
    int   ival;
    char* sval;
    unsigned int uival;
    unsigned long  dword;
    unsigned short word;
    unsigned char  byte;
    bool bval;
    unsigned int vers;
} YYSTYPE;
/* Line 196 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1152

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  301
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  241
/* YYNRULES -- Number of rules. */
#define YYNRULES  624
/* YYNRULES -- Number of states. */
#define YYNSTATES  1086

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   553

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short int yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     299,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   300,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    10,    19,    22,    23,
      25,    28,    30,    32,    34,    36,    38,    40,    42,    43,
      45,    47,    50,    51,    53,    54,    56,    58,    60,    62,
      66,    67,    88,   105,   106,   127,   128,   145,   146,   159,
     160,   165,   166,   171,   176,   178,   182,   184,   216,   217,
     223,   224,   230,   231,   235,   238,   240,   242,   244,   246,
     248,   250,   252,   254,   256,   258,   260,   262,   264,   269,
     270,   277,   278,   286,   287,   295,   296,   304,   305,   313,
     314,   321,   322,   329,   330,   337,   343,   348,   349,   355,
     358,   360,   362,   364,   366,   368,   370,   372,   374,   376,
     378,   380,   382,   386,   390,   394,   398,   402,   406,   410,
     414,   418,   422,   426,   427,   434,   437,   438,   440,   442,
     444,   446,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
     484,   486,   488,   490,   492,   495,   497,   499,   501,   503,
     505,   507,   509,   511,   513,   515,   517,   519,   521,   523,
     525,   527,   529,   531,   533,   535,   537,   539,   541,   543,
     545,   547,   549,   551,   553,   555,   557,   559,   561,   563,
     565,   567,   569,   571,   573,   575,   578,   580,   583,   586,
     589,   597,   602,   609,   613,   614,   620,   623,   625,   627,
     629,   632,   634,   636,   638,   640,   642,   645,   649,   653,
     657,   661,   665,   669,   672,   674,   676,   678,   680,   682,
     684,   686,   688,   689,   690,   699,   705,   711,   717,   723,
     729,   735,   741,   747,   752,   757,   762,   767,   768,   775,
     776,   783,   784,   790,   800,   802,   804,   806,   808,   811,
     812,   814,   815,   817,   818,   824,   825,   831,   832,   838,
     842,   845,   848,   851,   855,   858,   860,   864,   868,   872,
     874,   877,   879,   883,   887,   891,   892,   893,   903,   905,
     907,   910,   913,   916,   920,   923,   925,   929,   933,   937,
     939,   942,   944,   948,   952,   953,   954,   964,   966,   968,
     971,   974,   977,   981,   984,   986,   990,   994,   998,  1000,
    1003,  1005,  1009,  1013,  1014,  1015,  1025,  1027,  1029,  1030,
    1036,  1039,  1041,  1043,  1045,  1047,  1049,  1051,  1053,  1055,
    1057,  1059,  1062,  1066,  1070,  1074,  1078,  1082,  1090,  1096,
    1100,  1102,  1106,  1110,  1114,  1118,  1120,  1122,  1124,  1126,
    1129,  1131,  1133,  1135,  1136,  1144,  1145,  1147,  1150,  1152,
    1154,  1156,  1158,  1160,  1162,  1165,  1167,  1169,  1171,  1173,
    1175,  1177,  1179,  1181,  1183,  1185,  1187,  1189,  1191,  1193,
    1195,  1197,  1199,  1201,  1204,  1208,  1212,  1216,  1220,  1222,
    1224,  1226,  1228,  1230,  1232,  1234,  1236,  1239,  1243,  1247,
    1251,  1255,  1259,  1263,  1267,  1271,  1275,  1279,  1283,  1287,
    1291,  1295,  1299,  1303,  1307,  1311,  1315,  1319,  1323,  1327,
    1332,  1337,  1342,  1346,  1350,  1352,  1354,  1356,  1358,  1359,
    1361,  1363,  1365,  1367,  1369,  1371,  1373,  1375,  1377,  1379,
    1381,  1383,  1385,  1387,  1389,  1391,  1393,  1395,  1397,  1399,
    1401,  1403,  1405,  1407,  1409,  1411,  1413,  1416,  1419,  1422,
    1425,  1428,  1431,  1434,  1437,  1438,  1440,  1443,  1446,  1448,
    1450,  1451,  1453,  1455,  1457,  1459,  1461,  1464,  1468,  1472,
    1473,  1478,  1479,  1481,  1483,  1485,  1487,  1489,  1491,  1494,
    1495,  1503,  1504,  1511,  1512,  1514,  1517,  1519,  1521,  1523,
    1525,  1528,  1530,  1532,  1534,  1536,  1538,  1540,  1542,  1544,
    1546,  1548,  1550,  1552,  1554,  1556,  1558,  1560,  1562,  1565,
    1569,  1573,  1577,  1581,  1585,  1589,  1593,  1597,  1601,  1605,
    1609,  1613,  1617,  1621,  1625,  1629,  1633,  1637,  1641,  1645,
    1649,  1653,  1657,  1661,  1665,  1669,  1673,  1675,  1677,  1679,
    1681,  1683,  1685,  1687,  1690,  1692,  1694,  1696,  1698,  1700,
    1702,  1704,  1706,  1708,  1710,  1712,  1715,  1717,  1719,  1722,
    1724,  1726,  1728,  1730,  1733,  1734,  1742,  1743,  1750,  1751,
    1753,  1756,  1758,  1760,  1763,  1767,  1771,  1775,  1779,  1782,
    1784,  1786,  1788,  1790,  1792,  1794,  1796,  1798,  1800,  1802,
    1803,  1810,  1811,  1813,  1816,  1818,  1820,  1822,  1824,  1826,
    1828,  1830,  1832,  1834,  1836,  1837,  1838,  1847,  1851,  1855,
    1859,  1863,  1865,  1868,  1869
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     302,     0,    -1,   302,   303,    -1,   303,    -1,    -1,    -1,
      28,    24,     5,   304,   311,   305,   306,     6,    -1,     1,
     299,    -1,    -1,   307,    -1,   307,   308,    -1,   308,    -1,
     324,    -1,   350,    -1,   326,    -1,   365,    -1,   535,    -1,
     540,    -1,    -1,    24,    -1,    25,    -1,   310,    25,    -1,
      -1,   310,    -1,    -1,    25,    -1,     4,    -1,    24,    -1,
      25,    -1,    11,    24,    12,    -1,    -1,     9,     7,    22,
      15,    22,    15,    22,    15,    22,     8,     7,    22,    15,
      22,    15,    22,    15,    22,     8,    10,    -1,     9,     7,
      22,    15,    22,    15,    22,     8,     7,    22,    15,    22,
      15,    22,     8,    10,    -1,    -1,     9,     7,    22,    15,
      22,    15,    22,    15,    22,     8,     7,    22,    15,    22,
      15,    22,    15,    22,     8,    10,    -1,    -1,     9,     7,
      22,    15,    22,    15,    22,     8,     7,    22,    15,    22,
      15,    22,     8,    10,    -1,    -1,     9,     7,    22,    15,
      22,     8,     7,    22,    15,    22,     8,    10,    -1,    -1,
       9,    22,    22,    10,    -1,    -1,     9,    23,    23,    10,
      -1,     9,    22,    22,    10,    -1,   322,    -1,   322,    15,
     321,    -1,    22,    -1,    22,    15,    22,    15,    22,    15,
      22,    15,    22,    15,    22,    15,    22,    15,    22,    15,
      22,    15,    22,    15,    22,    15,    22,    15,    22,    15,
      22,    15,    22,    15,    22,    -1,    -1,    32,     5,   325,
     329,     6,    -1,    -1,    33,     5,   327,   329,     6,    -1,
      -1,     9,    23,    10,    -1,   329,   330,    -1,   330,    -1,
     332,    -1,   333,    -1,   335,    -1,   337,    -1,   339,    -1,
     341,    -1,   343,    -1,   345,    -1,   347,    -1,   349,    -1,
      29,    -1,    30,    -1,    35,    24,   331,    26,    -1,    -1,
      37,   334,    24,   331,    23,   320,    -1,    -1,    38,   336,
      24,   328,   331,   321,   319,    -1,    -1,    39,   338,    24,
     328,   331,   321,   318,    -1,    -1,    40,   340,    24,   328,
     331,   321,   317,    -1,    -1,    41,   342,    24,   328,   331,
     321,   316,    -1,    -1,    42,   344,    24,   328,   331,   321,
      -1,    -1,    43,   346,    24,   328,   331,   321,    -1,    -1,
      44,   348,    24,   331,   321,   315,    -1,    45,    24,   331,
      23,   312,    -1,    45,    24,   331,   312,    -1,    -1,   284,
       5,   351,   352,     6,    -1,   352,   353,    -1,   353,    -1,
     354,    -1,   355,    -1,   356,    -1,   357,    -1,   358,    -1,
     359,    -1,   360,    -1,   361,    -1,   362,    -1,   363,    -1,
     364,    -1,   285,    23,    24,    -1,   286,    23,    24,    -1,
     287,    23,    24,    -1,   288,    23,    24,    -1,   289,    23,
      24,    -1,   290,    23,    24,    -1,   291,    23,    24,    -1,
     293,    23,    24,    -1,   294,    23,    24,    -1,   295,    23,
      24,    -1,   296,    23,    24,    -1,    -1,    46,    24,     5,
     366,   372,     6,    -1,    46,    24,    -1,    -1,    69,    -1,
      70,    -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,
      75,    -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,
      80,    -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,
      90,    -1,    91,    -1,    92,    -1,    93,    -1,    94,    -1,
      95,    -1,     1,   299,    -1,    49,    -1,    50,    -1,    51,
      -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,    56,
      -1,    57,    -1,    58,    -1,    59,    -1,    60,    -1,    61,
      -1,    62,    -1,    63,    -1,    64,    -1,    65,    -1,    66,
      -1,    67,    -1,    96,    -1,    97,    -1,    98,    -1,    99,
      -1,   100,    -1,   101,    -1,   102,    -1,   103,    -1,   104,
      -1,   105,    -1,   106,    -1,   107,    -1,   108,    -1,   109,
      -1,   110,    -1,   111,    -1,   112,    -1,   113,    -1,   114,
      -1,   115,    -1,   116,    -1,   372,   373,    -1,   373,    -1,
       1,   299,    -1,    47,    23,    -1,    48,    26,    -1,    68,
      23,    23,   368,   370,   371,    23,    -1,    68,    23,    23,
     368,    -1,   369,    23,   368,   370,   371,    23,    -1,   369,
      23,   368,    -1,    -1,   117,     5,   375,   376,     6,    -1,
     376,   377,    -1,   377,    -1,   378,    -1,   379,    -1,   379,
      31,    -1,   382,    -1,   384,    -1,   383,    -1,   380,    -1,
     381,    -1,     1,   299,    -1,    24,    14,   314,    -1,    24,
      14,    24,    -1,    24,    14,    26,    -1,    24,    14,    22,
      -1,    24,    14,    21,    -1,    24,    14,    23,    -1,   385,
     386,    -1,   386,    -1,   390,    -1,   391,    -1,   392,    -1,
     394,    -1,   396,    -1,   398,    -1,   387,    -1,    -1,    -1,
       5,   388,   142,    14,   445,   389,   385,     6,    -1,   118,
     133,    23,    23,    23,    -1,   118,   133,   313,    23,    23,
      -1,   118,   134,    23,    23,    23,    -1,   118,   134,   313,
      23,    23,    -1,   118,   135,    23,    23,    23,    -1,   118,
     135,   313,    23,    23,    -1,   118,   136,    23,    23,    23,
      -1,   118,   136,   313,    23,    23,    -1,   118,   313,    23,
      23,    -1,   118,   313,   313,    23,    -1,   298,    24,    24,
      23,    -1,   298,    24,    24,    24,    -1,    -1,   119,   393,
     313,    23,    23,    23,    -1,    -1,   120,   395,   313,    23,
      23,   321,    -1,    -1,   121,   397,   313,    23,    23,    -1,
     122,    24,    23,    23,    24,   399,    24,   401,   400,    -1,
      17,    -1,    18,    -1,    19,    -1,    20,    -1,     1,   299,
      -1,    -1,   184,    -1,    -1,   183,    -1,    -1,   123,     5,
     403,   385,     6,    -1,    -1,   125,     5,   405,   385,     6,
      -1,    -1,   127,     5,   407,   385,     6,    -1,   152,    14,
     313,    -1,   124,     4,    -1,   124,    24,    -1,   124,    25,
      -1,   409,    24,    24,    -1,   409,    24,    -1,   409,    -1,
     129,    14,     4,    -1,   129,    14,    24,    -1,   129,    14,
      25,    -1,   413,    -1,   413,   412,    -1,   411,    -1,   130,
      14,    24,    -1,   131,    14,    24,    -1,   132,    14,    26,
      -1,    -1,    -1,   124,     5,   415,   142,    14,   445,   416,
     412,     6,    -1,   414,    -1,   410,    -1,   126,     4,    -1,
     126,    24,    -1,   126,    25,    -1,   418,    24,    24,    -1,
     418,    24,    -1,   418,    -1,   129,    14,     4,    -1,   129,
      14,    24,    -1,   129,    14,    25,    -1,   422,    -1,   422,
     421,    -1,   420,    -1,   130,    14,    24,    -1,   131,    14,
      24,    -1,    -1,    -1,   126,     5,   424,   142,    14,   445,
     425,   421,     6,    -1,   423,    -1,   419,    -1,   128,     4,
      -1,   128,    24,    -1,   128,    25,    -1,   427,    24,    24,
      -1,   427,    24,    -1,   427,    -1,   129,    14,     4,    -1,
     129,    14,    24,    -1,   129,    14,    25,    -1,   431,    -1,
     431,   430,    -1,   429,    -1,   130,    14,    24,    -1,   131,
      14,    24,    -1,    -1,    -1,   128,     5,   433,   142,    14,
     445,   434,   430,     6,    -1,   432,    -1,   428,    -1,    -1,
     137,     5,   437,   438,     6,    -1,   438,   439,    -1,   439,
      -1,   440,    -1,   441,    -1,   442,    -1,   443,    -1,   444,
      -1,   447,    -1,   449,    -1,   448,    -1,   446,    -1,     1,
     299,    -1,   138,    14,    27,    -1,   139,    14,    27,    -1,
     140,    14,    27,    -1,   141,    14,    27,    -1,   142,    14,
     445,    -1,    24,    13,    24,    13,    24,    13,    24,    -1,
      24,    13,    24,    13,    24,    -1,    24,    13,    24,    -1,
      24,    -1,    24,    14,    26,    -1,   143,    14,    23,    -1,
     145,    14,    23,    -1,   144,    14,   450,    -1,   146,    -1,
     147,    -1,   149,    -1,   148,    -1,     1,   299,    -1,   452,
      -1,   492,    -1,   521,    -1,    -1,   154,    23,   309,     5,
     453,   454,     6,    -1,    -1,   455,    -1,   455,   456,    -1,
     456,    -1,   460,    -1,   488,    -1,   457,    -1,   458,    -1,
     459,    -1,   459,    31,    -1,   463,    -1,   464,    -1,   465,
      -1,   466,    -1,   467,    -1,   468,    -1,   469,    -1,   470,
      -1,   471,    -1,   472,    -1,   473,    -1,   474,    -1,   475,
      -1,   476,    -1,   477,    -1,   478,    -1,   479,    -1,   480,
      -1,     1,   299,    -1,   155,    14,   461,    -1,   155,    14,
     462,    -1,   155,    14,   314,    -1,   155,    14,    24,    -1,
     233,    -1,   234,    -1,   235,    -1,   236,    -1,   237,    -1,
     238,    -1,   239,    -1,   240,    -1,   241,    23,    -1,   156,
      14,   483,    -1,   157,    14,   484,    -1,   158,    14,   484,
      -1,   159,    14,   484,    -1,   160,    14,   483,    -1,   161,
      14,   484,    -1,   162,    14,   484,    -1,   163,    14,   484,
      -1,   164,    14,   484,    -1,   165,    14,    21,    -1,   166,
      14,    22,    -1,   166,    14,   314,    -1,   167,    14,    22,
      -1,   167,    14,   314,    -1,   168,    14,    22,    -1,   168,
      14,   314,    -1,   169,    14,    22,    -1,   169,    14,   314,
      -1,   170,    14,    22,    -1,   170,    14,   314,    -1,   171,
      14,    22,    -1,   171,    14,   314,    -1,   172,    14,   487,
      23,    -1,   172,    14,   487,   182,    -1,   173,    14,   481,
     482,    -1,   173,    14,   180,    -1,   173,    14,   175,    -1,
     176,    -1,   177,    -1,   178,    -1,   179,    -1,    -1,   180,
      -1,   198,    -1,   199,    -1,   200,    -1,   201,    -1,   202,
      -1,   203,    -1,   204,    -1,   205,    -1,   206,    -1,   207,
      -1,   208,    -1,   209,    -1,   210,    -1,   211,    -1,   212,
      -1,   213,    -1,   214,    -1,   215,    -1,   216,    -1,   217,
      -1,   218,    -1,   219,    -1,   220,    -1,   221,    -1,   222,
      -1,   223,    -1,     1,   299,    -1,   224,   485,    -1,   225,
     485,    -1,   226,   485,    -1,   227,   485,    -1,   228,   485,
      -1,   229,   485,    -1,   230,   485,    -1,    -1,   486,    -1,
     231,   232,    -1,   232,   231,    -1,   231,    -1,   232,    -1,
      -1,   193,    -1,   194,    -1,   195,    -1,   196,    -1,   197,
      -1,     1,   299,    -1,   174,    14,   489,    -1,   185,   491,
      24,    -1,    -1,   186,   490,   491,   323,    -1,    -1,   187,
      -1,   188,    -1,   189,    -1,   190,    -1,   191,    -1,   192,
      -1,     1,   299,    -1,    -1,   242,    23,   309,     5,   493,
     495,     6,    -1,    -1,   242,   309,     5,   494,   495,     6,
      -1,    -1,   496,    -1,   496,   497,    -1,   497,    -1,   498,
      -1,   499,    -1,   460,    -1,   499,    31,    -1,   500,    -1,
     501,    -1,   502,    -1,   503,    -1,   504,    -1,   505,    -1,
     506,    -1,   507,    -1,   508,    -1,   509,    -1,   510,    -1,
     511,    -1,   512,    -1,   513,    -1,   514,    -1,   515,    -1,
     516,    -1,     1,   299,    -1,   243,    14,   517,    -1,   244,
      14,   517,    -1,   245,    14,   517,    -1,   246,    14,    21,
      -1,   246,    14,   314,    -1,   247,    14,   518,    -1,   248,
      14,   518,    -1,   249,    14,   518,    -1,   250,    14,    23,
      -1,   250,    14,   314,    -1,   251,    14,    23,    -1,   251,
      14,   314,    -1,   252,    14,    23,    -1,   252,    14,   314,
      -1,   253,    14,    23,    -1,   253,    14,   314,    -1,   254,
      14,    23,    -1,   254,    14,   314,    -1,   255,    14,    23,
      -1,   255,    14,   314,    -1,   256,    14,   519,    -1,   256,
      14,   314,    -1,   257,    14,   520,    -1,   258,    14,    24,
      -1,   258,    14,   300,    -1,   259,    14,    21,    -1,   259,
      14,   314,    -1,   260,    -1,   261,    -1,   262,    -1,   263,
      -1,   264,    -1,   265,    -1,   297,    -1,     1,   299,    -1,
     266,    -1,   267,    -1,   268,    -1,   269,    -1,   270,    -1,
     271,    -1,   274,    -1,   272,    -1,   273,    -1,   275,    -1,
     297,    -1,     1,   299,    -1,   276,    -1,   277,    -1,     1,
     299,    -1,   278,    -1,   279,    -1,   280,    -1,   281,    -1,
       1,   299,    -1,    -1,   282,    23,   309,     5,   522,   524,
       6,    -1,    -1,   282,   309,     5,   523,   524,     6,    -1,
      -1,   525,    -1,   525,   526,    -1,   526,    -1,   527,    -1,
       1,   299,    -1,   283,    14,   461,    -1,   283,    14,   462,
      -1,   283,    14,   314,    -1,   283,    14,    24,    -1,   528,
     529,    -1,   529,    -1,   374,    -1,   417,    -1,   402,    -1,
     426,    -1,   404,    -1,   435,    -1,   406,    -1,   451,    -1,
     540,    -1,    -1,   153,   309,     5,   531,   528,     6,    -1,
      -1,   533,    -1,   533,   534,    -1,   534,    -1,   374,    -1,
     367,    -1,   530,    -1,   436,    -1,   402,    -1,   404,    -1,
     406,    -1,   408,    -1,   540,    -1,    -1,    -1,   151,    24,
       5,   536,   311,   537,   532,     6,    -1,   313,    14,    23,
      -1,   313,    14,    26,    -1,   313,    14,   321,    -1,   313,
      14,   313,    -1,   538,    -1,   538,   539,    -1,    -1,   150,
      24,     5,   541,   539,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   506,   506,   507,   514,   557,   511,   579,   586,   588,
     592,   593,   597,   598,   599,   600,   601,   602,   609,   610,
     614,   618,   630,   631,   635,   636,   637,   641,   642,   646,
     654,   657,   676,   697,   700,   723,   726,   747,   750,   765,
     768,   778,   781,   787,   801,   802,   806,   813,   845,   843,
     874,   872,   903,   907,   915,   916,   921,   922,   923,   924,
     925,   926,   927,   928,   929,   930,   936,   937,   942,   973,
     972,  1037,  1036,  1127,  1126,  1218,  1217,  1311,  1310,  1406,
    1405,  1497,  1496,  1585,  1584,  1739,  1764,  1800,  1798,  1826,
    1827,  1832,  1833,  1834,  1835,  1836,  1837,  1838,  1839,  1840,
    1841,  1842,  1847,  1861,  1875,  1889,  1903,  1917,  1931,  1945,
    1959,  1973,  1987,  2006,  2003,  2038,  2075,  2076,  2077,  2078,
    2079,  2080,  2081,  2082,  2083,  2084,  2085,  2086,  2087,  2088,
    2089,  2090,  2091,  2092,  2093,  2094,  2095,  2096,  2097,  2098,
    2099,  2100,  2101,  2102,  2103,  2111,  2113,  2115,  2117,  2119,
    2121,  2123,  2125,  2127,  2129,  2131,  2133,  2135,  2137,  2139,
    2141,  2143,  2145,  2147,  2152,  2154,  2156,  2158,  2160,  2162,
    2164,  2169,  2171,  2173,  2175,  2177,  2179,  2181,  2183,  2185,
    2187,  2189,  2191,  2193,  2195,  2200,  2201,  2202,  2210,  2216,
    2222,  2275,  2310,  2355,  2395,  2393,  2428,  2429,  2433,  2443,
    2456,  2460,  2461,  2462,  2463,  2464,  2465,  2473,  2506,  2562,
    2604,  2646,  2684,  2728,  2729,  2733,  2734,  2735,  2736,  2737,
    2738,  2739,  2744,  2756,  2743,  2769,  2808,  2847,  2886,  2925,
    2964,  3003,  3042,  3081,  3106,  3133,  3211,  3294,  3293,  3320,
    3319,  3381,  3380,  3406,  3438,  3440,  3442,  3444,  3446,  3454,
    3455,  3459,  3460,  3468,  3466,  3507,  3505,  3546,  3544,  3584,
    3599,  3611,  3623,  3638,  3656,  3670,  3679,  3680,  3681,  3685,
    3686,  3691,  3715,  3724,  3733,  3745,  3751,  3743,  3764,  3765,
    3772,  3784,  3796,  3811,  3829,  3843,  3852,  3853,  3854,  3858,
    3859,  3864,  3888,  3897,  3911,  3917,  3909,  3930,  3931,  3938,
    3950,  3962,  3977,  3995,  4009,  4018,  4019,  4020,  4024,  4025,
    4030,  4054,  4063,  4077,  4083,  4075,  4096,  4097,  4106,  4104,
    4143,  4144,  4148,  4149,  4150,  4151,  4152,  4153,  4154,  4155,
    4156,  4157,  4165,  4176,  4187,  4207,  4218,  4229,  4277,  4315,
    4344,  4366,  4404,  4415,  4428,  4443,  4444,  4445,  4446,  4457,
    4467,  4468,  4469,  4478,  4476,  4510,  4512,  4516,  4517,  4521,
    4522,  4523,  4527,  4545,  4566,  4570,  4571,  4572,  4573,  4574,
    4575,  4576,  4577,  4578,  4579,  4580,  4581,  4582,  4583,  4584,
    4585,  4586,  4587,  4588,  4597,  4602,  4607,  4696,  4729,  4730,
    4731,  4732,  4733,  4734,  4735,  4736,  4741,  4748,  4761,  4774,
    4787,  4800,  4813,  4826,  4839,  4852,  4865,  4882,  4892,  4902,
    4912,  4922,  4932,  4942,  4952,  4962,  4972,  4982,  4992,  5002,
    5013,  5030,  5048,  5062,  5079,  5080,  5081,  5082,  5087,  5088,
    5093,  5097,  5101,  5105,  5109,  5113,  5117,  5121,  5125,  5129,
    5133,  5137,  5141,  5145,  5149,  5153,  5157,  5161,  5165,  5169,
    5173,  5177,  5181,  5185,  5189,  5193,  5197,  5205,  5208,  5211,
    5214,  5217,  5220,  5223,  5229,  5230,  5234,  5240,  5245,  5247,
    5253,  5256,  5260,  5264,  5268,  5272,  5276,  5284,  5291,  5308,
    5307,  5351,  5352,  5354,  5356,  5358,  5360,  5362,  5364,  5377,
    5375,  5405,  5403,  5431,  5433,  5437,  5438,  5442,  5461,  5480,
    5488,  5492,  5493,  5494,  5495,  5496,  5497,  5498,  5499,  5500,
    5501,  5502,  5503,  5504,  5505,  5506,  5507,  5508,  5509,  5517,
    5546,  5575,  5615,  5626,  5637,  5666,  5695,  5724,  5735,  5746,
    5757,  5768,  5779,  5790,  5801,  5812,  5823,  5834,  5845,  5856,
    5866,  5881,  5894,  5906,  5919,  5929,  5944,  5946,  5948,  5950,
    5952,  5954,  5956,  5961,  5969,  5971,  5973,  5975,  5977,  5979,
    5981,  5983,  5985,  5987,  5989,  5994,  6002,  6003,  6004,  6012,
    6013,  6014,  6015,  6016,  6029,  6027,  6056,  6054,  6082,  6084,
    6088,  6089,  6093,  6094,  6102,  6107,  6112,  6201,  6236,  6237,
    6241,  6242,  6243,  6244,  6245,  6246,  6247,  6248,  6249,  6255,
    6253,  6287,  6289,  6293,  6294,  6298,  6299,  6300,  6301,  6302,
    6303,  6304,  6305,  6306,  6312,  6331,  6310,  6360,  6391,  6422,
    6514,  6549,  6550,  6556,  6554
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EOLN", "PATH", "L_ACCOLADE",
  "R_ACCOLADE", "L_PARENTHESE", "R_PARENTHESE", "L_BRACKET", "R_BRACKET",
  "L_ANGLEBRACKET", "R_ANGLEBRACKET", "OR", "ASSIGN", "COMMA",
  "UNDERSCORE", "ASTERIK", "FORWARDSLASH", "PLUS", "MINUS", "N_HEX",
  "N_FLOAT", "N_INT", "N_STRING", "N_QUOTE", "N_BOOL", "N_VERSION",
  "NSFSHADER", "ARTIST", "HIDDEN", "SAVE", "ATTRIBUTES",
  "GLOBALATTRIBUTES", "ATTRIB", "ATTRIB_BOOL", "ATTRIB_STRING",
  "ATTRIB_UINT", "ATTRIB_FLOAT", "ATTRIB_POINT2", "ATTRIB_POINT3",
  "ATTRIB_POINT4", "ATTRIB_MATRIX3", "ATTRIB_TRANSFORM", "ATTRIB_COLOR",
  "ATTRIB_TEXTURE", "PACKINGDEF", "PD_STREAM", "PD_FIXEDFUNCTION",
  "PDP_POSITION", "PDP_BLENDWEIGHTS", "PDP_BLENDINDICES", "PDP_NORMAL",
  "PDP_POINTSIZE", "PDP_COLOR", "PDP_COLOR2", "PDP_TEXCOORD0",
  "PDP_TEXCOORD1", "PDP_TEXCOORD2", "PDP_TEXCOORD3", "PDP_TEXCOORD4",
  "PDP_TEXCOORD5", "PDP_TEXCOORD6", "PDP_TEXCOORD7", "PDP_POSITION2",
  "PDP_NORMAL2", "PDP_TANGENT", "PDP_BINORMAL", "PDP_EXTRADATA",
  "PDT_FLOAT1", "PDT_FLOAT2", "PDT_FLOAT3", "PDT_FLOAT4", "PDT_UBYTECOLOR",
  "PDT_SHORT1", "PDT_SHORT2", "PDT_SHORT3", "PDT_SHORT4", "PDT_UBYTE4",
  "PDT_NORMSHORT1", "PDT_NORMSHORT2", "PDT_NORMSHORT3", "PDT_NORMSHORT4",
  "PDT_NORMPACKED3", "PDT_PBYTE1", "PDT_PBYTE2", "PDT_PBYTE3",
  "PDT_PBYTE4", "PDT_FLOAT2H", "PDT_NORMUBYTE4", "PDT_NORMUSHORT2",
  "PDT_NORMUSHORT4", "PDT_UDEC3", "PDT_NORMDEC3", "PDT_FLOAT16_2",
  "PDT_FLOAT16_4", "PDTESS_DEFAULT", "PDTESS_PARTIALU", "PDTESS_PARTIALV",
  "PDTESS_CROSSUV", "PDTESS_UV", "PDTESS_LOOKUP",
  "PDTESS_LOOKUPPRESAMPLED", "PDU_POSITION", "PDU_BLENDWEIGHT",
  "PDU_BLENDINDICES", "PDU_NORMAL", "PDU_PSIZE", "PDU_TEXCOORD",
  "PDU_TANGENT", "PDU_BINORMAL", "PDU_TESSFACTOR", "PDU_POSITIONT",
  "PDU_COLOR", "PDU_FOG", "PDU_DEPTH", "PDU_SAMPLE", "RENDERSTATES",
  "CMDEFINED", "CMATTRIBUTE", "CMCONSTANT", "CMGLOBAL", "CMOPERATOR",
  "VSCONSTANTMAP", "VSPROGRAM", "GSCONSTANTMAP", "GSPROGRAM",
  "PSCONSTANTMAP", "PSPROGRAM", "PROGRAM", "ENTRYPOINT", "SHADERTARGET",
  "SOFTWAREVP", "BONEMATRIX3", "BONEMATRIX4", "SKINBONEMATRIX3",
  "SKINBONEMATRIX4", "REQUIREMENTS", "VSVERSION", "GSVERSION", "PSVERSION",
  "USERVERSION", "PLATFORM", "BONESPERPARTITION", "BINORMALTANGENTMETHOD",
  "BINORMALTANGENTUVSOURCE", "NBTMETHOD_NONE", "NBTMETHOD_NI",
  "NBTMETHOD_MAX", "NBTMETHOD_ATI", "USERDEFINEDDATA", "IMPLEMENTATION",
  "CLASSNAME", "PASS", "STAGE", "TSS_TEXTURE", "TSS_COLOROP",
  "TSS_COLORARG0", "TSS_COLORARG1", "TSS_COLORARG2", "TSS_ALPHAOP",
  "TSS_ALPHAARG0", "TSS_ALPHAARG1", "TSS_ALPHAARG2", "TSS_RESULTARG",
  "TSS_CONSTANT_DEPRECATED", "TSS_BUMPENVMAT00", "TSS_BUMPENVMAT01",
  "TSS_BUMPENVMAT10", "TSS_BUMPENVMAT11", "TSS_BUMPENVLSCALE",
  "TSS_BUMPENVLOFFSET", "TSS_TEXCOORDINDEX", "TSS_TEXTURETRANSFORMFLAGS",
  "TSS_TEXTRANSMATRIX", "TTFF_DISABLE", "TTFF_COUNT1", "TTFF_COUNT2",
  "TTFF_COUNT3", "TTFF_COUNT4", "TTFF_PROJECTED", "PROJECTED",
  "USEMAPINDEX", "INVERSE", "TRANSPOSE", "TTSRC_GLOBAL", "TTSRC_CONSTANT",
  "TT_WORLD_PARALLEL", "TT_WORLD_PERSPECTIVE", "TT_WORLD_SPHERE_MAP",
  "TT_CAMERA_SPHERE_MAP", "TT_SPECULAR_CUBE_MAP", "TT_DIFFUSE_CUBE_MAP",
  "TCI_PASSTHRU", "TCI_CAMERASPACENORMAL", "TCI_CAMERASPACEPOSITION",
  "TCI_CAMERASPACEREFLECT", "TCI_SPHEREMAP", "TOP_DISABLE",
  "TOP_SELECTARG1", "TOP_SELECTARG2", "TOP_MODULATE", "TOP_MODULATE2X",
  "TOP_MODULATE4X", "TOP_ADD", "TOP_ADDSIGNED", "TOP_ADDSIGNED2X",
  "TOP_SUBTRACT", "TOP_ADDSMOOTH", "TOP_BLENDDIFFUSEALPHA",
  "TOP_BLENDTEXTUREALPHA", "TOP_BLENDFACTORALPHA",
  "TOP_BLENDTEXTUREALPHAPM", "TOP_BLENDCURRENTALPHA", "TOP_PREMODULATE",
  "TOP_MODULATEALPHA_ADDCOLOR", "TOP_MODULATECOLOR_ADDALPHA",
  "TOP_MODULATEINVALPHA_ADDCOLOR", "TOP_MODULATEINVCOLOR_ADDALPHA",
  "TOP_BUMPENVMAP", "TOP_BUMPENVMAPLUMINANCE", "TOP_DOTPRODUCT3",
  "TOP_MULTIPLYADD", "TOP_LERP", "TA_CURRENT", "TA_DIFFUSE",
  "TA_SELECTMASK", "TA_SPECULAR", "TA_TEMP", "TA_TEXTURE", "TA_TFACTOR",
  "TA_ALPHAREPLICATE", "TA_COMPLEMENT", "NTM_BASE", "NTM_DARK",
  "NTM_DETAIL", "NTM_GLOSS", "NTM_GLOW", "NTM_BUMP", "NTM_NORMAL",
  "NTM_PARALLAX", "NTM_DECAL", "SAMPLER", "TSAMP_ADDRESSU",
  "TSAMP_ADDRESSV", "TSAMP_ADDRESSW", "TSAMP_BORDERCOLOR",
  "TSAMP_MAGFILTER", "TSAMP_MINFILTER", "TSAMP_MIPFILTER",
  "TSAMP_MIPMAPLODBIAS", "TSAMP_MAXMIPLEVEL", "TSAMP_MAXANISOTROPY",
  "TSAMP_SRGBTEXTURE", "TSAMP_ELEMENTINDEX", "TSAMP_DMAPOFFSET",
  "TSAMP_ALPHAKILL_DEPRECATED", "TSAMP_COLORKEYOP_DEPRECATED",
  "TSAMP_COLORSIGN_DEPRECATED", "TSAMP_COLORKEYCOLOR_DEPRECATED",
  "TADDR_WRAP", "TADDR_MIRROR", "TADDR_CLAMP", "TADDR_BORDER",
  "TADDR_MIRRORONCE", "TADDR_CLAMPTOEDGE_DEPRECATED", "TEXF_NONE",
  "TEXF_POINT", "TEXF_LINEAR", "TEXF_ANISOTROPIC", "TEXF_PYRAMIDALQUAD",
  "TEXF_GAUSSIANQUAD", "TEXF_FLATCUBIC_DEPRECATED",
  "TEXF_GAUSSIANCUBIC_DEPRECATED", "TEXF_QUINCUNX_DEPRECATED",
  "TEXF_MAX_DEPRECATED", "TAK_DISABLE_DEPRECATED", "TAK_ENABLE_DEPRECATED",
  "TCKOP_DISABLE_DEPRECATED", "TCKOP_ALPHA_DEPRECATED",
  "TCKOP_RGBA_DEPRECATED", "TCKOP_KILL_DEPRECATED", "TEXTURE",
  "TEXTURE_SOURCE", "OBJECTS", "EFFECT_GENERALLIGHT", "EFFECT_POINTLIGHT",
  "EFFECT_DIRECTIONALLIGHT", "EFFECT_SPOTLIGHT", "EFFECT_SHADOWPOINTLIGHT",
  "EFFECT_SHADOWDIRECTIONALLIGHT", "EFFECT_SHADOWSPOTLIGHT", "EFFECT",
  "EFFECT_ENVIRONMENTMAP", "EFFECT_PROJECTEDSHADOWMAP",
  "EFFECT_PROJECTEDLIGHTMAP", "EFFECT_FOGMAP", "USEMAPVALUE", "CMOBJECT",
  "'\\n'", "'0'", "$accept", "shader_file", "shader", "@1", "@2",
  "nsfshader_components_optional", "nsfshader_components_list",
  "nsfshader_component", "optional_string", "optional_multi_string",
  "optional_description", "optional_filename", "string_or_quote",
  "attribute_name", "range_color_optional", "range_point4_optional",
  "range_point3_optional", "range_point2_optional", "range_float_optional",
  "range_int_optional", "float_values_arbitrary_list", "float_values_1",
  "float_values_16", "attribute_list_with_brackets", "@3",
  "global_attribute_list_with_brackets", "@4", "optional_multiplicity",
  "attribute_list", "attribute_value", "artist_conditional",
  "attribute_bool", "attribute_uint", "@5", "attribute_float", "@6",
  "attribute_point2", "@7", "attribute_point3", "@8", "attribute_point4",
  "@9", "attribute_matrix3", "@10", "attribute_transform", "@11",
  "attribute_color", "@12", "attribute_texture",
  "object_list_with_brackets", "@13", "object_list", "object_value",
  "object_effect_general_light", "object_effect_point_light",
  "object_effect_directional_light", "object_effect_spot_light",
  "object_effect_shadow_point_light",
  "object_effect_shadow_directional_light",
  "object_effect_shadow_spot_light", "object_effect_environment_map",
  "object_effect_projected_shadow_map",
  "object_effect_projected_light_map", "object_effect_fog_map",
  "packing_definition_definition", "@14", "packing_definition_declaration",
  "packing_definition_type", "packing_definition_parameter",
  "packing_definition_tesselator", "packing_definition_usage",
  "packing_definition_entries", "packing_definition_entry",
  "renderstate_list_with_brackets", "@15", "renderstate_list",
  "renderstate_entry_save_optional", "renderstate_entry_save",
  "renderstate_entry", "renderstate_attribute", "renderstate_string",
  "renderstate_bool", "renderstate_float", "renderstate_hex",
  "constantmap_list", "constantmap_entry", "constantmap_platform_block",
  "@16", "@17", "constantmap_entry_defined", "constantmap_entry_object",
  "constantmap_entry_attribute", "@18", "constantmap_entry_constant",
  "@19", "constantmap_entry_global", "@20", "constantmap_entry_operator",
  "operator_type", "operator_optional_transpose",
  "operator_optional_inverse", "vs_constantmap_with_brackets", "@21",
  "gs_constantmap_with_brackets", "@22", "ps_constantmap_with_brackets",
  "@23", "userdefined_classname", "vertexshader_program_name",
  "vertexshader_program", "vertexshader_program_name_only",
  "vertexshader_program_bracket_contents_list",
  "vertexshader_program_bracket_content", "vertexshader_program_bracket",
  "@24", "@25", "vertexshader_program_entry",
  "geometryshader_program_name", "geometryshader_program",
  "geometryshader_program_name_only",
  "geometryshader_program_bracket_contents_list",
  "geometryshader_program_bracket_content",
  "geometryshader_program_bracket", "@26", "@27",
  "geometryshader_program_entry", "pixelshader_program_name",
  "pixelshader_program", "pixelshader_program_name_only",
  "pixelshader_program_bracket_contents_list",
  "pixelshader_program_bracket_content", "pixelshader_program_bracket",
  "@28", "@29", "pixelshader_program_entry",
  "requirement_list_with_brackets", "@30", "requirement_list",
  "requirement_entry", "requirement_vsversion", "requirement_gsversion",
  "requirement_psversion", "requirement_userdefined",
  "requirement_platform", "requirement_platform_list",
  "requirement_remaining", "requirement_bonesperpartition",
  "requirement_usesbinormaltangentuvsource",
  "requirement_usesbinormaltangent", "binormaltanget_method",
  "stage_or_sampler_or_texture", "stage", "@31",
  "stage_entry_list_optional", "stage_entry_list",
  "stage_entry_or_texture", "stage_entry_save_optional",
  "stage_entry_save", "stage_entry", "stage_texture",
  "stage_texture_map_ndl", "stage_texture_map_ndl_decal", "stage_color_op",
  "stage_color_arg0", "stage_color_arg1", "stage_color_arg2",
  "stage_alpha_op", "stage_alpha_arg0", "stage_alpha_arg1",
  "stage_alpha_arg2", "stage_result_arg", "stage_constant_deprecated",
  "stage_bumpenvmat00", "stage_bumpenvmat01", "stage_bumpenvmat10",
  "stage_bumpenvmat11", "stage_bumpenvlscale", "stage_bumpenvloffset",
  "stage_texcoordindex", "stage_textransflags",
  "stage_texturetransformflags_count",
  "stage_texturetransformflags_optional_projection",
  "stage_texture_operation", "stage_texture_argument",
  "stage_texture_argument_modifiers_optional",
  "stage_texture_argument_modifier", "stage_texcoordindex_flags",
  "stage_textransmatrix", "stage_textransmatrix_assignment", "@32",
  "stage_textransmatrix_option", "sampler", "@33", "@34",
  "sampler_entry_list_optional", "sampler_entry_list",
  "sampler_entry_save_optional", "sampler_entry_save", "sampler_entry",
  "sampler_addressu", "sampler_addressv", "sampler_addressw",
  "sampler_bordercolor", "sampler_magfilter", "sampler_minfilter",
  "sampler_mipfilter", "sampler_mipmaplodbias", "sampler_maxmiplevel",
  "sampler_maxanisotropy", "sampler_srgbtexture", "sampler_elementindex",
  "sampler_dmapoffset", "sampler_alphakill_deprecated",
  "sampler_colorkeyop_deprecated", "sampler_colorsign_deprecated",
  "sampler_colorkeycolor_deprecated", "sampler_texture_address",
  "sampler_texture_filter", "sampler_texture_alphakill",
  "sampler_texture_colorkeyop", "texture", "@35", "@36",
  "texture_entry_list_optional", "texture_entry_list", "texture_entry",
  "texture_source", "pass_component_list", "pass_component", "pass", "@37",
  "implementation_component_list_optional",
  "implementation_component_list", "implementation_component",
  "implementation", "@38", "@39", "userdefineddata",
  "userdefineddata_list", "userdefineddata_block", "@40", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,    10,
      48
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,   301,   302,   302,   304,   305,   303,   303,   306,   306,
     307,   307,   308,   308,   308,   308,   308,   308,   309,   309,
     310,   310,   311,   311,   312,   312,   312,   313,   313,   314,
     315,   315,   315,   316,   316,   317,   317,   318,   318,   319,
     319,   320,   320,   320,   321,   321,   322,   323,   325,   324,
     327,   326,   328,   328,   329,   329,   330,   330,   330,   330,
     330,   330,   330,   330,   330,   330,   331,   331,   332,   334,
     333,   336,   335,   338,   337,   340,   339,   342,   341,   344,
     343,   346,   345,   348,   347,   349,   349,   351,   350,   352,
     352,   353,   353,   353,   353,   353,   353,   353,   353,   353,
     353,   353,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   364,   366,   365,   367,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   369,   369,   369,   369,   369,
     369,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,   369,   369,   370,   370,   370,   370,   370,   370,
     370,   371,   371,   371,   371,   371,   371,   371,   371,   371,
     371,   371,   371,   371,   371,   372,   372,   372,   373,   373,
     373,   373,   373,   373,   375,   374,   376,   376,   377,   377,
     378,   379,   379,   379,   379,   379,   379,   380,   381,   382,
     383,   384,   384,   385,   385,   386,   386,   386,   386,   386,
     386,   386,   388,   389,   387,   390,   390,   390,   390,   390,
     390,   390,   390,   390,   390,   391,   391,   393,   392,   395,
     394,   397,   396,   398,   399,   399,   399,   399,   399,   400,
     400,   401,   401,   403,   402,   405,   404,   407,   406,   408,
     409,   409,   409,   410,   410,   410,   411,   411,   411,   412,
     412,   413,   413,   413,   413,   415,   416,   414,   417,   417,
     418,   418,   418,   419,   419,   419,   420,   420,   420,   421,
     421,   422,   422,   422,   424,   425,   423,   426,   426,   427,
     427,   427,   428,   428,   428,   429,   429,   429,   430,   430,
     431,   431,   431,   433,   434,   432,   435,   435,   437,   436,
     438,   438,   439,   439,   439,   439,   439,   439,   439,   439,
     439,   439,   440,   441,   442,   443,   444,   445,   445,   445,
     445,   446,   447,   448,   449,   450,   450,   450,   450,   450,
     451,   451,   451,   453,   452,   454,   454,   455,   455,   456,
     456,   456,   457,   457,   458,   459,   459,   459,   459,   459,
     459,   459,   459,   459,   459,   459,   459,   459,   459,   459,
     459,   459,   459,   459,   460,   460,   460,   460,   461,   461,
     461,   461,   461,   461,   461,   461,   462,   463,   464,   465,
     466,   467,   468,   469,   470,   471,   472,   473,   473,   474,
     474,   475,   475,   476,   476,   477,   477,   478,   478,   479,
     479,   480,   480,   480,   481,   481,   481,   481,   482,   482,
     483,   483,   483,   483,   483,   483,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   483,   484,   484,   484,
     484,   484,   484,   484,   485,   485,   486,   486,   486,   486,
     487,   487,   487,   487,   487,   487,   487,   488,   489,   490,
     489,   491,   491,   491,   491,   491,   491,   491,   491,   493,
     492,   494,   492,   495,   495,   496,   496,   497,   497,   497,
     498,   499,   499,   499,   499,   499,   499,   499,   499,   499,
     499,   499,   499,   499,   499,   499,   499,   499,   499,   500,
     501,   502,   503,   503,   504,   505,   506,   507,   507,   508,
     508,   509,   509,   510,   510,   511,   511,   512,   512,   513,
     513,   514,   515,   515,   516,   516,   517,   517,   517,   517,
     517,   517,   517,   517,   518,   518,   518,   518,   518,   518,
     518,   518,   518,   518,   518,   518,   519,   519,   519,   520,
     520,   520,   520,   520,   522,   521,   523,   521,   524,   524,
     525,   525,   526,   526,   527,   527,   527,   527,   528,   528,
     529,   529,   529,   529,   529,   529,   529,   529,   529,   531,
     530,   532,   532,   533,   533,   534,   534,   534,   534,   534,
     534,   534,   534,   534,   536,   537,   535,   538,   538,   538,
     538,   539,   539,   541,   540
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     0,     0,     8,     2,     0,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     2,     0,     1,     0,     1,     1,     1,     1,     3,
       0,    20,    16,     0,    20,     0,    16,     0,    12,     0,
       4,     0,     4,     4,     1,     3,     1,    31,     0,     5,
       0,     5,     0,     3,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     0,
       6,     0,     7,     0,     7,     0,     7,     0,     7,     0,
       6,     0,     6,     0,     6,     5,     4,     0,     5,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     0,     6,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     2,     2,
       7,     4,     6,     3,     0,     5,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     2,     3,     3,     3,
       3,     3,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     0,     8,     5,     5,     5,     5,     5,
       5,     5,     5,     4,     4,     4,     4,     0,     6,     0,
       6,     0,     5,     9,     1,     1,     1,     1,     2,     0,
       1,     0,     1,     0,     5,     0,     5,     0,     5,     3,
       2,     2,     2,     3,     2,     1,     3,     3,     3,     1,
       2,     1,     3,     3,     3,     0,     0,     9,     1,     1,
       2,     2,     2,     3,     2,     1,     3,     3,     3,     1,
       2,     1,     3,     3,     0,     0,     9,     1,     1,     2,
       2,     2,     3,     2,     1,     3,     3,     3,     1,     2,
       1,     3,     3,     0,     0,     9,     1,     1,     0,     5,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     3,     3,     3,     3,     7,     5,     3,
       1,     3,     3,     3,     3,     1,     1,     1,     1,     2,
       1,     1,     1,     0,     7,     0,     1,     2,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     4,
       4,     4,     3,     3,     1,     1,     1,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     0,     1,     2,     2,     1,     1,
       0,     1,     1,     1,     1,     1,     2,     3,     3,     0,
       4,     0,     1,     1,     1,     1,     1,     1,     2,     0,
       7,     0,     6,     0,     1,     2,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     2,     1,
       1,     1,     1,     2,     0,     7,     0,     6,     0,     1,
       2,     1,     1,     2,     3,     3,     3,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       6,     0,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     0,     8,     3,     3,     3,
       3,     1,     2,     0,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     3,     7,     0,     1,     2,     4,
      22,    20,    23,     5,    21,     8,     0,     0,     0,     0,
       0,     0,     0,     9,    11,    12,    14,    13,    15,    16,
      17,    48,    50,     0,     0,     0,    87,     6,    10,     0,
       0,   113,   623,   614,     0,     0,    69,    71,    73,    75,
      77,    79,    81,    83,     0,     0,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,     0,     0,     0,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    49,    54,    51,     0,     0,
       0,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
       0,     0,     0,   186,    27,    28,     0,   621,     0,   615,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    88,    89,    66,    67,     0,     0,    52,    52,    52,
      52,    52,    52,     0,    24,   187,   188,   189,     0,     0,
     114,   185,     0,   622,   624,   601,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,    68,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,    24,    25,
      86,     0,     0,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     193,    46,   617,   618,   620,   619,    44,     0,     0,     0,
       0,     0,     0,     0,    18,   606,   605,   609,   610,   611,
     612,   608,   607,     0,   602,   604,   613,    41,     0,     0,
       0,     0,     0,     0,     0,    30,    85,   191,   144,   164,
     165,   166,   167,   168,   169,   170,     0,     0,   115,   194,
     253,   255,   257,   318,     0,    19,     0,   616,   603,     0,
      70,    53,    39,    37,    35,    33,    80,    82,     0,    84,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,     0,    45,     0,     0,     0,
       0,     0,   259,   599,     0,     0,     0,    72,     0,    74,
       0,    76,     0,    78,     0,     0,   192,     0,     0,     0,
     197,   198,   199,   204,   205,   201,   203,   202,   222,     0,
     237,   239,   241,     0,     0,     0,   214,   221,   215,   216,
     217,   218,   219,   220,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   321,   322,   323,
     324,   325,   326,   330,   327,   329,   328,     0,     0,     0,
       0,     0,     0,     0,     0,   190,   206,     0,   195,   196,
     200,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   254,   213,   256,   258,   331,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   319,   320,     0,     0,
       0,     0,    18,    18,   590,   592,   594,   596,   265,   279,
     278,   591,   285,   298,   297,   593,   304,   317,   316,   595,
     597,   350,   351,   352,     0,   589,   598,    43,    42,     0,
       0,     0,     0,     0,     0,   211,   210,   212,   208,   209,
     207,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   341,   332,   333,
     334,   335,   340,   336,   342,     0,   345,   346,   348,   347,
     344,   343,   260,   275,   261,   262,   280,   294,   281,   282,
     299,   313,   300,   301,    18,    18,     0,    18,     0,   264,
     284,   303,   600,   588,    40,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   233,
     234,     0,     0,     0,     0,   235,   236,     0,   349,     0,
       0,     0,     0,     0,   491,     0,   576,   263,   283,   302,
       0,     0,     0,     0,    29,   223,   225,   226,   227,   228,
     229,   230,   231,   232,     0,     0,   242,     0,   339,     0,
       0,     0,   353,   489,     0,   574,     0,     0,     0,     0,
       0,     0,   238,   240,     0,   244,   245,   246,   247,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   499,     0,     0,   496,   497,
     498,   501,   502,   503,   504,   505,   506,   507,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,     0,     0,
       0,     0,     0,   581,   582,     0,     0,     0,     0,     0,
       0,   248,   251,   338,   276,   295,   314,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   358,
     361,   362,   363,   359,   365,   366,   367,   368,   369,   370,
     371,   372,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   360,     0,   518,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   492,   495,   500,     0,   583,     0,   577,
     580,     0,     0,     0,     0,     0,   224,   252,   249,     0,
       0,     0,     0,   383,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   354,   357,   364,   490,   387,   388,   389,
     390,   391,   392,   393,   394,   395,     0,   386,   384,   385,
       0,   546,   547,   548,   549,   550,   551,   552,   519,   520,
     521,   522,   523,     0,   554,   555,   556,   557,   558,   559,
     561,   562,   560,   563,   564,   524,   525,   526,   527,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
       0,   566,   567,   540,   539,     0,   569,   570,   571,   572,
     541,   542,   543,   544,   545,   575,   587,   586,   584,   585,
       0,     0,     0,     0,     0,   250,   243,   337,     0,     0,
       0,     0,   271,     0,   269,     0,     0,     0,   291,     0,
     289,     0,     0,     0,   310,     0,   308,     0,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   397,   464,   464,   464,   464,   464,
     464,   464,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   418,     0,   471,   472,   473,   474,   475,     0,
     423,   424,   425,   426,   427,   422,   428,     0,   479,   477,
     396,   553,   565,   568,   573,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   277,   270,     0,     0,     0,   296,
     290,     0,     0,     0,   315,   309,   456,   468,   469,   457,
     465,   458,   459,   460,   461,   462,   463,   476,   419,   420,
     429,   421,     0,   482,   483,   484,   485,   486,   487,     0,
       0,     0,     0,     0,     0,     0,   266,   267,   268,   272,
     273,   274,   286,   287,   288,   292,   293,   305,   306,   307,
     311,   312,   466,   467,   488,   478,     0,    38,     0,     0,
       0,     0,     0,   480,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    32,     0,     0,    36,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    34,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    47
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     3,     4,    10,    15,    22,    23,    24,   286,    12,
      13,   200,   136,   470,   299,   333,   331,   329,   327,   290,
     235,   236,  1033,    25,    39,    26,    40,   190,    55,    56,
     155,    57,    58,    96,    59,    97,    60,    98,    61,    99,
      62,   100,    63,   101,    64,   102,    65,   103,    66,    27,
      44,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    28,    68,   245,   230,   131,   276,
     315,   132,   133,   246,   317,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   355,   356,   357,   401,   591,   358,
     359,   360,   407,   361,   408,   362,   409,   363,   599,   856,
     748,   247,   318,   248,   319,   249,   320,   250,   438,   439,
     862,   863,   864,   440,   549,   750,   441,   442,   443,   868,
     869,   870,   444,   550,   751,   445,   446,   447,   874,   875,
     876,   448,   551,   752,   449,   251,   321,   376,   377,   378,
     379,   380,   381,   382,   493,   383,   384,   385,   386,   500,
     450,   451,   604,   687,   688,   689,   690,   691,   692,   625,
     788,   789,   694,   695,   696,   697,   698,   699,   700,   701,
     702,   703,   704,   705,   706,   707,   708,   709,   710,   711,
     946,   991,   904,   912,   979,   980,   939,   712,   949,  1000,
     999,   452,   605,   584,   626,   627,   628,   629,   630,   631,
     632,   633,   634,   635,   636,   637,   638,   639,   640,   641,
     642,   643,   644,   645,   646,   647,   798,   815,   834,   840,
     453,   648,   586,   651,   652,   653,   654,   454,   455,   252,
     387,   253,   254,   255,    29,    70,   175,   137,   138,    30,
      69
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -582
static const short int yypact[] =
{
      32,  -257,    22,    49,  -582,  -582,    57,  -582,  -582,  -582,
      46,  -582,    68,  -582,  -582,    37,    97,   110,   161,   176,
     180,   229,   258,    37,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,   292,   317,   373,  -582,  -582,  -582,   545,
     545,  -582,  -582,  -582,   247,   276,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,   388,   407,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,   462,   428,   132,
      46,   293,   417,   443,   475,   488,   516,   558,   721,   722,
     723,   724,    -1,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,   209,   725,   726,   727,   728,
     729,   730,   731,   732,   209,  -582,  -582,  -582,   449,   734,
     733,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
     735,   737,   648,  -582,  -582,  -582,   747,   132,   756,  -582,
     739,   740,   741,   742,   743,   744,   745,   746,   748,   749,
     750,  -582,  -582,  -582,  -582,   751,   209,   762,   762,   762,
     762,   762,   762,   209,    69,  -582,  -582,  -582,   752,   551,
    -582,  -582,   392,  -582,  -582,   -39,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,   753,   755,
     209,   209,   209,   209,   209,   209,   757,  -582,    54,  -582,
    -582,   551,   481,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
     559,  -582,  -582,  -582,  -582,  -582,   766,   758,   778,   779,
     780,   781,   782,   774,   765,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,   784,   -39,  -582,  -582,   783,   785,   757,
     757,   757,   757,   757,   757,   787,  -582,   559,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,   620,   757,  -582,  -582,
    -582,  -582,  -582,  -582,   132,  -582,   786,  -582,  -582,    34,
    -582,  -582,   788,   789,   790,   791,  -582,  -582,   794,  -582,
     620,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,   770,  -582,    52,    19,    19,
      19,   406,  -582,  -582,   772,   792,   795,  -582,   796,  -582,
     797,  -582,   798,  -582,   799,   793,  -582,   503,   800,    35,
    -582,  -582,   775,  -582,  -582,  -582,  -582,  -582,  -582,    27,
    -582,  -582,  -582,   801,   802,    -2,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,     3,     9,   508,   804,   805,   806,
     808,   809,   810,   813,   814,   815,   102,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,   311,   803,   820,
     811,   812,   816,   817,   821,  -582,  -582,   432,  -582,  -582,
    -582,   666,   295,   439,   485,   531,   646,   132,   132,   132,
     818,   807,  -582,  -582,  -582,  -582,  -582,   819,   822,   823,
     824,   825,   829,   831,     5,   832,  -582,  -582,    50,   168,
     193,   833,   242,   341,  -582,  -582,  -582,  -582,   834,  -582,
    -582,  -582,   835,  -582,  -582,  -582,   836,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,    41,  -582,  -582,  -582,  -582,   827,
     828,   842,   846,   826,   838,  -582,  -582,  -582,  -582,  -582,
    -582,   830,   840,   841,   843,   844,   845,   847,   848,   849,
     850,   851,   852,   853,   854,   855,   386,  -582,  -582,  -582,
    -582,  -582,   856,  -582,  -582,   510,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,   765,   765,   837,   765,   860,   857,
     858,   859,  -582,  -582,  -582,   862,   863,   864,   865,   867,
     829,   866,   868,   869,   870,   871,   872,   873,   874,  -582,
    -582,   875,   876,   877,   878,  -582,  -582,   879,  -582,   668,
     669,   670,   882,   883,  -582,   885,  -582,  -582,  -582,  -582,
     839,   886,   889,   884,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,   887,   757,  -582,   231,   892,   893,
     894,   895,  -582,  -582,    81,  -582,    16,   904,   890,   891,
     140,    19,  -582,  -582,   533,  -582,  -582,  -582,  -582,   896,
     897,   829,   829,   829,   357,    81,   536,   900,   901,   902,
     903,   905,   908,   909,   910,   911,   912,   913,   914,   915,
     916,   917,   918,   919,   920,  -582,   929,    98,  -582,  -582,
     906,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,    16,   541,
     922,   932,    20,  -582,  -582,   921,   931,   925,   934,   923,
      14,  -582,   663,   933,  -582,  -582,  -582,   619,   928,   930,
     935,   936,   937,   938,   939,   940,   941,   942,   943,   944,
     945,   946,   947,   948,   949,   950,   951,   960,   405,  -582,
    -582,  -582,   952,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,   961,  -582,    21,    17,    17,    17,   420,
       1,     1,     1,   163,   173,   199,   210,   212,   214,    28,
      30,    13,   446,  -582,  -582,  -582,   962,  -582,   135,  -582,
    -582,   954,   963,   926,   953,   964,  -582,  -582,   763,   955,
     230,   544,   563,  -582,   182,   513,   513,   513,   182,   513,
     513,   513,   513,   956,    74,    78,    84,   241,   355,   397,
      12,   294,   274,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,   957,  -582,  -582,  -582,
     672,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,   674,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
     675,  -582,  -582,  -582,  -582,   677,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
     959,   965,   970,   967,   977,  -582,  -582,  -582,   971,   972,
     974,   975,  -582,   984,   230,   978,   979,   980,  -582,   985,
     544,   981,   982,   983,  -582,   992,   563,   700,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,   201,   201,   201,   201,   201,
     201,   201,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,   701,  -582,  -582,  -582,  -582,  -582,    -7,
    -582,  -582,  -582,  -582,  -582,  -582,   861,    24,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,   993,   987,   996,   986,   988,
     146,   989,   990,   991,  -582,  -582,   178,   994,   995,  -582,
    -582,   195,   997,   998,  -582,  -582,  -582,   773,   776,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,   705,  -582,  -582,  -582,  -582,  -582,  -582,   999,
      39,  1001,  1002,  1003,  1000,  1005,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  1004,  -582,  1012,  1013,
    1007,  1008,  1016,  -582,  1010,  1011,  1026,  1020,  1014,  1029,
    1023,  1006,  1017,  1025,  1032,  1021,  -582,  1030,  1022,  -582,
    1031,  1027,  1033,  1028,  1039,  1034,  1043,  1042,  1038,  1044,
    -582,  1035,  -582,  1040,  1036,  1045,  1037,  1046,  1041,  1047,
    1048,  1049,  1050,  1051,  1052,  1053,  1054,  1056,  1055,  1058,
    1057,  1060,  1059,  1063,  1061,  -582
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -582,  -582,  1009,  -582,  -582,  -582,  -582,  1062,  -405,  -582,
    1018,   888,    18,   -47,  -582,  -582,  -582,  -582,  -582,  -582,
    -196,  -582,  -582,  -582,  -582,  -582,  -582,   528,   966,   134,
     -14,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,   927,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,   881,  -582,   880,
     767,  -582,   958,  -357,  -582,  -582,   754,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -276,  -354,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -353,  -582,  -349,  -582,  -307,  -582,  -582,  -582,  -582,
    -582,   205,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
     217,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,   189,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,   704,  -582,
    -582,  -582,  -582,  -582,  -458,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,   396,  -582,  -582,  -582,  -581,
     351,   353,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,   334,  -165,  -244,  -582,  -582,  -582,  -582,  -582,
      94,  -582,  -582,  -582,   490,  -582,   469,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -159,   -38,  -582,  -582,
    -582,  -582,  -582,   450,  -582,   445,  -582,  -582,   645,  -582,
    -582,  -582,  -582,   898,  -582,  -582,  -582,  -582,   968,  -163,
    -582
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -580
static const short int yytable[] =
{
     265,   413,   803,   348,   412,   151,   495,   237,   348,   414,
     413,   413,   256,   933,   348,   415,   988,   649,   790,   348,
     746,   649,  -578,   693,   348,   992,  -579,   516,   518,   830,
     434,   835,   464,     1,   435,  -470,   337,   841,   436,   464,
     992,   398,     5,   364,   365,   777,     6,   522,  -481,     7,
       1,   134,   135,   337,   502,   503,   324,   325,   197,   338,
       2,  -481,     9,   292,   293,   294,   295,   296,   297,    16,
      17,    11,   565,   197,   504,   505,   338,     2,   238,   199,
     437,   316,   606,    18,   239,   464,   240,  -493,   241,   464,
     164,   256,   198,    14,   199,   464,   921,   434,   242,   606,
     923,   435,    31,   366,  -494,   436,   925,   693,   426,   552,
     553,    19,   555,   243,   244,    32,   349,   350,   351,   352,
     353,   349,   350,   351,   352,   353,   367,   349,   350,   351,
     352,   353,   349,   350,   351,   352,   353,   349,   350,   351,
     352,   353,   188,   664,   665,   666,   464,   437,   658,   196,
    1006,   496,   497,   498,   499,   659,   134,   135,   238,   846,
     402,   403,   404,   405,   239,   428,   240,   429,   241,   430,
    1007,  1008,   506,   507,   464,   989,   259,   260,   261,   262,
     263,   264,  1012,   877,   464,    33,   818,    19,    20,   106,
     234,    19,   508,   509,  -470,   431,   820,   510,   511,  1017,
      34,   106,  1013,  1014,    35,   934,   935,   936,   937,   938,
     464,   993,   994,   995,   996,   997,   998,   512,   513,  1018,
    1019,   464,   822,   464,   456,   464,   993,   994,   995,   996,
     997,   998,   594,   824,    36,   826,   607,   828,   153,   154,
     368,   369,   370,   371,   372,   373,   374,   375,   595,   596,
     597,   598,   464,   607,   778,   779,   780,   781,   782,   783,
     784,   785,   786,   927,    37,   515,   285,   804,   805,   806,
     807,   808,   809,   810,   811,   812,   813,   791,   792,   793,
     794,   795,   796,   432,    71,    72,    73,    74,    75,    76,
      77,   456,    78,    79,    80,    81,   354,    41,   814,   650,
      95,   354,   322,   650,   831,   832,   413,   354,   836,   837,
     838,   839,   354,   842,   797,   660,   140,   354,   472,   134,
     135,    21,    42,   433,   608,   609,   610,   611,   612,   613,
     614,   615,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   608,   609,   610,   611,   612,   613,   614,   615,   616,
     617,   618,   619,   620,   621,   622,   623,   624,   667,   858,
     859,   860,   861,  -355,   517,   285,   464,   406,   778,   779,
     780,   781,   782,   783,   784,   785,   786,   929,    43,   593,
     878,   879,   880,   881,   882,   883,   884,   885,   886,   887,
     888,   889,   890,   891,   892,   893,   894,   895,   896,   897,
     898,   899,   900,   901,   902,   903,   667,   366,   464,   545,
     546,  -356,   104,   105,   231,   232,   134,   135,   233,   931,
     473,   475,   477,   479,   481,   482,   483,   484,   238,   108,
     367,   464,   977,   978,   239,   428,   240,   429,   241,   430,
     141,   801,    45,   464,    46,    47,    48,    49,    50,    51,
      52,    53,    54,   465,   466,   467,   468,   464,   469,   947,
     948,    19,   474,   134,   135,   431,   142,   843,   107,   940,
     941,   942,   943,   944,   945,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,    45,   143,    46,
      47,    48,    49,    50,    51,    52,    53,    54,   476,   134,
     135,   144,   607,   668,   669,   670,   671,   672,   673,   674,
     675,   676,   677,   678,   679,   680,   681,   682,   683,   684,
     685,   686,    71,    72,    73,    74,    75,    76,    77,   145,
      78,    79,    80,    81,   368,   369,   370,   371,   372,   373,
     374,   375,   202,   432,   478,   134,   135,  -116,   799,   800,
     607,   668,   669,   670,   671,   672,   673,   674,   675,   676,
     677,   678,   679,   680,   681,   682,   683,   684,   685,   686,
      45,   146,    46,    47,    48,    49,    50,    51,    52,    53,
      54,   913,   914,   433,   916,   917,   918,   919,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,   170,   269,   270,   271,   272,   273,
     274,   275,   981,   982,   983,   984,   985,   986,   787,   480,
     134,   135,   802,   865,   866,   867,   819,   821,   823,   825,
     827,   829,   833,   816,   817,   844,   191,   192,   193,   194,
     195,   847,   871,   872,   873,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   922,   924,   926,
     928,   930,   932,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   905,   906,   907,
     908,   909,   910,   911,   147,   148,   149,   150,   165,   156,
     157,   158,   159,   160,   161,   162,   163,   166,   168,   167,
     169,   172,   174,   176,   177,   178,   179,   180,   181,   182,
     183,   189,   184,   185,   186,   201,   257,   187,   258,   231,
     268,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     287,   323,   289,   336,   388,   291,   298,   326,   328,   330,
     332,   334,   396,   391,   392,   393,   400,   416,   471,   548,
     579,   580,   581,   457,   397,   389,   395,   390,   417,   418,
     419,   394,   420,   421,   422,   410,   411,   423,   424,   425,
     458,   486,   661,   459,   460,   714,   463,   524,   461,   462,
     737,   485,   554,   525,   530,   487,   747,   587,   528,   488,
     489,   490,   491,   492,   494,   501,   514,   526,   519,   520,
     521,   527,   529,   531,   532,   556,   533,   534,   535,   547,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   564,
     563,   557,   558,   559,   560,   561,   562,   582,   583,   566,
     585,   567,   568,   569,   570,   571,   572,   573,   574,   575,
     576,   588,   577,   578,   589,   600,   590,   601,   602,   603,
     592,   655,   656,   657,   715,   716,   717,   718,   753,   719,
     662,   663,   720,   721,   722,   723,   724,   725,   726,   727,
     728,   729,   730,   731,   732,   733,   738,   735,   739,   742,
     743,   744,   754,   741,   755,   745,   749,   855,   852,   756,
     757,   758,   759,   760,   761,   762,   763,   764,   765,   766,
     767,   768,   769,   770,   771,   772,   773,   776,   845,   850,
     851,   951,   854,   952,   953,   853,   954,   920,   957,   857,
     950,   955,   958,   775,   959,   960,   961,   956,   962,   963,
     964,   969,   966,   967,   968,   971,   972,   973,   974,   976,
     987,  1001,  1002,  1003,  1024,  1022,    67,  1023,  1004,   152,
    1005,  1027,     8,  1009,  1010,  1030,  1046,  1011,  1015,  1016,
    1031,  1020,  1021,  1025,  1028,  1029,  1032,  1034,  1035,  1036,
    1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,  1047,
    1048,   990,  1049,  1050,  1052,  1051,  1053,  1057,  1055,  1054,
    1056,  1059,  1060,  1061,  1062,  1064,  1058,  1063,  1065,  1067,
    1066,  1068,  1070,  1069,  1072,   975,  1074,   335,  1076,   965,
    1071,  1078,  1073,  1080,  1075,  1082,  1077,  1079,  1084,  1081,
     427,  1083,   267,  1085,   774,    38,   266,   970,   139,   848,
     171,   849,   915,   399,  1026,   713,   734,   740,   736,   523,
       0,     0,     0,     0,     0,   173,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   300,     0,     0,
       0,     0,   288
};

static const short int yycheck[] =
{
     196,   355,     1,     5,     6,     6,     1,    46,     5,     6,
     364,   365,   175,     1,     5,     6,    23,     1,     1,     5,
       6,     1,     6,   604,     5,     1,     6,   432,   433,     1,
     387,     1,    11,     1,   387,    23,     1,    24,   387,    11,
       1,     6,   299,   319,   320,    24,    24,     6,    24,     0,
       1,    24,    25,     1,     4,     5,    22,    23,     4,    24,
      28,    22,     5,   259,   260,   261,   262,   263,   264,    32,
      33,    25,   530,     4,    24,    25,    24,    28,   117,    25,
     387,   277,     1,    46,   123,    11,   125,     6,   127,    11,
     104,   254,    23,    25,    25,    11,    22,   454,   137,     1,
      22,   454,     5,     1,     6,   454,    22,   688,     6,   514,
     515,   150,   517,   152,   153,     5,   118,   119,   120,   121,
     122,   118,   119,   120,   121,   122,    24,   118,   119,   120,
     121,   122,   118,   119,   120,   121,   122,   118,   119,   120,
     121,   122,   156,   601,   602,   603,    11,   454,     8,   163,
       4,   146,   147,   148,   149,    15,    24,    25,   117,    24,
     133,   134,   135,   136,   123,   124,   125,   126,   127,   128,
      24,    25,     4,     5,    11,   182,   190,   191,   192,   193,
     194,   195,     4,     1,    11,    24,    23,   150,   151,    55,
     172,   150,    24,    25,   182,   154,    23,     4,     5,     4,
      24,    67,    24,    25,    24,   193,   194,   195,   196,   197,
      11,   187,   188,   189,   190,   191,   192,    24,    25,    24,
      25,    11,    23,    11,   387,    11,   187,   188,   189,   190,
     191,   192,     1,    23,     5,    23,   155,    23,    29,    30,
     138,   139,   140,   141,   142,   143,   144,   145,    17,    18,
      19,    20,    11,   155,   233,   234,   235,   236,   237,   238,
     239,   240,   241,    22,     6,    23,    24,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   260,   261,   262,
     263,   264,   265,   242,   285,   286,   287,   288,   289,   290,
     291,   454,   293,   294,   295,   296,   298,     5,   297,   283,
      24,   298,   284,   283,   276,   277,   660,   298,   278,   279,
     280,   281,   298,   300,   297,   591,    23,   298,    23,    24,
      25,   284,     5,   282,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,     1,   129,
     130,   131,   132,     6,    23,    24,    11,   349,   233,   234,
     235,   236,   237,   238,   239,   240,   241,    22,     5,   575,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,     1,     1,    11,    23,
      24,     6,    24,     6,    22,    23,    24,    25,    26,    22,
     402,   403,   404,   405,   406,   407,   408,   409,   117,     1,
      24,    11,   231,   232,   123,   124,   125,   126,   127,   128,
      23,    21,    35,    11,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    21,    22,    23,    24,    11,    26,   185,
     186,   150,    23,    24,    25,   154,    23,    21,     6,   175,
     176,   177,   178,   179,   180,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    35,    23,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    23,    24,
      25,    23,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   285,   286,   287,   288,   289,   290,   291,    23,
     293,   294,   295,   296,   138,   139,   140,   141,   142,   143,
     144,   145,     1,   242,    23,    24,    25,     6,   717,   718,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
      35,    23,    37,    38,    39,    40,    41,    42,    43,    44,
      45,   756,   757,   282,   759,   760,   761,   762,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,     6,    96,    97,    98,    99,   100,
     101,   102,   906,   907,   908,   909,   910,   911,   715,    23,
      24,    25,   719,   129,   130,   131,   723,   724,   725,   726,
     727,   728,   729,   721,   722,   732,   158,   159,   160,   161,
     162,   738,   129,   130,   131,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,   764,   765,   766,
     767,   768,   769,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   224,   225,   226,
     227,   228,   229,   230,    23,    23,    23,    23,   299,    24,
      24,    24,    24,    24,    24,    24,    24,    23,    23,    26,
      23,    14,     6,    24,    24,    24,    24,    24,    24,    24,
      24,     9,    24,    24,    24,    23,    23,    26,    23,    22,
     299,    15,    24,     5,     5,     5,     5,     5,    14,    24,
       6,     5,     9,    23,    22,    10,     9,     9,     9,     9,
       9,     7,   299,     7,     7,     7,    31,   299,   142,   299,
     142,   142,   142,    10,    14,    23,    23,    22,    14,    14,
      14,    22,    14,    14,    14,    24,    24,    14,    14,    14,
      10,    24,   299,    22,    22,   299,    15,    10,    22,    22,
     299,    23,     5,    15,    14,    26,   183,     8,    22,    27,
      27,    27,    27,    24,    23,    23,    23,    15,    24,    24,
      24,    15,    24,    23,    23,     5,    23,    23,    23,    13,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    12,
      15,    24,    24,    24,    22,    22,    22,     5,     5,    23,
       5,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      23,    15,    24,    24,    15,    13,    22,    14,    14,    14,
      23,     7,    22,    22,    14,    14,    14,    14,   299,    14,
      24,    24,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,     6,    14,    31,     6,     8,
      15,     7,    14,    22,    14,    22,    13,   184,    22,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,     6,     6,     6,    15,
       7,   299,     8,   299,   299,    22,   299,    21,     8,    24,
      23,    22,    15,    31,     7,    14,    14,    22,    14,    14,
       6,     6,    14,    14,    14,    14,    14,    14,     6,   299,
     299,     8,    15,     7,   299,   232,    40,   231,    22,    82,
      22,    10,     3,    24,    24,    15,    10,    26,    24,    24,
      15,    24,    24,    24,    22,    22,    22,    15,    15,    22,
      22,    15,    22,    22,     8,    15,    22,     8,    15,    22,
      15,   180,    10,    22,    22,    15,    15,     8,    15,    22,
      22,     8,    10,    15,    10,    15,    22,    22,    22,    22,
      15,    15,    15,    22,    15,   876,    15,   300,    15,   864,
      22,    15,    22,    15,    22,    15,    22,    22,    15,    22,
     376,    22,   201,    22,   688,    23,   198,   870,    70,   738,
     132,   738,   758,   339,  1000,   605,   627,   652,   648,   454,
      -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   267,    -1,    -1,
      -1,    -1,   254
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,     1,    28,   302,   303,   299,    24,     0,   303,     5,
     304,    25,   310,   311,    25,   305,    32,    33,    46,   150,
     151,   284,   306,   307,   308,   324,   326,   350,   365,   535,
     540,     5,     5,    24,    24,    24,     5,     6,   308,   325,
     327,     5,     5,     5,   351,    35,    37,    38,    39,    40,
      41,    42,    43,    44,    45,   329,   330,   332,   333,   335,
     337,   339,   341,   343,   345,   347,   349,   329,   366,   541,
     536,   285,   286,   287,   288,   289,   290,   291,   293,   294,
     295,   296,   352,   353,   354,   355,   356,   357,   358,   359,
     360,   361,   362,   363,   364,    24,   334,   336,   338,   340,
     342,   344,   346,   348,    24,     6,   330,     6,     1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,   369,   372,   373,    24,    25,   313,   538,   539,   311,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      23,     6,   353,    29,    30,   331,    24,    24,    24,    24,
      24,    24,    24,    24,   331,   299,    23,    26,    23,    23,
       6,   373,    14,   539,     6,   537,    24,    24,    24,    24,
      24,    24,    24,    24,    24,    24,    24,    26,   331,     9,
     328,   328,   328,   328,   328,   328,   331,     4,    23,    25,
     312,    23,     1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
     368,    22,    23,    26,   313,   321,   322,    46,   117,   123,
     125,   127,   137,   152,   153,   367,   374,   402,   404,   406,
     408,   436,   530,   532,   533,   534,   540,    23,    23,   331,
     331,   331,   331,   331,   331,   321,   312,   368,   299,    96,
      97,    98,    99,   100,   101,   102,   370,    15,    24,     5,
       5,     5,     5,     5,    14,    24,   309,     6,   534,     9,
     320,    10,   321,   321,   321,   321,   321,   321,     9,   315,
     370,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   371,   321,   375,   403,   405,
     407,   437,   313,     5,    22,    23,     9,   319,     9,   318,
       9,   317,     9,   316,     7,   371,    23,     1,    24,   376,
     377,   378,   379,   380,   381,   382,   383,   384,     5,   118,
     119,   120,   121,   122,   298,   385,   386,   387,   390,   391,
     392,   394,   396,   398,   385,   385,     1,    24,   138,   139,
     140,   141,   142,   143,   144,   145,   438,   439,   440,   441,
     442,   443,   444,   446,   447,   448,   449,   531,    22,    23,
      22,     7,     7,     7,    22,    23,   299,    14,     6,   377,
      31,   388,   133,   134,   135,   136,   313,   393,   395,   397,
      24,    24,     6,   386,     6,     6,   299,    14,    14,    14,
      14,    14,    14,    14,    14,    14,     6,   439,   124,   126,
     128,   154,   242,   282,   374,   402,   404,   406,   409,   410,
     414,   417,   418,   419,   423,   426,   427,   428,   432,   435,
     451,   452,   492,   521,   528,   529,   540,    10,    10,    22,
      22,    22,    22,    15,    11,    21,    22,    23,    24,    26,
     314,   142,    23,   313,    23,   313,    23,   313,    23,   313,
      23,   313,   313,   313,   313,    23,    24,    26,    27,    27,
      27,    27,    24,   445,    23,     1,   146,   147,   148,   149,
     450,    23,     4,     5,    24,    25,     4,     5,    24,    25,
       4,     5,    24,    25,    23,    23,   309,    23,   309,    24,
      24,    24,     6,   529,    10,    15,    15,    15,    22,    24,
      14,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      23,    23,    23,    23,    23,    23,    24,    13,   299,   415,
     424,   433,   309,   309,     5,   309,     5,    24,    24,    24,
      22,    22,    22,    15,    12,   445,    23,    23,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    24,    24,   142,
     142,   142,     5,     5,   494,     5,   523,     8,    15,    15,
      22,   389,    23,   321,     1,    17,    18,    19,    20,   399,
      13,    14,    14,    14,   453,   493,     1,   155,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   460,   495,   496,   497,   498,
     499,   500,   501,   502,   503,   504,   505,   506,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   522,     1,
     283,   524,   525,   526,   527,     7,    22,    22,     8,    15,
     385,   299,    24,    24,   445,   445,   445,     1,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   454,   455,   456,
     457,   458,   459,   460,   463,   464,   465,   466,   467,   468,
     469,   470,   471,   472,   473,   474,   475,   476,   477,   478,
     479,   480,   488,   495,   299,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,     6,   497,    31,   524,   299,    14,     6,
     526,    22,     8,    15,     7,    22,     6,   183,   401,    13,
     416,   425,   434,   299,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,     6,   456,    31,     6,    24,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   314,   461,   462,
       1,   260,   261,   262,   263,   264,   265,   297,   517,   517,
     517,    21,   314,     1,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   297,   518,   518,   518,    23,   314,
      23,   314,    23,   314,    23,   314,    23,   314,    23,   314,
       1,   276,   277,   314,   519,     1,   278,   279,   280,   281,
     520,    24,   300,    21,   314,     6,    24,   314,   461,   462,
      15,     7,    22,    22,     8,   184,   400,    24,   129,   130,
     131,   132,   411,   412,   413,   129,   130,   131,   420,   421,
     422,   129,   130,   131,   429,   430,   431,     1,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   483,   224,   225,   226,   227,   228,
     229,   230,   484,   484,   484,   483,   484,   484,   484,   484,
      21,    22,   314,    22,   314,    22,   314,    22,   314,    22,
     314,    22,   314,     1,   193,   194,   195,   196,   197,   487,
     175,   176,   177,   178,   179,   180,   481,   185,   186,   489,
      23,   299,   299,   299,   299,    22,    22,     8,    15,     7,
      14,    14,    14,    14,     6,   412,    14,    14,    14,     6,
     421,    14,    14,    14,     6,   430,   299,   231,   232,   485,
     486,   485,   485,   485,   485,   485,   485,   299,    23,   182,
     180,   482,     1,   187,   188,   189,   190,   191,   192,   491,
     490,     8,    15,     7,    22,    22,     4,    24,    25,    24,
      24,    26,     4,    24,    25,    24,    24,     4,    24,    25,
      24,    24,   232,   231,   299,    24,   491,    10,    22,    22,
      15,    15,    22,   323,    15,    15,    22,    22,    15,    22,
      22,     8,    15,    22,     8,    15,    10,    22,    15,    10,
      22,    15,    22,    15,    22,    15,    22,     8,    22,     8,
      10,    15,    10,    22,    15,    22,    15,    22,    15,    22,
      15,    22,    15,    22,    15,    22,    15,    22,    15,    22,
      15,    22,    15,    22,    15,    22
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
//   switch (yytype)
//     {
//       default:
//         break;
//     }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

//   switch (yytype)
//     {
// 
//       default:
//         break;
//     }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

    {
                g_bGlobalAttributes = false;
                g_pkCurrAttribTable = 0;
                g_pkCurrObjectTable = 0;
                g_uiCurrPDStream = 0;    
                g_bCurrPDFixedFunction = false;
                g_pkCurrPackingDef = 0;
                g_pkCurrRequirements = 0;
                g_uiCurrImplementation = 0;
                g_pkCurrImplementation = 0;
                g_pkCurrRSGroup = 0;
                g_uiCurrImplemVSConstantMap = 0;
                g_uiCurrImplemGSConstantMap = 0;
                g_uiCurrImplemPSConstantMap = 0;
                g_uiCurrPassVSConstantMap = 0;
                g_uiCurrPassGSConstantMap = 0;
                g_uiCurrPassPSConstantMap = 0;
                g_pkCurrConstantMap = 0;
                g_uiCurrPassIndex = 0;
                g_pkCurrPass = 0;
                g_pkCurrTextureStage = 0;
                g_uiCurrTextureSlot = 0;
                g_pkCurrTexture = 0;
                
                g_pkCurrShader = NiNew NSFParsedShader();
                if (g_pkCurrShader)
                {
                    g_kParsedShaderList.AddTail(g_pkCurrShader);
                    g_pkCurrNSBShader = g_pkCurrShader->GetShader();
                    g_pkCurrNSBShader->SetName((yyvsp[-1].sval));
                    g_uiCurrImplementation = 0;
                }
                else
                {
                    DebugStringOut("Failed to create NSFParsedShader!\n");
                }
                
                g_iDSOIndent = 0;
                NiSprintf(g_szDSO, 1024, "\nNSF Shader - %s\n",(yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 5:

    {
                    NiSprintf(g_szDSO, 1024, "Description: %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                    
                    if (g_pkCurrNSBShader)
                        g_pkCurrNSBShader->SetDescription((yyvsp[0].sval));
                        
                    if ((yyvsp[0].sval) && strcmp((yyvsp[0].sval), ""))
                        NiFree((yyvsp[0].sval));
                ;}
    break;

  case 6:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Completed NSF Shader - %s\n",(yyvsp[-6].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrShader = 0;
                g_pkCurrNSBShader = 0;
                
                NiFree((yyvsp[-6].sval));
            ;}
    break;

  case 7:

    {
                NSFParsererror("Syntax Error: shader");
                yyclearin;
            ;}
    break;

  case 18:

    {   (yyval.sval) = "";    ;}
    break;

  case 19:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 20:

    {
                (yyval.sval) = (yyvsp[0].sval);
            ;}
    break;

  case 21:

    {
                // Assumes $1 is a null-terminated string
                unsigned int uiLen = strlen((yyvsp[-1].sval)) + 1;
                if (uiLen < MAX_QUOTE_LENGTH)
                    uiLen = MAX_QUOTE_LENGTH;
                (yyval.sval) = NiStrcat((yyvsp[-1].sval), uiLen, (yyvsp[0].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 22:

    {   (yyval.sval) = "";    ;}
    break;

  case 23:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 24:

    {   (yyval.sval) = "";    ;}
    break;

  case 25:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 26:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 27:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 28:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 29:

    {   (yyval.sval) = (yyvsp[-1].sval);    ;}
    break;

  case 30:

    {
                g_bRanged = false;
            ;}
    break;

  case 31:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-17].fval));
                AddFloatToLowArray((yyvsp[-15].fval));
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToHighArray((yyvsp[-8].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 32:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 33:

    {
                g_bRanged = false;
            ;}
    break;

  case 34:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-17].fval));
                AddFloatToLowArray((yyvsp[-15].fval));
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToHighArray((yyvsp[-8].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 35:

    {
                g_bRanged = false;
            ;}
    break;

  case 36:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 37:

    {
                g_bRanged = false;
            ;}
    break;

  case 38:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToLowArray((yyvsp[-7].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 39:

    {
                g_bRanged = false;
            ;}
    break;

  case 40:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-2].fval));
                AddFloatToHighArray((yyvsp[-1].fval));
            ;}
    break;

  case 41:

    {
                g_bRanged = false;
            ;}
    break;

  case 42:

    {
                g_bRanged    = true;
                AddFloatToLowArray((float)(yyvsp[-2].ival));
                AddFloatToHighArray((float)(yyvsp[-1].ival));
            ;}
    break;

  case 43:

    {
                // Allow floats for backwards compatibility
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-2].fval));
                AddFloatToHighArray((yyvsp[-1].fval));
            ;}
    break;

  case 46:

    {
                AddFloatToValueArray((yyvsp[0].fval));
            ;}
    break;

  case 47:

    {
                AddFloatToValueArray((yyvsp[-30].fval));
                AddFloatToValueArray((yyvsp[-28].fval));
                AddFloatToValueArray((yyvsp[-26].fval));
                AddFloatToValueArray((yyvsp[-24].fval));
                AddFloatToValueArray((yyvsp[-22].fval));
                AddFloatToValueArray((yyvsp[-20].fval));
                AddFloatToValueArray((yyvsp[-18].fval));
                AddFloatToValueArray((yyvsp[-16].fval));
                AddFloatToValueArray((yyvsp[-14].fval));
                AddFloatToValueArray((yyvsp[-12].fval));
                AddFloatToValueArray((yyvsp[-10].fval));
                AddFloatToValueArray((yyvsp[-8].fval));
                AddFloatToValueArray((yyvsp[-6].fval));
                AddFloatToValueArray((yyvsp[-4].fval));
                AddFloatToValueArray((yyvsp[-2].fval));
                AddFloatToValueArray((yyvsp[0].fval));
            ;}
    break;

  case 48:

    {
                NiSprintf(g_szDSO, 1024, "Attribute Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            ;}
    break;

  case 49:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Attribute Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrAttribTable = 0;
            ;}
    break;

  case 50:

    {
                NiSprintf(g_szDSO, 1024, "Global Attribute Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = true;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetGlobalAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            ;}
    break;

  case 51:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Global Attribute Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrAttribTable = 0;
            ;}
    break;

  case 52:

    {
        (yyval.ival) = 0;
    ;}
    break;

  case 53:

    {
        (yyval.ival) = (yyvsp[-1].ival);
    ;}
    break;

  case 66:

    {   (yyval.bval) = true;      ;}
    break;

  case 67:

    {   (yyval.bval) = false;     ;}
    break;

  case 68:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Bool(
                        (yyvsp[-2].sval), 0, (yyvsp[-1].bval) ? false : true, 
                        (yyvsp[0].bval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Bool at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Boolean: %16s - %6s - %s\n",
                    (yyvsp[-2].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 69:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 70:

    {
                if (g_pkCurrAttribTable)
                {
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            (unsigned int)(yyvsp[-1].ival),
                            (unsigned int)g_afLowValues[0], 
                            (unsigned int)g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            (unsigned int)(yyvsp[-1].ival)))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                }                

                NiSprintf(g_szDSO, 1024, "       uint: %16s - %6s - %d\n",
                    (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[%4d..%4d]\n", 
                        (unsigned int)g_afLowValues[0], 
                        (unsigned int)g_afHighValues[0]);
                }

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 71:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 72:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 1;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Float(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            *(g_afValues->GetBase()),
                            g_afLowValues[0], g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                }

                NiSprintf(g_szDSO, 1024, 
                    "      Float: %16s - %6s - %8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    *(g_afValues->GetBase()));
                DebugStringOut(g_szDSO);

                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[%8.5f..%8.5f]\n",
                        g_afLowValues[0], g_afHighValues[0]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 73:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 74:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 2;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point2(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, "     Point2: %16s - %6s - "
                    "%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f)..(%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1],
                        g_afHighValues[0], g_afHighValues[1]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 75:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 76:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 3;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT3,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point3(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                }
            
                NiSprintf(g_szDSO, 1024, "     Point3: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f)..(%8.5f,%8.5f,%8.5f)]"
                        "\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afHighValues[0],
                        g_afHighValues[1], g_afHighValues[2]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 77:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 78:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 4;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point4(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, "     Point4: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1), 
                    g_afValues->GetAt(2), g_afValues->GetAt(3));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                        "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afLowValues[3],
                        g_afHighValues[0], g_afHighValues[1], 
                        g_afHighValues[2], g_afHighValues[3]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 79:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 80:

    {
                unsigned int uiExpectedLength = (((yyvsp[-2].ival) != 0) ? (yyvsp[-2].ival) : 1) * 9;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-2].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3,
                            (yyvsp[-2].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix3(
                                (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, 
                    "    Matrix3: %16s - %6s - %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-3].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2),
                    " ", " ",
                    g_afValues->GetAt(3),
                    g_afValues->GetAt(4),
                    g_afValues->GetAt(5),
                    " ", " ",
                    g_afValues->GetAt(6),
                    g_afValues->GetAt(7),
                    g_afValues->GetAt(8));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 81:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 82:

    {
                unsigned int uiExpectedLength = (((yyvsp[-2].ival) != 0) ? (yyvsp[-2].ival) : 1) * 16;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-2].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4,
                            (yyvsp[-2].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix4(
                                (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                }                

                NiSprintf(g_szDSO, 1024, 
                    "  Transform: %16s - %6s - %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-3].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                            g_afValues->GetAt( 0), g_afValues->GetAt( 1), 
                            g_afValues->GetAt( 2), g_afValues->GetAt( 3),
                    " ", " ", g_afValues->GetAt( 4), g_afValues->GetAt( 5),
                            g_afValues->GetAt( 6), g_afValues->GetAt( 7),
                    " ", " ", g_afValues->GetAt( 8), g_afValues->GetAt( 9),
                            g_afValues->GetAt(10), g_afValues->GetAt(11),
                    " ", " ", g_afValues->GetAt(12), g_afValues->GetAt(13),
                            g_afValues->GetAt(14), g_afValues->GetAt(15));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 83:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 84:

    {
                // This one is a bit special, the expected length is 3 *or* 4
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < 3)
                {
                    // Pad out to 3 if too short (with 0.0f's)
                    for (; uiFoundLength < 3; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > 4)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected 3 or 4 float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiFoundLength);

                    g_afValues->SetSize(4);
                }

                if (g_afValues->GetSize() == 3)
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                    }                

                    NiSprintf(g_szDSO, 1024, "      Color: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f\n",
                        (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0),
                        g_afValues->GetAt(1),
                        g_afValues->GetAt(2));
                    DebugStringOut(g_szDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_szDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2]);
                        DebugStringOut(g_szDSO);
                    }
                }
                else
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                    }                

                    NiSprintf(g_szDSO, 1024, "     ColorA: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f,%8.5f\n",
                        (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0), g_afValues->GetAt(1), 
                        g_afValues->GetAt(2), g_afValues->GetAt(3));
                    DebugStringOut(g_szDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_szDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2], g_afLowValues[3],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2], g_afHighValues[3]);
                        DebugStringOut(g_szDSO);
                    }
                }

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 85:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                        (unsigned int)(yyvsp[-1].ival), (yyvsp[0].sval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].bval));
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-3].sval));
                if (strcmp((yyvsp[0].sval), ""))
                    NiFree((yyvsp[0].sval));
            ;}
    break;

  case 86:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        (yyvsp[-2].sval), 0, (yyvsp[-1].bval) ? false : true, 
                        g_uiCurrTextureSlot, (yyvsp[0].sval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-1].bval));
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    (yyvsp[-2].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    g_uiCurrTextureSlot);
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
                if (strcmp((yyvsp[0].sval), ""))
                    NiFree((yyvsp[0].sval));
                    
                g_uiCurrTextureSlot++;
            ;}
    break;

  case 87:

    {
                NiSprintf(g_szDSO, 1024, "Object Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                else
                {
                    g_pkCurrObjectTable = 0;
                }
            ;}
    break;

  case 88:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Object Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrObjectTable = 0;
            ;}
    break;

  case 102:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_GeneralLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 103:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_PointLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 104:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_DirectionalLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 105:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_SpotLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 106:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowPointLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 107:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowDirectionalLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 108:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowSpotLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 109:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_EnvironmentMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 110:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ProjectedShadowMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 111:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ProjectedLightMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 112:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_FOGMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_FogMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 113:

    {
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef((yyvsp[-1].sval), true);
                }
                else
                {
                    g_pkCurrPackingDef = 0;
                }

                g_bCurrPDFixedFunction = false;
                
                NiSprintf(g_szDSO, 1024, "PackingDefinition Start %s\n", 
                    (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 114:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PackingDefinition End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrPackingDef = 0;
                
                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 115:

    {
                NiSprintf(g_szDSO, 1024, "Using PackingDefinition %s\n",(yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                if (g_pkCurrNSBShader)
                {
                    NSBPackingDef* pkPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef((yyvsp[0].sval), false);
                    if (!pkPackingDef)
                    {
                        NiSprintf(g_szDSO, 1024, "    WARNING: PackingDefinition %s "
                            "NOT FOUND\n",(yyvsp[0].sval));
                        DebugStringOut(g_szDSO);
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition %s\n"
                            "    at line %d\n"
                            "    NOT FOUND!\n",
                            g_pkFile->GetFilename(), (yyvsp[0].sval), 
                            NSFParserGetLineNumber());
                    }
                    else
                    {
                        if (g_pkCurrImplementation)
                        {
                            g_pkCurrImplementation->SetPackingDef(
                                (yyvsp[0].sval));
                        }
                    }
                }

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 116:

    {   (yyval.ival) = 0x7fffffff;                            ;}
    break;

  case 117:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT1;        ;}
    break;

  case 118:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT2;        ;}
    break;

  case 119:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT3;        ;}
    break;

  case 120:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT4;        ;}
    break;

  case 121:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_UBYTECOLOR;    ;}
    break;

  case 122:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT1;        ;}
    break;

  case 123:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT2;        ;}
    break;

  case 124:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT3;        ;}
    break;

  case 125:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT4;        ;}
    break;

  case 126:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_UBYTE4;        ;}
    break;

  case 127:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT1;    ;}
    break;

  case 128:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT2;    ;}
    break;

  case 129:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT3;    ;}
    break;

  case 130:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT4;    ;}
    break;

  case 131:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMPACKED3;   ;}
    break;

  case 132:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE1;        ;}
    break;

  case 133:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE2;        ;}
    break;

  case 134:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE3;        ;}
    break;

  case 135:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE4;        ;}
    break;

  case 136:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT2H;       ;}
    break;

  case 137:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMUBYTE4;    ;}
    break;

  case 138:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMUSHORT2;   ;}
    break;

  case 139:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMUSHORT4;   ;}
    break;

  case 140:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_UDEC3;         ;}
    break;

  case 141:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMDEC3;      ;}
    break;

  case 142:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT16_2;     ;}
    break;

  case 143:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT16_4;     ;}
    break;

  case 144:

    {
                NSFParsererror("Syntax Error: packing_definition_type");
                yyclearin;
            ;}
    break;

  case 145:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION;       ;}
    break;

  case 146:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT;    ;}
    break;

  case 147:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES;   ;}
    break;

  case 148:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_NORMAL;         ;}
    break;

  case 149:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 150:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_COLOR;          ;}
    break;

  case 151:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 152:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0;      ;}
    break;

  case 153:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1;      ;}
    break;

  case 154:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2;      ;}
    break;

  case 155:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3;      ;}
    break;

  case 156:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4;      ;}
    break;

  case 157:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5;      ;}
    break;

  case 158:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6;      ;}
    break;

  case 159:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7;      ;}
    break;

  case 160:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 161:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 162:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TANGENT;        ;}
    break;

  case 163:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BINORMAL;       ;}
    break;

  case 164:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_DEFAULT;            ;}
    break;

  case 165:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_PARTIALU;           ;}
    break;

  case 166:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_PARTIALV;           ;}
    break;

  case 167:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_CROSSUV;            ;}
    break;

  case 168:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_UV;                 ;}
    break;

  case 169:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_LOOKUP;             ;}
    break;

  case 170:

    {   (yyval.ival) = NiD3DShaderDeclaration::SPTESS_LOOKUPPRESAMPLED;   ;}
    break;

  case 171:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_POSITION;      ;}
    break;

  case 172:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BLENDWEIGHT;   ;}
    break;

  case 173:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BLENDINDICES;  ;}
    break;

  case 174:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_NORMAL;        ;}
    break;

  case 175:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_PSIZE;         ;}
    break;

  case 176:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TEXCOORD;      ;}
    break;

  case 177:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TANGENT;       ;}
    break;

  case 178:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BINORMAL;      ;}
    break;

  case 179:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TESSFACTOR;    ;}
    break;

  case 180:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_POSITIONT;     ;}
    break;

  case 181:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_COLOR;         ;}
    break;

  case 182:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_FOG;           ;}
    break;

  case 183:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_DEPTH;         ;}
    break;

  case 184:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_SAMPLE;        ;}
    break;

  case 187:

    {
                NSFParsererror("Syntax Error: packing_definition_entries");
                yyclearin;
            ;}
    break;

  case 188:

    {
                g_uiCurrPDStream    = (unsigned int)(yyvsp[0].ival);
                NiSprintf(g_szDSO, 1024, "Stream %d\n", g_uiCurrPDStream);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 189:

    {
                g_bCurrPDFixedFunction = (yyvsp[0].bval);
                if (g_pkCurrPackingDef)
                    g_pkCurrPackingDef->SetFixedFunction((yyvsp[0].bval));
            ;}
    break;

  case 190:

    {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= (yyvsp[-5].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-4].ival);

                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[-3].ival);
                NiD3DShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiD3DShaderDeclaration::ShaderParameterTesselator)
                    (yyvsp[-2].ival);
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    (yyvsp[-1].ival);
                unsigned int uiUsageIndex = (unsigned int)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pszParam, (int)(yyvsp[-5].ival), uiRegister, pszType, eTess, 
                    eUsage, uiUsageIndex);
                DebugStringOut(g_szDSO);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pszParam, (int)(yyvsp[-5].ival), uiRegister, pszType);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 191:

    {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= (yyvsp[-2].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-1].ival);
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[0].ival);
                
                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(g_uiCurrPDStream, 
                        uiRegister, uiParam, eType,

                        NiD3DShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }
                
                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pszParam, (int)(yyvsp[-2].ival), uiRegister, pszType);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 192:

    {
                unsigned int uiParam = (yyvsp[-5].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-4].ival);
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[-3].ival);
                NiD3DShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiD3DShaderDeclaration::ShaderParameterTesselator)
                    (yyvsp[-2].ival);
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    (yyvsp[-1].ival);
                unsigned int uiUsageIndex = (unsigned int)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s    - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pszParam, uiRegister, pszType, eTess, eUsage, 
                    uiUsageIndex);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 193:

    {
                unsigned int uiParam = (yyvsp[-2].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-1].ival);
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType,
                        NiD3DShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s    - Reg %3d - %16s\n", 
                    pszParam, uiRegister, pszType);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 194:

    {
                NiSprintf(g_szDSO, 1024, "RenderState List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrRSGroup = g_pkCurrPass->GetRenderStateGroup();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrRSGroup = 
                        g_pkCurrImplementation->GetRenderStateGroup();
                }
                else
                {
                    g_pkCurrRSGroup = 0;
                }
            ;}
    break;

  case 195:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "RenderState List End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrRSGroup = 0;
            ;}
    break;

  case 198:

    {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, true);

                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 199:

    {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, false);

                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 206:

    {
                NSFParsererror("Syntax Error: renderstate_entry");
                yyclearin;
            ;}
    break;

  case 207:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                }
                
                NiSprintf(g_szDSO, 1024, "    %32s = ATTRIBUTE - %s", 
                    (yyvsp[-2].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 208:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue; 
                    if (!NSBRenderStates::LookupRenderStateValue(eRS, 
                        (yyvsp[0].sval), uiValue))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    Invalid RenderStateValue (%s)\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), (yyvsp[0].sval), 
                            NSFParserGetLineNumber());
                        g_bCurrStateValid = false;
                    }
                    else
                    {
                        g_uiCurrStateState = (unsigned int)eRS;
                        g_uiCurrStateValue = uiValue;
                        g_bCurrStateValid = true;
                    }
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %s", 
                    (yyvsp[-2].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 209:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].bval) ? 1 : 0;

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %s", (yyvsp[-2].sval), 
                    (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 210:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = F2DW((yyvsp[0].fval));
                    
                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %8.5f", 
                    (yyvsp[-2].sval), (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 211:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].dword);

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_szDSO, 1024, "    %32s = 0x%08x", 
                    (yyvsp[-2].sval), (yyvsp[0].dword));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 212:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].ival);

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_szDSO, 1024, "    %32s = 0x%08x", 
                    (yyvsp[-2].sval), (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 222:

    {
                if (g_bConstantMapPlatformBlock)
                {
                    NSFParsererror("Syntax Error: "
                        "Embedded ConstantMap Platform-block");
                }
                NiSprintf(g_szDSO, 1024,"ConstantMap Platform-block Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                g_bConstantMapPlatformBlock = true;
            ;}
    break;

  case 223:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 224:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "ConstantMap Platform-block End\n");
                DebugStringOut(g_szDSO);
                g_bConstantMapPlatformBlock = false;
            ;}
    break;

  case 225:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "BoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("BoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_NO_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: BoneMatrix3              "
                    "%3d %3d %3d\n", (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 226:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "BoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("BoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_NO_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: BoneMatrix3              "
                    "%24s %3d %3d\n", (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 227:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 4 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "BoneMatrix4", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("BoneMatrix4", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(4);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_NO_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: BoneMatrix4              "
                    "%3d %3d %3d\n", (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 228:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 4 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "BoneMatrix4", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("BoneMatrix4", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(4);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_NO_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: BoneMatrix4              "
                    "%24s %3d %3d\n", (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval))
            ;}
    break;

  case 229:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: SkinBoneMatrix4          "
                    "%3d %3d %3d\n", (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 230:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: SkinBoneMatrix4          "
                    "%24s %3d %3d\n", (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval))
            ;}
    break;

  case 231:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 4 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix4", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix4", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        (char*)0);
                }
                
                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(4);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: SkinBoneMatrix4          "
                    "%3d %3d %3d\n", (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 232:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 4 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix4", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix4", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                
                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(4);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024, "    Defined: SkinBoneMatrix4          "
                    "%24s %3d %3d\n", (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval))
            ;}
    break;

  case 233:

    {
                // Name, RegStart, Extra
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-2].sval), uiFlags, 
                        (unsigned int)(yyvsp[0].ival), 
                        (unsigned int)(yyvsp[-1].ival), 0, 0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry((yyvsp[-2].sval), uiFlags, 
                        (unsigned int)(yyvsp[0].ival), 
                        (unsigned int)(yyvsp[-1].ival), 0, 0);
                }
                            
                NiSprintf(g_szDSO, 1024, "    Defined: %24s %3d %3d\n",
                    (yyvsp[-2].sval), (int)(yyvsp[-1].ival), (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 234:

    {
                // Name, VariableName, Extra
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-2].sval), uiFlags, 
                        (unsigned int)(yyvsp[0].ival), -1, 0, (yyvsp[-1].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry((yyvsp[-2].sval), uiFlags, 
                        (unsigned int)(yyvsp[0].ival), -1, 0, (yyvsp[-1].sval));
                }
                
                NiSprintf(g_szDSO, 1024, "    Defined: %24s %3d %24s %3d\n",
                    (yyvsp[-2].sval), -1, (yyvsp[-1].sval), (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 235:

    {
                // CM_Object, NameOfObject, Parameter, RegStart
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[-2].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    break;
                }

                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping((yyvsp[-1].sval),
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Object name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval),
                        (yyvsp[-1].sval));
                    break;
                }

                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                unsigned int uiBufSize = strlen((yyvsp[-1].sval)) + strlen((yyvsp[-2].sval)) + 3;
                char* pcKey = NiAlloc(char, uiBufSize);
                NiSprintf(pcKey, uiBufSize, "%s@@%s", (yyvsp[-1].sval), (yyvsp[-2].sval));
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) (yyvsp[0].ival), 0, 0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) (yyvsp[0].ival), 0, 0);
                }
                
                NiSprintf(g_szDSO, 1024, "    Object: %24s %16s %3d\n",
                    (yyvsp[-2].sval), (yyvsp[-1].sval), (yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                
                NiFree(pcKey);
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 236:

    {
                // CM_Object, NameOfObject, Parameter, VariableName
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[-2].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    break;
                }
                
                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping((yyvsp[-1].sval),
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Local name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval),
                        (yyvsp[-1].sval));
                    break;
                }
                
                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                unsigned int uiBufSize = strlen((yyvsp[-1].sval)) + strlen((yyvsp[-2].sval)) + 3;
                char* pcKey = NiAlloc(char, uiBufSize);
                NiSprintf(pcKey, uiBufSize, "%s@@%s", (yyvsp[-1].sval), (yyvsp[-2].sval));
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, (yyvsp[0].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, (yyvsp[0].sval));
                }
                
                NiSprintf(g_szDSO, 1024, "    Object: %24s %16s %16s\n",
                    (yyvsp[-2].sval), (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                
                NiFree(pcKey);
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
                
            ;}
    break;

  case 237:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 238:

    {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap((yyvsp[-3].sval), (yyvsp[-2].ival), (yyvsp[-1].ival), (yyvsp[0].ival), false))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_szDSO, 1024, "     Attrib: %24s %3d %3d %3d\n",
                    (yyvsp[-3].sval), (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                                
                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 239:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 240:

    {
                // Name, RegStart, RegCount, Values

                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_CONSTANT |
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;

                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-3].sval), uiFlags, 
                        0, (unsigned int)(yyvsp[-2].ival), 
                        (unsigned int)(yyvsp[-1].ival), (char*)0, 
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry((yyvsp[-3].sval), uiFlags, 
                        0, (unsigned int)(yyvsp[-2].ival), 
                        (unsigned int)(yyvsp[-1].ival), (char*)0, 
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                
                NiSprintf(g_szDSO, 1024, "      Const: %24s %3d %3d - %3d values\n",
                    (yyvsp[-3].sval), (int)(yyvsp[-2].ival), 
                    (int)(yyvsp[-1].ival), g_afValues->GetSize());
                DebugStringOut(g_szDSO);
                
                NiSprintf(g_szDSO, 1024, "             %24s         - ", " ");
                DebugStringOut(g_szDSO);
                
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    NiSprintf(g_szDSO, 1024, "%8.5f,", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui == (g_afValues->GetSize() - 1)))
                    {
                        NiStrcat(g_szDSO, 1024, "\n");
                    }
                    DebugStringOut(g_szDSO, false);
                    if ((((ui + 1) % 4) == 0) &&
                        (ui < (g_afValues->GetSize() - 1)))
                    {
                        NiSprintf(g_szDSO, 1024, "             %24s         - ", " ");
                        DebugStringOut(g_szDSO);
                    }
                }
                
                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 241:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 242:

    {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap((yyvsp[-2].sval), (yyvsp[-1].ival), (yyvsp[0].ival), 0, true))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_szDSO, 1024, "     Global: %24s %3d %3d\n",
                    (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 243:

    {
                // Name, type, RegStart, RegCount, Entry1 * Entry2
                if (!SetupOperatorEntry((yyvsp[-7].sval), (yyvsp[-6].ival), 
                    (yyvsp[-5].ival), (yyvsp[-4].sval), (int)(yyvsp[-3].ival), 
                    (yyvsp[-2].sval), (yyvsp[-1].bval), (yyvsp[0].bval)))
                {
                    NiFree((yyvsp[-7].sval));
                    NiFree((yyvsp[-4].sval));
                    NiFree((yyvsp[-2].sval));
                    // Report the error
                    NSFParsererror("Syntax Error: SetupOperatorEntry!");
                    yyclearin;
                }
                else
                {
                    NiFree((yyvsp[-7].sval));
                    NiFree((yyvsp[-4].sval));
                    NiFree((yyvsp[-2].sval));
                }
            ;}
    break;

  case 244:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY;   ;}
    break;

  case 245:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE;     ;}
    break;

  case 246:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_ADD;        ;}
    break;

  case 247:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT;   ;}
    break;

  case 248:

    {
                NSFParsererror("Syntax Error: operator_type");
                yyclearin;
            ;}
    break;

  case 249:

    {   (yyval.bval) = false;     ;}
    break;

  case 250:

    {   (yyval.bval) = true;      ;}
    break;

  case 251:

    {   (yyval.bval) = false;     ;}
    break;

  case 252:

    {   (yyval.bval) = true;      ;}
    break;

  case 253:

    {
                g_eConstMapMode = CONSTMAP_VERTEX;
                NiSprintf(g_szDSO, 1024, "VertexShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetVertexConstantMap(
                        g_uiCurrPassVSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetVertexConstantMap(g_uiCurrImplemVSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            ;}
    break;

  case 254:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "VertexShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 255:

    {
                g_eConstMapMode = CONSTMAP_GEOMETRY;
                NiSprintf(g_szDSO, 1024, "GeometryShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetGeometryConstantMap(
                        g_uiCurrPassGSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetGeometryConstantMap(g_uiCurrImplemGSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            ;}
    break;

  case 256:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "GeometryShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 257:

    {
                g_eConstMapMode = CONSTMAP_PIXEL;
                NiSprintf(g_szDSO, 1024, "PixelShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetPixelConstantMap(
                        g_uiCurrPassPSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetPixelConstantMap(g_uiCurrImplemPSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            ;}
    break;

  case 258:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PixelShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 259:

    {
                if (g_pkCurrImplementation)
                    g_pkCurrImplementation->SetClassName((yyvsp[0].sval));

                NiSprintf(g_szDSO, 1024, "ClassName = %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 260:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 261:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 262:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetVertexShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 263:

    {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "VertexShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramEntryPoint(
                        (yyvsp[-1].sval));
                    g_pkCurrPass->SetVertexShaderProgramShaderTarget(
                        (yyvsp[0].sval));
                }
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 264:

    {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "VertexShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramEntryPoint(
                        (yyvsp[0].sval));
                }
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 265:

    {
                // shader
                NiSprintf(g_szDSO, 1024, "VertexShader File\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 266:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 267:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 268:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 271:

    {
                if (g_uiCurrentPlatforms & NiShader::NISHADER_DX9)
                {
                    g_pkCurrPass->SetVertexShaderProgramFileDX9(
                        (yyvsp[0].sval));
                }
                if (g_uiCurrentPlatforms & NiShader::NISHADER_XENON)
                {
                    g_pkCurrPass->SetVertexShaderProgramFileXenon(
                        (yyvsp[0].sval));
                }
                if (g_uiCurrentPlatforms & NiShader::NISHADER_PS3)
                {
                    g_pkCurrPass->SetVertexShaderProgramFilePS3(
                        (yyvsp[0].sval));
                }
                if (g_uiCurrentPlatforms & NiShader::NISHADER_D3D10)
                {
                    g_pkCurrPass->SetVertexShaderProgramFileD3D10(
                        (yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 272:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramEntryPoint(
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                }
            ;}
    break;

  case 273:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetVertexShaderProgramShaderTarget(
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                }
            ;}
    break;

  case 274:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetSoftwareVertexProcessing((yyvsp[0].bval));
                }
            ;}
    break;

  case 275:

    {
                NiSprintf(g_szDSO, 1024, "VertexShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 276:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 277:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "VertexShader End\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 280:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 281:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 282:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetGeometryShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 283:

    {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "GeometryShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramEntryPoint(
                        (yyvsp[-1].sval));
                    g_pkCurrPass->SetGeometryShaderProgramShaderTarget(
                        (yyvsp[0].sval));
                }
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 284:

    {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "GeometryShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramEntryPoint(
                        (yyvsp[0].sval));
                }
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 285:

    {
                // shader
                NiSprintf(g_szDSO, 1024, "GeometryShader File\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 286:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 287:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 288:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 291:

    {
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_DX9)
                    {
                        g_pkCurrPass->SetGeometryShaderProgramFileDX9(
                            (yyvsp[0].sval));
                    }
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_XENON)
                    {
                        g_pkCurrPass->SetGeometryShaderProgramFileXenon(
                            (yyvsp[0].sval));
                    }
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_PS3)
                    {
                        g_pkCurrPass->SetGeometryShaderProgramFilePS3(
                            (yyvsp[0].sval));
                    }
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_D3D10)
                    {
                        g_pkCurrPass->SetGeometryShaderProgramFileD3D10(
                            (yyvsp[0].sval));
                    }
                    NiFree((yyvsp[0].sval));
            ;}
    break;

  case 292:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramEntryPoint(
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                }
            ;}
    break;

  case 293:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetGeometryShaderProgramShaderTarget(
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                }
            ;}
    break;

  case 294:

    {
                NiSprintf(g_szDSO, 1024, "GeometryShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 295:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 296:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "GeometryShader End\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 299:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 300:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 301:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramFileXenon(
                        (yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFileDX9((yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFilePS3((yyvsp[0].sval));
                    g_pkCurrPass->SetPixelShaderProgramFileD3D10((yyvsp[0].sval));
                }
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 302:

    {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "PixelShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramEntryPoint(
                        (yyvsp[-1].sval));
                    g_pkCurrPass->SetPixelShaderProgramShaderTarget(
                        (yyvsp[0].sval));
                }
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 303:

    {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "PixelShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramEntryPoint(
                        (yyvsp[0].sval));
                }
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 304:

    {
                // shader
                NiSprintf(g_szDSO, 1024, "PixelShader File\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 305:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 306:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 307:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 310:

    {
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_DX9)
                    {
                        g_pkCurrPass->SetPixelShaderProgramFileDX9(
                            (yyvsp[0].sval));
                    }
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_XENON)
                    {
                        g_pkCurrPass->SetPixelShaderProgramFileXenon(
                            (yyvsp[0].sval));
                    }
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_PS3)
                    {
                        g_pkCurrPass->SetPixelShaderProgramFilePS3(
                            (yyvsp[0].sval));
                    }
                    if (g_uiCurrentPlatforms & NiShader::NISHADER_D3D10)
                    {
                        g_pkCurrPass->SetPixelShaderProgramFileD3D10(
                            (yyvsp[0].sval));
                    }
                    NiFree((yyvsp[0].sval));
            ;}
    break;

  case 311:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramEntryPoint(
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                }
            ;}
    break;

  case 312:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetPixelShaderProgramShaderTarget(
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                }
            ;}
    break;

  case 313:

    {
                NiSprintf(g_szDSO, 1024, "PixelShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 314:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 315:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PixelShader End\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 318:

    {
                NiSprintf(g_szDSO, 1024, "Requirement List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                }
            ;}
    break;

  case 319:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Requirement List End\n");
                DebugStringOut(g_szDSO);

                if (g_pkCurrNSBShader && g_pkCurrRequirements)
                {
                    // Update the shader requirements
                    g_pkCurrNSBShader->AddVertexShaderVersion(
                        g_pkCurrRequirements->GetVSVersion());
                    g_pkCurrNSBShader->AddGeometryShaderVersion(
                        g_pkCurrRequirements->GetGSVersion());
                    g_pkCurrNSBShader->AddPixelShaderVersion(
                        g_pkCurrRequirements->GetPSVersion());
                    g_pkCurrNSBShader->AddUserVersion(
                        g_pkCurrRequirements->GetUserVersion());
                    g_pkCurrNSBShader->AddPlatform(
                        g_pkCurrRequirements->GetPlatformFlags());
                }
                g_pkCurrRequirements = 0;
            ;}
    break;

  case 331:

    {
                NSFParsererror("Syntax Error: requirement_entry");
                yyclearin;
            ;}
    break;

  case 332:

    {
                NiSprintf(g_szDSO, 1024, "    VSVersion   0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetVSVersion((yyvsp[0].vers));
            ;}
    break;

  case 333:

    {
                NiSprintf(g_szDSO, 1024, "    GSVersion   0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetGSVersion((yyvsp[0].vers));
            ;}
    break;

  case 334:

    {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the PS method here
#ifndef _PS3
                unsigned int uiMaj = D3DSHADER_VERSION_MAJOR((yyvsp[0].vers));
                unsigned int uiMin = D3DSHADER_VERSION_MINOR((yyvsp[0].vers));
                unsigned int uiVers = D3DPS_VERSION(uiMaj, uiMin);
#else
                unsigned int uiVers = (yyvsp[0].vers);
#endif                
                NiSprintf(g_szDSO, 1024, "    PSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPSVersion(uiVers);
            ;}
    break;

  case 335:

    {
                NiSprintf(g_szDSO, 1024, "    UserDefined 0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetUserVersion((yyvsp[0].vers));
            ;}
    break;

  case 336:

    {
                NiSprintf(g_szDSO, 1024, "       Platform 0x%08x\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPlatformFlags((yyvsp[0].ival));
            ;}
    break;

  case 337:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[-6].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-6].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[-4].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-4].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[-2].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-2].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;

                if ((yyvsp[-6].sval))
                     NiFree((yyvsp[-6].sval));
                if ((yyvsp[-4].sval))
                     NiFree((yyvsp[-4].sval));
                if ((yyvsp[-2].sval))
                     NiFree((yyvsp[-2].sval));
                if ((yyvsp[0].sval))
                     NiFree((yyvsp[0].sval));
                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 338:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[-4].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-4].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[-2].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-2].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;

                if ((yyvsp[-4].sval))
                     NiFree((yyvsp[-4].sval));
                if ((yyvsp[-2].sval))
                     NiFree((yyvsp[-2].sval));
                if ((yyvsp[0].sval))
                     NiFree((yyvsp[0].sval));
                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 339:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[-2].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-2].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                
                if ((yyvsp[-2].sval))
                     NiFree((yyvsp[-2].sval));
                if ((yyvsp[0].sval))
                     NiFree((yyvsp[0].sval));

                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 340:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                
                if ((yyvsp[0].sval))
                     NiFree((yyvsp[0].sval));

                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 341:

    {
                NiSprintf(g_szDSO, 1024, "%15s %s\n", (yyvsp[-2].sval),
                    (yyvsp[0].bval) ? "true" : "false");
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    // Determine the requirement field
                    if (NiStricmp((yyvsp[-2].sval), "USESNIRENDERSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiRenderState(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "USESNILIGHTSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiLightState(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "SOFTWAREVPREQUIRED") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPRequired(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "SOFTWAREVPFALLBACK") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPAcceptable(
                            (yyvsp[0].bval));
                    }
                }
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 342:

    {
                NiSprintf(g_szDSO, 1024, "Bones/Partition %d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetBonesPerPartition((yyvsp[0].ival));
            ;}
    break;

  case 343:

    {
                NiSprintf(g_szDSO, 1024, "BinormalTangentUVSource %d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentUVSource((yyvsp[0].ival));
                }
            ;}
    break;

  case 344:

    {
                NiSprintf(g_szDSO, 1024, "BinormalTangent %d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentMethod(
                        (NiGeometryData::DataFlags)(yyvsp[0].ival));
                }
            ;}
    break;

  case 345:

    {   (yyval.ival) = NiGeometryData::NBT_METHOD_NONE;   ;}
    break;

  case 346:

    {   (yyval.ival) = NiGeometryData::NBT_METHOD_NDL;    ;}
    break;

  case 347:

    {   (yyval.ival) = NiGeometryData::NBT_METHOD_ATI;    ;}
    break;

  case 348:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    Deprecated value for BinormalTangentMethod "
                    "Requirement\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                (yyval.ival) = NiGeometryData::NBT_METHOD_NDL;    
            ;}
    break;

  case 349:

    {
                NSFParsererror("Syntax Error: binormaltanget_method");
                yyclearin;
            ;}
    break;

  case 353:

    {
                NiSprintf(g_szDSO, 1024, "Stage Start %3d - %s\n", (yyvsp[-2].ival), 
                    (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)(yyvsp[-2].ival));
                    if (g_pkCurrTextureStage)
                    {
                        g_pkCurrTextureStage->SetUseTextureTransformation(
                            false);
                    }
                }
            ;}
    break;

  case 354:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Stage End %3d - %s\n", (int)(yyvsp[-5].ival), 
                    (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                if (strcmp((yyvsp[-4].sval), ""))
                    NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 362:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, true);
                        }
                    }
                
                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 363:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false);
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 383:

    {
                NSFParsererror("Syntax Error: stage_entry");
                yyclearin;
            ;}
    break;

  case 384:

    {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetNDLMap((yyvsp[0].ival));
            ;}
    break;

  case 385:

    {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetDecalMap((yyvsp[0].ival));
            ;}
    break;

  case 386:

    {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName((yyvsp[0].sval));
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pszValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pszValue))
                        {
                            uiValue |= NiD3DTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTextureStage)
                                g_pkCurrTextureStage->SetShaderMap(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName((yyvsp[0].sval));
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                        else
                        {
                            if (g_pkCurrTextureStage)
                            {
                                g_pkCurrTextureStage->SetObjTextureSettings(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            (yyvsp[0].sval));
                    }
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 387:

    {           
        NSBObjectTable::ObjectDesc* pkDesc = NULL;
        NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
        if (pkTable)
        {
            pkDesc = pkTable->GetObjectByName((yyvsp[0].sval));
        }
        if (!pkDesc)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                false,
                "* PARSE ERROR: %s\n"
                "    GetObjectByName at line %d\n"
                "    Local name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(),
                (yyvsp[0].sval));
            NiFree((yyvsp[0].sval));                             
            break;
        }
        if (g_pkCurrTextureStage)
        {
            g_pkCurrTextureStage->SetObjTextureSettings(
                pkDesc->GetType(), pkDesc->GetIndex());
        }
                
        NiFree((yyvsp[0].sval));             
    ;}
    break;

  case 388:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_BASE;      ;}
    break;

  case 389:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_DARK;      ;}
    break;

  case 390:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_DETAIL;    ;}
    break;

  case 391:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_GLOSS;     ;}
    break;

  case 392:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_GLOW;      ;}
    break;

  case 393:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_BUMP;      ;}
    break;

  case 394:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_NORMAL;      ;}
    break;

  case 395:

    {   (yyval.ival) = NiD3DTextureStage::TSTF_NDL_PARALLAX;      ;}
    break;

  case 396:

    {
                (yyval.ival) = NiD3DTextureStage::TSTF_MAP_DECAL | (yyvsp[0].ival);
            ;}
    break;

  case 397:

    {
                NiSprintf(g_szDSO, 1024, "ColorOp            = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLOROP;
                g_uiCurrStateValue = (yyvsp[0].ival);  
                g_bCurrStateValid = true;  
            ;}
    break;

  case 398:

    {
                NiSprintf(g_szDSO, 1024, "ColorArg0          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG0;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 399:

    {
                NiSprintf(g_szDSO, 1024, "ColorArg1          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG1;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 400:

    {
                NiSprintf(g_szDSO, 1024, "ColorArg2          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG2;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 401:

    {
                NiSprintf(g_szDSO, 1024, "AlphaOp            = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAOP;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 402:

    {
                NiSprintf(g_szDSO, 1024, "AlphaArg0          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG0;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 403:

    {
                NiSprintf(g_szDSO, 1024, "AlphaArg1          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG1;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 404:

    {
                NiSprintf(g_szDSO, 1024, "AlphaArg2          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG2;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 405:

    {
                NiSprintf(g_szDSO, 1024, "ResultArg          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_RESULTARG;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 406:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_INVALID;
                g_uiCurrStateValue = (yyvsp[0].dword);
                    
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedTextureStageState (TSS_CONSTANT)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;  
            ;}
    break;

  case 407:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat00       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT00;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 408:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat00       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 409:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat01       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT01;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 410:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat01       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 411:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat10       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT10;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 412:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat10       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 413:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat11       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT11;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 414:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat11       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 415:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLScale      = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVLSCALE;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 416:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLScale      = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 417:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLOffset     = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVLOFFSET;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 418:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLOffset     = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 419:

    {
                int iValue = (yyvsp[-1].ival) | (yyvsp[0].ival);
                NiSprintf(g_szDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  
            ;}
    break;

  case 420:

    {
                int iValue = (yyvsp[-1].ival);
                NiSprintf(g_szDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  

                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseIndexFromMap(true);
            ;}
    break;

  case 421:

    {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= COUNT%d", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)(yyvsp[-1].ival) | 
                    (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 422:

    {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= NID3DTTFF_PROJECTED");
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)
                    NSBStageAndSamplerStates::NID3DTTFF_PROJECTED;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 423:

    {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= DISABLE");
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = 
                    NSBStageAndSamplerStates::NID3DTTFF_DISABLE;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 424:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT1; ;}
    break;

  case 425:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT2; ;}
    break;

  case 426:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT3; ;}
    break;

  case 427:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT4; ;}
    break;

  case 428:

    {   (yyval.ival) = 0; ;}
    break;

  case 429:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_PROJECTED;  ;}
    break;

  case 430:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_DISABLE;                    
            ;}
    break;

  case 431:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_SELECTARG1;                 
            ;}
    break;

  case 432:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_SELECTARG2;                 
            ;}
    break;

  case 433:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE;                   
            ;}
    break;

  case 434:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE2X;                 
            ;}
    break;

  case 435:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE4X;                 
            ;}
    break;

  case 436:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADD;                        
            ;}
    break;

  case 437:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSIGNED;                  
            ;}
    break;

  case 438:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSIGNED2X;                
            ;}
    break;

  case 439:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_SUBTRACT;                   
            ;}
    break;

  case 440:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSMOOTH;                  
            ;}
    break;

  case 441:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDDIFFUSEALPHA;          
            ;}
    break;

  case 442:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDTEXTUREALPHA;          
            ;}
    break;

  case 443:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDFACTORALPHA;           
            ;}
    break;

  case 444:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDTEXTUREALPHAPM;        
            ;}
    break;

  case 445:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDCURRENTALPHA;          
            ;}
    break;

  case 446:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_PREMODULATE;                
            ;}
    break;

  case 447:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEALPHA_ADDCOLOR;     
            ;}
    break;

  case 448:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATECOLOR_ADDALPHA;     
            ;}
    break;

  case 449:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEINVALPHA_ADDCOLOR;
            ;}
    break;

  case 450:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEINVCOLOR_ADDALPHA;  
            ;}
    break;

  case 451:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BUMPENVMAP;                 
            ;}
    break;

  case 452:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BUMPENVMAPLUMINANCE;        
            ;}
    break;

  case 453:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_DOTPRODUCT3;                
            ;}
    break;

  case 454:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MULTIPLYADD;                
            ;}
    break;

  case 455:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_LERP;                       
            ;}
    break;

  case 456:

    {
                NSFParsererror("Syntax Error: stage_texture_operation");
                yyclearin;
            ;}
    break;

  case 457:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_CURRENT | 
                (yyvsp[0].ival); ;}
    break;

  case 458:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_DIFFUSE | 
                (yyvsp[0].ival); ;}
    break;

  case 459:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_SELECTMASK | 
                (yyvsp[0].ival); ;}
    break;

  case 460:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_SPECULAR | 
                (yyvsp[0].ival); ;}
    break;

  case 461:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_TEMP | 
                (yyvsp[0].ival); ;}
    break;

  case 462:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_TEXTURE | 
                (yyvsp[0].ival); ;}
    break;

  case 463:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_TFACTOR | 
                (yyvsp[0].ival); ;}
    break;

  case 464:

    {   (yyval.ival) = 0;     ;}
    break;

  case 465:

    {   (yyval.ival) = (yyvsp[0].ival);    ;}
    break;

  case 466:

    {
                (yyval.ival) = 
                    NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE | 
                    NSBStageAndSamplerStates::NID3DTA_COMPLEMENT;
            ;}
    break;

  case 467:

    {
                (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_COMPLEMENT |
                    NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE;
            ;}
    break;

  case 468:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE;    ;}
    break;

  case 469:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_COMPLEMENT;        ;}
    break;

  case 470:

    {   (yyval.ival) = 
                0;
            ;}
    break;

  case 471:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_PASSTHRU;
            ;}
    break;

  case 472:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACENORMAL;
            ;}
    break;

  case 473:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACEPOSITION;
            ;}
    break;

  case 474:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACEREFLECTIONVECTOR;
            ;}
    break;

  case 475:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_SPHEREMAP;
            ;}
    break;

  case 476:

    {
                NSFParsererror("Syntax Error: stage_texcoordindex_flags");
                yyclearin;
            ;}
    break;

  case 477:

    {
                // The set will be performed in the assignment section!
            ;}
    break;

  case 478:

    {
                unsigned int uiFlags = NiD3DTextureStage::TSTTF_GLOBAL;
                
                uiFlags |= (int)(yyvsp[-1].ival);

                if (g_pkCurrTextureStage)
                {
                    const char* pszGlobalName = (yyvsp[0].sval);
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetGlobalName(pszGlobalName);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 479:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 480:

    {
                unsigned int uiFlags = NiD3DTextureStage::TSTTF_CONSTANT;
        
                uiFlags |= (int)(yyvsp[-1].ival);

#ifndef _PS3
                if (g_pkCurrTextureStage)
                {
                    D3DMATRIX kTrans;
                    
                    kTrans._11 = g_afValues->GetAt( 0);
                    kTrans._12 = g_afValues->GetAt( 1);
                    kTrans._13 = g_afValues->GetAt( 2);
                    kTrans._14 = g_afValues->GetAt( 3);
                    kTrans._21 = g_afValues->GetAt( 4);
                    kTrans._22 = g_afValues->GetAt( 5);
                    kTrans._23 = g_afValues->GetAt( 6);
                    kTrans._24 = g_afValues->GetAt( 7);
                    kTrans._31 = g_afValues->GetAt( 8);
                    kTrans._32 = g_afValues->GetAt( 9);
                    kTrans._33 = g_afValues->GetAt(10);
                    kTrans._34 = g_afValues->GetAt(11);
                    kTrans._41 = g_afValues->GetAt(12);
                    kTrans._42 = g_afValues->GetAt(13);
                    kTrans._43 = g_afValues->GetAt(14);
                    kTrans._44 = g_afValues->GetAt(15);
                    
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetTextureTransformation(kTrans);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
#endif
                
            ;}
    break;

  case 481:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_NO_CALC;           ;}
    break;

  case 482:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_WORLD_PARALLEL;    ;}
    break;

  case 483:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_WORLD_PERSPECTIVE; ;}
    break;

  case 484:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_WORLD_SPHERE_MAP;  ;}
    break;

  case 485:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_CAMERA_SPHERE_MAP; ;}
    break;

  case 486:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_SPECULAR_CUBE_MAP; ;}
    break;

  case 487:

    {   (yyval.ival) = NiD3DTextureStage::TSTTF_NI_DIFFUSE_CUBE_MAP;  ;}
    break;

  case 488:

    {
                NSFParsererror("Syntax Error: stage_textransmatrix_option");
                yyclearin;
            ;}
    break;

  case 489:

    {
                NiSprintf(g_szDSO, 1024, "Sampler Start %3d - %s\n", 
                    (yyvsp[-2].ival), (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)(yyvsp[-2].ival));
                    g_pkCurrTextureStage->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 490:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Sampler End %3d - %s\n", 
                    (int)(yyvsp[-5].ival), (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                if (strcmp((yyvsp[-4].sval), ""))
                    NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 491:

    {
                NiSprintf(g_szDSO, 1024, "Sampler Start - %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage(g_pkCurrPass->GetStageCount());
                    g_pkCurrTextureStage->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 492:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Sampler End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                if (strcmp((yyvsp[-4].sval), ""))
                    NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 497:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState,
                                g_uiCurrStateValue, true, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 498:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 499:

    {
                // Do nothing. It's handles in the stage_texture block!
                DebugStringOut("Sampler Texture!\n");
            ;}
    break;

  case 518:

    {
                NSFParsererror("Syntax Error: sampler_entry");
                yyclearin;
            ;}
    break;

  case 519:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSU;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSU "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressU             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }                
            ;}
    break;

  case 520:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSV;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSV "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressV             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 521:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSW;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (g_bUseMapValue)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false, "* PARSE ERROR: %s\n"
                        "    UseMapValue not allowed for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressW             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }                
            ;}
    break;

  case 522:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_BORDERCOLOR;
                g_uiCurrStateValue = (yyvsp[0].dword);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "BorderColor         = 0x%08x", 
                    (int)(yyvsp[0].dword));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 523:

    {
                NiSprintf(g_szDSO, 1024, "BorderColor         = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 524:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAGFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MAGFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "MagFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 525:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MINFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MINFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_szDSO, 1024, "MinFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 526:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MIPFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MIPFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_szDSO, 1024, "MipFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 527:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MIPMAPLODBIAS;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MipMapLODBias      = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 528:

    {
                NiSprintf(g_szDSO, 1024, "MipMapLODBias      = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 529:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAXMIPLEVEL;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MaxMipLevel        = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 530:

    {
                NiSprintf(g_szDSO, 1024, "MaxMipLevel        = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 531:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAXANISOTROPY;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MaxAnisotropy      = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 532:

    {
                NiSprintf(g_szDSO, 1024, "MaxAnisotropy      = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 533:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_SRGBTEXTURE;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "SRGBTexture        = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 534:

    {
                NiSprintf(g_szDSO, 1024, "SRGBTexture        = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 535:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ELEMENTINDEX;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "ElementIndex       = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 536:

    {
                NiSprintf(g_szDSO, 1024, "ElementIndex       = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 537:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_DMAPOFFSET;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "DMapOffset         = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 538:

    {
                NiSprintf(g_szDSO, 1024, "DMapOffset         = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 539:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 540:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 541:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYOP)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 542:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 543:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 544:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 545:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 546:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_WRAP;          ;}
    break;

  case 547:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_MIRROR;        ;}
    break;

  case 548:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_CLAMP;         ;}
    break;

  case 549:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_BORDER;        ;}
    break;

  case 550:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_MIRRORONCE;    ;}
    break;

  case 551:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_INVALID;       ;}
    break;

  case 552:

    {
                g_bUseMapValue = true;
                (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_INVALID;
            ;}
    break;

  case 553:

    {
                NSFParsererror("Syntax Error: sampler_texture_address");
                yyclearin;
            ;}
    break;

  case 554:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_NONE;              ;}
    break;

  case 555:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_POINT;             ;}
    break;

  case 556:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_LINEAR;            ;}
    break;

  case 557:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_ANISOTROPIC;       ;}
    break;

  case 558:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_PYRAMIDALQUAD;     ;}
    break;

  case 559:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_GAUSSIANQUAD;      ;}
    break;

  case 560:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 561:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 562:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 563:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 564:

    {
                g_bUseMapValue = true;
                (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;
            ;}
    break;

  case 565:

    {
                NSFParsererror("Syntax Error: sampler_texture_filter");
                yyclearin;
            ;}
    break;

  case 566:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 567:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 568:

    {
                NSFParsererror("Syntax Error: sampler_texture_alphakill");
                yyclearin;
            ;}
    break;

  case 569:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 570:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 571:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 572:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 573:

    {
                NSFParsererror("Syntax Error: sampler_texture_colorkeyop");
                yyclearin;
            ;}
    break;

  case 574:

    {
                NiSprintf(g_szDSO, 1024, "Texture Start %3d - %s\n", 
                    (yyvsp[-2].ival), (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = g_pkCurrPass->GetTexture((int)(yyvsp[-2].ival));
                    g_pkCurrTexture->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 575:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Texture End %3d - %s\n", 
                    (int)(yyvsp[-5].ival), (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTexture = 0;

                if (strcmp((yyvsp[-4].sval), ""))
                    NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 576:

    {
                NiSprintf(g_szDSO, 1024, "Texture Start - %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = 
                        g_pkCurrPass->GetTexture(g_pkCurrPass->GetTextureCount());
                    g_pkCurrTexture->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 577:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Texture End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTexture = 0;

                if (strcmp((yyvsp[-4].sval), ""))
                    NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 583:

    {
                NSFParsererror("Syntax Error: texture_entry");
                yyclearin;
            ;}
    break;

  case 584:

    {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSource((yyvsp[0].ival));
            ;}
    break;

  case 585:

    {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSourceDecal((yyvsp[0].ival));
            ;}
    break;

  case 586:

    {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName((yyvsp[0].sval));
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pszValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pszValue))
                        {
                            uiValue |= NiD3DTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTexture)
                                g_pkCurrTexture->SetSourceShader(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName((yyvsp[0].sval));
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                        else
                        {
                            if (g_pkCurrTexture)
                            {
                                g_pkCurrTexture->SetSourceObject(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            (yyvsp[0].sval));
                    }
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 587:

    {           
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[0].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));                             
                    break;
                }
                if (g_pkCurrTexture)
                {
                    g_pkCurrTexture->SetSourceObject(
                        pkDesc->GetType(), pkDesc->GetIndex());
                }
                        
                NiFree((yyvsp[0].sval));             
            ;}
    break;

  case 599:

    {
                NiSprintf(g_szDSO, 1024, "Pass Start %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrImplementation)
                {
                    g_pkCurrPass = 
                        g_pkCurrImplementation->GetPass(g_uiCurrPassIndex);
                }
            ;}
    break;

  case 600:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Pass End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrPass = 0;
                g_uiCurrPassVSConstantMap = 0;
                g_uiCurrPassGSConstantMap = 0;
                g_uiCurrPassPSConstantMap = 0;
                g_uiCurrPassIndex++;

                if (strcmp((yyvsp[-4].sval), ""))
                    NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 614:

    {
                NiSprintf(g_szDSO, 1024, "Implementation Start %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_pkCurrImplementation = 
                        g_pkCurrNSBShader->GetImplementation((yyvsp[-1].sval), 
                            true, g_uiCurrImplementation);
                    if (g_pkCurrImplementation->GetIndex() == 
                        g_uiCurrImplementation)
                    {
                        g_uiCurrImplementation++;
                    }
                    g_uiCurrPassIndex = 0;
                }                    
            ;}
    break;

  case 615:

    {
                    NiSprintf(g_szDSO, 1024, "Description: %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                    
                    if (g_pkCurrImplementation)
                        g_pkCurrImplementation->SetDesc((yyvsp[0].sval));
                    if ((yyvsp[0].sval) && strcmp((yyvsp[0].sval), ""))
                        NiFree((yyvsp[0].sval));
                ;}
    break;

  case 616:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Implementation End %s\n", (yyvsp[-6].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrImplementation = 0;
                g_uiCurrImplemVSConstantMap = 0;
                g_uiCurrImplemGSConstantMap = 0;
                g_uiCurrImplemPSConstantMap = 0;
                
                NiFree((yyvsp[-6].sval));
            ;}
    break;

  case 617:

    {
                NiSprintf(g_szDSO, 1024, "Integer............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiValue = (unsigned int)(yyvsp[0].ival);
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(unsigned int), sizeof(unsigned int), 
                        (void*)&uiValue, true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 618:

    {
                NiSprintf(g_szDSO, 1024, "Boolean............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%s\n", (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;

                if (g_pkCurrUDDataBlock)
                {
                    bool bValue = (yyvsp[0].bval) ? true : false;
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(bool), sizeof(bool), (void*)&bValue, 
                        true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 619:

    {
                NiSprintf(g_szDSO, 1024, "Floats (%2d)........%s\n", 
                    g_afValues->GetSize(), (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    if ((ui % 4) == 0)
                        DebugStringOut("");
                    NiSprintf(g_szDSO, 1024, "%-8.5f", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui + 1 == g_afValues->GetSize()))
                    {
                        NiStrcat(g_szDSO, 1024, "\n");
                    }
                    DebugStringOut(g_szDSO, false);
                }
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 0;
                    
                    switch (g_afValues->GetSize())
                    {
                    case 1:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                        break;
                    case 2:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                        break;
                    case 3:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                        break;
                    case 4:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                        break;
                    case 8:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
                        break;
                    case 9:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                        break;
                    case 12:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
                        break;
                    case 16:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                        break;
                    default:
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            false, "* %s(%d): Unsupported or unexpected "
                            "attribute size\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                        break;
                    }
                    uiFlags |= 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;
                    
                    if (uiFlags == 0)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Invalid number of floats!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                    else if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(float) * g_afValues->GetSize(), 
                        sizeof(float), (void*)g_afValues->GetBase(), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                // Reset the float arrays, so any entries that follow this
                // one will be handled correctly
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 620:

    {
                NiSprintf(g_szDSO, 1024, "String.............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_STRING;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(char) * strlen((yyvsp[0].sval)), 
                        sizeof(char) * strlen((yyvsp[0].sval)), 
                        (void*)(yyvsp[0].sval), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 623:

    {
                NiSprintf(g_szDSO, 1024, "UserDefinedDataBlock Start %s\n", 
                    (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                // Reset the float arrays, then each one will reset them
                // when they are done being processed
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                if (g_pkCurrPass)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrPass->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrImplementation->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrNSBShader->GetUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataSet = NiNew NSBUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* ALLOCATION ERROR: %s\n"
                        "    UserDefinedDataSet creation at line %d\n", 
                        g_pkFile->GetFilename(), 
                        NSFParserGetLineNumber());
                }
                else
                {
                    if (g_pkCurrPass)
                    {
                        g_pkCurrPass->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrImplementation)
                    {
                        g_pkCurrImplementation->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrNSBShader)
                    {
                        g_pkCurrNSBShader->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                }
                
                if (g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataBlock = 
                        g_pkCurrUDDataSet->GetBlock((yyvsp[-1].sval), false);
                    if (g_pkCurrUDDataBlock)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedDataBlock at line %d\n"
                            "    Name = %s\n"
                            "    ALREADY EXISTS!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-1].sval));
                    }
                    else
                    {
                        g_pkCurrUDDataBlock = 
                            g_pkCurrUDDataSet->GetBlock((yyvsp[-1].sval), true);
                    }
                    NIASSERT(g_pkCurrUDDataBlock);
                }
            ;}
    break;

  case 624:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "UserDefinedDataBlock End %s\n", 
                    (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrUDDataSet = 0;
                g_pkCurrUDDataBlock = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}





//---------------------------------------------------------------------------
int iErrors = 0;
extern void NSFParserResetLineNumber();
extern void NSFParserReset(); 
extern void NSFParserReleaseBuffer();
//---------------------------------------------------------------------------
int ParseShader(const char* pszFileName)
{ 
    NSFParserResetLineNumber();
    g_pkFile = NiNew NSFTextFile();

    NSFParserlval.fval = 0.0f;
    NSFParserlval.ival = 0;
    NSFParserlval.sval = 0;
    NSFParserlval.dword = 0;
    NSFParserlval.word = 0;
    NSFParserlval.byte = 0;
    NSFParserlval.bval = false;

    if (!pszFileName)
    {
        NiDelete g_pkFile;
        NiSprintf(g_szDSO, 1024, "err: need input file\n");
        DebugStringOut(g_szDSO);
        return -1;
    };

    NSFParsedShader* pkParsedShader;
    NiTListIterator    pos = g_kParsedShaderList.GetHeadPos();
    while (pos)
    {
        pkParsedShader = g_kParsedShaderList.GetNext(pos);
        NiDelete pkParsedShader;
    }
    g_kParsedShaderList.RemoveAll();
    g_pkCurrShader = 0;

    if (g_pkFile->Load(pszFileName) != 0)
    {
        NiDelete g_pkFile;
        NiSprintf(g_szDSO, 1024, "err: file not found!\n");
        DebugStringOut(g_szDSO);
        return -1;
    }

    // create array
    g_afValues = NiNew NiTPrimitiveArray<float>;

    iErrors = -1;
    yyparse ();
    if (iErrors != -1)
    {
        iErrors++;
        printf ( "*FAILURE! %d errors found.\n" , iErrors);
        NiSprintf(g_szDSO, 1024, "*FAILURE! %d errors found.\n" , iErrors);
        DebugStringOut(g_szDSO);

        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            false, "* PARSE ERROR: %s\n"
            "    FAILED - %d errors found\n",
            g_pkFile->GetFilename(), iErrors);
    }
        
    NiDelete g_afValues;
    NiDelete g_pkFile;
    g_pkFile = 0;

    if (iErrors == -1)    
        return 0;

    return iErrors;
}
//---------------------------------------------------------------------------
void ResetParser()
{
    NSFParserReset();
} 
//---------------------------------------------------------------------------
void CleanupParser()
{
    NSFParserReleaseBuffer();
}
//---------------------------------------------------------------------------
void DebugStringOut(char* pszOut, bool bIndent)
{
#if defined(_ENABLE_DEBUG_STRING_OUT_)
    if (g_bFirstDSOFileAccess)
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "wt");
        g_bFirstDSOFileAccess = false;
    }
    else
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "at");
    }

    if (bIndent)
    {
        for (int ii = 0; ii < g_iDSOIndent; ii++)
        {
            printf(" ");
            OutputDebugString(" ");
            if (g_pfDSOFile)
                fprintf(g_pfDSOFile, " ");
        }
    }
    printf(pszOut);
    OutputDebugString(pszOut);
    if (g_pfDSOFile)
    {
        fprintf(g_pfDSOFile, pszOut);
        fclose(g_pfDSOFile);
    }
#endif    //#if defined(_ENABLE_DEBUG_STRING_OUT_)
}
//---------------------------------------------------------------------------
unsigned int ResetFloatValueArray(void)
{
    g_afValues->RemoveAll();
    return g_afValues->GetSize();
}
//---------------------------------------------------------------------------
unsigned int AddFloatToValueArray(float fValue)
{
    g_afValues->Add(fValue);
    return g_afValues->GetSize();
}
//---------------------------------------------------------------------------
void ResetFloatRangeArrays(void)
{
    g_uiLowFloatValues    = 0;
    g_uiHighFloatValues    = 0;

    g_afLowValues[0] = 0.0f;
    g_afLowValues[1] = 0.0f;
    g_afLowValues[2] = 0.0f;
    g_afLowValues[3] = 0.0f;
    g_afHighValues[0] = 0.0f;
    g_afHighValues[1] = 0.0f;
    g_afHighValues[2] = 0.0f;
    g_afHighValues[3] = 0.0f;
}
//---------------------------------------------------------------------------
unsigned int AddFloatToLowArray(float fValue)
{
    if ((g_uiLowFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("Low Float array overflow!");
        return 0;
    }
    g_afLowValues[g_uiLowFloatValues++] = fValue;
    return g_uiLowFloatValues;
}
//---------------------------------------------------------------------------
unsigned int AddFloatToHighArray(float fValue)
{
    if ((g_uiHighFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("High Float array overfHigh!");
        return 0;
    }
    g_afHighValues[g_uiHighFloatValues++] = fValue;
    return g_uiHighFloatValues;
}
//---------------------------------------------------------------------------
void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
    unsigned int uiIndex, const char* pcName, const char* pcDebugString)
{
    if (g_pkCurrObjectTable)
    {
        if (!g_pkCurrObjectTable->AddObject(pcName, eType, uiIndex))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* PARSE ERROR: %s\n"
                "    AddObject at line %d\n"
                "    Object name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(), pcName);
        }
    }
    NiSprintf(g_szDSO, 1024, "    %24s: %d - %16s\n", pcDebugString, uiIndex,
        pcName);
    DebugStringOut(g_szDSO);
}
//---------------------------------------------------------------------------
unsigned int DecodeAttribTypeString(char* pszAttribType)
{
    if (!pszAttribType || (strcmp(pszAttribType, "") == 0))
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        
    // We need to look up the attribute in the attribute map, and then
    // return the type
    if (!g_pkCurrNSBShader)
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

    // Local attributes override global!
    g_pkCurrAttribTable = g_pkCurrNSBShader->GetAttributeTable();
    if (!g_pkCurrAttribTable)
    {
        NIASSERT(!"Invalid attribute table during parse!");
        return 0;
    }
    
    NSBAttributeDesc* pkAttribDesc = 
        g_pkCurrAttribTable->GetAttributeByName(pszAttribType);
    if (!pkAttribDesc)
    {
        g_pkCurrAttribTable = g_pkCurrNSBShader->GetGlobalAttributeTable();
        if (!g_pkCurrAttribTable)
        {
            NIASSERT(!"Invalid attribute table during parse!");
            return 0;
        }
        pkAttribDesc = g_pkCurrAttribTable->GetAttributeByName(pszAttribType);
        if (!pkAttribDesc)
        {
            NIASSERT(!"Attribute not found!");
            return 0;
        }
    }
    
    g_pkCurrAttribTable = 0;
    
    return (unsigned int)(pkAttribDesc->GetType());
}
//---------------------------------------------------------------------------
unsigned int DecodePlatformString(char* pszPlatform)
{
    if (!pszPlatform || (strcmp(pszPlatform, "") == 0))
        return 0;
        
    char acTemp[64];

    unsigned int ui = 0;
    for (; ui < strlen(pszPlatform); ui++)
        acTemp[ui] = toupper(pszPlatform[ui]);
    acTemp[ui] = 0;
    
    if (NiStricmp(acTemp, "DX9") == 0)
        return NiShader::NISHADER_DX9;
    if (NiStricmp(acTemp, "XENON") == 0)
        return NiShader::NISHADER_XENON;
    if (NiStricmp(acTemp, "PS3") == 0)
        return NiShader::NISHADER_PS3;
    if (NiStricmp(acTemp, "D3D10") == 0)
        return NiShader::NISHADER_D3D10;

    return 0;
}
//---------------------------------------------------------------------------
bool AddAttributeToConstantMap(char* pszName, 
    unsigned int uiRegisterStart, unsigned int uiRegisterCount, 
    unsigned int uiExtraNumber, bool bIsGlobal)
{
    if (!g_pkCurrConstantMap)
        return false;
    
    // Cheat to force a copy of the data
    unsigned int uiFlags = bIsGlobal
        ? NiShaderConstantMapEntry::SCME_MAP_GLOBAL
        : NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;

    unsigned int uiSize;

    // Look up the global attribute
    if (!g_pkCurrNSBShader)
    {
        return false;
    }
    else
    {
        if (bIsGlobal)
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetGlobalAttributeTable();
        }
        else
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetAttributeTable();
        }

        NSBAttributeDesc* pkAttribDesc = 
            g_pkCurrAttribTable->GetAttributeByName(pszName);

        if (!pkAttribDesc)
        {
            return false;
        }
        else
        {
            if (NiShaderConstantMapEntry::IsBool(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                uiSize = sizeof(bool);
                bool bValue;
                
                pkAttribDesc->GetValue_Bool(bValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsUnsignedInt(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                uiSize = sizeof(unsigned int);
                unsigned int uiValue;
                
                pkAttribDesc->GetValue_UnsignedInt(uiValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsFloat(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                uiSize = sizeof(float);
                float fValue;
                
                pkAttribDesc->GetValue_Float(fValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint2(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                uiSize = sizeof(NiPoint2);
                NiPoint2 kPt2Value;
                
                pkAttribDesc->GetValue_Point2(kPt2Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                uiSize = sizeof(NiPoint3);
                NiPoint3 kPt3Value;
                
                pkAttribDesc->GetValue_Point3(kPt3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                uiSize = sizeof(float) * 4;
                float afValue[4];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Point4(pfValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                uiSize = sizeof(NiMatrix3);
                NiMatrix3 kMat3Value;
                
                pkAttribDesc->GetValue_Matrix3(kMat3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                uiSize = sizeof(float) * 16;
                float afValue[16];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Matrix4(pfValue, uiSize);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsColor(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                uiSize = sizeof(NiColorA);
                NiColorA kClrValue;
                
                pkAttribDesc->GetValue_ColorA(kClrValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0,
                        uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsArray(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;

                // get description of array data
                NiShaderAttributeDesc::AttributeType eType;
                unsigned int uiElementSize;
                unsigned int uiNumElements;
                pkAttribDesc->GetArrayParams(
                    eType,
                    uiElementSize,
                    uiNumElements);

                // get copy of data
                const unsigned int uiSize = uiElementSize*uiNumElements;
                float* pfValues = NiAlloc(float,uiSize/sizeof(float));
                pkAttribDesc->GetValue_Array(pfValues,uiSize);

                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM
                        NiFree(pfValues);
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM!
                        NiFree(pfValues);
                        return false;
                    }
                }
                
                NiFree(pfValues);
                return true;
            }
            else
            {
                NIASSERT(!"Invalid Attribute Type");
                return false;
            }
        }
    }        
    
    return true;
}
//---------------------------------------------------------------------------
bool SetupOperatorEntry(char* pszName, int iRegStart, int iRegCount, 
    char* pszEntry1, int iOperation, char* pszEntry2, bool bInverse, 
    bool bTranspose)
{
    if (!g_pkCurrConstantMap)
        return false;

    // Look up the 2 entries
    NSBConstantMap::NSBCM_Entry* pkEntry1;
    NSBConstantMap::NSBCM_Entry* pkEntry2;
    
    pkEntry1 = g_pkCurrConstantMap->GetEntryByKey(pszEntry1);
    pkEntry2 = g_pkCurrConstantMap->GetEntryByKey(pszEntry2);

    if (!pkEntry1 || !pkEntry2)    
    {
        NSFParsererror("CM_Operator operand not found\n");
        return false;
    }
    if (!(pkEntry1->IsDefined() || pkEntry1->IsGlobal() ||
          pkEntry1->IsAttribute() || pkEntry1->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }
    if (!(pkEntry2->IsDefined() || pkEntry2->IsGlobal() ||
          pkEntry2->IsAttribute() || pkEntry2->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }

    unsigned int uiEntry1;
    unsigned int uiEntry2;
    
    uiEntry1 = g_pkCurrConstantMap->GetEntryIndexByKey(pszEntry1);
    uiEntry2 = g_pkCurrConstantMap->GetEntryIndexByKey(pszEntry2);

    if ((uiEntry1 == 0xffffffff) || (uiEntry2 == 0xffffffff))    
    {
        NSFParsererror("CM_Operator operand INVALID INDEX\n");
        return false;
    }

    // Determine the results data type and set it in the flags
    NiShaderAttributeDesc::AttributeType eType1 = 
        pkEntry1->GetAttributeType();
    NiShaderAttributeDesc::AttributeType eType2 = 
        pkEntry2->GetAttributeType();

    if (pkEntry1->IsDefined())
    {
        // We have to look-up the type
        if (g_eConstMapMode == CONSTMAP_VERTEX)
        {
            eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry1->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_GEOMETRY)
        {
            eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry1->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_PIXEL)
        {
            eType1 = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else
        {
            NIASSERT(!"Invalid constant map mode!");
            return false;
        }
    }
    if (pkEntry2->IsDefined())
    {
        // We have to look-up the type
        if (g_eConstMapMode == CONSTMAP_VERTEX)
        {
            eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry2->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_GEOMETRY)
        {
            eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry2->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_PIXEL)
        {
            eType2 = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else
        {
            NIASSERT(!"Invalid constant map mode!");
            return false;
        }
    }
        
    if ((eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType1) ||
        NiShaderConstantMapEntry::IsString(eType1) ||
        NiShaderConstantMapEntry::IsTexture(eType1) ||
        (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType2) ||
        NiShaderConstantMapEntry::IsString(eType2) ||
        NiShaderConstantMapEntry::IsTexture(eType2))
    {
        NSFParsererror("Invalid Operator Type");
        return false;
    }

    NiShaderAttributeDesc::AttributeType eResultType = 
        DetermineOperatorResult(iOperation, eType1, eType2);
    if (eResultType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        NSFParsererror("Invalid Operator - Result was invalid");
        return false;
    }
    
    // We have two valid entries, so let's setup the ConstantMapEntry
    // NOTE: If NSBConstantMap did not store it's list the proper way,
    // ie, via AddTail, this method would not work!
    unsigned int uiExtra = iOperation | uiEntry1 | 
        (uiEntry2 << NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT);
    if (bTranspose)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_TRANSPOSE;
    if (bInverse)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_INVERSE;

    unsigned int uiFlags = NiShaderConstantMapEntry::SCME_MAP_OPERATOR | 
        NiShaderConstantMapEntry::GetAttributeFlags(eResultType);

    if (g_bConstantMapPlatformBlock)
    {
        if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
            g_uiCurrentPlatforms, pszName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
    else
    {
        if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
        
    return true;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineOperatorResult(int iOperation, 
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (iOperation)
    {
    case NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY:
        return DetermineResultMultiply(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE:
        return DetermineResultDivide(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_ADD:
        return DetermineResultAdd(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT:
        return DetermineResultSubtract(eType1, eType2);
    default:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultDivide(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultAdd(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------

