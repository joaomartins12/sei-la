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
#include "NiVSMBlurMaterialNodeLibrary.h"

//---------------------------------------------------------------------------
NiMaterialNodeLibrary* 
    NiVSMBlurMaterialNodeLibrary::CreateMaterialNodeLibrary()
{

    // Create a new NiMaterialNodeLibrary
    NiMaterialNodeLibrary* pkLib = NiNew NiMaterialNodeLibrary(1);

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
                "    ProjPos = mul(WorldPosition, ViewProjection );\n"
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
                "    ViewPos = mul(WorldPosition, ViewTransform );\n"
                "    ProjPos = mul(ViewPos, ProjTransform );\n"
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
        pkFrag->SetType("Pixel/Vertex");

        // Set the fragment name
        pkFrag->SetName("AddFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment is responsible for adding two float4's.   \n"
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
            "    This fragment is responsible for adding two float3's.   \n"
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
            "    This fragment is responsible for adding two float2's.   \n"
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
            "    This fragment is responsible for adding two floats.   \n"
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
            "   \n"
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
            "   \n"
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
            "   \n"
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
            "    This fragment is responsible for multiplying two floats.  "
            " \n"
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
            "stant.     \n"
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
            "stant.   \n"
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
            "stant.   \n"
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
            "    This fragment is responsible for saturating a float4.   \n"
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
            "    This fragment is responsible for saturating a float3.   \n"
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
            "    This fragment is responsible for saturating a float2.   \n"
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
            "    This fragment is responsible for saturating a float.   \n"
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
            "o float4's.   \n"
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
            "o float3's.   \n"
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
        pkFrag->SetName("BlurColor");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment writes projected depth to all color componen"
            "t outputs.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Proj");

            // Set resource variable name
            pkRes->SetVariable("WorldPosProjected");

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
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    float Depth = WorldPosProjected.z / WorldPosProjected."
                "w;\n"
                "    OutputColor = Depth;\n"
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
        pkFrag->SetName("WriteDepthToColor");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment writes projected depth to all color componen"
            "t outputs.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Proj");

            // Set resource variable name
            pkRes->SetVariable("WorldPosProjected");

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
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    float Depth = WorldPosProjected.z / WorldPosProjected."
                "w;\n"
                "    OutputColor = Depth;\n"
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
        pkFrag->SetName("WriteVSMDepthToColor");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "    This fragment writes projected depth to all color componen"
            "t outputs.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource semantic
            pkRes->SetSemantic("Position");

            // Set resource label
            pkRes->SetLabel("Proj");

            // Set resource variable name
            pkRes->SetVariable("WorldPosProjected");

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
            pkBlock->SetTarget("ps_2_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "    float Depth = WorldPosProjected.z / WorldPosProjected."
                "w;\n"
                "            \n"
                "    OutputColor.x = Depth;\n"
                "    OutputColor.y = Depth * Depth;\n"
                "    OutputColor.z = 1.0;\n"
                "    OutputColor.w = 1.0;\n"
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
        pkFrag->SetName("TeeFloat4");

        // Set the fragment description
        pkFrag->SetDescription("\n"
            "      This fragment splits and passes through a single float4 "
            "input into two\n"
            "      float4 outputs.\n"
            "    ");

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Input");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output1");

            // Insert resource
            pkFrag->AddOutputResource(pkRes);
        }

        // Insert an output resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float4");

            // Set resource variable name
            pkRes->SetVariable("Output2");

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
                "      Output1 = Input;\n"
                "      Output2 = Input;\n"
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
            pkRes->SetDefaultValue("(false)");

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
            pkRes->SetDefaultValue("(false)");

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
                "    float4 ProjTexCoord = TexCoord.xyzz;\n"
                "    ColorOut.a = 1.0f;    \n"
                "    ColorOut.rgb = tex2Dproj(Sampler, ProjTexCoord).rgb;  "
                "  \n"
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
            pkRes->SetDefaultValue("(false)");

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
                "    ColorOut.a = 1.0f;\n"
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
            pkRes->SetDefaultValue("(false)");

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
                "    ColorOut.a = 1.0f;\n"
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
        pkFrag->SetName("HorzBlurSample");

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
            pkRes->SetSemantic("TexSizeBase");

            // Set resource variable name
            pkRes->SetVariable("MapSize");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("KernelSize");

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
            pkBlock->SetTarget("ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "\n"
                "    // Blurs the texel at vTexCoord in the X direction.   \n"
                "    float fInvTexSize = 1.0 / MapSize.x;\n"
                "    float fHalfBlurKernelSize = KernelSize * 0.5f;\n"
                "    float fInvTotalWeights = 1.0 / (KernelSize * KernelSiz"
                "e);    \n"
                "    float fBlurWeight = 2;\n"
                "   \n"
                "    float4 fTotalColor = 0;   \n"
                "    float2 vTexCoord = TexCoord;\n"
                "    vTexCoord.x -= fInvTexSize * fHalfBlurKernelSize;    \n"
                "    float i = 0;\n"
                "\n"
                "    // Blur x2\n"
                "    if ( 2 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "\n"
                "    // Blur x4\n"
                "    if ( 4 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "    \n"
                "    // Blur x6\n"
                "    if ( 6 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }    \n"
                "    \n"
                "    // Blur x8\n"
                "    if ( 8 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }    \n"
                "    \n"
                "    // Blur x10\n"
                "    if ( 10 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "\n"
                "    // Blur x12\n"
                "    if ( 12 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "       \n"
                "    // Blur x14\n"
                "    if ( 14 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }    \n"
                "    \n"
                "    // Blur x16\n"
                "    if ( 16 <= KernelSize)\n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.x += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.x += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "        \n"
                "    ColorOut = fTotalColor;\n"
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
        pkFrag->SetName("VertBlurSample");

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
            pkRes->SetSemantic("TexSizeBase");

            // Set resource variable name
            pkRes->SetVariable("MapSize");

            // Insert resource
            pkFrag->AddInputResource(pkRes);
        }

        // Insert an input resource
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();

            // Set resource type
            pkRes->SetType("float");

            // Set resource variable name
            pkRes->SetVariable("KernelSize");

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
            pkBlock->SetTarget("ps_2_0/vs_4_0/ps_4_0");

            // Set code block text
            pkBlock->SetText("\n"
                "\n"
                "    // Blurs the texel at vTexCoord in the Y direction.   \n"
                "    float fInvTexSize = 1.0 / MapSize.y;\n"
                "    float fHalfBlurKernelSize = KernelSize * 0.5f;\n"
                "    float fInvTotalWeights = 1.0 / (KernelSize * KernelSiz"
                "e);    \n"
                "    float fBlurWeight = 2;\n"
                "   \n"
                "    float4 fTotalColor = 0;   \n"
                "    float2 vTexCoord = TexCoord;\n"
                "    vTexCoord.y -= fInvTexSize * fHalfBlurKernelSize;    \n"
                "    float i = 0;\n"
                "\n"
                "    // Blur x2\n"
                "    if ( 2 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "    \n"
                "    // Blur x4\n"
                "    if ( 4 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }    \n"
                "\n"
                "    // Blur x6\n"
                "    if ( 6 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "\n"
                "    // Blur x8\n"
                "    if ( 8 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "\n"
                "    // Blur x10\n"
                "    if ( 10 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "\n"
                "    // Blur x12\n"
                "    if ( 12 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "    \n"
                "    // Blur x14\n"
                "    if ( 14 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }    \n"
                "\n"
                "    // Blur x16\n"
                "    if ( 16 <= KernelSize)    \n"
                "    {\n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;        \n"
                "        vTexCoord.y += fInvTexSize;\n"
                "        \n"
                "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;\n"
                "        \n"
                "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
                "            fBlurWeight * fInvTotalWeights;\n"
                "        vTexCoord.y += fInvTexSize;            \n"
                "        \n"
                "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
                "        fBlurWeight -= fSign * 4;        \n"
                "        i+=2;\n"
                "    }\n"
                "\n"
                "    ColorOut = fTotalColor;\n"
                "    ");

            // Insert code block
            pkFrag->AddCodeBlock(pkBlock);
        }

        pkLib->AddNode(pkFrag);
    }

    return pkLib;
}
//---------------------------------------------------------------------------

