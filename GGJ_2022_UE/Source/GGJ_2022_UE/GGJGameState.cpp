
#include "GGJGameState.h"

#include "GGJPlayerState.h"

AGGJGameState::AGGJGameState()
{
}

void AGGJGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (AGGJPlayerState* ggjPlayerState = Cast<AGGJPlayerState>(PlayerState))
	{
		bool bHasDevilPlayer = false;

		for (APlayerState* playerState : PlayerArray)
		{
			if (AGGJPlayerState* ggjPlayerStateIt = Cast<AGGJPlayerState>(playerState))
			{
				if (ggjPlayerStateIt->Duality == EPlayerDuality::Devil)
				{
					bHasDevilPlayer = true;
				}
			}
		}

		if (bHasDevilPlayer)
		{
			ggjPlayerState->Duality = EPlayerDuality::Angel;
		}
		else
		{
			ggjPlayerState->Duality = EPlayerDuality::Devil;
		}

		FlushNetDormancy();
	}
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

FColor AGGJGameState::GetResourceColor_Implementation(int resourceIndex) const
{
	// Implemented in BP
	return FColor::White;
}