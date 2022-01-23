
#include "GGJGameMode.h"

#include "GGJGameState.h"
#include "GGJPlayerState.h"
#include "Utilities.h"

AGGJGameMode::AGGJGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGGJGameMode::BeginPlay()
{
	Super::BeginPlay();

	RandomSeed = FMath::RandRange(0, 99999999);

	if(ObjectiveDefinitions.Num() > 0)
	{
		{
			FGGJObjective& angleObjective = DesiredAngelObjectives.AddZeroed_GetRef();
			angleObjective = ObjectiveDefinitions[0];
			angleObjective.GenerateShape(RandomSeed);

			FGGJObjective& devilObjective = DesiredDevilObjectives.AddZeroed_GetRef();
			devilObjective = ObjectiveDefinitions[0];
			devilObjective.GenerateShape(RandomSeed);
		}

		{
			FGGJObjective& angleObjective = DesiredAngelObjectives.AddZeroed_GetRef();
			angleObjective = ObjectiveDefinitions[1];
			angleObjective.GenerateShape(RandomSeed);

			FGGJObjective& devilObjective = DesiredDevilObjectives.AddZeroed_GetRef();
			devilObjective = ObjectiveDefinitions[1];
			devilObjective.GenerateShape(RandomSeed);
		}
	}
}

void AGGJGameMode::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (AGGJGameState* gameState = Utils::GetGameState())
	{
		for (APlayerState* playerState : gameState->PlayerArray)
		{
			if (AGGJPlayerState* ggjPlayerState = Cast<AGGJPlayerState>(playerState))
			{
				if (ggjPlayerState->Duality == EPlayerDuality::Angel)
				{
					for (const FGGJObjective& objective : DesiredAngelObjectives)
					{
						if (!ggjPlayerState->HasObjective(objective.ObjectiveId))
						{
							ggjPlayerState->ActiveObjectives.Add(objective);
							ggjPlayerState->OnRep_ActiveObjectives();
							ggjPlayerState->FlushNetDormancy();
						}
					}
				}
				else if (ggjPlayerState->Duality == EPlayerDuality::Devil)
				{
					for (const FGGJObjective& objective : DesiredDevilObjectives)
					{
						if (!ggjPlayerState->HasObjective(objective.ObjectiveId))
						{
							ggjPlayerState->ActiveObjectives.Add(objective);
							ggjPlayerState->OnRep_ActiveObjectives();
							ggjPlayerState->FlushNetDormancy();
						}
					}
				}
			}
		}
	}
}


FColor AGGJGameMode::GetResourceColor_Implementation(int resourceIndex) const
{
	// Implemented in BP
	return FColor::White;
}