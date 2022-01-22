
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

void ADraggableActor::Drag(FVector& dragPoint)
{
	if (UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		FVector location = staticMesh->GetComponentLocation();
		FVector dragVector = Vec2D(dragPoint - location);

		FVector dragDir;
		float dragLength;
		dragVector.ToDirectionAndLength(dragDir, dragLength);

		float forceStrength = DragForce.GetRichCurveConst()->Eval(dragLength) * DragForceBaseMultiplier;
		FVector force = dragDir * forceStrength;

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