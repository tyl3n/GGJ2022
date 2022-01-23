
#include "GGJBlueprintLibrary.h"

#include "Utilities.h"
#include "GGJCharacter.h"
#include "GGJShapeDefinition.h"
#include "GGJObjective.h"

class AGGJCharacter* UGGJBlueprintLibrary::GetLocalPlayer()
{
	return Utils::GetLocalPlayer();
}

class AGGJPlayerState* UGGJBlueprintLibrary::GetLocalPlayerState()
{
	return Utils::GetLocalPlayerState();
}

void UGGJBlueprintLibrary::TriggerMerge()
{
	if (AGGJCharacter* player = GetLocalPlayer())
	{
		player->MergeTouchingDraggableActors();
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