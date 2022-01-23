
#pragma once

#include "SacrificeDraggableZone.generated.h"

UCLASS(config=Game)
class ASacrificeDraggableZone : public AActor
{
	GENERATED_BODY()
public:
	
	UPROPERTY(Transient)
	TArray<class UPrimitiveComponent*> SafeZones;

	UPROPERTY(Transient)
	TArray<class UPrimitiveComponent*> SacrificeZones;
	
	ASacrificeDraggableZone();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);
};

