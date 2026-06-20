-- EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
--
-- This software is supplied under the terms of a license agreement or
-- nondisclosure agreement with Emergent Game Technologies and may not
-- be copied or disclosed except in accordance with the terms of that
-- agreement.
--
--      Copyright (c) 1996-2007 Emergent Game Technologies.
--      All Rights Reserved.
--
-- Emergent Game Technologies, Chapel Hill, North Carolina 27517
-- http://www.emergent.net
-----------------------------------------------------------------------------
LightShadowsCA = attributes "GamebryoShadowSettings" version:2 attribID:#(0x5f33d85d, 0x27645730)
( 

	parameters NiShadowParams rollout:ShadowParamsRollout
	( 
		NiShadowStatic		type:#boolean 	default:false	animatable:false	ui:chkStatic
		NiShadowTechnique	type:#string	default:"NiStandardShadowTechnique"
		NiShadowBackfacing	type:#boolean	default:true	animatable:false	ui:chkBackfacing
		NiShadowForceSize	type:#boolean	default:false	animatable:false	ui:chkForceSize
		NiShadowAutoBias	type:#boolean	default:true	animatable:false	ui:chkAutoBias
	) 

	rollout ShadowParamsRollout "Gamebryo Shadow Params" 
	( 
		dropdownlist	dropTechnique "Technique" items:#()
		checkbox chkStatic	"Static" align:#left
		checkbox chkBackfacing "Use Back Faces" align:#left
		checkbox chkForceSize "Force Map Size" align:#left
		checkbox chkAutoBias "Automatic Bias" align:#left

		on dropTechnique selected newIndex do
		(
			-- change the NiShadowTechnique string to equal the selection text
			NiShadowTechnique = dropTechnique.selected
		)

		on ShadowParamsRollout open do
		(
			-- enumerate the available techniques
			techArray = NiGetShadowTechniqueList()
			-- create the drop down list array
			dropTechnique.items = techArray
			
			-- set the initial index to the previous technique
			found = false
			for i = 1 to techArray.count do
			(
				if techArray[i] == NiShadowTechnique then
				(
					dropTechnique.selection = i
					found = true;
					break;
				)
			)
			
			-- issue warning when previous technique is not found
			if found == false do
			(
				-- if the technique was set to some value, issue a warning
				if NiShadowTechnique != "" then
				(
					messageString = "The previously used shadow technique "
					messageString += NiShadowTechnique + " was not found in the technique list. \n"

					messageBox messageString
				)
				
				if techArray.count > 0 then
				(
					NiShadowTechnique = techArray[1]
				)
				else
				(
					NiShadowTechnique = "NiStandardShadowTechnique"
				)
			)
		)
		
		on attachedToNode ownerNode do
		(
			-- initialize the NiShadowTechnique to a valid name
			techArray = NiGetShadowTechniqueList()
			if techArray.count > 0 then
				NiShadowTechnique = techArray[1]
			else
				NiShadowTechnique = "NiStandardShadowTechnique"
		)
	)

) 	