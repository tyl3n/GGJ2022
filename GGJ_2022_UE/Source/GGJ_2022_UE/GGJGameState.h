
#pragma once

#include "CoreMinimal.h"
#include "GGJObjective.h"

#include "GameFramework/GameStateBase.h"

#include "GGJPlayerState.h"

#include "GGJGameState.generated.h"
UENUM(BlueprintType)
enum class EGameStatus : uint8
{
	Unassigned,
	Won,
	Lost,
};

UCLASS(minimalapi)
class AGGJGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	/// Dynamic
	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	TArray<float> ResourcesBalance;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_GameStatus,BlueprintReadOnly)
	EGameStatus GameStatus = EGameStatus::Unassigned;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	void AdjustResources(EPlayerDuality duality, int resourceID, float adjustment);

	bool CheckIfGameCompleted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DoGameCompletionLogic();

	UFUNCTION()
	void OnRep_GameStatus();

	AGGJGameState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FColor GetResourceColor(int resourceID) const;
};



