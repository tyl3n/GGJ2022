
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GGJCharacter.generated.h"

UCLASS(config=Game)
class AGGJCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGGJCharacter();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	void SelectPressed();
	void SelectReleased();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


private:
	UPROPERTY(Transient)
	class ADraggableActor* DraggedActor = nullptr;
};

