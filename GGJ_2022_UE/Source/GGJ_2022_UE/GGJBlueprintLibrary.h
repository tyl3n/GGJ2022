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
	static class AGGJCharacter* GetLocalPlayer(UObject* context);

	UFUNCTION(BlueprintCallable, Category = "GGJ|Getters")
	static class AGGJPlayerState* GetLocalPlayerState(UObject* context);

	UFUNCTION(BlueprintPure, Category = "GGJ|Getters")
	static float GetGameTime();

	UFUNCTION(BlueprintPure, Category = "GGJ|Getters")
	static float GetElapsedGameTime(float timestamp);

	UFUNCTION(BlueprintPure, Category = "GGJ|Getters")
	static FColor GetResourceColor(UObject* context, int resourceID);

	UFUNCTION(BlueprintPure, Category = "GGJ|Getters")
	static class AGGJWorldSettings* GetWorldSettings(UObject* context);

	UFUNCTION(BlueprintCallable, Category="GGJ")
	static void TriggerMerge(UObject* context);

	UFUNCTION(BlueprintCallable, Category = "GGJ|ShapeDefinition")
	static bool GetShapeDefinitionValue(const FGGJShapeDefinition& shapeDefinition, int x, int y);

	UFUNCTION(BlueprintCallable, Category = "GGJ|ShapeDefinition")
	static FGGJShapeDefinition GetShapeDefinitionFromObjective(const struct FGGJObjective& objective);
};
