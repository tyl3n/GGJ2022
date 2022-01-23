
#include "GGJPlayerState.h"

#include "Net/UnrealNetwork.h"

AGGJPlayerState::AGGJPlayerState()
{
	
}

void AGGJPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGGJPlayerState, Duality);
	DOREPLIFETIME(AGGJPlayerState, ActiveObjectives);
	DOREPLIFETIME(AGGJPlayerState, CompletedObjectives);
}

bool AGGJPlayerState::HasObjective(int objectiveId) const
{
	for (const FGGJObjective& objective : ActiveObjectives)
	{
		if (objective.ObjectiveId == objectiveId)
		{
			return true;
		}
	}

	for (const FGGJObjective& objective : CompletedObjectives)
	{
		if (objective.ObjectiveId == objectiveId)
		{
			return true;
		}
	}

	return false;
}