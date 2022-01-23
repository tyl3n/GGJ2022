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
	FGGJShapeDefinition(const FString& string);
	~FGGJShapeDefinition();

	void MakeFromString(const FString& string);
	bool GetShapeValue(int x, int y) const;
	void SetShapeValue(int x, int y, bool shapeValue);

	void RotateCW();
	void PackShapeValues(); // Remove all leading void spaces

	bool Matches(const FGGJShapeDefinition& otherShape) const;
	
	uint32 ToUInt32() const;
	FString ToString() const;
};



