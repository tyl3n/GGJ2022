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