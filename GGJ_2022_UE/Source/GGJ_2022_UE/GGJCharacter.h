
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GGJCharacter.generated.h"

UCLASS(config=Game)
class AGGJCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	/// Dynamic

	UPROPERTY(Transient)
	TArray<class ADraggableActor*> TouchingDraggableActors;

	UPROPERTY(Transient)
	TArray<class ADraggableActor*> MergeableDraggableActors;

	AGGJCharacter();

protected:

	virtual void BeginPlay() override;
	virtual void PossessedBy(class AController* NewController) override;

	virtual void Tick(float deltaTime) override;
	void TickMergables();

	void SelectPressed();
	void SelectReleased();

	void RotateDraggable(float value);

public:
	UFUNCTION(BlueprintCallable)
	void MergeTouchingDraggableActors();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


private:
	UPROPERTY(Transient)
	class ADraggableActor* DraggedActor = nullptr;
};

