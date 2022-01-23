
#include "GGJGameState.h"

#include "Net/UnrealNetwork.h"

#include "GGJPlayerState.h"
#include "GGJGameMode.h"

#include "Utilities.h"
AGGJGameState::AGGJGameState()
{
}

void AGGJGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGGJGameState, ResourcesBalance);
}

void AGGJGameState::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if (HasAuthority())
	{
		IsGameCompleted();
	}
}

void AGGJGameState::BeginPlay()
{
	Super::BeginPlay();

	ResourcesBalance.SetNum(5);

	if (HasAuthority())
	{
		for (int i = 0; i < ResourcesBalance.Num(); ++i)
		{
			ResourcesBalance[i] = 0.5f;
		}
	}
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
			ggjPlayerState->OnRep_Duality();
			ggjPlayerState->FlushNetDormancy();
		}
		else
		{
			ggjPlayerState->Duality = EPlayerDuality::Devil;
			ggjPlayerState->OnRep_Duality();
			ggjPlayerState->FlushNetDormancy();
		}

		FlushNetDormancy();
	}
}

void AGGJGameState::AdjustResources(EPlayerDuality duality, int resourceID, float adjustment)
{
	if (ResourcesBalance.IsValidIndex(resourceID))
	{
		if (duality == EPlayerDuality::Devil)
		{
			adjustment *= -1.0f;
		}

		ResourcesBalance[resourceID] = FMath::Clamp(ResourcesBalance[resourceID] + adjustment, -1.0f, 1.0f);
	}
}

bool AGGJGameState::IsGameCompleted_Implementation() const
{
	AGGJGameMode* gameMode = Utils::GetGameMode();

	if (gameMode == nullptr)
	{
		return false;
	}

	for (int i = 0; i < ResourcesBalance.Num(); ++i)
	{
		if (ResourcesBalance[i] <= 0.0f || ResourcesBalance[i] >= 1.0f)
		{
			return true;
		}	
	}
	bool haveAllPlayersFinishedObjectives = true;
	for (APlayerState* playerState : PlayerArray)
	{
		if (AGGJPlayerState* ggjPlayerStateIt = Cast<AGGJPlayerState>(playerState))
		{
			haveAllPlayersFinishedObjectives &= (ggjPlayerStateIt->ActiveObjectives.Num() == 0 &&
				ggjPlayerStateIt->CompletedObjectives.Num() + ggjPlayerStateIt->FailedObjectives.Num() == gameMode->AvailableMissions.Num());
		}
	}
	return haveAllPlayersFinishedObjectives;

}

FColor AGGJGameState::GetResourceColor_Implementation(int resourceIndex) const
{
	// Implemented in BP
	return FColor::White;
}