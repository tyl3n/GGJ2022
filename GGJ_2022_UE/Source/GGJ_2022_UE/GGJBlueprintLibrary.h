#pragma once

#include "CoreMinimal.h"

#include "GGJShapeDefinition.h"
#include "GGJObjective.h"

#include "GGJBlueprintLibrary.generated.h"

UCLASS()
class UGGJBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GGJ|Getters")
	static class AGGJCharacter* GetLocalPlayer();

	UFUNCTION(BlueprintCallable, Category = "GGJ|Getters")
	static class AGGJPlayerState* GetLocalPlayerState();

	UFUNCTION(BlueprintCallable, Category="GGJ")
	static void TriggerMerge();

	UFUNCTION(BlueprintCallable, Category = "GGJ|ShapeDefinition")
	static bool GetShapeDefinitionValue(const FGGJShapeDefinition& shapeDefinition, int x, int y);

	UFUNCTION(BlueprintCallable, Category = "GGJ|ShapeDefinition")
	static FGGJShapeDefinition GetShapeDefinitionFromObjective(const struct FGGJObjective& objective);
};
