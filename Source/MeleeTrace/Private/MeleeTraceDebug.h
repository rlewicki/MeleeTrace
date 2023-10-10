// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#ifdef ENABLE_DRAW_DEBUG

#include "Kismet/KismetSystemLibrary.h"

class UWorld;

namespace MeleeTrace
{
	void DrawDebugSphereTraceMulti(const UWorld* World,
		const FVector& Start,
		const FVector& End,
		float Radius,
		EDrawDebugTrace::Type DrawDebugType,
		bool bHit,
		const TArray<FHitResult>& OutHits,
		FLinearColor TraceColor,
		FLinearColor TraceHitColor,
		float DrawTime);
	void DrawDebugSweptSphere(const UWorld* InWorld,
		FVector const& Start,
		FVector const& End,
		float Radius,
		FColor const& Color,
		bool bPersistentLines,
		float LifeTime,
		uint8 DepthPriority = 0);
}

#endif