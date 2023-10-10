// Copyright 2023, Robert Lewicki, All rights reserved.

#include "MeleeTraceDebug.h"

#ifdef ENABLE_DRAW_DEBUG

#include "DrawDebugHelpers.h"

static const float KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE = 16.f;

void MeleeTrace::DrawDebugSphereTraceMulti(const UWorld* World,
	const FVector& Start,
	const FVector& End,
	float Radius,
	EDrawDebugTrace::Type DrawDebugType,
	bool bHit,
	const TArray<FHitResult>& OutHits,
	FLinearColor TraceColor,
	FLinearColor TraceHitColor,
	float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().Location;
			DrawDebugSweptSphere(World,
				Start,
				BlockingHitPoint,
				Radius,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
			DrawDebugSweptSphere(World,
				BlockingHitPoint,
				End,
				Radius,
				TraceHitColor.ToFColor(true),
				bPersistent,
				LifeTime);
		}
		else
		{
			// no hit means all red
			DrawDebugSweptSphere(World, Start, End, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World,
				Hit.ImpactPoint,
				KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE,
				(Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)),
				bPersistent,
				LifeTime);
		}
	}
}

void MeleeTrace::DrawDebugSweptSphere(const UWorld* InWorld,
	FVector const& Start,
	FVector const& End,
	float Radius,
	FColor const& Color,
	bool bPersistentLines,
	float LifeTime,
	uint8 DepthPriority /* = 0 */)
{
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;
	float const HalfHeight = (Dist * 0.5f) + Radius;

	FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	::DrawDebugCapsule(InWorld,
		Center,
		HalfHeight,
		Radius,
		CapsuleRot,
		Color,
		bPersistentLines,
		LifeTime,
		DepthPriority);
}

#endif
