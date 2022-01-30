
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
	DOREPLIFETIME(AGGJGameState, GameStatus);
	DOREPLIFETIME(AGGJGameState, ResourcesBalance);
	DOREPLIFETIME(AGGJGameState, NumberOfPlayers);
}

void AGGJGameState::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	
	if (HasAuthority())
	{
		CheckIfGameCompleted();
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
		GameStatus = EGameStatus::Unassigned;
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

		++NumberOfPlayers;
		OnRep_NumberOfPlayers(); 

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

bool AGGJGameState::CheckIfGameCompleted()
{
	AGGJGameMode* gameMode = Utils::GetGameMode();

	if (gameMode == nullptr || GameStatus != EGameStatus::Unassigned)
	{
		return false;
	}

	if(PlayerArray.Num() >= 1)
	{
		for (int i = 0; i < ResourcesBalance.Num(); ++i)
		{
			if (ResourcesBalance[i] <= 0.001f || ResourcesBalance[i] >= 0.999999f)
			{
				GameStatus = EGameStatus::Lost;
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("You Lose"));
				OnRep_GameStatus();
				return true;
			
			}	
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

	if (haveAllPlayersFinishedObjectives) 
	{
		GameStatus = EGameStatus::Won;
		OnRep_GameStatus();
		return true;
	}
	return false;
}

void AGGJGameState::OnRep_GameStatus()
{
	if ( GameStatus != EGameStatus::Unassigned)
	{
		DoGameCompletionLogic();
	}
}

void AGGJGameState::OnRep_NumberOfPlayers()
{
	OnPlayerCountChanged.Broadcast();
}

void AGGJGameState::DoGameCompletionLogic_Implementation()
{
	
}

FColor AGGJGameState::GetResourceColor_Implementation(int resourceIndex) const
{
	// Implemented in BP
	return FColor::White;
}