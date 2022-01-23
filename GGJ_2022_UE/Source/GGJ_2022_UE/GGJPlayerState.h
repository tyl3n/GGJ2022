
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "GGJObjective.h"

#include "GGJPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateEvent, class AGGJPlayerState*, playerState);

UENUM(BlueprintType)
enum class EPlayerDuality : uint8
{
	Unassigned,
	Angel,
	Devil,
};

UCLASS(minimalapi)
class AGGJPlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	EPlayerDuality Duality;

	UPROPERTY(Transient, BlueprintReadWrite, ReplicatedUsing=OnRep_ActiveObjectives)
	TArray<FGGJObjective> ActiveObjectives;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	TArray<FGGJObjective> CompletedObjectives;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	TArray<FGGJObjective> FailedObjectives;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateEvent OnActiveObjectiveChanged;

	AGGJPlayerState();

	virtual void Tick(float deltaTime) override;

	bool HasObjective(int objectiveId) const;

	UFUNCTION()
	void OnRep_ActiveObjectives();
};



