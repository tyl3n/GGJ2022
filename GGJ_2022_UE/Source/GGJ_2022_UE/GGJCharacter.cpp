
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
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "Utilities.h"
#include "GGJWorldSettings.h"
#include "DraggableActor.h"
#include "MergeDraggableZone.h"
#include "GGJGameState.h"
#include "GGJPlayerState.h"
#include "GGJGameMode.h"

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

							FVector overlapCompLocation = overlapComp->GetComponentLocation();

							float overlapDistSqr = (overlapCompLocation - primComp->GetComponentLocation()).SizeSquared2D();
							float dot = primComp->GetForwardVector() | overlapComp->GetForwardVector();
							if (overlapDistSqr < DistanceLimitSqr && (FMath::Abs(dot) >= (1.0f - DotLimit) || FMath::Abs(dot) <= DotLimit))
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
			EMergeState mergeState = GetAssociatedMergeState(dragActor);
			dragActor->SetMergeState(mergeState);
		}
	}

	int mergeableCount = MergeableDraggableActors.Num();

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
		}
	}

	if (MergeableDraggableActors.Num())
	{
		for (ADraggableActor * dragActor : MergeableDraggableActors)
		{
			dragActor->SetMergeState(GetAssociatedMergeState(dragActor));
		}
	}
}

EMergeState AGGJCharacter::GetAssociatedMergeState(ADraggableActor* actor) const
{
	if (MergeableDraggableActors.Contains(actor))
	{
		TPair<UBoxComponent*, UStaticMeshComponent*> dummy;
		if (GetDraggableActorBestMergeMatch(actor, dummy))
		{
			return EMergeState::OnPadAndMergable;
		}
		else
		{
			return EMergeState::OnPadButUnmergable;
		}
	}
	else
	{
		if (TouchingDraggableActors.Contains(actor))
		{
			return EMergeState::None;
		}
		else
		{
			return EMergeState::None;
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

	FInputActionBinding CheatSpawnResource0("Cheat_SpawnResource0", IE_Pressed);
	CheatSpawnResource0.ActionDelegate.GetDelegateForManualSet().BindLambda([this](){ CheatSpawnResource(0); });
	PlayerInputComponent->AddActionBinding(CheatSpawnResource0);

	FInputActionBinding Cheat_SpawnResource1("Cheat_SpawnResource1", IE_Pressed);
	Cheat_SpawnResource1.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() { CheatSpawnResource(1); });
	PlayerInputComponent->AddActionBinding(Cheat_SpawnResource1);

	FInputActionBinding Cheat_SpawnResource2("Cheat_SpawnResource2", IE_Pressed);
	Cheat_SpawnResource2.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() { CheatSpawnResource(2); });
	PlayerInputComponent->AddActionBinding(Cheat_SpawnResource2);

	FInputActionBinding Cheat_SpawnResource3("Cheat_SpawnResource3", IE_Pressed);
	Cheat_SpawnResource3.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() { CheatSpawnResource(3); });
	PlayerInputComponent->AddActionBinding(Cheat_SpawnResource3);

	FInputActionBinding Cheat_SpawnResource4("Cheat_SpawnResource4", IE_Pressed);
	Cheat_SpawnResource4.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() { CheatSpawnResource(4); });
	PlayerInputComponent->AddActionBinding(Cheat_SpawnResource4);

	FInputActionBinding Cheat_SpawnResourceRandom("Cheat_SpawnResourceRandom", IE_Pressed);
	Cheat_SpawnResourceRandom.ActionDelegate.GetDelegateForManualSet().BindLambda([this]() { CheatSpawnResource(-1); });
	PlayerInputComponent->AddActionBinding(Cheat_SpawnResourceRandom);

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

