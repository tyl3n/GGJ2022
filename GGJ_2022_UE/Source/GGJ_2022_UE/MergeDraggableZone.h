
#pragma once

#include "MergeDraggableZone.generated.h"

UCLASS(config=Game)
class AMergeDraggableZone : public AActor
{
	GENERATED_BODY()
public:
	
	AMergeDraggableZone();

	UFUNCTION(BlueprintCallable)
	void MergeTouchingDraggableActors();
};

