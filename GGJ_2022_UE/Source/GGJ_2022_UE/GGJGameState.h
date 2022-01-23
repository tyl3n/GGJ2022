
#pragma once

#include "CoreMinimal.h"
#include "GGJObjective.h"
#include "GameFramework/GameStateBase.h"
#include "GGJGameState.generated.h"

UCLASS(minimalapi)
class AGGJGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	UFUNCTION(BlueprintCallable)
	void CompleteObjective(UPARAM(ref) TArray<FGGJObjective>& Objectives, UPARAM(ref) TArray<float>& Ressources, UPARAM(ref) int ObjectiveId
		, UPARAM(ref) bool SentByServer);
	AGGJGameState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FColor GetResourceColor(int resourceIndex) const;
};



