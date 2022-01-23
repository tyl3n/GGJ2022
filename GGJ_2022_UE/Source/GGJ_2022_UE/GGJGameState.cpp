
#include "GGJGameState.h"

#include "Net/UnrealNetwork.h"

#include "GGJPlayerState.h"

AGGJGameState::AGGJGameState()
{
}

void AGGJGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGGJGameState, ResourcesBalance);
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
		}
		else
		{
			ggjPlayerState->Duality = EPlayerDuality::Devil;
		}

		FlushNetDormancy();
	}
}

void AGGJGameState::AdjustResources(EPlayerDuality duality, int resourceIndex, float adjustment)
{
	if (ResourcesBalance.IsValidIndex(resourceIndex))
	{
		if (duality == EPlayerDuality::Devil)
		{
			adjustment *= -1.0f;
		}

		ResourcesBalance[resourceIndex] = FMath::Clamp(ResourcesBalance[resourceIndex] + adjustment, -1.0f, 1.0f);
	}
}

FColor AGGJGameState::GetResourceColor_Implementation(int resourceIndex) const
{
	// Implemented in BP
	return FColor::White;
}