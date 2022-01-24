
#include "DraggableActor.h"

#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Engine/World.h"

#include "Utilities.h"
#include "GGJShapeDefinition.h"

ADraggableActor::ADraggableActor()
{
}

void ADraggableActor::BeginPlay()
{
	Super::BeginPlay();

	RefreshComponents();
	RefreshMass();

	for (UStaticMeshComponent* staticMesh : StaticMeshes)
	{
		if (staticMesh->GetAttachParent() == nullptr)
		{
			MasterStaticMesh = staticMesh;
			break;
		}
	}

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

void ADraggableActor::OnDraggableSelected_Implementation(const FVector& dragPoint)
{
	if(MasterStaticMesh != nullptr)
	{
		RelativeDragPoint = MasterStaticMesh->GetComponentTransform().InverseTransformPosition(dragPoint);
	}
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
	if (MasterStaticMesh != nullptr)
	{
		FVector location = MasterStaticMesh->GetComponentTransform().TransformPosition(RelativeDragPoint);
		FVector dragVector = Vec2D(dragPoint - location);

		FVector dragDir;
		float dragLength;
		dragVector.ToDirectionAndLength(dragDir, dragLength);

		float forceStrength = DragForce.GetRichCurveConst()->Eval(dragLength) * DragForceBaseMultiplier;

		TWEAKABLE float SpringMax = 1000.0f;
		TWEAKABLE float SpringMin = 500.0f;
		float springMultiplier = MapClamped(dragLength, SpringMax, SpringMin, 0.0f, 1.0f);

		FVector force = dragDir * (forceStrength  - (DragSprintMultiplier * dragLength * springMultiplier));

		MasterStaticMesh->AddForce(force);

		OnDragged(dragLength);


		GetWorld()->LineBatcher->DrawLine(Vec2D(location) + VecZ(MasterStaticMesh->GetComponentLocation().Z), dragPoint, DrawLineColor, SDPG_World, DragLineThickness);
	}
}

float ADraggableActor::GetDraggableZ() const
{
	if (MasterStaticMesh != nullptr)
	{
		return MasterStaticMesh->GetComponentLocation().Z;
	}

	return 0.0f;
}

void ADraggableActor::Rotate(float dirSign)
{
	if (MasterStaticMesh != nullptr)
	{
		MasterStaticMesh->AddAngularImpulseInRadians(FVector(dirSign * RotateMultiplier, dirSign * RotateMultiplier, dirSign * RotateMultiplier) * StaticMeshes.Num(), NAME_None, RotateAsVelChange);
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
			CreateMeshTag(primComp);
		}

		if (UBoxComponent* boxComp = Cast<UBoxComponent>(component))
		{
			BoxComponents.AddUnique(boxComp);
		}
	}

	if (staticMeshesCount != StaticMeshes.Num())
	{
		OnStaticMeshComponentChanged();
		GenerateShapeDefinition();
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
		while (actorA->MergedInto != nullptr)
		{
			actorA = actorA->MergedInto;
		}

		while (actorB->MergedInto)
		{
			actorB = actorB->MergedInto;
		}

		if (actorA == actorB)
		{
			// Already merged
			return;
		}

		if (actorA->MasterStaticMesh != nullptr)
		{
			for (UActorComponent* component : actorB->GetComponents())
			{
				if (UStaticMeshComponent* primComp = Cast<UStaticMeshComponent>(component))
				{
					UStaticMeshComponent* actorComponent = NewObject<UStaticMeshComponent>(actorA);
					actorComponent->OnComponentCreated();
					actorComponent->SetupAttachment(actorA->MasterStaticMesh);
					actorComponent->SetMobility(EComponentMobility::Movable);
					actorComponent->SetCollisionProfileName(primComp->GetCollisionProfileName());
					actorComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					actorComponent->SetStaticMesh(primComp->GetStaticMesh());
					actorComponent->SetMaterial(0, primComp->GetMaterial(0));
					actorComponent->SetWorldTransform(primComp->GetComponentTransform());
					actorComponent->RegisterComponent();
					actorComponent->InitializeComponent();
					CopyMeshTag(primComp, actorComponent);
				}

				if (UBoxComponent* boxComp = Cast<UBoxComponent>(component))
				{
					UBoxComponent* actorComponent = NewObject<UBoxComponent>(actorA);
					actorComponent->OnComponentCreated();
					actorComponent->SetupAttachment(actorA->MasterStaticMesh);
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

		actorB->MergedInto = actorA;
		actorB->Destroy();
		actorA->RefreshComponents();
		actorA->RefreshMass();
	}
}

float ADraggableActor::ComputeSnapPair(ADraggableActor* snapPivot, ADraggableActor* snapMover, class UBoxComponent*& out_SnapSocket, class UStaticMeshComponent*& out_SnapComponent)
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

	return bestOverlapDistanceSqr;
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

	UPrimitiveComponent* primComp;
	if (snapComponent->GetAttachParent() == nullptr)
	{
		// Root
		primComp = snapComponent;
	}
	else
	{
		primComp = Cast<UPrimitiveComponent>(snapComponent->GetAttachParent());
	}

	FTransform newTransform = FTransform(snapSocketQuat, snapSocketLocation, snapSocketScale);
	FTransform deltaTransform = newTransform * primComp->GetComponentTransform().Inverse();

	primComp->SetWorldTransform(newTransform);
}

void ADraggableActor::GenerateShapeDefinition()
{
	int minX = 1000;
	int minY = 1000;
	TArray< TPair<int, int> > coords;

	for (UStaticMeshComponent* staticMesh : StaticMeshes)
	{
		TPair<int, int>& coord = coords.AddZeroed_GetRef();
		if (staticMesh->GetAttachParent() == nullptr)
		{
			coord.Key = 0;
			coord.Value = 0;
		}
		else
		{
			FVector relativeLocation = staticMesh->GetRelativeLocation();
			coord.Key = FMath::RoundToInt(relativeLocation.X / 100.0f);
			coord.Value = FMath::RoundToInt(relativeLocation.Y / 100.0f);
		}

		if (coord.Key < minX)
		{
			minX = coord.Key;
		}

		if (coord.Value < minY)
		{
			minY = coord.Value;
		}
	}

	for (TPair<int, int>& coord : coords)
	{
		coord.Key -= minX;
		coord.Value -= minY;
	}

	ShapeDefinition = FGGJShapeDefinition();

	for (TPair<int, int>& coord : coords)
	{
		ShapeDefinition.SetShapeValue(coord.Key, coord.Value, true);
	}
}

UStaticMeshComponent* ADraggableActor::FindStaticMeshByTag(FString meshTag)
{
	for (UStaticMeshComponent* staticMesh : StaticMeshes)
	{
		FString tagMesh = GetMeshTag(staticMesh);
		if (tagMesh == meshTag)
		{
			return staticMesh;
		}
	}

	return nullptr;
}

FString ADraggableActor::GetMeshTag(class UPrimitiveComponent* primComp)
{
	if (primComp != nullptr)
	{
		for (const FName& tag : primComp->ComponentTags)
		{
			if (tag.ToString().StartsWith(TEXT("Name:")))
			{
				return tag.ToString();
			}
		}
	}

	return TEXT("");
}

FString ADraggableActor::CreateMeshTag(class UPrimitiveComponent* primComp)
{
	if (primComp != nullptr)
	{
		FString tag = GetMeshTag(primComp);
		if (tag.IsEmpty())
		{
			FString newTag = FString::Printf(TEXT("Name:%s.%s"), *primComp->GetOwner()->GetName(), *primComp->GetName());
			primComp->ComponentTags.Add(*newTag);

			return newTag;
		}
		else
		{
			return tag;
		}
	}

	return TEXT("");
}

void ADraggableActor::CopyMeshTag(class UPrimitiveComponent* primCompFrom, class UPrimitiveComponent* primCompTo)
{
	FString tagFrom = CreateMeshTag(primCompFrom);
	primCompTo->ComponentTags.Add(FName(*tagFrom));
}