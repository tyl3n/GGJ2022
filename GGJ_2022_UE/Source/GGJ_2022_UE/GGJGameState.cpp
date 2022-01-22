// Copyright Epic Games, Inc. All Rights Reserved.

#include "GGJGameState.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/UnrealMathUtility.h"

AGGJGameState::AGGJGameState()
{
}

void AGGJGameState::CompleteObjective( TArray<FGGJObjective>& Objectives, TArray<float>& Ressources,  int ObjectiveId
	, bool SentByServer)
{
	for (FGGJObjective& objective : Objectives)
	{
		if (objective.ObjectiveId == ObjectiveId)
		{
			if (objective.RessourceIndex < Ressources.Num())
			{
				float valueModifier = objective.RessourceValue * SentByServer ? -1.0f : 1.0f;
				Ressources[objective.RessourceIndex] = FMath::Clamp(Ressources[objective.RessourceIndex] + (objective.RessourceValue * valueModifier), 0.0f, 1.0f);
			}
		}
	}
}