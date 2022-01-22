// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GGJPlayerController.generated.h"

UCLASS(minimalapi)
class AGGJPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGGJPlayerController();

	virtual void BeginPlay() override;
	virtual void SpawnPlayerCameraManager() override;
};



