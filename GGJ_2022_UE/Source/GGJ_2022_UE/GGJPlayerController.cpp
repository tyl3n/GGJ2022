// Copyright Epic Games, Inc. All Rights Reserved.

#include "GGJPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Utilities.h"
#include "GGJWorldSettings.h"
#include "DraggableActor.h"
#include "Camera/CameraActor.h"

AGGJPlayerController::AGGJPlayerController()
{
}

void AGGJPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (AGGJWorldSettings* worldSettings = Utils::GetWorldSettings())
	{
		FViewTargetTransitionParams params;
		params.BlendTime = 0.0f;
		this->SetViewTarget(worldSettings->LevelCamera.Get(), params);
		
	}
}