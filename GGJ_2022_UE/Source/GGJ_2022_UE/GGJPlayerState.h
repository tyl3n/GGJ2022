
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

	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing=OnRep_Duality)
	EPlayerDuality Duality;

	UPROPERTY(Transient, BlueprintReadWrite, ReplicatedUsing=OnRep_ActiveObjectives)
	TArray<FGGJObjective> ActiveObjectives;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated = OnRep_CompletedObjectives)
	TArray<FGGJObjective> CompletedObjectives;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated = OnRep_FailedObjectives)
	TArray<FGGJObjective> FailedObjectives;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateEvent OnActiveObjectiveChanged;

	AGGJPlayerState();

	virtual void Tick(float deltaTime) override;

	bool HasObjective(int objectiveId) const;
	void CompleteObjective_Server(int objectiveId);

	UFUNCTION()
	void OnRep_ActiveObjectives(const TArray<FGGJObjective>& oldActiveObjectives);

	UFUNCTION()
	void OnRep_CompletedObjectives(const TArray<FGGJObjective>& oldCompletedObjectives);

	UFUNCTION()
	void OnRep_FailedObjectives(const TArray<FGGJObjective>& oldFailedObjectives);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDualityChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewObjective();

	UFUNCTION(BlueprintImplementableEvent)
	void OnObjectiveFailed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnObjectiveCompleted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnObjectiveQuarterLeft();

	UFUNCTION()
	void OnRep_Duality() { OnDualityChanged(); }
};



