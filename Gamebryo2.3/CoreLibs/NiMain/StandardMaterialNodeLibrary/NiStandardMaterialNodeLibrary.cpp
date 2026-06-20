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

//---------------------------------------------------------------------------
// This file has been automatically generated using the
// NiMaterialNodeXMLLibraryParser tool. It should not be directly edited.
//---------------------------------------------------------------------------

#include "NiMainPCH.h"

#include <NiMaterialFragmentNode.h>
#include <NiMaterialNodeLibrary.h>
#include <NiMaterialResource.h>
#include <NiCodeBlock.h>
#include "NiStandardMaterialNodeLibrary.h"

//---------------------------------------------------------------------------
NiMaterialNodeLibrary* 
    NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary()
{

    // Create a new NiMaterialNodeLibrary
    NiMaterialNodeLibrary* pkLib = NiNew NiMaterialNodeLibrary(1);

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("TransformNormal");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the world transf"
            "orm to the\n"
            "    normal.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Normal");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("World");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNrm");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // Transform the normal into world space for lighting\n"
                "    WorldNrm = mul( Normal, (float3x3)World );\n"
                "\n"
                "    // Should not need to normalize here since we will nor"
                "malize in the pixel \n"
                "    // shader due to linear interpolation across triangle "
                "not preserving\n"
                "    // normality.\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("TransformNBT");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the world transf"
            "orm to the\n"
            "    normal, binormal, and tangent.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Normal");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Binormal");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Binormal");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Tangent");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Tangent");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("World");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNrm");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldBinormal");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldTangent");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // Transform the normal into world space for lighting\n"
                "    WorldNrm      = mul( Normal, (float3x3)World );\n"
                "    WorldBinormal = mul( Binormal, (float3x3)World );\n"
                "    WorldTangent  = mul( Tangent, (float3x3)World );\n"
                "    \n"
                "    // Should not need to normalize here since we will nor"
                "malize in the pixel \n"
                "    // shader due to linear interpolation across triangle "
                "not preserving\n"
                "    // normality.\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("TransformPosition");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the view project"
            "ion transform\n"
            "    to the input position. Additionally, this fragment applies"
            " the world \n"
            "    transform to the input position. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Position");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("World");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // Transform the position into world space for lightin"
                "g, and projected \n"
                "    // space for display\n"
                "    WorldPos = mul( float4(Position, 1.0f), World );\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("WorldToTangent");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for transforming a vector fro"
            "m world space\n"
            "    to tangent space.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("VectorIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNormalIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Binormal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldBinormalIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Tangent");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldTangentIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource label
            pkRes->SetLabel("Tangent");

            // Set resource variable name
            pkRes->SetVariable("VectorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    float3x3 xForm = float3x3(WorldTangentIn, WorldBinorma"
                "lIn, WorldNormalIn);\n"
                "    VectorOut = mul(xForm, VectorIn.xyz);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("TransformSkinnedPosition");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the view project"
            "ion and skinning \n"
            "    transform to the input position. Additionally, this fragme"
            "nt applies the \n"
            "    computed world transform to the input position. The weight"
            "ed world \n"
            "    transform defined by the blendweights is output for use in"
            " normals or\n"
            "    other calculations as the new world matrix.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Position");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int4");

            // Set resource semantic
            pkRes->SetSemantic("BlendIndices");

            // Set resource variable name
            pkRes->SetVariable("BlendIndices");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("BlendWeight");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("BlendWeights");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x3");

            // Set resource count
            pkRes->SetCount(30);

            // Set resource semantic
            pkRes->SetSemantic("SkinBoneMatrix");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("Bones");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("SkinToWorldTransform");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("SkinBoneTransform");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // TransformSkinnedPosition **************************"
                "*******************\n"
                "    // Transform the skinned position into world space\n"
                "    // Composite the skinning transform which will take th"
                "e vertex\n"
                "    // and normal to world space.\n"
                "    float fWeight3 = 1.0 - BlendWeights[0] - BlendWeights["
                "1] - BlendWeights[2];\n"
                "    float4x3 ShortSkinBoneTransform;\n"
                "    ShortSkinBoneTransform  = Bones[BlendIndices[0]] * Ble"
                "ndWeights[0];\n"
                "    ShortSkinBoneTransform += Bones[BlendIndices[1]] * Ble"
                "ndWeights[1];\n"
                "    ShortSkinBoneTransform += Bones[BlendIndices[2]] * Ble"
                "ndWeights[2];\n"
                "    ShortSkinBoneTransform += Bones[BlendIndices[3]] * fWe"
                "ight3;\n"
                "    SkinBoneTransform = float4x4(ShortSkinBoneTransform[0]"
                ", 0.0f, \n"
                "        ShortSkinBoneTransform[1], 0.0f, \n"
                "        ShortSkinBoneTransform[2], 0.0f, \n"
                "        ShortSkinBoneTransform[3], 1.0f);\n"
                "\n"
                "    // Transform into world space.\n"
                "    WorldPos.xyz = mul(float4(Position, 1.0), ShortSkinBon"
                "eTransform);\n"
                "    WorldPos = mul(float4(WorldPos.xyz, 1.0), SkinToWorldT"
                "ransform);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("TransformSkinnedPositionNoIndices");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the view project"
            "ion and skinning \n"
            "    transform to the input position. Additionally, this fragme"
            "nt applies the \n"
            "    computed world transform to the input position. The weight"
            "ed world \n"
            "    transform defined by the blendweights is output for use in"
            " normals or\n"
            "    other calculations as the new world matrix.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("Position");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("BlendWeight");

            // Set resource label
            pkRes->SetLabel("Local");

            // Set resource variable name
            pkRes->SetVariable("BlendWeights");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x3");

            // Set resource count
            pkRes->SetCount(4);

            // Set resource semantic
            pkRes->SetSemantic("SkinBoneMatrix");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("Bones");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("SkinToWorldTransform");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("WorldMatrix");

            // Set resource variable name
            pkRes->SetVariable("SkinBoneTransform");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // TransformSkinnedPosition **************************"
                "*******************\n"
                "    // Transform the skinned position into world space\n"
                "    // Composite the skinning transform which will take th"
                "e vertex\n"
                "    // and normal to world space.\n"
                "    float fWeight3 = 1.0 - BlendWeights[0] - BlendWeights["
                "1] - BlendWeights[2];\n"
                "    float4x3 ShortSkinBoneTransform;\n"
                "    ShortSkinBoneTransform  = Bones[0] * BlendWeights[0];\n"
                "    ShortSkinBoneTransform += Bones[1] * BlendWeights[1];\n"
                "    ShortSkinBoneTransform += Bones[2] * BlendWeights[2];\n"
                "    ShortSkinBoneTransform += Bones[3] * fWeight3;\n"
                "    SkinBoneTransform = float4x4(ShortSkinBoneTransform[0]"
                ", 0.0f, \n"
                "        ShortSkinBoneTransform[1], 0.0f, \n"
                "        ShortSkinBoneTransform[2], 0.0f, \n"
                "        ShortSkinBoneTransform[3], 1.0f);\n"
                "\n"
                "    // Transform into world space.\n"
                "    WorldPos.xyz = mul(float4(Position, 1.0), ShortSkinBon"
                "eTransform);\n"
                "    WorldPos = mul(float4(WorldPos.xyz, 1.0), SkinToWorldT"
                "ransform);\n"
                "\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("ProjectPositionWorldToProj");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the view project"
            "ion transform\n"
            "    to the input world position.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPosition");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("ViewProjMatrix");

            // Set resource variable name
            pkRes->SetVariable("ViewProjection");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Proj");

            // Set resource variable name
            pkRes->SetVariable("ProjPos");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    ProjPos = mul(WorldPosition, ViewProjection);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("ProjectPositionWorldToViewToProj");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the view project"
            "ion transform\n"
            "    to the input world position.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPosition");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("ViewMatrix");

            // Set resource variable name
            pkRes->SetVariable("ViewTransform");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("ProjMatrix");

            // Set resource variable name
            pkRes->SetVariable("ProjTransform");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("View");

            // Set resource variable name
            pkRes->SetVariable("ViewPos");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Proj");

            // Set resource variable name
            pkRes->SetVariable("ProjPos");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    ViewPos = mul(WorldPosition, ViewTransform);\n"
                "    ProjPos = mul(ViewPos, ProjTransform);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("9thOrderSphericalHarmonicLighting");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for generating the diffuse \n"
            "    lighting environment as compressed in spherical harmonics.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNormal");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("SHCoefficients");

            // Set resource variable name
            pkRes->SetVariable("SHCoefficients");

            // Set resource count
            pkRes->SetCount(9);

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("DiffuseColorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    DiffuseColorOut  = SHCoefficients[0];\n"
                "    DiffuseColorOut += SHCoefficients[1] * WorldNormal.x;\n"
                "    DiffuseColorOut += SHCoefficients[2] * WorldNormal.y;\n"
                "    DiffuseColorOut += SHCoefficients[3] * WorldNormal.z;\n"
                "    DiffuseColorOut += SHCoefficients[4] * WorldNormal.x *"
                " WorldNormal.z;\n"
                "    DiffuseColorOut += SHCoefficients[5] * WorldNormal.y *"
                " WorldNormal.z;\n"
                "    DiffuseColorOut += SHCoefficients[6] * WorldNormal.x *"
                " WorldNormal.y;\n"
                "    DiffuseColorOut += SHCoefficients[7] * \n"
                "        (3.0 * WorldNormal.z * WorldNormal.z  - 1.0);\n"
                "    DiffuseColorOut += SHCoefficients[8] * \n"
                "        (WorldNormal.x * WorldNormal.x  - WorldNormal.y * "
                "WorldNormal.y);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("CalculateNormalFromColor");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for sampling a normal map to "
            "generate the\n"
            "    new world-space normal.\n"
            "    \n"
            "    The normal map type is an enumerated value that indicates "
            "the following:\n"
            "        0 - Standard (rgb = normal/binormal/tangent)\n"
            "        1 - DXN (rg = normal.xy need to calculate z)\n"
            "        2 - DXT5 (ag = normal.xy need to calculate z)\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("NormalMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNormalIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Binormal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldBinormalIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Tangent");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldTangentIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("NormalMapType");

            // Set resource variable name
            pkRes->SetVariable("NormalMapType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNormalOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    \n"
                "    NormalMap = NormalMap * 2.0 - 1.0;\n"
                "    \n"
                "    // Do nothing extra for Standard\n"
                "    // Handle compressed types:\n"
                "    if (NormalMapType == 1) // DXN\n"
                "    {\n"
                "        NormalMap.rgb = float3(NormalMap.r, NormalMap.g, \n"
                "            sqrt(1 - NormalMap.r * NormalMap.r - NormalMap"
                ".g * NormalMap.g));\n"
                "    }\n"
                "    else if (NormalMapType == 2) // DXT5\n"
                "    {\n"
                "        NormalMap.rg = NormalMap.ag;\n"
                "        NormalMap.b = sqrt(1 - NormalMap.r*NormalMap.r -  \n"
                "            NormalMap.g * NormalMap.g);\n"
                "    }\n"
                "       \n"
                "    float3x3 xForm = float3x3(WorldTangentIn, WorldBinorma"
                "lIn, WorldNormalIn);\n"
                "    xForm = transpose(xForm);\n"
                "    WorldNormalOut = mul(xForm, NormalMap.rgb);\n"
                "    \n"
                "    WorldNormalOut = normalize(WorldNormalOut);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("CalculateParallaxOffset");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the UV offset"
            " to apply\n"
            "    as a result of a parallax map.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Height");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("ParallaxOffsetScale");

            // Set resource variable name
            pkRes->SetVariable("OffsetScale");

            // Set resource default value
            pkRes->SetDefaultValue("(0.05)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("ViewVector");

            // Set resource label
            pkRes->SetLabel("Tangent");

            // Set resource variable name
            pkRes->SetVariable("TangentSpaceEyeVec");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("ParallaxOffsetUV");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // Calculate offset scaling constant bias.\n"
                "    float2 Bias = float2(OffsetScale, OffsetScale) * -0.5;\n"
                "\n"
                "    // Calculate offset\n"
                "    float2 Offset = Height.rg * OffsetScale + Bias;\n"
                "\n"
                "    // Get texcoord.\n"
                "    ParallaxOffsetUV = TexCoord + Offset * TangentSpaceEye"
                "Vec.xy;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("CalculateBumpOffset");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the UV offset"
            " to apply\n"
            "    as a result of a bump map.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("DuDv");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("BumpMatrix");

            // Set resource variable name
            pkRes->SetVariable("BumpMatrix");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("UVSet");

            // Set resource variable name
            pkRes->SetVariable("BumpOffset");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    BumpOffset.x = DuDv.x * BumpMatrix[0] + DuDv.y * BumpM"
                "atrix[2];\n"
                "    BumpOffset.y = DuDv.x * BumpMatrix[1] + DuDv.y * BumpM"
                "atrix[3];\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("OffsetUVFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying a UV offset to a"
            " texture\n"
            "    coordinate set.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOffset");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("UVSet");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    TexCoordOut = TexCoordIn + TexCoordOffset;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("OffsetUVFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying a UV offset to a"
            " texture\n"
            "    coordinate set.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOffset");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("UVSet");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    TexCoordOut = TexCoordIn + float3(TexCoordOffset.x, Te"
                "xCoordOffset.y, 0.0);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("OffsetUVFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying a UV offset to a"
            " texture\n"
            "    coordinate set.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOffset");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("UVSet");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    TexCoordOut = TexCoordIn + \n"
                "        float4(TexCoordOffset.x, TexCoordOffset.y, 0.0, 0."
                "0);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("TexTransformApply");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying a transform to t"
            "he input set\n"
            "    of texture coordinates.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("TexTransform");

            // Set resource variable name
            pkRes->SetVariable("TexTransform");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    \n"
                "    TexCoordOut = mul(float4(TexCoord.x, TexCoord.y, 0.0, "
                "1.0), TexTransform);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("ProjectTextureCoordinates");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying a projection to "
            "the input set\n"
            "    of texture coordinates.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource semantic
            pkRes->SetSemantic("TexTransform");

            // Set resource variable name
            pkRes->SetVariable("TexTransform");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoordOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "  \n"
                "    TexCoordOut = mul(float4(TexCoord, 1.0), TexTransform)"
                ";\n"
                "    \n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("TextureRGBSample");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for sampling a texture and re"
            "turning its value\n"
            "    as a RGB value.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("Sampler");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource variable name
            pkRes->SetVariable("Saturate");

            // Set resource default value
            pkRes->SetDefaultValue("(true)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("ColorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    ColorOut.rgb = tex2D(Sampler, TexCoord).rgb;\n"
                "    if (Saturate)\n"
                "    {\n"
                "        ColorOut.rgb = saturate(ColorOut.rgb);\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("TextureRGBASample");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for sampling a texture and re"
            "turning its value\n"
            "    as a RGB value and an A value.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("Sampler");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource variable name
            pkRes->SetVariable("Saturate");

            // Set resource default value
            pkRes->SetDefaultValue("(true)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("ColorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    ColorOut = tex2D(Sampler, TexCoord);\n"
                "    if (Saturate)\n"
                "    {\n"
                "        ColorOut = saturate(ColorOut);\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("TextureRGBProjectSample");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for sampling a texture and re"
            "turning its value\n"
            "    as a RGB value and an A value.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("Sampler");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource variable name
            pkRes->SetVariable("Saturate");

            // Set resource default value
            pkRes->SetDefaultValue("(true)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("ColorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    float4 ProjTexCoord = TexCoord.xyzz;\n"
                "    ColorOut.rgb = tex2Dproj(Sampler, ProjTexCoord).rgb;\n"
                "    if (Saturate)\n"
                "    {\n"
                "        ColorOut.rgb = saturate(ColorOut.rgb);\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("TextureRGBCubeSample");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for sampling a texture and re"
            "turning its value\n"
            "    as a RGB value.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("TexCoord");

            // Set resource variable name
            pkRes->SetVariable("TexCoord");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("samplerCUBE");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("Sampler");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource variable name
            pkRes->SetVariable("Saturate");

            // Set resource default value
            pkRes->SetDefaultValue("(true)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("ColorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    ColorOut.rgb = texCUBE(Sampler, TexCoord).rgb;\n"
                "    if (Saturate)\n"
                "    {\n"
                "        ColorOut.rgb = saturate(ColorOut.rgb);\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("CalculateFog");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for handling fogging calculat"
            "ions.\n"
            "    FogType can be one of 4 values:\n"
            "    \n"
            "        NONE   - 0\n"
            "        EXP    - 1\n"
            "        EXP2   - 2\n"
            "        LINEAR - 3\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("FogType");

            // Set resource variable name
            pkRes->SetVariable("FogType");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("FogDensity");

            // Set resource variable name
            pkRes->SetVariable("FogDensity");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("View");

            // Set resource variable name
            pkRes->SetVariable("ViewPosition");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource semantic
            pkRes->SetSemantic("FogRange");

            // Set resource variable name
            pkRes->SetVariable("FogRange");

            // Set resource default value
            pkRes->SetDefaultValue("(false)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource semantic
            pkRes->SetSemantic("FogStartEnd");

            // Set resource variable name
            pkRes->SetVariable("FogStartEnd");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Fog");

            // Set resource variable name
            pkRes->SetVariable("FogOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    float d;\n"
                "    if (FogRange)\n"
                "    {\n"
                "        d = length(ViewPosition);\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        d = ViewPosition.z;\n"
                "    }\n"
                "    \n"
                "    if (FogType == 0) // NONE\n"
                "    {\n"
                "        FogOut = 1.0;\n"
                "    }\n"
                "    else if (FogType == 1) // EXP\n"
                "    {\n"
                "        FogOut = 1.0 / exp( d * FogDensity);\n"
                "    }\n"
                "    else if (FogType == 2) // EXP2\n"
                "    {\n"
                "        FogOut = 1.0 / exp( pow( d * FogDensity, 2));\n"
                "    }\n"
                "    else if (FogType == 3) // LINEAR\n"
                "    {\n"
                "        FogOut = saturate( (FogStartEnd.y - d) / \n"
                "            (FogStartEnd.y - FogStartEnd.x));\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("WorldReflect");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for computing the reflection "
            "vector.\n"
            "    The WorldViewVector is negated because the HLSL \"reflect\""
            " function\n"
            "    expects a world-to-camera vector, rather than a camera-to-"
            "world vector.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNrm");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("ViewVector");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldViewVector");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource semantic
            pkRes->SetSemantic("NormalizeNormal");

            // Set resource variable name
            pkRes->SetVariable("NormalizeNormal");

            // Set resource default value
            pkRes->SetDefaultValue("(true)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("WorldReflect");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    if (NormalizeNormal)\n"
                "        WorldNrm = normalize(WorldNrm);\n"
                "    WorldReflect = reflect(-WorldViewVector, WorldNrm);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("CalculateViewVector");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the camera vi"
            "ew vector.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("CameraPosition");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("CameraPos");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("ViewVector");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldViewVector");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    WorldViewVector = CameraPos - WorldPos;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("ClippingPlaneTest");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating whether or no"
            "t the current\n"
            "    position is on the positive or negative side of a clipping"
            " plane. If the\n"
            "    point is on the positive side, the return value will be 1."
            "0. If the value \n"
            "    is on the negative side, the return value will be 0.0. If "
            "the point is on\n"
            "    the plane, the return value will be 0.0. If InvertClip is "
            "true, then the\n"
            "    fragment returns the opposite of the above.\n"
            "    \n"
            "    A plane is assumed to be the points X satisfying the expre"
            "ssion:\n"
            "        X * normal = plane_constant\n"
            "        \n"
            "    The WorldClipPlane must match the form of NiPlane, which i"
            "s of the form:\n"
            "        (normal.x, normal.y, normal.z, plane_constant)\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("ClippingPlane");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldClipPlane");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource semantic
            pkRes->SetSemantic("InvertClip");

            // Set resource variable name
            pkRes->SetVariable("InvertClip");

            // Set resource default value
            pkRes->SetDefaultValue("(false)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Scale");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("Scalar");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("       \n"
                "    float fDistance = dot(WorldClipPlane.xyz, WorldPos.xyz"
                ") - WorldClipPlane.w;\n"
                "    if (InvertClip)\n"
                "        Scalar = fDistance > 0.0 ? 0.0 : 1.0;\n"
                "    else\n"
                "        Scalar = fDistance > 0.0 ? 1.0 : 0.0;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("Light");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for accumulating the effect o"
            "f a light\n"
            "    on the current pixel.\n"
            "    \n"
            "    LightType can be one of three values:\n"
            "        0 - Directional\n"
            "        1 - Point \n"
            "        2 - Spot\n"
            "        \n"
            "    Note that the LightType must be a compile-time variable,\n"
            "    not a runtime constant/uniform variable on most Shader Mod"
            "el 2.0 cards.\n"
            "    \n"
            "    The compiler will optimize out any constants that aren't u"
            "sed.\n"
            "    \n"
            "    Attenuation is defined as (const, linear, quad, range).\n"
            "    Range is not implemented at this time.\n"
            "    \n"
            "    SpotAttenuation is stored as (cos(theta/2), cos(phi/2), fa"
            "lloff)\n"
            "    theta is the angle of the inner cone and phi is the angle "
            "of the outer\n"
            "    cone in the traditional DX manner. Gamebryo only allows se"
            "tting of\n"
            "    phi, so cos(theta/2) will typically be cos(0) or 1. To dis"
            "able spot\n"
            "    effects entirely, set cos(theta/2) and cos(phi/2) to -1 or"
            " lower.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Normal");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldNrm");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource semantic
            pkRes->SetSemantic("Specularity");

            // Set resource variable name
            pkRes->SetVariable("SpecularEnable");

            // Set resource default value
            pkRes->SetDefaultValue("(false)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("Shadow");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("ViewVector");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldViewVector");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("LightPos");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("LightPos");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightAmbient");

            // Set resource variable name
            pkRes->SetVariable("LightAmbient");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightDiffuse");

            // Set resource variable name
            pkRes->SetVariable("LightDiffuse");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightSpecular");

            // Set resource variable name
            pkRes->SetVariable("LightSpecular");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightAttenuation");

            // Set resource variable name
            pkRes->SetVariable("LightAttenuation");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 1.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightSpotAttenuation");

            // Set resource variable name
            pkRes->SetVariable("LightSpotAttenuation");

            // Set resource default value
            pkRes->SetDefaultValue("(-1.0, -1.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightDirection");

            // Set resource variable name
            pkRes->SetVariable("LightDirection");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Specular");

            // Set resource variable name
            pkRes->SetVariable("SpecularPower");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Ambient");

            // Set resource variable name
            pkRes->SetVariable("AmbientAccum");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Diffuse");

            // Set resource variable name
            pkRes->SetVariable("DiffuseAccum");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Specular");

            // Set resource variable name
            pkRes->SetVariable("SpecularAccum");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Ambient");

            // Set resource variable name
            pkRes->SetVariable("AmbientAccumOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Diffuse");

            // Set resource variable name
            pkRes->SetVariable("DiffuseAccumOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource label
            pkRes->SetLabel("Specular");

            // Set resource variable name
            pkRes->SetVariable("SpecularAccumOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("   \n"
                "    // Get the world space light vector.\n"
                "    float3 LightVector;\n"
                "    float DistanceToLight;\n"
                "    float DistanceToLightSquared;\n"
                "        \n"
                "    if (LightType == 0)\n"
                "    {\n"
                "        LightVector = -LightDirection;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        LightVector = LightPos - WorldPos;\n"
                "        DistanceToLightSquared = dot(LightVector, LightVec"
                "tor);\n"
                "        DistanceToLight = length(LightVector);\n"
                "        LightVector = normalize(LightVector);\n"
                "    }\n"
                "    \n"
                "    // Take N dot L as intensity.\n"
                "    float LightNDotL = dot(LightVector, WorldNrm);\n"
                "    float LightIntensity = max(0, LightNDotL);\n"
                "\n"
                "    float Attenuate = Shadow;\n"
                "    \n"
                "    if (LightType != 0)\n"
                "    {\n"
                "        // Attenuate Here\n"
                "        Attenuate = LightAttenuation.x +\n"
                "            LightAttenuation.y * DistanceToLight +\n"
                "            LightAttenuation.z * DistanceToLightSquared;\n"
                "        Attenuate = max(1.0, Attenuate);\n"
                "        Attenuate = 1.0 / Attenuate;\n"
                "        Attenuate *= Shadow;\n"
                "\n"
                "        if (LightType == 2)\n"
                "        {\n"
                "            // Get intensity as cosine of light vector and"
                " direction.\n"
                "            float CosAlpha = dot(-LightVector, LightDirect"
                "ion);\n"
                "\n"
                "            // Factor in inner and outer cone angles.\n"
                "            CosAlpha = saturate((CosAlpha - LightSpotAtten"
                "uation.y) / \n"
                "                (LightSpotAttenuation.x - LightSpotAttenua"
                "tion.y));\n"
                "\n"
                "            // Power to falloff.\n"
                "            CosAlpha = pow(CosAlpha, LightSpotAttenuation."
                "z);\n"
                "\n"
                "            // Multiply the spot attenuation into the over"
                "all attenuation.\n"
                "            Attenuate *= CosAlpha;\n"
                "        }\n"
                "\n"
                "    }\n"
                "    // Determine the interaction of diffuse color of light"
                " and material.\n"
                "    // Scale by the attenuated intensity.\n"
                "    DiffuseAccumOut = DiffuseAccum;\n"
                "    DiffuseAccumOut.rgb += LightDiffuse.rgb * LightIntensi"
                "ty * Attenuate;\n"
                "\n"
                "    // Determine ambient contribution - Is affected by sha"
                "dow\n"
                "    AmbientAccumOut = AmbientAccum;\n"
                "    AmbientAccumOut.rgb += LightAmbient.rgb * Attenuate;\n"
                "\n"
                "    SpecularAccumOut = SpecularAccum;\n"
                "    if (SpecularEnable)\n"
                "    {\n"
                "        // Get the half vector.\n"
                "        float3 LightHalfVector = LightVector + WorldViewVe"
                "ctor;\n"
                "        LightHalfVector = normalize(LightHalfVector);\n"
                "\n"
                "        // Determine specular intensity.\n"
                "        float LightNDotH = max(0, dot(WorldNrm, LightHalfV"
                "ector));\n"
                "        float LightSpecIntensity = pow(LightNDotH, Specula"
                "rPower.x);\n"
                "        \n"
                "        //if (LightNDotL < 0.0)\n"
                "        //    LightSpecIntensity = 0.0;\n"
                "        // Must use the code below rather than code above.\n"
                "        // Using previous lines will cause the compiler to"
                " generate incorrect\n"
                "        // output.\n"
                "        float SpecularMultiplier = LightNDotL > 0.0 ? 1.0 "
                ": 0.0;\n"
                "        \n"
                "        // Attenuate Here\n"
                "        LightSpecIntensity = LightSpecIntensity * Attenuat"
                "e * \n"
                "            SpecularMultiplier;\n"
                "        \n"
                "        // Determine the interaction of specular color of "
                "light and material.\n"
                "        // Scale by the attenuated intensity.\n"
                "        SpecularAccumOut.rgb += LightSpecIntensity * Light"
                "Specular;\n"
                "    }       \n"
                "\n"
                "    \n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("ApplyFog");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for applying the fog based on"
            " the \n"
            "    calculations in the vertex shader.\n"
            "    \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("UnfoggedColor");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("FogColor");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Fog");

            // Set resource variable name
            pkRes->SetVariable("FogAmount");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("FoggedColor");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    FoggedColor = lerp(FogColor, UnfoggedColor, FogAmount)"
                ";\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex");

        // Set the fragment name
        pkFrag->SetName("ApplyAlphaTest");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for alpha testing based on th"
            "e alpha\n"
            "    reference value and alpha test function.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Alpha");

            // Set resource variable name
            pkRes->SetVariable("AlphaTestValue");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Function");

            // Set resource variable name
            pkRes->SetVariable("AlphaTestFunction");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Fog");

            // Set resource variable name
            pkRes->SetVariable("AlphaTestRef");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    // These tests will evaluate to less than 0 if they ar"
                "e true.\n"
                "    float fLessTest = (AlphaTestValue - AlphaTestRef);\n"
                "    float fGreaterTest = -fLessTest;\n"
                "    float fEqualTest = 0.0;\n"
                "    if (fLessTest == 0.0)\n"
                "        fEqualTest = -1.0;\n"
                "\n"
                "    // Clip if AlphaTestValue < AlphaTestRef for:\n"
                "    //  TEST_LESS \n"
                "    //  TEST_EQUAL \n"
                "    //  TEST_LESSEQUAL \n"
                "    clip(fGreaterTest * AlphaTestFunction.x);\n"
                "    \n"
                "    // Clip if AlphaTestValue > AlphaTestRef for:\n"
                "    //  TEST_EQUAL \n"
                "    //  TEST_GREATER \n"
                "    //  TEST_GREATEREQUAL \n"
                "    clip(fLessTest * AlphaTestFunction.y);\n"
                "    \n"
                "    // Clip if AlphaTestValue == AlphaTestRef for:\n"
                "    //  TEST_LESS \n"
                "    //  TEST_GREATER \n"
                "    //  TEST_NOTEQUAL \n"
                "    clip(fEqualTest * AlphaTestFunction.z);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("ComputeShadingCoefficients");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for computing the coefficient"
            "s for the \n"
            "    following equations:\n"
            "    \n"
            "    Kdiffuse = MatEmissive + \n"
            "        MatAmbient * Summation(0...N){LightAmbientContribution"
            "[N]} + \n"
            "        MatDiffuse * Summation(0..N){LightDiffuseContribution["
            "N]}\n"
            "        \n"
            "    Kspecular = MatSpecular * Summation(0..N){LightSpecularCon"
            "tribution[N]}\n"
            "    \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("MatEmissive");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("MatDiffuse");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("MatAmbient");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("MatSpecular");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("LightSpecularAccum");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("LightDiffuseAccum");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("LightAmbientAccum");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("bool");

            // Set resource variable name
            pkRes->SetVariable("Saturate");

            // Set resource default value
            pkRes->SetDefaultValue("(true)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Diffuse");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Specular");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Diffuse = MatEmissive + MatAmbient * LightAmbientAccum"
                " + \n"
                "        MatDiffuse * LightDiffuseAccum;\n"
                "    Specular = MatSpecular * LightSpecularAccum;\n"
                "    \n"
                "    if (Saturate)\n"
                "    {\n"
                "        Diffuse = saturate(Diffuse);\n"
                "        Specular = saturate(Specular);\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("SplitColorAndOpacity");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    Separate a float4 into a float3 and a float.   \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("ColorAndOpacity");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Color");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Opacity");

            // Set resource variable name
            pkRes->SetVariable("Opacity");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Color.rgb = ColorAndOpacity.rgb;\n"
                "    Opacity = ColorAndOpacity.a;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("SplitRGBA");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    Separate a float4 into 4 floats.   \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("ColorAndOpacity");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Red");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Green");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Blue");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Opacity");

            // Set resource variable name
            pkRes->SetVariable("Alpha");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Red = ColorAndOpacity.r;\n"
                "    Green = ColorAndOpacity.g;\n"
                "    Blue = ColorAndOpacity.b;\n"
                "    Alpha = ColorAndOpacity.a;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("CompositeFinalRGBColor");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for computing the final RGB c"
            "olor.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("DiffuseColor");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("SpecularColor");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("OutputColor");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    OutputColor.rgb = DiffuseColor.rgb + SpecularColor.rgb"
                ";\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("CompositeFinalRGBAColor");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for computing the final RGBA "
            "color.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("FinalColor");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("FinalOpacity");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("OutputColor");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    OutputColor.rgb = FinalColor.rgb;\n"
                "    OutputColor.a = saturate(FinalOpacity);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("AddFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for adding two float4's. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 + V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("AddFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for adding two float3's. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 + V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("AddFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for adding two float2's. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 + V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("AddFloat");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for adding two floats. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 + V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("MultiplyFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for multiplying two float4's."
            " \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 * V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("MultiplyFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for multiplying two float3's."
            " \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 * V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("MultiplyFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for multiplying two float2's."
            " \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 * V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("MultiplyFloat");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for multiplying two floats. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 * V2;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("ScaleFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for scaling a float4 by a con"
            "stant. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Scale");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Scale * V1;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("ScaleFloat3");

        // Set the fragment description
        pkFrag->SetDescription("    \n"
            "    This fragment is responsible for scaling a float3 by a con"
            "stant. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Scale");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Scale * V1;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("ScaleFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for scaling a float2 by a con"
            "stant. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Scale");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Scale * V1;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("SaturateFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for saturating a float4. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = saturate(V1);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("SaturateFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for saturating a float3. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = saturate(V1);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("SaturateFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for saturating a float2. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = saturate(V1);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("SaturateFloat");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for saturating a float. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = saturate(V1);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("LerpFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for linearly interpolating tw"
            "o float4's. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("LerpAmount");

            // Set resource default value
            pkRes->SetDefaultValue("(0.5)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = lerp(V1, V2, LerpAmount);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("LerpFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for linearly interpolating tw"
            "o float3's. \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("LerpAmount");

            // Set resource default value
            pkRes->SetDefaultValue("(0.5)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = lerp(V1, V2, LerpAmount);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("LerpFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for linearly interpolating tw"
            "o float2's.   \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("LerpAmount");

            // Set resource default value
            pkRes->SetDefaultValue("(0.5)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = lerp(V1, V2, LerpAmount);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("LerpFloat");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for linearly interpolating tw"
            "o floats.   \n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("LerpAmount");

            // Set resource default value
            pkRes->SetDefaultValue("(0.5)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = lerp(V1, V2, LerpAmount);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("NormalizeFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for normalizing a float4.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("VectorIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("VectorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    VectorOut = normalize(VectorIn);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("NormalizeFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for normalizing a float3.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("VectorIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("VectorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    VectorOut = normalize(VectorIn);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("NormalizeFloat2");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for normalizing a float2.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("VectorIn");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("VectorOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    VectorOut = normalize(VectorIn);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("MultiplyScalarSatAddFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for accumulating into the cur"
            "rent color by\n"
            "    multiplying the sampled texture color with the current col"
            "or. The \n"
            "    original input alpha channel is preserved. The scalar valu"
            "e is added to the\n"
            "    sampled color value and then clamped to the range [0.0, 1."
            "0].\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Scalar");

            // Set resource variable name
            pkRes->SetVariable("Scalar");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = V1 * saturate(V2.rgb + Scalar.rrr);\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("MultiplyAddFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment implements the equation:\n"
            "    Output = (V1 * V2) + V3\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("V1");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("V2");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Scalar");

            // Set resource variable name
            pkRes->SetVariable("V3");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = (V1 * V2) + V3;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("FloatToFloat3");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment implements the operation:\n"
            "    Output = float3(Input, Input, Input);\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Input");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Input.rrr;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("FloatToFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment implements the operation:\n"
            "    Output = float4(Input, Input, Input, Input);\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("Input");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Input.rrrr;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("Float4ToFloat");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment implements the operation:\n"
            "    Output = Input.r;\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Input");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Input.r;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Vertex/Pixel");

        // Set the fragment name
        pkFrag->SetName("Float3ToFloat");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment implements the operation:\n"
            "    Output = Input.r;\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource variable name
            pkRes->SetVariable("Input");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Color");

            // Set resource variable name
            pkRes->SetVariable("Output");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    Output = Input.r;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("ShadowMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource variable name
            pkRes->SetVariable("WorldToLightProjMat");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    \n"
                "    if ( LightType != 0)\n"
                "    {\n"
                "      // This fragment only supports directional lights.\n"
                "        ShadowOut = 0.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float4 LightProjPos = mul(WorldPos, WorldToLightPr"
                "ojMat);\n"
                "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightP"
                "rojPos.w + \n"
                "                  float2( 0.5, 0.5 );\n"
                "    \n"
                "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "\n"
                "        float ShadowLookup = tex2D(ShadowMap, ShadowTexC.x"
                "y).r;\n"
                "        float LightSpaceDepth = (LightProjPos.z / LightPro"
                "jPos.w) ;\n"
                "        float2 borderTest = saturate(ShadowTexC) - ShadowT"
                "exC;\n"
                "\n"
                "        if ( (ShadowLookup > LightSpaceDepth - ShadowBias)"
                " || \n"
                "            any(borderTest))\n"
                "        {\n"
                "            ShadowOut = 1.0;\n"
                "        }\n"
                "        else\n"
                "        {\n"
                "            ShadowOut = 0.0;\n"
                "        }\n"
                "        \n"
                "       }\n"
                "\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("VSMShadowMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource variable name
            pkRes->SetVariable("WorldToLightProjMat");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowVSMPowerEpsilon");

            // Set resource default value
            pkRes->SetDefaultValue("(10.0, 0.001)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    \n"
                "    if ( LightType != 0)\n"
                "    {\n"
                "      // This fragment only supports directional lights.\n"
                "        ShadowOut = 0.0;\n"
                "    }\n"
                "    else\n"
                "    { \n"
                "        float4 LightProjPos = mul(WorldPos, WorldToLightPr"
                "ojMat);\n"
                "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightP"
                "rojPos.w + \n"
                "            float2( 0.5, 0.5 );\n"
                "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "        \n"
                "        float LightSpaceDepth = (LightProjPos.z / LightPro"
                "jPos.w);\n"
                "        float2 borderTest = saturate(ShadowTexC) - ShadowT"
                "exC;\n"
                "\n"
                "        float4 vVSM   = tex2D( ShadowMap, ShadowTexC.xy );\n"
                "        float  fAvgZ  = vVSM.r; // Filtered z\n"
                "        float  fAvgZ2 = vVSM.g; // Filtered z-squared\n"
                "        \n"
                "        // Standard shadow map comparison\n"
                "        if((LightSpaceDepth) - ShadowBias <= fAvgZ || any("
                "borderTest))\n"
                "        {\n"
                "            ShadowOut = 1.0f;\n"
                "        }\n"
                "        else\n"
                "        {            \n"
                "            // Use variance shadow mapping to compute the "
                "maximum probability \n"
                "            // that the pixel is in shadow\n"
                "            float variance = ( fAvgZ2 ) - ( fAvgZ * fAvgZ "
                ");\n"
                "            variance       = \n"
                "                min( 1.0f, max( 0.0f, variance + ShadowVSM"
                "PowerEpsilon.y ) );\n"
                "            ShadowOut = variance;\n"
                "            \n"
                "            float mean     = fAvgZ;\n"
                "            float d        = LightSpaceDepth - mean;\n"
                "            float p_max    = variance / ( variance + d*d )"
                ";\n"
                "            \n"
                "            ShadowOut = pow( p_max, ShadowVSMPowerEpsilon."
                "x);\n"
                "        }\n"
                "    }\n"
                "\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("PCFShadowMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource variable name
            pkRes->SetVariable("WorldToLightProjMat");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("DX9");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    \n"
                "    if ( LightType != 0)\n"
                "    {\n"
                "      // This fragment only supports directional lights.\n"
                "        ShadowOut = 0.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float4 LightProjPos = mul(WorldPos, WorldToLightPr"
                "ojMat);\n"
                "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightP"
                "rojPos.w + \n"
                "                  float2( 0.5, 0.5 );\n"
                "    \n"
                "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "    \n"
                "        float ShadowMapSizeInverse = 1.0f / ShadowMapSize;\n"
                "        float2 lerps = frac( ShadowTexC * ShadowMapSize);\n"
                "        float LightSpaceDepth = (LightProjPos.z / LightPro"
                "jPos.w);\n"
                "        float2 borderTest = saturate(ShadowTexC) - ShadowT"
                "exC;\n"
                "\n"
                "        if (any(borderTest))\n"
                "        {\n"
                "            ShadowOut = 1.0;\n"
                "        }\n"
                "        else\n"
                "        {\n"
                "            LightSpaceDepth -= ShadowBias;\n"
                "                   \n"
                "            float4 SourceVals;\n"
                "            SourceVals.x = tex2D( ShadowMap, ShadowTexC )."
                "r;\n"
                "    \n"
                "            ShadowTexC.x += ShadowMapSizeInverse;\n"
                "            SourceVals.y = tex2D( ShadowMap, ShadowTexC).r"
                ";  \n"
                "    \n"
                "            ShadowTexC.y += ShadowMapSizeInverse;         "
                "       \n"
                "            SourceVals.w = tex2D( ShadowMap, ShadowTexC )."
                "r;\n"
                "                \n"
                "            ShadowTexC.x -= ShadowMapSizeInverse;         "
                "       \n"
                "            SourceVals.z = tex2D( ShadowMap, ShadowTexC).r"
                ";\n"
                "                \n"
                "            float4 Shade = (LightSpaceDepth < SourceVals);\n"
                "    \n"
                "            // lerp between the shadow values to calculate"
                " our light amount\n"
                "            ShadowOut = lerp(\n"
                "                lerp(Shade.x, Shade.y, lerps.x),\n"
                "                lerp(Shade.z, Shade.w, lerps.x), lerps.y )"
                ";        \n"
                "        }\n"
                "    }\n"
                "\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("DX9/D3D10/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_3_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    \n"
                "    if ( LightType != 0)\n"
                "    {\n"
                "      // This fragment only supports directional lights.\n"
                "        ShadowOut = 0.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float4 LightProjPos = mul(WorldPos, WorldToLightPr"
                "ojMat);\n"
                "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightP"
                "rojPos.w + \n"
                "                  float2( 0.5, 0.5 );\n"
                "        \n"
                "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "        \n"
                "        float ShadowMapSizeInverse = 1.0f / ShadowMapSize;\n"
                "        float LightSpaceDepth = (LightProjPos.z / LightPro"
                "jPos.w);\n"
                "        float2 borderTest = saturate(ShadowTexC) - ShadowT"
                "exC;\n"
                "\n"
                "        if (any(borderTest))\n"
                "        {\n"
                "            ShadowOut = 1.0;\n"
                "        }\n"
                "        else\n"
                "        {    \n"
                "            LightSpaceDepth -= ShadowBias;\n"
                "        \n"
                "            ShadowOut = 0;\n"
                "            float4 fOnes = float4(1.0, 1.0, 1.0, 1.0);\n"
                "            float4 fKernels = float4(1.0, 1.0, 1.0, 1.0);\n"
                "            float faKernels[4] = {1.0, 1.0, 1.0, 1.0};\n"
                "            \n"
                "            faKernels[0] = 1-frac(ShadowTexC.y * ShadowMap"
                "Size.y);\n"
                "            faKernels[3]= frac(ShadowTexC.y  * ShadowMapSi"
                "ze.y); \n"
                "            \n"
                "            fKernels.x = 1-frac(ShadowTexC.x * ShadowMapSi"
                "ze.x);\n"
                "            fKernels.w = frac(ShadowTexC.x  * ShadowMapSiz"
                "e.x);\n"
                "                      \n"
                "            float fTotalPercent = 0;\n"
                "            \n"
                "            // This loop is manually unrolled here to avoi"
                "d long\n"
                "            // shader compilation times.\n"
                "            //for (int i=0; i < 4; i++)\n"
                "            // i == 0\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC;                \n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth < shadowMap"
                "Depth);\n"
                "    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[0];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[0];\n"
                "            }\n"
                "            // i == 1\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC;                \n"
                "                pos.y += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth < shadowMap"
                "Depth);\n"
                "    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[1];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[1];\n"
                "            }\n"
                "            // i == 2\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC;                \n"
                "                pos.y += 2 * ShadowMapSizeInverse;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth < shadowMap"
                "Depth);\n"
                "    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[2];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[2];\n"
                "            }\n"
                "            // i == 3\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC;                \n"
                "                pos.y += 3 * ShadowMapSizeInverse;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth < shadowMap"
                "Depth);\n"
                "    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[3];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[3];\n"
                "            }\n"
                "            \n"
                "            ShadowOut = ShadowOut / fTotalPercent;        "
                " \n"
                "           }\n"
                "       }\n"
                "\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("SpotShadowMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightDirection");

            // Set resource variable name
            pkRes->SetVariable("LightDirection");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Light");

            // Set resource variable name
            pkRes->SetVariable("LightPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("CosOfCutoff");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource variable name
            pkRes->SetVariable("WorldToLightProjMat");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    if (LightType != 2)\n"
                "    {\n"
                "        // This version only supports spotlights.\n"
                "        ShadowOut = 1.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float3 LightToWorldPos = normalize(float3(WorldPos"
                " - LightPos));\n"
                "\n"
                "        if (dot(LightDirection, LightToWorldPos) > CosOfCu"
                "toff)\n"
                "        {\n"
                "            float4 LightProjPos = mul(WorldPos, WorldToLig"
                "htProjMat);\n"
                "\n"
                "            float3 ShadowTexC = LightProjPos.xyz / LightPr"
                "ojPos.w;               \n"
                "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2"
                "( 0.5, 0.5 );\n"
                "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "\n"
                "            float LightSpaceDepth = clamp(ShadowTexC.z - S"
                "hadowBias, \n"
                "                0.0, 1.0);\n"
                "            float ShadowLookup = tex2D(ShadowMap, ShadowTe"
                "xC.xy).r;                \n"
                "            ShadowOut = ShadowLookup < LightSpaceDepth ? 0"
                ".0 : 1.0;\n"
                "        }\n"
                "        else\n"
                "        {\n"
                "            // Outside of the light cone is shadowed compl"
                "etely\n"
                "            ShadowOut = 0.0;\n"
                "        }\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("SpotVSMShadowMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightDirection");

            // Set resource variable name
            pkRes->SetVariable("LightDirection");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Light");

            // Set resource variable name
            pkRes->SetVariable("LightPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("CosOfCutoff");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource variable name
            pkRes->SetVariable("WorldToLightProjMat");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowVSMPowerEpsilon");

            // Set resource default value
            pkRes->SetDefaultValue("(10.0, 0.001)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    if (LightType != 2)\n"
                "    {\n"
                "        // This version only supports spotlights.\n"
                "        ShadowOut = 1.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float3 LightToWorldPos = normalize(float3(WorldPos"
                " - LightPos));\n"
                "\n"
                "        if (dot(LightDirection, LightToWorldPos) > CosOfCu"
                "toff)\n"
                "        {\n"
                "            float4 LightProjPos = mul(WorldPos, WorldToLig"
                "htProjMat);\n"
                "            float3 ShadowTexC = LightProjPos.xyz / LightPr"
                "ojPos.w;               \n"
                "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2"
                "( 0.5, 0.5 );\n"
                "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "            float LightSpaceDepth = clamp(ShadowTexC.z - S"
                "hadowBias, \n"
                "                0.0, 1.0);\n"
                "                                                        \n"
                "            float4 vVSM   = tex2D( ShadowMap, ShadowTexC.x"
                "y );\n"
                "            float  fAvgZ  = vVSM.r; // Filtered z\n"
                "            float  fAvgZ2 = vVSM.g; // Filtered z-squared\n"
                "            \n"
                "            // Standard shadow map comparison\n"
                "            if( (LightSpaceDepth - ShadowBias) <= fAvgZ)\n"
                "            {\n"
                "                ShadowOut = 1.0f;\n"
                "            }\n"
                "            else\n"
                "            {               \n"
                "                // Use variance shadow mapping to compute "
                "the maximum \n"
                "                // probability that the pixel is in shadow\n"
                "                float variance = ( fAvgZ2 ) - ( fAvgZ * fA"
                "vgZ );\n"
                "                variance       = \n"
                "                    min( 1.0f, max( 0.0f, variance + Shado"
                "wVSMPowerEpsilon.y));\n"
                "                ShadowOut = variance;\n"
                "                \n"
                "                float mean     = fAvgZ;\n"
                "                float d        = LightSpaceDepth - mean;\n"
                "                float p_max    = variance / ( variance + d"
                "*d );\n"
                "                \n"
                "                ShadowOut = pow( p_max, ShadowVSMPowerEpsi"
                "lon.x);\n"
                "            }\n"
                "\n"
                "        }\n"
                "        else\n"
                "        {\n"
                "            // Outside of the light cone is shadowed compl"
                "etely\n"
                "            ShadowOut = 0.0;\n"
                "        }\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("ShadowCubeMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("samplerCUBE");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Light");

            // Set resource variable name
            pkRes->SetVariable("LightPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    if (LightType == 0 || LightType == 2)\n"
                "    {\n"
                "        // This version only supports point lights.\n"
                "        ShadowOut = 1.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float3 ViewVector = WorldPos - LightPos;    \n"
                "        ViewVector.z = -ViewVector.z;\n"
                "        float3 ViewVectorNrm = normalize(ViewVector);\n"
                "\n"
                "        float fDepth = dot(ViewVector, ViewVector);\n"
                "        fDepth = sqrt(fDepth) * ShadowBias;\n"
                "        float ShadowLookup = texCUBE(ShadowMap, ViewVector"
                "Nrm).r;\n"
                "         \n"
                "        if (ShadowLookup == 0 || ShadowLookup > fDepth )\n"
                "            ShadowOut = 1.0;\n"
                "        else\n"
                "            ShadowOut = 0.0;\n"
                "    }\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    // Create a new NiMaterialNode based on a fragment
    {
        NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

        // Set the fragment type
        pkFrag->SetType("Pixel");

        // Set the fragment name
        pkFrag->SetName("SpotPCFShadowMap");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for calculating the Shadow co"
            "efficient.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("World");

            // Set resource variable name
            pkRes->SetVariable("WorldPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("int");

            // Set resource semantic
            pkRes->SetSemantic("LightType");

            // Set resource variable name
            pkRes->SetVariable("LightType");

            // Set resource default value
            pkRes->SetDefaultValue("(0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("sampler2D");

            // Set resource semantic
            pkRes->SetSemantic("Texture");

            // Set resource variable name
            pkRes->SetVariable("ShadowMap");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float2");

            // Set resource variable name
            pkRes->SetVariable("ShadowMapSize");

            // Set resource default value
            pkRes->SetDefaultValue("(512,512)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("LightDirection");

            // Set resource variable name
            pkRes->SetVariable("LightDirection");

            // Set resource default value
            pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float3");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Light");

            // Set resource variable name
            pkRes->SetVariable("LightPos");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("CosOfCutoff");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4x4");

            // Set resource variable name
            pkRes->SetVariable("WorldToLightProjMat");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("ShadowBias");

            // Set resource default value
            pkRes->SetDefaultValue("(0.0005)");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource semantic
            pkRes->SetSemantic("Shadow");

            // Set resource variable name
            pkRes->SetVariable("ShadowOut");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("DX9");

            // Set code block compile target
            pkBlock->SetTarget("ps_2_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    if (LightType != 2)\n"
                "    {\n"
                "        // This version only supports spotlights.\n"
                "        ShadowOut = 1.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float3 LightToWorldPos = normalize(float3(WorldPos"
                " - LightPos));\n"
                "\n"
                "        if (dot(LightDirection, LightToWorldPos) > CosOfCu"
                "toff)\n"
                "        {\n"
                "            float4 LightProjPos = mul(WorldPos, WorldToLig"
                "htProjMat);\n"
                "            float3 ShadowTexC = LightProjPos.xyz / LightPr"
                "ojPos.w;               \n"
                "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2"
                "( 0.5, 0.5 );\n"
                "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "            float LightSpaceDepth = clamp(ShadowTexC.z - S"
                "hadowBias, \n"
                "                0.0, 1.0);\n"
                "            float2 ShadowMapSizeInverse = 1.0 / ShadowMapS"
                "ize;\n"
                "            float2 lerps = frac( ShadowTexC.xy * ShadowMap"
                "Size);\n"
                "\n"
                "            float4 SourceVals;\n"
                "            SourceVals.x = tex2D( ShadowMap, ShadowTexC.xy"
                " ).r;\n"
                "\n"
                "            ShadowTexC.x += ShadowMapSizeInverse;\n"
                "            SourceVals.y = tex2D( ShadowMap, ShadowTexC.xy"
                ").r;  \n"
                "\n"
                "            ShadowTexC.y += ShadowMapSizeInverse;         "
                "       \n"
                "            SourceVals.w = tex2D( ShadowMap, ShadowTexC.xy"
                ").r;\n"
                "                \n"
                "            ShadowTexC.x -= ShadowMapSizeInverse;         "
                "       \n"
                "            SourceVals.z = tex2D( ShadowMap, ShadowTexC.xy"
                ").r;\n"
                "                \n"
                "            float4 Shade = (LightSpaceDepth <= SourceVals)"
                ";\n"
                "\n"
                "            // lerp between the shadow values to calculate"
                " our light amount\n"
                "            ShadowOut = lerp(\n"
                "                lerp(Shade.x, Shade.y, lerps.x),\n"
                "                lerp(Shade.z, Shade.w, lerps.x), lerps.y )"
                ";\n"
                "        }\n"
                "        else\n"
                "        {\n"
                "            // Outside of the light cone is shadowed compl"
                "etely\n"
                "            ShadowOut = 0.0;\n"
                "        }\n"
                "    }    \n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        // Insert a code block
        {
            NiCodeBlock* pkBlock = NiNew NiCodeBlock();

            // Set code block language
            pkBlock->SetLanguage("hlsl/Cg");

            // Set code block platform
            pkBlock->SetPlatform("DX9/D3D10/Xenon/PS3");

            // Set code block compile target
            pkBlock->SetTarget("ps_3_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    if (LightType != 2)\n"
                "    {\n"
                "        // This version only supports spotlights.\n"
                "        ShadowOut = 1.0;\n"
                "    }\n"
                "    else\n"
                "    {\n"
                "        float3 LightToWorldPos = normalize(float3(WorldPos"
                " - LightPos));\n"
                "\n"
                "        if (dot(LightDirection, LightToWorldPos) > CosOfCu"
                "toff)\n"
                "        {\n"
                "            float4 LightProjPos = mul(WorldPos, WorldToLig"
                "htProjMat);\n"
                "            float3 ShadowTexC = LightProjPos.xyz / LightPr"
                "ojPos.w;               \n"
                "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2"
                "( 0.5, 0.5 );\n"
                "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
                "\n"
                "            float LightSpaceDepth = clamp(ShadowTexC.z - S"
                "hadowBias, \n"
                "                0.0, 1.0);\n"
                "            \n"
                "            float2 ShadowMapSizeInverse = 1.0 / ShadowMapS"
                "ize;\n"
                "\n"
                "            ShadowTexC.xy = ShadowTexC.xy - ShadowMapSizeI"
                "nverse;\n"
                "               \n"
                "            ShadowOut = 0;\n"
                "            float4 fOnes = float4(1.0, 1.0, 1.0, 1.0);\n"
                "            float4 fKernels = float4(1.0, 1.0, 1.0, 1.0);\n"
                "            float faKernels[4] = {1.0, 1.0, 1.0, 1.0};\n"
                "           \n"
                "            faKernels[0] = 1-frac(ShadowTexC.y * ShadowMap"
                "Size.y);\n"
                "            faKernels[3]= frac(ShadowTexC.y  * ShadowMapSi"
                "ze.y); \n"
                "           \n"
                "            fKernels.x = 1-frac(ShadowTexC.x * ShadowMapSi"
                "ze.x);\n"
                "            fKernels.w = frac(ShadowTexC.x  * ShadowMapSiz"
                "e.x);\n"
                "                              \n"
                "            float fTotalPercent = 0;\n"
                "            \n"
                "            // This loop is manually unrolled here to avoi"
                "d long\n"
                "            // shader compilation times.            \n"
                "            //for (int i=0; i < 4; i++)\n"
                "            // i == 0\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC.xy;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth <= shadowMa"
                "pDepth);\n"
                "                    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[0];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[0];\n"
                "            }\n"
                "            // i == 1\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC.xy;\n"
                "                pos.y += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth <= shadowMa"
                "pDepth);\n"
                "                    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[1];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[1];\n"
                "            }\n"
                "            // i == 2\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC.xy;\n"
                "                pos.y += 2 * ShadowMapSizeInverse;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth <= shadowMa"
                "pDepth);\n"
                "                    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[2];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[2];\n"
                "            }\n"
                "            // i == 3\n"
                "            {\n"
                "                float4 shadowMapDepth = 0;\n"
                "                float2 pos = ShadowTexC.xy;\n"
                "                pos.y += 3 * ShadowMapSizeInverse;\n"
                "                shadowMapDepth.x = tex2D(ShadowMap, pos).x"
                "; \n"
                "                pos.x += ShadowMapSizeInverse;\n"
                "                shadowMapDepth.y = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;        \n"
                "                shadowMapDepth.z = tex2D(ShadowMap, pos).x"
                ";\n"
                "                pos.x += ShadowMapSizeInverse;            "
                "    \n"
                "                shadowMapDepth.w = tex2D(ShadowMap, pos).x"
                ";\n"
                "                \n"
                "                float4 shad = (LightSpaceDepth <= shadowMa"
                "pDepth);\n"
                "                    \n"
                "                ShadowOut += dot(shad, fKernels) * faKerne"
                "ls[3];\n"
                "                fTotalPercent += dot(fOnes, fKernels) * fa"
                "Kernels[3];\n"
                "            }\n"
                "           \n"
                "            ShadowOut = ShadowOut / fTotalPercent;        "
                " \n"
                "        }\n"
                "        else\n"
                "        {\n"
                "            // Outside of the light cone is shadowed compl"
                "etely\n"
                "            ShadowOut = 0.0;\n"
                "        }\n"
                "    }    \n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    return pkLib;
}
//---------------------------------------------------------------------------

