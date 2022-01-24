
#pragma once

#include "GGJShapeDefinition.h"

#include "DraggableActor.generated.h"

UENUM(BlueprintType)
enum class EMergeState : uint8
{
	None,
	OnPadButUnmergable,
	OnPadAndMergable,
	JustMerged,
};


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
	float MassExpOnStaticMeshNum = 1.0f;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Drag Spring Multiplier"))
	float DragSprintMultiplier = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float RotateMultiplier = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float RotateExpOnStaticMeshNum = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	bool RotateAsVelChange = false;

	UPROPERTY(EditDefaultsOnly)
	FRuntimeFloatCurve DragForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Mass = 177.83;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DragLineThickness = 2.0f;

	////////////////////////////////////////
	/// Dynamic

	FVector RelativeDragPoint;

	UPROPERTY(BlueprintReadWrite)
	FColor Color = FColor::Yellow;

	UPROPERTY(BlueprintReadWrite)
	FColor DrawLineColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "4", UIMin = "0", UIMax = "4"))
	int ResourceID = -1;

	UPROPERTY(BlueprintReadOnly)
	bool bDraging = false;

	UPROPERTY(BlueprintReadOnly)
	EMergeState MergeState;

	UPROPERTY(BlueprintReadOnly)
	FGGJShapeDefinition ShapeDefinition;

	float ReleasedTimestamp;

	UPROPERTY(Transient, BlueprintReadOnly)
	class UStaticMeshComponent* MasterStaticMesh;

	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<class UStaticMeshComponent*> StaticMeshes;

	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<class UBoxComponent*> BoxComponents;

	FVector RelativeCenter;

	UPROPERTY(Transient)
	ADraggableActor* MergedInto = nullptr;

	FTransform SnapDeltaTransform;

	ADraggableActor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnDraggableSelected(const FVector& dragPoint);

	UFUNCTION(BlueprintNativeEvent)
	void OnDraggableReleased();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMergeStateChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaticMeshComponentChanged();

	void SetMergeState(EMergeState newMergeState);

	UFUNCTION(BlueprintNativeEvent)
	void OnDragged(float distance);

	UFUNCTION(BlueprintCallable)
	FVector GetDraggableCenter() const;

	virtual void Drag(FVector& dragPoint, float deltaTime);
	virtual float GetDraggableZ() const;

	virtual void Rotate(float dirSign);

	void RefreshComponents();
	void RefreshMass();

	static void MergeDraggable(ADraggableActor* actorA, ADraggableActor* actorB);
	// Return the dist squared of the match
	static float ComputeSnapPair(ADraggableActor* snapPivot, ADraggableActor* snapMover, class UBoxComponent*& out_SnapSocket, class UStaticMeshComponent*& out_SnapComponent);
	static void SnapDraggable(class UBoxComponent* snapSocket, class UPrimitiveComponent* snapComponent);

	void GenerateShapeDefinition();

	class UStaticMeshComponent* FindStaticMeshByTag(FString meshTag);

	static FString GetMeshTag(class UPrimitiveComponent* primComp);
	static FString CreateMeshTag(class UPrimitiveComponent* primComp);
	static void CopyMeshTag(class UPrimitiveComponent* primCompFrom, class UPrimitiveComponent* primCompTo);
};

