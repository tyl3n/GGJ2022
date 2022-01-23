#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"

#include "GGJObjective.h"

#include "GGJWorldSettings.generated.h"

UCLASS(minimalapi, Blueprintable)
class AGGJWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:

	//////////////////////////////////////////////////////////////////////////
	/// Config

	UPROPERTY(EditAnywhere, Category="GGJ")
	TSoftObjectPtr<class ACameraActor> LevelCamera;

	UPROPERTY(EditAnywhere, Category = "GGJ")
	float WorldZ = 185.0f;

	UPROPERTY(EditAnywhere)
	int NumberOfInitialMission = 3;

	UPROPERTY(EditAnywhere)
	TArray<float> InitialBonusTime;

	UPROPERTY(EditAnywhere, Category = "GGJ")
	float MissionsInterval = 20.0f;

	UPROPERTY(EditAnywhere, Category = "GGJ")
	TArray<FGGJObjective> ObjectiveDefinitions;

	AGGJWorldSettings();

	virtual void BeginPlay() override;
};



