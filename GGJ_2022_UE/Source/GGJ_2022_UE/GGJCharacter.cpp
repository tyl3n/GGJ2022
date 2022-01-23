
#include "GGJCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"

#include "Utilities.h"
#include "GGJWorldSettings.h"
#include "DraggableActor.h"
#include "MergeDraggableZone.h"
#include "GGJGameState.h"
#include "GGJPlayerState.h"

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

	if (AGGJWorldSettings* worldSettings = Utils::GetWorldSettings())
	{
		FViewTargetTransitionParams params;
		params.BlendTime = 0.0f;

		if(APlayerController* playerController = Cast<APlayerController>(GetController()))
		{
			playerController->SetViewTarget(worldSettings->LevelCamera.Get(), params);
		}
	}
}

void AGGJCharacter::PossessedBy(class AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AGGJWorldSettings* worldSettings = Utils::GetWorldSettings())
	{
		FViewTargetTransitionParams params;
		params.BlendTime = 0.0f;

		Cast<APlayerController>(NewController)->SetViewTarget(worldSettings->LevelCamera.Get(), params);
	}
}


void AGGJCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if(IsLocallyControlled())
	{
		if (DraggedActor != nullptr)
		{
			FVector mouseLocation, mouseDirection;
			APlayerController* playerController = Cast<APlayerController>(GetController());
			playerController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

			FPlane plane = FPlane(VecZ(DraggedActor->GetDraggableZ()), FVector::UpVector);
			FVector dragPoint = FMath::RayPlaneIntersection(mouseLocation, mouseDirection, plane);

			DraggedActor->Drag(dragPoint, deltaTime);
		}
	
		TickMergables();
	}
}

