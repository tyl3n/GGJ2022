// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GGJObjective.generated.h"

USTRUCT(Blueprintable)
struct FGGJObjective
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RessourceValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RessourceIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ObjectiveId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCompleted = false;

	FGGJObjective();
	~FGGJObjective();
};



