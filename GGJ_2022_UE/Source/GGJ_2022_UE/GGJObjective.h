// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GGJShapeDefinition.h"

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
	int RessourceBlockCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ObjectiveId = 0;

	UPROPERTY(BlueprintReadWrite)
	bool IsCompleted = false;

	uint32 ShapeCode;

	FGGJObjective();
	~FGGJObjective();

	bool operator==(const FGGJObjective& rhs) const { return ObjectiveId == rhs.ObjectiveId; }

	void GenerateShape(int randomSeed);
};



