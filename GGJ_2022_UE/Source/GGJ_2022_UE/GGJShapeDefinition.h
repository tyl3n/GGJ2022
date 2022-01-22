// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GGJShapeDefinition.generated.h"

USTRUCT(Blueprintable)
struct FGGJShapeDefinition 
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> ShapeArray;

	FGGJShapeDefinition();
	~FGGJShapeDefinition();
};



