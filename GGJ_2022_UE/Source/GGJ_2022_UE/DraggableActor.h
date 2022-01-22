
#pragma once

#include "GameFramework/Character.h"

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
	FRuntimeFloatCurve DragForce;

	////////////////////////////////////////
	/// Dynamic

	bool bDraging = false;

	ADraggableActor();

	UFUNCTION(BlueprintNativeEvent)
	void OnDraggableSelected();

	UFUNCTION(BlueprintNativeEvent)
	void OnDraggableReleased();

	UFUNCTION(BlueprintNativeEvent)
	void OnDragged(float distance);

	virtual void Drag(FVector& dragPoint);
	virtual float GetDraggableZ() const;
};

