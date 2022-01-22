
#pragma once

#include "DraggableActor.generated.h"

UCLASS(config=Game)
class ADraggableActor : public AActor
{
	GENERATED_BODY()
public:
	
	////////////////////////////////////////
	/// Config

	UPROPERTY(EditDefaultsOnly)
	float DragForceBaseMultiplier = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float DragSprintMultiplier = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	FRuntimeFloatCurve DragForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Mass = 177.83;

	////////////////////////////////////////
	/// Dynamic

	UPROPERTY(BlueprintReadOnly)
	bool bDraging = false;

	float ReleasedTimestamp;

	UPROPERTY(Transient)
	TArray<class UStaticMeshComponent*> StaticMeshes;

	UPROPERTY(Transient)
	TArray<class UBoxComponent*> BoxComponents;

	ADraggableActor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnDraggableSelected();

	UFUNCTION(BlueprintNativeEvent)
	void OnDraggableReleased();

	UFUNCTION(BlueprintNativeEvent)
	void OnDragged(float distance);

	virtual void Drag(FVector& dragPoint, float deltaTime);
	virtual float GetDraggableZ() const;

	void RefreshComponents();
	void RefreshMass();

	static void MergeDraggable(ADraggableActor* actorA, ADraggableActor* actorB);
	static void ComputeSnapPair(ADraggableActor* snapPivot, ADraggableActor* snapMover, class UBoxComponent*& out_SnapSocket, class UStaticMeshComponent*& out_SnapComponent);
	static void SnapDraggable(class UBoxComponent* snapSocket, class UPrimitiveComponent* snapComponent);
};

