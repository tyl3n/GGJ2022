
#pragma once

#include "CoreMinimal.h"
#include "GGJObjective.h"

#include "GameFramework/GameStateBase.h"

#include "GGJPlayerState.h"

#include "GGJGameState.generated.h"

UCLASS(minimalapi)
class AGGJGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	/// Dynamic
	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	TArray<float> ResourcesBalance;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	void AdjustResources(EPlayerDuality duality, int resourceID, float adjustment);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsGameCompleted() const;

	AGGJGameState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FColor GetResourceColor(int resourceID) const;
};



