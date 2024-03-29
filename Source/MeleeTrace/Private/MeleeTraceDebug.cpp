// Copyright 2023, Robert Lewicki, All rights reserved.

#include "MeleeTraceDebug.h"

#include "MeleeTraceSettings.h"

#ifdef ENABLE_DRAW_DEBUG

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

static const float KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE = 16.f;

void MeleeTrace::DrawDebugTrace(const UObject* WorldContextObject,
	const FCollisionShape& CollisionShape,
	const FTransform& StartTransform,
	const FTransform& EndTransform,
	EDrawDebugTrace::Type DrawDebugType,
	bool bHit,
	const TArray<FHitResult>& HitResults,
	float DrawTime)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	const UMeleeTraceSettings* Settings = GetDefault<UMeleeTraceSettings>();
	check(Settings);
	const FLinearColor TraceColor = Settings->TraceColor;
	const FLinearColor TraceHitColor = Settings->TraceHitColor;

	if (CollisionShape.IsLine())
	{
		DrawDebugLineTraceMulti(World,
			StartTransform.GetLocation(),
			EndTransform.GetLocation(),
			DrawDebugType,
			bHit,
			HitResults,
			TraceColor,
			TraceHitColor,
			DrawTime);
	}
	else if (CollisionShape.IsSphere())
	{
		DrawDebugSphereTraceMulti(World,
			StartTransform.GetLocation(),
			EndTransform.GetLocation(),
			CollisionShape.GetSphereRadius(),
			DrawDebugType,
			bHit,
			HitResults,
			TraceColor,
			TraceHitColor,
			DrawTime);
	}
	else if (CollisionShape.IsCapsule())
	{
		DrawDebugCapsuleTraceMulti(World,
			StartTransform.GetLocation(),
			EndTransform.GetLocation(),
			StartTransform.GetRotation(),
			CollisionShape.GetCapsuleRadius(),
			CollisionShape.GetCapsuleHalfHeight(),
			DrawDebugType,
			bHit,
			HitResults,
			TraceColor,
			TraceHitColor,
			DrawTime);
	}
	else if (CollisionShape.IsBox())
	{
		DrawDebugBoxTraceMulti(World,
			StartTransform.GetLocation(),
			EndTransform.GetLocation(),
			CollisionShape.GetExtent(),
			EndTransform.GetRotation().Rotator(),
			DrawDebugType,
			bHit,
			HitResults,
			TraceColor,
			TraceHitColor,
			DrawTime);
	}
}

