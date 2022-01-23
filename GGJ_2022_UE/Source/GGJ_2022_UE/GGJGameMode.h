// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GGJObjective.h"
#include "GGJPlayerState.h"

#include "GGJGameMode.generated.h"

UCLASS(minimalapi)
class AGGJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	/// Config

	UPROPERTY(EditAnywhere)
	int MaxConcurrentObjectives = 5;

	//////////////////////////////////////////////////////////////////////////
	/// Dynamic

	int RandomSeed;

	UPROPERTY(Transient)
	TArray<FGGJObjective> AvailableMissions;

	float MissionsInterval;
	float LastMissionAssignTimestamp;

	UPROPERTY(Transient)
	TArray<FGGJObjective> DesiredAngelObjectives;

	UPROPERTY(Transient)
	TArray<FGGJObjective> DesiredDevilObjectives;

	AGGJGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void GetAvailableMissionIndices(EPlayerDuality duality, TArray<int>& out_MissionIndices) const;

	void AssignNewObjective(EPlayerDuality duality, float bonusTime = 0.0f);

	void OnShapeSacrifice(class AGGJCharacter* sacrificer, uint32 shapeCode, int resourceID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FColor GetResourceColor(int resourceID) const;
};



