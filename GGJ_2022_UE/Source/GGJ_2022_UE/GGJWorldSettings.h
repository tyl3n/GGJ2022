#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"

#include "GGJWorldSettings.generated.h"

UCLASS(minimalapi, Blueprintable)
class AGGJWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category="GGJ")
	TSoftObjectPtr<class ACameraActor> LevelCamera;

	AGGJWorldSettings();
};



