#pragma once

#include "CoreMinimal.h"

FORCEINLINE FVector VecZ(float z) { return FVector(0, 0, z); }
FORCEINLINE FVector Vec2D(const FVector& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector Vec2D(const FVector2D& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector Vec3D(const FVector2D& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector2D ToVec2D(const FVector& v) { return FVector2D(v.X, v.Y); }

struct Utils
{
	static class UWorld* GetGameWorld();
	static class AGameStateBase* GetBaseGameState();
	static class AGGJGameMode* GetGameMode();
	static class AGGJWorldSettings* GetWorldSettings();


	static float GetGameTime(); // local, starts upon game world start
	static float ElapsedTime(float eventTime);
	static bool IsRecentEvent(float eventTime, float maxDelay); // uses GameTime
};