void AGGJCharacter::TickMergables()
{
	TArray<ADraggableActor*> touchingDraggables;
	TArray<ADraggableActor*> mergeableDraggableActors;

	for (TActorIterator<AMergeDraggableZone> it(GetWorld()); it; ++it)
	{
		for (UActorComponent* component : it->GetComponents())
		{
			if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(component))
			{
				if (primComp->ComponentTags.Contains(TEXT("MergeZone")))
				{
					TSet<UPrimitiveComponent*> overlappingComponent;
					primComp->GetOverlappingComponents(overlappingComponent);

					for (UPrimitiveComponent* overlapComp : overlappingComponent)
					{
						if (ADraggableActor* draggableActor = Cast<ADraggableActor>(overlapComp->GetOwner()))
						{
							touchingDraggables.AddUnique(draggableActor);

							TWEAKABLE float DistanceLimitSqr = 35.0f * 35.0f;
							TWEAKABLE float DotLimit = FMath::Cos(17.5f);

							float overlapDistSqr = (overlapComp->GetComponentLocation() - primComp->GetComponentLocation()).SizeSquared2D();
							float dot = primComp->GetForwardVector() | overlapComp->GetForwardVector();
							if (overlapDistSqr < DistanceLimitSqr && (dot > DotLimit || FMath::Abs(dot) < 1.0f - DotLimit))
							{
								mergeableDraggableActors.AddUnique(draggableActor);
							}
						}
					}
				}
			}
		}
	}

	TouchingDraggableActors.Remove(nullptr);
	MergeableDraggableActors.Remove(nullptr);

	for (int i = TouchingDraggableActors.Num() - 1; i >= 0; --i)
	{
		ADraggableActor* dragActor = TouchingDraggableActors[i];
		if (!touchingDraggables.Contains(dragActor))
		{
			// Not in zone anymore
			dragActor->SetMergeState(EMergeState::None);
			TouchingDraggableActors.RemoveAt(i);
		}
	}
	for (ADraggableActor* dragActor : touchingDraggables)
	{
		if (!TouchingDraggableActors.Contains(dragActor))
		{
			// Newly in zone
			TouchingDraggableActors.Add(dragActor);
			EMergeState mergeState = mergeableDraggableActors.Contains(dragActor) ? EMergeState::OnPadAndMergable : EMergeState::OnPadButUnmergable;
			dragActor->SetMergeState(mergeState);
		}
	}

	for (int i = MergeableDraggableActors.Num() - 1; i >= 0; --i)
	{
		ADraggableActor* dragActor = MergeableDraggableActors[i];
		if (!mergeableDraggableActors.Contains(dragActor))
		{
			if (TouchingDraggableActors.Contains(dragActor))
			{
				// Not in mergeable anymore
				dragActor->SetMergeState(EMergeState::OnPadButUnmergable);
			}

			MergeableDraggableActors.RemoveAt(i);
		}
	}
	for (ADraggableActor* dragActor : mergeableDraggableActors)
	{
		if (!MergeableDraggableActors.Contains(dragActor))
		{
			// Newly mergeable
			MergeableDraggableActors.Add(dragActor);
			dragActor->SetMergeState(EMergeState::OnPadAndMergable);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGGJCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AGGJCharacter::SelectPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AGGJCharacter::SelectReleased);

	PlayerInputComponent->BindAxis("Rotate", this, &AGGJCharacter::RotateDraggable);
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

	TWEAKABLE float Distance = 10000.0f;
	world->LineTraceSingleByChannel(selection, mouseLocation, mouseLocation + mouseDirection * Distance, ECC_Draggable, queryParams, responseParams);

	DraggedActor = Cast<ADraggableActor>(selection.Actor);

	if (DraggedActor != nullptr)
	{
		DraggedActor->OnDraggableSelected(selection.ImpactPoint);
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

void AGGJCharacter::RotateDraggable(float value)
{
	if(value != 0.0f)
	{
		if (DraggedActor != nullptr)
		{
			DraggedActor->Rotate(value);
		}
	}
}

void AGGJCharacter::MergeTouchingDraggableActors()
{
	if (MergeableDraggableActors.Num() > 1)
	{
		MergeableDraggableActors.Sort([](const ADraggableActor& A, const ADraggableActor& B)
			{
				return A.ReleasedTimestamp < B.ReleasedTimestamp;
			});

		TArray< TPair<UBoxComponent*, UStaticMeshComponent*> > snapPairs;

		for (int i = 0; i < MergeableDraggableActors.Num() - 1; ++i)
		{
			TPair<UBoxComponent*, UStaticMeshComponent*>& pair = snapPairs.AddZeroed_GetRef();
			ADraggableActor::ComputeSnapPair(MergeableDraggableActors[i], MergeableDraggableActors[i + 1], pair.Key, pair.Value);
			if (pair.Key == nullptr)
			{
				snapPairs.RemoveAt(snapPairs.Num() - 1);
			}
		}

		for (TPair<UBoxComponent*, UStaticMeshComponent*>& snapPair : snapPairs)
		{
			if (snapPair.Value != nullptr)
			{
				ADraggableActor::SnapDraggable(snapPair.Key, snapPair.Value);
				ADraggableActor::MergeDraggable(Cast<ADraggableActor>(snapPair.Key->GetOwner()), Cast<ADraggableActor>(snapPair.Value->GetOwner()));
			}
		}
	}
}

void AGGJCharacter::SacrificePiece_Client(ADraggableActor* piece)
{
	if(AGGJPlayerState* playerState = Cast<AGGJPlayerState>(GetPlayerState()))
	{
		Server_SacrificePiece(playerState->Duality, piece->RessourceID, piece->StaticMeshes.Num() / 20.0f);
	}

	if(piece != nullptr)
	{
		piece->Destroy();
	}
}

void AGGJCharacter::Server_SacrificePiece_Implementation(EPlayerDuality duality, int resourceIndex, float resourceAmount)
{
	if (AGGJGameState* gameState = Utils::GetGameState())
	{
		gameState->AdjustResources(duality, resourceIndex, resourceAmount);
	}
}