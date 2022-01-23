
#include "GGJBlueprintLibrary.h"

#include "Utilities.h"
#include "GGJCharacter.h"
#include "GGJShapeDefinition.h"
#include "GGJObjective.h"

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

bool UGGJBlueprintLibrary::GetShapeDefinitionValue(const FGGJShapeDefinition& shapeDefinition, int x, int y)
{
	return shapeDefinition.GetShapeValue(x, y);
}

FGGJShapeDefinition UGGJBlueprintLibrary::GetShapeDefinitionFromObjective(const FGGJObjective& objective)
{
	return FGGJShapeDefinition(objective.ShapeCode);
}