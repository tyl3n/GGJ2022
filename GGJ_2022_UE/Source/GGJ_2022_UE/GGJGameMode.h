// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GGJObjective.h"

#include "GGJGameMode.generated.h"

UCLASS(minimalapi)
class AGGJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	/// Config

	UPROPERTY(EditAnywhere)
	TArray<FGGJObjective> ObjectiveDefinitions;

	//////////////////////////////////////////////////////////////////////////
	/// Dynamic

	int RandomSeed;

	AGGJGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FColor GetResourceColor(int resourceIndex) const;

	UPROPERTY(Transient)
	TArray<FGGJObjective> DesiredAngelObjectives;

	UPROPERTY(Transient)
	TArray<FGGJObjective> DesiredDevilObjectives;
};



