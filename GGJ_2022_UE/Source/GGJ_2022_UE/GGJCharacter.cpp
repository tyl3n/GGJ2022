
#include "GGJCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraActor.h"

#include "Utilities.h"
#include "GGJWorldSettings.h"
#include "DraggableActor.h"

//////////////////////////////////////////////////////////////////////////
// AGGJCharacter

AGGJCharacter::AGGJCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
}

void AGGJCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(AGGJWorldSettings* worldSettings = Utils::GetWorldSettings())
	{
		FViewTargetTransitionParams params;
		params.BlendTime = 0.0f;
		Cast<APlayerController>(GetController())->SetViewTarget(worldSettings->LevelCamera.Get(), params);
	}

}

void AGGJCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (DraggedActor != nullptr)
	{
		FVector mouseLocation, mouseDirection;
		APlayerController* playerController = Cast<APlayerController>(GetController());
		playerController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

		FPlane plane = FPlane(VecZ(DraggedActor->GetDraggableZ()), FVector::UpVector);
		FVector dragPoint = FMath::RayPlaneIntersection(mouseLocation, mouseDirection, plane);

		DraggedActor->Drag(dragPoint);
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGGJCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AGGJCharacter::SelectPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AGGJCharacter::SelectReleased);
}

void AGGJCharacter::SelectPressed()
{
	if (DraggedActor != nullptr)
	{
		// Failsafe
		SelectReleased();
	}
	DraggedActor = nullptr;

	UWorld* world = GetWorld();

	FVector mouseLocation, mouseDirection;
	APlayerController* playerController = Cast<APlayerController>(GetController());
	playerController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

	FCollisionQueryParams queryParams;
	FCollisionResponseParams responseParams;
	FHitResult selection;

	TWEAKABLE float Distance = 5000.0f;
	world->LineTraceSingleByChannel(selection, mouseLocation, mouseLocation + mouseDirection * Distance, ECC_Draggable, queryParams, responseParams);

	DraggedActor = Cast<ADraggableActor>(selection.Actor);

	if (DraggedActor != nullptr)
	{
		DraggedActor->OnDraggableSelected();
	}
}

void AGGJCharacter::SelectReleased()
{
	if(DraggedActor != nullptr)
	{
		DraggedActor->OnDraggableReleased();
		DraggedActor = nullptr;
	}
}
