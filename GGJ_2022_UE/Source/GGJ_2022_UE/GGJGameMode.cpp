
#include "GGJGameMode.h"

#include "GGJGameState.h"
#include "GGJPlayerState.h"
#include "Utilities.h"
#include "GGJWorldSettings.h"

AGGJGameMode::AGGJGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGGJGameMode::BeginPlay()
{
	Super::BeginPlay();

	RandomSeed = FMath::RandRange(0, 99999999);

	if (AGGJWorldSettings* worldSettings = Utils::GetWorldSettings())
	{
		FRandomStream randomStream = FRandomStream(RandomSeed);

		MissionsInterval = worldSettings->MissionsInterval;
		TWEAKABLE float EPSILON = 0.001f;
		LastMissionAssignTimestamp = Utils::GetGameTime() + EPSILON;
		AvailableMissions = worldSettings->ObjectiveDefinitions;

		if (AvailableMissions.Num() > 0)
		{
			for (int i = 0; i < AvailableMissions.Num(); ++i)
			{
				AvailableMissions[i].ObjectiveId = i;
				AvailableMissions[i].RessourceIndex = randomStream.RandRange(0, 4);
			}

			for (int i = 0; i < worldSettings->NumberOfInitialMission; ++i)
			{
				AssignNewObjective(EPlayerDuality::Angel, worldSettings->InitialBonusTime.IsValidIndex(i) ? worldSettings->InitialBonusTime[i] : 0.0f);
				AssignNewObjective(EPlayerDuality::Devil, worldSettings->InitialBonusTime.IsValidIndex(i) ? worldSettings->InitialBonusTime[i] : 0.0f);
			}
		}
	}
}

void AGGJGameMode::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!Utils::IsRecentEvent(LastMissionAssignTimestamp, MissionsInterval))
	{
		AssignNewObjective(EPlayerDuality::Angel);
		AssignNewObjective(EPlayerDuality::Devil);

		LastMissionAssignTimestamp = Utils::GetGameTime();
	}

	if (AGGJGameState* gameState = Utils::GetGameState())
	{
		for (APlayerState* playerState : gameState->PlayerArray)
		{
			if (AGGJPlayerState* ggjPlayerState = Cast<AGGJPlayerState>(playerState))
			{
				if(ggjPlayerState->ActiveObjectives.Num() < MaxConcurrentObjectives)
				{
					TArray<FGGJObjective>& desiredObjectives = (ggjPlayerState->Duality == EPlayerDuality::Angel) ? DesiredAngelObjectives : DesiredDevilObjectives;
					for (const FGGJObjective& objective : desiredObjectives)
					{
						if (!ggjPlayerState->HasObjective(objective.ObjectiveId))
						{
							ggjPlayerState->ActiveObjectives.Add(objective);
							ggjPlayerState->OnRep_ActiveObjectives();
							ggjPlayerState->FlushNetDormancy();
						}

						if (ggjPlayerState->ActiveObjectives.Num() >= MaxConcurrentObjectives)
						{
							break;
						}
					}
				}
			}
		}
	}


}


void AGGJGameMode::GetAvailableMissionIndices(EPlayerDuality duality, TArray<int>& out_MissionIndices) const
{
	const TArray<FGGJObjective>& desiredMission = (duality == EPlayerDuality::Angel) ? DesiredAngelObjectives : DesiredDevilObjectives;
	for (int i = 0; i < AvailableMissions.Num(); ++i)
	{
		bool bAlreadyInList = false;

		for (const FGGJObjective& objective : desiredMission)
		{
			if (objective.ObjectiveId == i)
			{
				bAlreadyInList = true;
				break;
			}
		}

		if (!bAlreadyInList)
		{
			out_MissionIndices.Add(i);
		}
	}
}

void AGGJGameMode::AssignNewObjective(EPlayerDuality duality, float bonusTime)
{
	TArray<int> availableMissions;
	GetAvailableMissionIndices(duality, availableMissions);

	if (availableMissions.Num() > 0)
	{
		TArray<FGGJObjective>& desiredMission = (duality == EPlayerDuality::Angel) ? DesiredAngelObjectives : DesiredDevilObjectives;

		int availableMissionsIdx = FMath::RandRange(0, availableMissions.Num() - 1);
		FGGJObjective& newObjective = desiredMission.AddZeroed_GetRef();
		newObjective = AvailableMissions[availableMissions[availableMissionsIdx]];
		newObjective.Duration += bonusTime;
		newObjective.GenerateShape(RandomSeed);
	}
}

FColor AGGJGameMode::GetResourceColor_Implementation(int resourceIndex) const
{
	// Implemented in BP
	return FColor::White;
}