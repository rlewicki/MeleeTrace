// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#ifdef ENABLE_DRAW_DEBUG

#include "Kismet/KismetSystemLibrary.h"

class UWorld;

namespace MeleeTrace
{
	void DrawDebugTrace(const UObject* WorldContextObject,
		const FCollisionShape& CollisionShape,
		const FTransform& StartTransform,
		const FTransform& EndTransform,
		EDrawDebugTrace::Type DrawDebugType,
		bool bHit,
		const TArray<FHitResult>& HitResults,
		float DrawTime);
	void DrawDebugSphereTraceMulti(const UWorld* World,
		const FVector& Start,
		const FVector& End,
		float Radius,
		EDrawDebugTrace::Type DrawDebugType,
		bool bHit,
		const TArray<FHitResult>& Hits,
		const FLinearColor& TraceColor,
		const FLinearColor& TraceHitColor,
		float DrawTime);
	void DrawDebugSweptSphere(const UWorld* InWorld,
		FVector const& Start,
		FVector const& End,
		float Radius,
		FColor const& Color,
		bool bPersistentLines,
		float LifeTime,
		uint8 DepthPriority = 0);
	void DrawDebugLineTraceMulti(const UWorld* World,
		const FVector& Start,
		const FVector& End,
		EDrawDebugTrace::Type DrawDebugType,
		bool bHit,
		const TArray<FHitResult>& HitResults,
		const FLinearColor& TraceColor,
		const FLinearColor& TraceHitColor,
		float DrawTime);
	void DrawDebugCapsuleTraceMulti(const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const FQuat& Orientation,
		float Radius,
		float HalfHeight,
		EDrawDebugTrace::Type DrawDebugType,
		bool bHit,
		const TArray<FHitResult>& HitResults,
		const FLinearColor& TraceColor,
		const FLinearColor& TraceHitColor,
		float DrawTime);
	void DrawDebugBoxTraceMulti(const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const FVector& HalfSize,
		const FRotator& Orientation,
		EDrawDebugTrace::Type DrawDebugType,
		bool bHit,
		const TArray<FHitResult>& HitResults,
		const FLinearColor& TraceColor,
		const FLinearColor& TraceHitColor,
		float DrawTime);
}

#endif