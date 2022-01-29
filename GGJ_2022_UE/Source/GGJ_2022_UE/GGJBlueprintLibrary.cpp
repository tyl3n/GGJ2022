
#include "GGJBlueprintLibrary.h"

#include "Utilities.h"
#include "GGJCharacter.h"
#include "GGJShapeDefinition.h"
#include "GGJObjective.h"
#include "GGJWorldSettings.h"
#include "GGJGameState.h"

class AGGJCharacter* UGGJBlueprintLibrary::GetLocalPlayer(UObject* context)
{
	return Utils::GetLocalPlayer(context);
}

class AGGJPlayerState* UGGJBlueprintLibrary::GetLocalPlayerState(UObject* context)
{
	return Utils::GetLocalPlayerState(context);
}

void UGGJBlueprintLibrary::TriggerMerge(UObject* context)
{
	if (AGGJCharacter* player = GetLocalPlayer(context))
	{
		player->MergeTouchingDraggableActors();
	}
}

float UGGJBlueprintLibrary::GetGameTime()
{
	return Utils::GetGameTime();
}

float UGGJBlueprintLibrary::GetElapsedGameTime(float timestamp)
{
	return Utils::ElapsedTime(timestamp);
}

AGGJWorldSettings* UGGJBlueprintLibrary::GetWorldSettings(UObject* context)
{
	if (context != nullptr)
	{
		if (UWorld* world = context->GetWorld())
		{
			return Cast<AGGJWorldSettings>(world->GetWorldSettings());
		}
	}

	return nullptr;
}

FColor UGGJBlueprintLibrary::GetResourceColor(UObject* context, int resourceID)
{
	if (Utils::IsInEditor(context))
	{
		if (AGGJGameState* gameState = Utils::GetEditorGameState())
		{
			return gameState->GetResourceColor(resourceID);
		}
		else
		{
			return FColor::Black;
		}
	}
	else
	{
		if (AGGJGameState* gameState = Utils::GetGameState())
		{
			return gameState->GetResourceColor(resourceID);
		}
		else
		{
			return FColor::Black;
		}
	}
}

bool UGGJBlueprintLibrary::GetShapeDefinitionValue(const FGGJShapeDefinition& shapeDefinition, int x, int y)
{
	return shapeDefinition.GetShapeValue(x, y);
}

FGGJShapeDefinition UGGJBlueprintLibrary::GetShapeDefinitionFromObjective(const FGGJObjective& objective)
{
	return FGGJShapeDefinition(objective.ShapeCode);
}