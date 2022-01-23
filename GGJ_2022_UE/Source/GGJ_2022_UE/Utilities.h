#pragma once

#include "CoreMinimal.h"

#if UE_BUILD_SHIPPING
#define TWEAKABLE const
#else
#define TWEAKABLE static
#endif

FORCEINLINE FVector VecZ(float z) { return FVector(0, 0, z); }
FORCEINLINE FVector Vec2D(const FVector& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector Vec2D(const FVector2D& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector Vec3D(const FVector2D& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector2D ToVec2D(const FVector& v) { return FVector2D(v.X, v.Y); }

template< class T, class U > FORCEINLINE U MapClamped(T x, T minX, T maxX, U outEdge0, U outEdge1)
{
	return FMath::Clamp((U)(outEdge0 + (outEdge1 - outEdge0) * (x - minX) / (maxX - minX)), FMath::Min(outEdge0, outEdge1), FMath::Max(outEdge0, outEdge1));
}

struct Utils
{
	static bool IsInEditor(const UObject* worldContext, bool bAllowEditorPreview = true);

	static class UWorld* GetGameWorld(UObject* context = nullptr);
	static class UWorld* GetEditorWorld();
	static class AGGJGameState* GetGameState();
	static class AGGJGameState* GetEditorGameState();
	static class AGGJGameMode* GetGameMode();
	static class AGGJWorldSettings* GetWorldSettings();
	static class AGGJWorldSettings* GetEditorWorldSettings();
	static class AGGJCharacter* GetLocalPlayer(UObject* context);
	static class AGGJPlayerController* GetLocalPlayerController(UObject* context);
	static class AGGJPlayerState* GetLocalPlayerState(UObject* context);



	static float GetGameTime(); // local, starts upon game world start
	static float ElapsedTime(float eventTime);
	static bool IsRecentEvent(float eventTime, float maxDelay); // uses GameTime

	FORCEINLINE static float NormalizeRotAngle(float angle)
	{
		angle = FMath::Fmod(angle, 360.0f);

		if (angle <= -180.0f)
		{
			angle += 360.0f;
		}
		else if (angle >= 180.0f)
		{
			angle -= 360.0f;
		}
		checkSlow(angle >= -180.0f && angle <= 180.0f);
		return angle;
	}
};

#define ECC_Draggable					ECollisionChannel::ECC_GameTraceChannel1
//#define ECC_Trigger					ECollisionChannel::ECC_GameTraceChannel2
//#define ECC_Pushable					ECollisionChannel::ECC_GameTraceChannel3
//#define ECC_Audio						ECollisionChannel::ECC_GameTraceChannel4
//#define ECC_PlayerBlocking			ECollisionChannel::ECC_GameTraceChannel5
//#define ECC_NPCBlocking				ECollisionChannel::ECC_GameTraceChannel6
//#define ECC_PawnDetection  			ECollisionChannel::ECC_GameTraceChannel7
//#define ECC_WorldObjectTrace			ECollisionChannel::ECC_GameTraceChannel8
//#define ECC_EssentialItemBlocking		ECollisionChannel::ECC_GameTraceChannel9
//#define ECC_RoomDetection				ECollisionChannel::ECC_GameTraceChannel10
//#define ECC_FootStep					ECollisionChannel::ECC_GameTraceChannel11
//#define ECC_PlayerFalling				ECollisionChannel::ECC_GameTraceChannel12
//#define ECC_PhysicalParticles			ECollisionChannel::ECC_GameTraceChannel13

