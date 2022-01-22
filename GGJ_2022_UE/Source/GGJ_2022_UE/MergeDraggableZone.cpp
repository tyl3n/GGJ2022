
#include "MergeDraggableZone.h"

#include "Components/BoxComponent.h"

#include "DraggableActor.h"

AMergeDraggableZone::AMergeDraggableZone()
{
}

void AMergeDraggableZone::MergeTouchingDraggableActors()
{
	TArray<ADraggableActor*> touchingDraggables;

	for (UActorComponent* component : GetComponents())
	{
		if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(component))
		{
			if (primComp->ComponentTags.Contains(TEXT("MergeZone")))
			{
				TSet<AActor*> overlappingActors;
				primComp->GetOverlappingActors(overlappingActors);

				for (AActor* overlapActor : overlappingActors)
				{
					if (ADraggableActor* draggableActor = Cast<ADraggableActor>(overlapActor))
					{
						touchingDraggables.AddUnique(draggableActor);
					}
				}
			}
		}
	}

	touchingDraggables.Sort([](const ADraggableActor& A, const ADraggableActor& B)
		{
			return A.ReleasedTimestamp < B.ReleasedTimestamp;
		});

	if (touchingDraggables.Num() > 1)
	{
		TArray< TPair<UBoxComponent*, UStaticMeshComponent*> > snapPairs;

		for (int i = 0; i < touchingDraggables.Num() - 1; ++i)
		{
			TPair<UBoxComponent*, UStaticMeshComponent*>& pair = snapPairs.AddZeroed_GetRef();
			ADraggableActor::ComputeSnapPair(touchingDraggables[i], touchingDraggables[i + 1], pair.Key, pair.Value);
			if (pair.Key == nullptr)
			{
				snapPairs.RemoveAt(snapPairs.Num() - 1);
			}
		}

		for (TPair<UBoxComponent*, UStaticMeshComponent*>& snapPair : snapPairs)
		{
			if(snapPair.Value != nullptr)
			{
				ADraggableActor::SnapDraggable(snapPair.Key, snapPair.Value);
				ADraggableActor::MergeDraggable(Cast<ADraggableActor>(snapPair.Key->GetOwner()), Cast<ADraggableActor>(snapPair.Value->GetOwner()));
			}
		}
	}
}