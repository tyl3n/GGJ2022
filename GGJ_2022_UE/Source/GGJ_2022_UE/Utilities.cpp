
#include "Utilities.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameMapsSettings.h"

#include "GGJGameMode.h"
#include "GGJWorldSettings.h"
#include "GGJCharacter.h"
#include "GGJGameState.h"
#include "GGJPlayerController.h"
#include "GGJPlayerState.h"
#include "GGJWorldSettings.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

bool Utils::IsInEditor(const UObject* worldContext, bool bAllowEditorPreview)
{
#if !WITH_EDITOR
	return false;
#else
	if (IsRunningCommandlet())
	{
		return false; // not editor, but not the game either - not sure what we want in this context
	}

	if (worldContext == nullptr)
	{
		return false;
	}

	UWorld* world = worldContext->GetWorld();

	if (!world)
	{
		return GIsEditor;
	}

	switch (world->WorldType)
	{
	case EWorldType::Editor:
		return true;
	case EWorldType::EditorPreview:
		return bAllowEditorPreview;
	case EWorldType::PIE:
		return false;
	}

	return GIsEditor;
#endif
}

UWorld* Utils::GetGameWorld(UObject* context)
{
	if (context != nullptr)
	{
		return context->GetWorld();
	}

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

UWorld* Utils::GetEditorWorld()
{
	UWorld* theWorld = nullptr;

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* curWorld = Context.World();
		if (curWorld && curWorld->WorldType == EWorldType::Editor)
		{
#if WITH_EDITOR
			if (GIsEditor && GEditor->PlayWorld == curWorld)
			{
				return curWorld;
			}
#endif

			if(theWorld != nullptr)
			{
				return theWorld;
			}

			theWorld = curWorld;
		}
	}

	return theWorld;
}

AGGJGameState* Utils::GetGameState()
{
	UWorld* world = GetGameWorld();
	if (world != nullptr)
	{
		return Cast<AGGJGameState>(world->GetGameState());
	}
	else
	{
		return nullptr;
	}
}

AGGJGameState* Utils::GetEditorGameState()
{
	if (AGGJWorldSettings* worldSettings = GetEditorWorldSettings())
	{
		AGGJGameMode* gameMode = Cast<AGGJGameMode>(UGameplayStatics::GetGameMode(worldSettings));

		if (gameMode == nullptr)
		{
			FSoftClassPath classPath = UGameMapsSettings::GetGlobalDefaultGameMode();
			TSubclassOf<AGameModeBase> gameModeClass = classPath.TryLoadClass<AGameModeBase>();
			gameMode = Cast<AGGJGameMode>(gameModeClass->GetDefaultObject<AGGJGameMode>());
		}

		if(gameMode)
		{
			if (gameMode->GameStateClass != nullptr)
			{
				return gameMode->GameStateClass->GetDefaultObject<AGGJGameState>();
			}
		}
	}
	
	return nullptr;
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

AGGJWorldSettings* Utils::GetEditorWorldSettings()
{
	UWorld* world = GetEditorWorld();
	if (world != nullptr)
	{
		return Cast<AGGJWorldSettings>(world->GetWorldSettings());
	}
	else
	{
		return nullptr;
	}
}

AGGJCharacter* Utils::GetLocalPlayer(UObject* context)
{
	
	if (AGGJPlayerController* playerController = Cast<AGGJPlayerController>(GetLocalPlayerController(context)))
	{
		if (AGGJCharacter * character  = Cast<AGGJCharacter>(playerController->GetPawn()))
		{
			return character;
		}
	}

	return nullptr;
}

AGGJPlayerController* Utils::GetLocalPlayerController(UObject* context)
{
	UWorld* theWorld = GetGameWorld(context);
	if (theWorld == nullptr) // happens e.g. for cases downstack of OnUnregister() called from blueprint compile
	{
		return nullptr;
	}

	for (FConstPlayerControllerIterator Iterator = theWorld->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if(AGGJPlayerController* ggjPC = Cast<AGGJPlayerController>(Iterator->Get()))
		{
			if (ggjPC->IsLocalController())
			{
				return ggjPC;
			}
		}
	}

	return nullptr;
}

AGGJPlayerState* Utils::GetLocalPlayerState(UObject* context)
{
	if(AGGJPlayerController* controller = GetLocalPlayerController(context))
	{
		return Cast<AGGJPlayerState>(controller->PlayerState);
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