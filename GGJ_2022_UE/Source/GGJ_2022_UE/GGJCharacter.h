
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GGJPlayerState.h"
#include "DraggableActor.h"

#include "GGJCharacter.generated.h"

UCLASS(config=Game)
class AGGJCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	/// Config
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADraggableActor> CheatSpawnActor;

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

	void CheatSpawnResource(int resourceId);

public:

	bool GetDraggableActorBestMergeMatch(ADraggableActor* draggable, TPair<class UBoxComponent*, class UStaticMeshComponent*>& out_MergeMatch) const;

	UFUNCTION(BlueprintCallable)
	void MergeTouchingDraggableActors();

	void SacrificePiece_Client(class ADraggableActor* piece);

	UFUNCTION(Server, Reliable)
	void Server_SacrificePiece(EPlayerDuality duality, int resourceID, float resourceAmount, uint32 shapeCode);
	
	EMergeState GetAssociatedMergeState(ADraggableActor* actor) const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnMergeSuccessful();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSacrifice();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UPROPERTY(Transient, BlueprintReadOnly)
	class ADraggableActor* DraggedActor = nullptr;
};