void AGGJCharacter::CheatSpawnResource(int resourceId)
{
#if !UE_BUILD_SHIPPING
	if (resourceId < 0 || resourceId > 4)
	{
		resourceId = FMath::RandRange(0, 4);
	}

	UWorld* world = GetWorld();

	FVector mouseLocation, mouseDirection;
	APlayerController* playerController = Cast<APlayerController>(GetController());
	playerController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

	FPlane plane = FPlane(VecZ(Utils::GetWorldSettings()->WorldZ), FVector::UpVector);
	FVector spawnPoint = FMath::RayPlaneIntersection(mouseLocation, mouseDirection, plane);

	FTransform transform = FTransform(FQuat::Identity, spawnPoint);
	ADraggableActor* cheatActor = GetWorld()->SpawnActorDeferred<ADraggableActor>(CheatSpawnActor, transform, nullptr, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	cheatActor->ResourceID = resourceId;
	UGameplayStatics::FinishSpawningActor(cheatActor, transform);
#endif
}

bool AGGJCharacter::GetDraggableActorBestMergeMatch(ADraggableActor* draggable, TPair<UBoxComponent*, UStaticMeshComponent*>& out_MergeMatch) const
{
	if (draggable == nullptr)
	{
		return nullptr;
	}

	float bestMatchDistSqr = BIG_NUMBER;
	ADraggableActor* out_ActorMatch = nullptr;

	for (int j = 0; j < MergeableDraggableActors.Num(); ++j)
	{
		if (MergeableDraggableActors[j] == draggable || MergeableDraggableActors[j] == nullptr)
		{
			// Don't check same piece
			continue;
		}

		if (draggable->ResourceID != MergeableDraggableActors[j]->ResourceID)
		{
			// not of same type
			continue;
		}

		TPair<UBoxComponent*, UStaticMeshComponent*> pair;
		float distSqr = ADraggableActor::ComputeSnapPair(draggable, MergeableDraggableActors[j], pair.Key, pair.Value);

		if (pair.Key != nullptr && distSqr < bestMatchDistSqr)
		{
			bestMatchDistSqr = distSqr;
			out_ActorMatch = MergeableDraggableActors[j];
			out_MergeMatch = pair;
		}
	}

	return out_ActorMatch != nullptr;
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

		for (int i = 0; i < MergeableDraggableActors.Num(); ++i)
		{
			TPair<UBoxComponent*, UStaticMeshComponent*> bestPair;

			if (GetDraggableActorBestMergeMatch(MergeableDraggableActors[i], bestPair))
			{
				snapPairs.Add(bestPair);
			}
		}

		for (TPair<UBoxComponent*, UStaticMeshComponent*>& snapPair : snapPairs)
		{
			if (snapPair.Value != nullptr)
			{
				UPrimitiveComponent* snapComponent = snapPair.Value;

				if (ADraggableActor* draggable = Cast<ADraggableActor>(snapComponent->GetOwner()))
				{
					ADraggableActor* mergeInto = draggable;
					while (mergeInto->MergedInto)
					{
						mergeInto = mergeInto->MergedInto;
					}

					if (draggable != mergeInto)
					{
						snapComponent = mergeInto->FindStaticMeshByTag(ADraggableActor::GetMeshTag(snapComponent));
					}
				}

				ADraggableActor::SnapDraggable(snapPair.Key, snapComponent);
				ADraggableActor::MergeDraggable(Cast<ADraggableActor>(snapPair.Key->GetOwner()), Cast<ADraggableActor>(snapPair.Value->GetOwner()));
			}
		}
	}
}

void AGGJCharacter::SacrificePiece_Client(ADraggableActor* piece)
{
	if (piece != nullptr)
	{
		if(AGGJPlayerState* playerState = Cast<AGGJPlayerState>(GetPlayerState()))
		{
			Server_SacrificePiece(playerState->Duality, piece->ResourceID, piece->StaticMeshes.Num() / 20.0f, piece->ShapeDefinition.ToUInt32());
		}
	
		piece->Destroy();
	}
}

void AGGJCharacter::Server_SacrificePiece_Implementation(EPlayerDuality duality, int resourceID, float resourceAmount, uint32 shapeCode)
{
	if (AGGJGameState* gameState = Utils::GetGameState())
	{
		gameState->AdjustResources(duality, resourceID, resourceAmount);
	}

	if (AGGJGameMode* gameMode = Utils::GetGameMode())
	{
		gameMode->OnShapeSacrifice(this, shapeCode, resourceID);
	}
}