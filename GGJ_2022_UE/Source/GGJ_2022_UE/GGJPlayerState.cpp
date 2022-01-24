
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
		TArray<FGGJObjective> oldObjectives = ActiveObjectives;
		TArray<FGGJObjective> oldFailedObjectives = FailedObjectives;
		
		for (int i = ActiveObjectives.Num() - 1; i >= 0; --i)
		{
			float elapsedTime = Utils::ElapsedTime(ActiveObjectives[i].AddedTimestamp);
			if (elapsedTime > ActiveObjectives[i].Duration)
			{
				FailedObjectives.Add(ActiveObjectives[i]);
				ActiveObjectives.RemoveAt(i);
			}
		}

		if(oldObjectives.Num() != ActiveObjectives.Num())
		{
			OnRep_ActiveObjectives(oldObjectives);
			FlushNetDormancy();
		}

		if (oldFailedObjectives.Num() != FailedObjectives.Num())
		{
			OnRep_FailedObjectives(oldFailedObjectives);
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

void AGGJPlayerState::CompleteObjective_Server(int objectiveId)
{
	TArray<FGGJObjective> oldObjectives = ActiveObjectives;
	TArray<FGGJObjective> oldCompletedObjectives = CompletedObjectives;

	for (int i = ActiveObjectives.Num() - 1; i >= 0; --i)
	{
		if (ActiveObjectives[i].ObjectiveId == objectiveId)
		{
			CompletedObjectives.Add(ActiveObjectives[i]);
			ActiveObjectives.RemoveAt(i);

			OnRep_ActiveObjectives(oldObjectives);
			OnRep_CompletedObjectives(oldCompletedObjectives);
			FlushNetDormancy();

			return; // DONE
		}
	}
}

void AGGJPlayerState::OnRep_ActiveObjectives(const TArray<FGGJObjective>& oldActiveObjectives)
{
	for (FGGJObjective& objective : ActiveObjectives)
	{
		if (objective.AddedTimestamp < 0.0f)
		{
			TWEAKABLE float EPSILON = 0.001f;
			objective.AddedTimestamp = Utils::GetGameTime() + EPSILON;
		}
	}

	if (ActiveObjectives.Num() > oldActiveObjectives.Num())
	{
		OnNewObjective();
	}

	OnActiveObjectiveChanged.Broadcast(this);
}

void AGGJPlayerState::OnRep_CompletedObjectives(const TArray<FGGJObjective>& oldCompletedObjectives)
{
	if (CompletedObjectives.Num() > oldCompletedObjectives.Num())
	{
		OnObjectiveCompleted();
	}
}

void AGGJPlayerState::OnRep_FailedObjectives(const TArray<FGGJObjective>& oldFailedObjectives)
{
	if (FailedObjectives.Num() > oldFailedObjectives.Num())
	{
		OnObjectiveFailed();
	}
}