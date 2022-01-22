#pragma once

#include "CoreMinimal.h"

#include "GGJBlueprintLibrary.generated.h"

UCLASS()
class UGGJBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GGJ|Getters")
	static class AGGJCharacter* GetLocalPlayer();

	UFUNCTION(BlueprintCallable, Category="GGJ")
	void TriggerMerge();
};