void MeleeTrace::DrawDebugSphereTraceMulti(const UWorld* World,
	const FVector& Start,
	const FVector& End,
	float Radius,
	EDrawDebugTrace::Type DrawDebugType,
	bool bHit,
	const TArray<FHitResult>& HitResults,
	const FLinearColor& TraceColor,
	const FLinearColor& TraceHitColor,
	float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && HitResults.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = HitResults.Last().Location;
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
		for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
		{
			FHitResult const& Hit = HitResults[HitIdx];
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

void MeleeTrace::DrawDebugLineTraceMulti(const UWorld* World,
	const FVector& Start,
	const FVector& End,
	EDrawDebugTrace::Type DrawDebugType,
	bool bHit,
	const TArray<FHitResult>& HitResults,
	const FLinearColor& TraceColor,
	const FLinearColor& TraceHitColor,
	float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && HitResults.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = HitResults.Last().ImpactPoint;
			::DrawDebugLine(World, Start, BlockingHitPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
		{
			FHitResult const& Hit = HitResults[HitIdx];
			::DrawDebugPoint(World,
				Hit.ImpactPoint,
				KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE,
				(Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)),
				bPersistent,
				LifeTime);
		}
	}
}

void MeleeTrace::DrawDebugCapsuleTraceMulti(const UWorld* World,
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
	float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && HitResults.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = HitResults.Last().Location;
			::DrawDebugCapsule(World,
				Start,
				HalfHeight,
				Radius,
				Orientation,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
			::DrawDebugCapsule(World,
				BlockingHitPoint,
				HalfHeight,
				Radius,
				Orientation,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
			::DrawDebugLine(World, Start, BlockingHitPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);

			::DrawDebugCapsule(World,
				End,
				HalfHeight,
				Radius,
				Orientation,
				TraceHitColor.ToFColor(true),
				bPersistent,
				LifeTime);
			::DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugCapsule(World,
				Start,
				HalfHeight,
				Radius,
				Orientation,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
			::DrawDebugCapsule(World,
				End,
				HalfHeight,
				Radius,
				Orientation,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
		{
			FHitResult const& Hit = HitResults[HitIdx];
			::DrawDebugPoint(World,
				Hit.ImpactPoint,
				KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE,
				(Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)),
				bPersistent,
				LifeTime);
		}
	}
}

namespace MeleeTrace
{
	void DrawDebugSweptBox(const UWorld* InWorld,
		FVector const& Start,
		FVector const& End,
		FRotator const& Orientation,
		FVector const& HalfSize,
		FColor const& Color,
		bool bPersistentLines,
		float LifeTime,
		uint8 DepthPriority = 0)
	{
		FVector const TraceVec = End - Start;
		FQuat const CapsuleRot = Orientation.Quaternion();
		::DrawDebugBox(InWorld, Start, HalfSize, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);

		//now draw lines from vertices
		FVector Vertices[8];
		Vertices[0] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, -HalfSize.Y, -HalfSize.Z)); //flt
		Vertices[1] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, HalfSize.Y, -HalfSize.Z));  //frt
		Vertices[2] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, -HalfSize.Y, HalfSize.Z));  //flb
		Vertices[3] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, HalfSize.Y, HalfSize.Z));   //frb
		Vertices[4] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, -HalfSize.Y, -HalfSize.Z));  //blt
		Vertices[5] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, HalfSize.Y, -HalfSize.Z));   //brt
		Vertices[6] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, -HalfSize.Y, HalfSize.Z));   //blb
		Vertices[7] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, HalfSize.Y, HalfSize.Z));    //brb
		for (int32 VertexIdx = 0; VertexIdx < 8; ++VertexIdx)
		{
			::DrawDebugLine(InWorld,
				Vertices[VertexIdx],
				Vertices[VertexIdx] + TraceVec,
				Color,
				bPersistentLines,
				LifeTime,
				DepthPriority);
		}

		::DrawDebugBox(InWorld, End, HalfSize, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);
	}
}

void MeleeTrace::DrawDebugBoxTraceMulti(const UWorld* World,
	const FVector& Start,
	const FVector& End,
	const FVector& HalfSize,
	const FRotator& Orientation,
	EDrawDebugTrace::Type DrawDebugType,
	bool bHit,
	const TArray<FHitResult>& HitResults,
	const FLinearColor& TraceColor,
	const FLinearColor& TraceHitColor,
	float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None && (World != nullptr))
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && HitResults.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = HitResults.Last().Location;
			DrawDebugSweptBox(World,
				Start,
				BlockingHitPoint,
				Orientation,
				HalfSize,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
			DrawDebugSweptBox(World,
				BlockingHitPoint,
				End,
				Orientation,
				HalfSize,
				TraceHitColor.ToFColor(true),
				bPersistent,
				LifeTime);
		}
		else
		{
			// no hit means all red
			DrawDebugSweptBox(World,
				Start,
				End,
				Orientation,
				HalfSize,
				TraceColor.ToFColor(true),
				bPersistent,
				LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
		{
			FHitResult const& Hit = HitResults[HitIdx];
			::DrawDebugPoint(World,
				Hit.ImpactPoint,
				KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE,
				(Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)),
				bPersistent,
				LifeTime);
		}
	}
}

#endif
