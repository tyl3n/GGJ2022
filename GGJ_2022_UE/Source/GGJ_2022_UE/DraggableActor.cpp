
#include "DraggableActor.h"

#include "DrawDebugHelpers.h"

#include "Utilities.h"

ADraggableActor::ADraggableActor()
{
}

void ADraggableActor::OnDraggableSelected_Implementation()
{

}

void ADraggableActor::OnDraggableReleased_Implementation()
{
	bDraging = false;
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

		DrawDebugLine(GetWorld(), location, dragPoint, FColor::Blue);
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