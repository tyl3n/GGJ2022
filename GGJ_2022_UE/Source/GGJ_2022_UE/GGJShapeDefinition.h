// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GGJShapeDefinition.generated.h"

UCLASS(Blueprintable)
class UGGJShapeDefinition : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> ShapeArray;

	UGGJShapeDefinition();
	~UGGJShapeDefinition();
};



