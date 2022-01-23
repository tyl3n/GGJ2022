// Fill out your copyright notice in the Description page of Project Settings.


#include "GGJShapeDefinition.h"

FGGJShapeDefinition::FGGJShapeDefinition()
{
	ShapeArray.SetNumZeroed(25);
}

FGGJShapeDefinition::FGGJShapeDefinition(const FString& string)
{
	ShapeArray.SetNumZeroed(25);
	MakeFromString(string);
}

FGGJShapeDefinition::FGGJShapeDefinition(uint32 code)
{
	ShapeArray.SetNumZeroed(25);
	MakeFromUInt32(code);
}

FGGJShapeDefinition::~FGGJShapeDefinition()
{
}

void FGGJShapeDefinition::MakeFromString(const FString& string)
{
	int idx = 0;
	for (int i = 0; i < string.Len(); ++i)
	{
		const TCHAR& value = string[i];

		if (value == 'X')
		{
			ShapeArray[idx++] = true;
		}
		else if(value == '_' || value == '0')
		{
			ShapeArray[idx++] = false;
		}
	}
}

void FGGJShapeDefinition::MakeFromUInt32(uint32 code)
{
	for (int i = 0; i < 25; ++i)
	{
		ShapeArray[24 - i] = code & 1;
		code = code >> 1;
	}
}

bool FGGJShapeDefinition::GetShapeValue(int x, int y) const
{
	int index = x + y * 5;
	if (ShapeArray.IsValidIndex(index))
	{
		return ShapeArray[index];
	}

	return false;
}

void FGGJShapeDefinition::SetShapeValue(int x, int y, bool shapeValue)
{
	int index = x + y * 5;
	if (ShapeArray.IsValidIndex(index))
	{
		ShapeArray[index] = shapeValue;
	}
}

void FGGJShapeDefinition::RotateCW()
{
	TArray<bool> oldShapeArray = ShapeArray;

	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			int index = x + y * 5;
			bool value = oldShapeArray[index];
			SetShapeValue(4 - y, x, value);
		}
	}

	PackShapeValues();
}

void FGGJShapeDefinition::PackShapeValues()
{
	int count = 0;
	bool bFirstLineEmpty = true;
	while(bFirstLineEmpty && count < 5)
	{
		bFirstLineEmpty = true;
		for (int i = 0; i < 5; ++i)
		{
			if (GetShapeValue(i, 0))
			{
				bFirstLineEmpty = false;
				break;
			}
		}

		if (bFirstLineEmpty)
		{
			for (int y = 0; y < 5; ++y)
			{
				for (int x = 0; x < 5; ++x)
				{
					bool value = GetShapeValue(x, y + 1);
					SetShapeValue(x, y, value);
				}
			}
			++count;
		}
	}

	count = 0;
	bool bFirstColumnEmpty = true;
	while (bFirstColumnEmpty && count < 5)
	{
		bFirstColumnEmpty = true;
		for (int i = 0; i < 5; ++i)
		{
			if (GetShapeValue(0, i))
			{
				bFirstColumnEmpty = false;
				break;
			}
		}

		if (bFirstColumnEmpty)
		{
			for (int y = 0; y < 5; ++y)
			{
				for (int x = 0; x < 5; ++x)
				{
					bool value = GetShapeValue(x + 1, y);
					SetShapeValue(x, y, value);
				}
			}
			++count;
		}
	}
}

bool FGGJShapeDefinition::Matches(const FGGJShapeDefinition& otherShape) const
{
	uint32 shapeCode = ToUInt32();

	FGGJShapeDefinition otherShapeCopy = otherShape;

	for (int i = 0; i < 4; ++i)
	{
		if (otherShapeCopy.ToUInt32() == shapeCode)
		{
			return true;
		}

		otherShapeCopy.RotateCW();
	}

	return false;
}

FGGJShapeDefinition FGGJShapeDefinition::GenerateShape(int seed, int shapeSize)
{
	FRandomStream random = FRandomStream(seed);

	shapeSize = FMath::Min(shapeSize, 25);

	TArray< TPair<int, int> > shapeCoords;
	TArray< TPair<int, int> > availableCoords;

	TPair<int, int>& initialCoord = availableCoords.AddZeroed_GetRef();
	initialCoord.Key = random.RandRange(0, 4);
	initialCoord.Value = random.RandRange(0, 4);

	while (shapeCoords.Num() < shapeSize && availableCoords.Num() > 0)
	{
		int nextIdx = random.RandRange(0, availableCoords.Num() - 1);

		TPair<int, int>& coord = availableCoords[nextIdx];

		TPair<int, int>& newCoord = shapeCoords.AddZeroed_GetRef();
		newCoord = coord;

		availableCoords.RemoveAt(nextIdx);

		TArray< TPair<int, int> > neighborCoords;
		GetNeighbors(newCoord.Key, newCoord.Value, neighborCoords);

		for (const TPair<int, int>& neighborCoord : neighborCoords)
		{
			if (!shapeCoords.Contains(neighborCoord))
			{
				availableCoords.AddUnique(neighborCoord);
			}
		}
	}

	FGGJShapeDefinition shapeDefinition;
	for (const TPair<int, int>& coord : shapeCoords)
	{
		shapeDefinition.SetShapeValue(coord.Key, coord.Value, true);
	}
	shapeDefinition.PackShapeValues();

	return shapeDefinition;
}

void FGGJShapeDefinition::GetNeighbors(int x, int y, TArray< TPair<int, int> >& out_NeighborCoords)
{
	if (y > 0)
	{
		out_NeighborCoords.Add(TPair<int, int>(x, y - 1));
	}

	if (y < 4)
	{
		out_NeighborCoords.Add(TPair<int, int>(x, y + 1));
	}

	if (x > 0)
	{
		out_NeighborCoords.Add(TPair<int, int>(x - 1, y));
	}

	if (x < 4)
	{
		out_NeighborCoords.Add(TPair<int, int>(x + 1, y));
	}
}

bool FGGJShapeDefinition::ShapeCodesMatch(uint32 codeA, uint32 codeB)
{
	// All 4 rotations
	for (int i = 0; i < 4; ++i)
	{
		if (codeA == codeB)
		{
			return true;
		}

		FGGJShapeDefinition rotatedCW = FGGJShapeDefinition(codeB);
		rotatedCW.RotateCW();
		codeB = rotatedCW.ToUInt32();
	}

	return false;
}

uint32 FGGJShapeDefinition::ToUInt32() const
{
	uint32 output = 0;
	for (int i = 0; i < 25; ++i)
	{
		output = output << 1;
		if (ShapeArray[i])
		{
			output |= 0x0001;
		}
	}

	return output;
}

FString FGGJShapeDefinition::ToString() const
{
	FString resultString;
	for (int y = 0; y < 5; ++y)
	{
		FString line;
		for (int x = 0; x < 5; ++x)
		{
			int index = x + y * 5;
			line += ShapeArray[index] ? TEXT("X") : TEXT("_");
		}

		resultString += line;

		if (y < 4)
		{
			resultString += TEXT("\r\n");
		}
	}

	return resultString;
}