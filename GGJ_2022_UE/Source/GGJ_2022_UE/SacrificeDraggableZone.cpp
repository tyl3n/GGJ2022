
#include "SacrificeDraggableZone.h"

#include "DraggableActor.h"
#include "GGJCharacter.h"
#include "Utilities.h"

ASacrificeDraggableZone::ASacrificeDraggableZone()
{
}

void ASacrificeDraggableZone::BeginPlay()
{
	Super::BeginPlay();

	for (UActorComponent* actorComponent : GetComponents())
	{
		if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(actorComponent))
		{
			if (actorComponent->ComponentHasTag(TEXT("SafeZone")))
			{
				SafeZones.Add(primComp);
				primComp->OnComponentEndOverlap.AddDynamic(this, &ASacrificeDraggableZone::OnOverlapEnd);
			}
			else if (actorComponent->ComponentHasTag(TEXT("SacrificeZone")))
			{
				SacrificeZones.Add(primComp);
			}
		}
	}
}

void ASacrificeDraggableZone::OnOverlapEnd(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (ADraggableActor* draggableActor = Cast<ADraggableActor>(otherActor))
	{
		for (UPrimitiveComponent* primComp : SafeZones)
		{
			TArray<AActor*> overlappingActors;
			primComp->GetOverlappingActors(overlappingActors);
			if (overlappingActors.Contains(draggableActor))
			{
				// Still overlapping a safe zone
				return;
			}
		}

		// We're not overlaping any safe zone
		for (UPrimitiveComponent* primComp : SacrificeZones)
		{
			TArray<AActor*> overlappingActors;
			primComp->GetOverlappingActors(overlappingActors);
			if (overlappingActors.Contains(draggableActor))
			{
				// OH! SACRIFICE!

				if (AGGJCharacter* player = Utils::GetLocalPlayer(this))
				{
					player->SacrificePiece_Client(draggableActor);
				}

				return;
			}
		}
	}
	
}
