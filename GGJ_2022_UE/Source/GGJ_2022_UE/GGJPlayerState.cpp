
#include "GGJPlayerState.h"

#include "Net/UnrealNetwork.h"

#include "Utilities.h"

AGGJPlayerState::AGGJPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGGJPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGGJPlayerState, Duality);
	DOREPLIFETIME(AGGJPlayerState, ActiveObjectives);
	DOREPLIFETIME(AGGJPlayerState, CompletedObjectives);
	DOREPLIFETIME(AGGJPlayerState, FailedObjectives);
}

void AGGJPlayerState::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if(HasAuthority())
	{
		bool bObjectivesChanged = false;
		for (int i = ActiveObjectives.Num() - 1; i >= 0; --i)
		{
			float elapsedTime = Utils::ElapsedTime(ActiveObjectives[i].AddedTimestamp);
			if (elapsedTime > ActiveObjectives[i].Duration)
			{
				FailedObjectives.Add(ActiveObjectives[i]);
				ActiveObjectives.RemoveAt(i);
				bObjectivesChanged = true;
			}
		}

		if(bObjectivesChanged)
		{
			OnRep_ActiveObjectives();
			FlushNetDormancy();
		}
	}
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

	for (const FGGJObjective& objective : FailedObjectives)
	{
		if (objective.ObjectiveId == objectiveId)
		{
			return true;
		}
	}

	return false;
}

void AGGJPlayerState::OnRep_ActiveObjectives()
{
	for (FGGJObjective& objective : ActiveObjectives)
	{
		if (objective.AddedTimestamp < 0.0f)
		{
			TWEAKABLE float EPSILON = 0.001f;
			objective.AddedTimestamp = Utils::GetGameTime() + EPSILON;
		}
	}

	OnActiveObjectiveChanged.Broadcast(this);
}