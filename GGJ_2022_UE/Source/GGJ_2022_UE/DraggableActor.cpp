
#include "DraggableActor.h"

#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Engine/World.h"

#include "Utilities.h"

ADraggableActor::ADraggableActor()
{
}

void ADraggableActor::BeginPlay()
{
	Super::BeginPlay();

	RefreshComponents();
	RefreshMass();

	for (UBoxComponent* boxCOmp : BoxComponents)
	{
		// Refresh overlaps
		boxCOmp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		boxCOmp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	ReleasedTimestamp = 0.0f;
}

void ADraggableActor::SetMergeState(EMergeState newMergeState)
{
	if (newMergeState != MergeState)
	{
		MergeState = newMergeState;
		OnMergeStateChanged();
	}
}

void ADraggableActor::OnDraggableSelected_Implementation()
{

}

void ADraggableActor::OnDraggableReleased_Implementation()
{
	bDraging = false;
	ReleasedTimestamp = Utils::GetGameTime();
}

void ADraggableActor::OnDragged_Implementation(float length)
{
	bDraging = true;
}

void ADraggableActor::Drag(FVector& dragPoint, float deltaTime)
{
	if (UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		FVector location = staticMesh->GetComponentLocation();
		FVector dragVector = Vec2D(dragPoint - location);

		FVector dragDir;
		float dragLength;
		dragVector.ToDirectionAndLength(dragDir, dragLength);

		float forceStrength = DragForce.GetRichCurveConst()->Eval(dragLength) * DragForceBaseMultiplier;

		TWEAKABLE float SpringMax = 1000.0f;
		TWEAKABLE float SpringMin = 500.0f;
		float springMultiplier = MapClamped(dragLength, SpringMax, SpringMin, 0.0f, 1.0f);

		FVector force = dragDir * (forceStrength  - (DragSprintMultiplier * dragLength * springMultiplier));

		staticMesh->AddForce(force);

		OnDragged(dragLength);


		GetWorld()->LineBatcher->DrawLine(location, dragPoint, DrawLineColor, SDPG_World, DragLineThickness);
	}
}

float ADraggableActor::GetDraggableZ() const
{
	if (UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		return staticMesh->GetComponentLocation().Z;
	}

	return 0.0f;
}

void ADraggableActor::Rotate(float dirSign)
{
	if (UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		staticMesh->AddAngularImpulseInRadians(FVector(dirSign * RotateMultiplier, dirSign * RotateMultiplier, dirSign * RotateMultiplier) * StaticMeshes.Num(), NAME_None, RotateAsVelChange);
	}
}

void ADraggableActor::RefreshComponents()
{
	int staticMeshesCount = StaticMeshes.Num();
	for (UActorComponent* component : GetComponents())
	{
		if (UStaticMeshComponent* primComp = Cast<UStaticMeshComponent>(component))
		{
			
			StaticMeshes.AddUnique(primComp);

		}

		if (UBoxComponent* boxComp = Cast<UBoxComponent>(component))
		{
			BoxComponents.AddUnique(boxComp);
		}
	}

	if (staticMeshesCount != StaticMeshes.Num())
	{
		OnStaticMeshComponentChanged();
	}
}

void ADraggableActor::RefreshMass()
{
	if(StaticMeshes.Num() > 0)
	{
		float splitMass = Mass / (float)(StaticMeshes.Num());
		for (UStaticMeshComponent* component : StaticMeshes)
		{
			component->SetMassOverrideInKg(NAME_None, splitMass);
		}
	}
}

void ADraggableActor::MergeDraggable(ADraggableActor* actorA, ADraggableActor* actorB)
{
	if (ensure(actorA != nullptr && actorB != nullptr))
	{
		if (UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(actorA->GetComponentByClass(UStaticMeshComponent::StaticClass())))
		{
			for (UActorComponent* component : actorB->GetComponents())
			{
				if (UStaticMeshComponent* primComp = Cast<UStaticMeshComponent>(component))
				{
					UStaticMeshComponent* actorComponent = NewObject<UStaticMeshComponent>(actorA);
					actorComponent->OnComponentCreated();
					actorComponent->SetupAttachment(staticMesh);
					actorComponent->SetMobility(EComponentMobility::Movable);
					actorComponent->SetCollisionProfileName(primComp->GetCollisionProfileName());
					actorComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					actorComponent->SetStaticMesh(primComp->GetStaticMesh());
					actorComponent->SetMaterial(0, primComp->GetMaterial(0));
					actorComponent->SetWorldTransform(primComp->GetComponentTransform());
					actorComponent->RegisterComponent();
					actorComponent->InitializeComponent();
				}

				if (UBoxComponent* boxComp = Cast<UBoxComponent>(component))
				{
					UBoxComponent* actorComponent = NewObject<UBoxComponent>(actorA);
					actorComponent->OnComponentCreated();
					actorComponent->SetupAttachment(staticMesh);
					actorComponent->SetMobility(EComponentMobility::Movable);
					actorComponent->SetBoxExtent(boxComp->GetScaledBoxExtent());
					actorComponent->SetCollisionObjectType(boxComp->GetCollisionObjectType());
					actorComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					actorComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
					actorComponent->SetCollisionResponseToChannel(ECC_Draggable, ECollisionResponse::ECR_Overlap);
					actorComponent->SetWorldTransform(boxComp->GetComponentTransform());
					actorComponent->RegisterComponent();
					actorComponent->InitializeComponent();
				}
			}
		}

		actorB->Destroy();
		actorA->RefreshComponents();
		actorA->RefreshMass();
	}
}

void ADraggableActor::ComputeSnapPair(ADraggableActor* snapPivot, ADraggableActor* snapMover, class UBoxComponent*& out_SnapSocket, class UStaticMeshComponent*& out_SnapComponent)
{
	float bestOverlapDistanceSqr = BIG_NUMBER;
	out_SnapSocket = nullptr;
	out_SnapComponent = nullptr;

	for (UBoxComponent* boxComponent : snapPivot->BoxComponents)
	{
		TArray<UPrimitiveComponent*> overlappingComponents;
		boxComponent->GetOverlappingComponents(overlappingComponents);

		for (UPrimitiveComponent* overlappingComp : overlappingComponents)
		{
			if(UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(overlappingComp))
			{
				if (overlappingComp->GetOwner() == snapMover)
				{
					float distSqr = (overlappingComp->GetComponentLocation() - boxComponent->GetComponentLocation()).SizeSquared();
					if (distSqr < bestOverlapDistanceSqr)
					{
						bestOverlapDistanceSqr = distSqr;
						out_SnapSocket = boxComponent;
						out_SnapComponent = staticMesh;
					}

				}
			}
		}
	}
}

void ADraggableActor::SnapDraggable(class UBoxComponent* snapSocket, class UPrimitiveComponent* snapComponent)
{
	FVector snapSocketLocation = snapSocket->GetComponentLocation() + snapSocket->GetForwardVector() * 25.0f;

	float socketAngle = snapSocket->GetComponentRotation().Yaw;
	float snapAngle = snapComponent->GetComponentRotation().Yaw;
	float deltaYaw = FMath::Fmod(Utils::NormalizeRotAngle(socketAngle - snapAngle), 90.0f);

	if (deltaYaw > 45.0f)
	{
		deltaYaw = -90.0f + deltaYaw;
	}
	else if (deltaYaw < -45.0f)
	{
		deltaYaw = 90.0f + deltaYaw;
	}

	FRotator snapSocketRotator = FRotator(0.0f, snapAngle + deltaYaw, 0.0f);
	FQuat snapSocketQuat = snapSocketRotator.Quaternion();

	if(snapComponent->GetAttachParent() != nullptr)
	{
		FVector offset = -snapSocketQuat.RotateVector(snapComponent->GetRelativeLocation());
		snapSocketLocation += offset;
	}

	FVector snapSocketScale = snapComponent->GetComponentScale();


	if (snapComponent->GetAttachParent() == nullptr)
	{
		// Root
		snapComponent->SetWorldTransform(FTransform(snapSocketQuat, snapSocketLocation, snapSocketScale));
	}
	else
	{
		snapComponent->GetAttachParent()->SetWorldTransform(FTransform(snapSocketQuat, snapSocketLocation, snapSocketScale));
	}
}