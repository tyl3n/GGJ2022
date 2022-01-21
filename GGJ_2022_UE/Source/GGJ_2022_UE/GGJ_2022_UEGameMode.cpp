// Copyright Epic Games, Inc. All Rights Reserved.

#include "GGJ_2022_UEGameMode.h"
#include "GGJ_2022_UECharacter.h"
#include "UObject/ConstructorHelpers.h"

AGGJ_2022_UEGameMode::AGGJ_2022_UEGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
