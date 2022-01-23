
#include "GGJObjective.h"
#include "GGJShapeDefinition.h"

FGGJObjective::FGGJObjective()
{
}

FGGJObjective::~FGGJObjective()
{
}

void FGGJObjective::GenerateShape(int randomSeed)
{
	ShapeCode = FGGJShapeDefinition::GenerateShape(ObjectiveId + randomSeed, RessourceBlockCount).ToUInt32();
}