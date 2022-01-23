// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GGJShapeDefinition.generated.h"

USTRUCT(BlueprintType)
struct FGGJShapeDefinition 
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> ShapeArray;

	FGGJShapeDefinition();
	FGGJShapeDefinition(const FString& string);
	FGGJShapeDefinition(uint32 code);
	~FGGJShapeDefinition();

	void MakeFromString(const FString& string);
	void MakeFromUInt32(uint32 code);
	bool GetShapeValue(int x, int y) const;
	void SetShapeValue(int x, int y, bool shapeValue);

	void RotateCW();
	void PackShapeValues(); // Remove all leading void spaces

	bool Matches(const FGGJShapeDefinition& otherShape) const;
	
	static FGGJShapeDefinition GenerateShape(int seed, int shapeSize);
	static void GetNeighbors(int x, int y, TArray< TPair<int, int> >& out_NeighborCoords);

	static bool ShapeCodesMatch(uint32 codeA, uint32 codeB);

	uint32 ToUInt32() const;
	FString ToString() const;
};



