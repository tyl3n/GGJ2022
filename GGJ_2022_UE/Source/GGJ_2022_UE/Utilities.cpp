
#include "Utilities.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#include "GGJGameMode.h"
#include "GGJWorldSettings.h"
#include "GGJCharacter.h"

UWorld* Utils::GetGameWorld()
{
	UWorld* theWorld = nullptr;

	if (GEngine != nullptr)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			UWorld* curWorld = Context.World();
			if (curWorld && curWorld->IsGameWorld()) // not sure if this is enough?
			{
				if (theWorld != nullptr)
				{
					return theWorld;
				}

				theWorld = curWorld;
			}
		}
	}
	return theWorld;
}

AGameStateBase* Utils::GetBaseGameState()
{
	UWorld* world = GetGameWorld();
	if (world != nullptr)
	{
		return world->GetGameState();
	}
	else
	{
		return nullptr;
	}
}

AGGJGameMode* Utils::GetGameMode()
{
	UWorld* world = GetGameWorld();
	if (world != nullptr)
	{
		return Cast<AGGJGameMode>(world->GetAuthGameMode());
	}
	else
	{
		return nullptr;
	}
}

AGGJWorldSettings* Utils::GetWorldSettings()
{
	UWorld* world = GetGameWorld();
	if (world == nullptr)
	{
		return nullptr;
	}

	return Cast<AGGJWorldSettings>(world->GetWorldSettings());
}

AGGJCharacter* Utils::GetLocalPlayer()
{
	for (TActorIterator<AGGJCharacter> it(GetGameWorld()); it; ++it)
	{
		if(AGGJCharacter* character = *it)
		{
			if (character->IsLocallyControlled())
			{
				return character;
			}
		}
	}

	return nullptr;
}


float Utils::GetGameTime()
{
	UWorld* world = Utils::GetGameWorld();

	if (world)
	{
		return world->GetTimeSeconds();
	}

	return 0.0f;

}

float Utils::ElapsedTime(float eventTime)
{
	if (eventTime <= 0.0f)
	{
		return BIG_NUMBER;
	}

	return GetGameWorld()->GetTimeSeconds() - eventTime;
}

bool Utils::IsRecentEvent(float eventTime, float maxDelay)
{
	if (eventTime <= 0.0f)
	{
		return false;
	}

	float elapsedTime = ElapsedTime(eventTime);
	return elapsedTime <= maxDelay;
}