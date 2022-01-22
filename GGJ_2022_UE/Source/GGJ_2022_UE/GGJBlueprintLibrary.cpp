
#include "GGJBlueprintLibrary.h"

#include "Utilities.h"
#include "GGJCharacter.h"

class AGGJCharacter* UGGJBlueprintLibrary::GetLocalPlayer()
{
	return Utils::GetLocalPlayer();
}

void UGGJBlueprintLibrary::TriggerMerge()
{
	if (AGGJCharacter* player = GetLocalPlayer())
	{
		player->MergeTouchingDraggableActors();
	}
}