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
	int RessourceBlockCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 35.0f;

	UPROPERTY(BlueprintReadWrite)
	int ResourceID = 0;

	UPROPERTY(BlueprintReadWrite)
	int ObjectiveId = 0;

	UPROPERTY(NotReplicated, BlueprintReadOnly)
	float AddedTimestamp = -1.0f;

	UPROPERTY(Transient)
	uint32 ShapeCode;

	FGGJObjective();
	~FGGJObjective();

	float TimeLeft = -1.0f;

	bool operator==(const FGGJObjective& rhs) const { return ObjectiveId == rhs.ObjectiveId; }

	void GenerateShape(int randomSeed);
};



