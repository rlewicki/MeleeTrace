// Copyright 2024, Robert Lewicki, All rights reserved.

#include "Async_WaitForMeleeTraceEvent.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

#include "MeleeTraceCommon.h"
#include "MeleeTraceComponent.h"

UAsync_WaitForMeleeTraceEvent* UAsync_WaitForMeleeTraceEvent::WaitForMeleeTraceEventHit(
	UObject* WorldContextObject, AActor* ActorToWatch, bool OncePerMultiTrace)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	if (!ActorToWatch)
	{
		UE_LOG(
			LogMeleeTrace,
			Error,
			TEXT("%s: Invalid reference for actor to watch passed as parameter"), StringCast<TCHAR>(__FUNCTION__).Get());
		return nullptr;
	}

	auto* MeleeTraceComponent = ActorToWatch->FindComponentByClass<UMeleeTraceComponent>();
	if (!MeleeTraceComponent)
	{
		UE_LOG(
			LogMeleeTrace,
			Error,
			TEXT("%s: Actor to watch doesn't have MeleeTraceComponent attached"), StringCast<TCHAR>(__FUNCTION__).Get());
		return nullptr;
	}

	auto* NewAction = NewObject<UAsync_WaitForMeleeTraceEvent>();
	NewAction->OncePerMultiTrace = OncePerMultiTrace;
	NewAction->ActorToWatch = ActorToWatch;
	NewAction->MeleeTraceComponent = MeleeTraceComponent;
	NewAction->RegisterWithGameInstance(World);
	return NewAction;
}

void UAsync_WaitForMeleeTraceEvent::Cancel()
{
	if (MeleeTraceComponent.IsValid())
	{
		MeleeTraceComponent->OnTraceHit.RemoveDynamic(this, &ThisClass::HandleTraceHit);
		MeleeTraceComponent->OnTraceStart.RemoveDynamic(this, &ThisClass::HandleTraceStarted);
		MeleeTraceComponent->OnTraceEnd.RemoveDynamic(this, &ThisClass::HandleTraceEnded);
	}

	ActorToWatch.Reset();
	MeleeTraceComponent.Reset();

	Super::Cancel();
}

void UAsync_WaitForMeleeTraceEvent::Activate()
{
	MeleeTraceComponent->OnTraceHit.AddDynamic(this, &ThisClass::HandleTraceHit);
	MeleeTraceComponent->OnTraceStart.AddDynamic(this, &ThisClass::HandleTraceStarted);
	MeleeTraceComponent->OnTraceEnd.AddDynamic(this, &ThisClass::HandleTraceEnded);
}

void UAsync_WaitForMeleeTraceEvent::HandleTraceHit(
	UMeleeTraceComponent* ThisComponent,
	AActor* HitActor,
	const FVector& HitLocation,
	const FVector& HitNormal,
	FName HitBoneName,
	FMeleeTraceInstanceHandle TraceHandle)
{
	if (!CanHit)
	{
		return;
	}
	
	FAsyncMeleeHitInfo HitInfo;
	HitInfo.OwnerTraceComponent = ThisComponent;
	HitInfo.HitActor = HitActor;
	HitInfo.HitLocation = HitLocation;
	HitInfo.HitNormal = HitNormal;
	HitInfo.HitBoneName = HitBoneName;
	OnHit.Broadcast(HitInfo, TraceHandle);
	
	if (CanHit && OncePerMultiTrace)
	{
		CanHit = false;
	}
}

void UAsync_WaitForMeleeTraceEvent::HandleTraceStarted(
	UMeleeTraceComponent* ThisComponent, FMeleeTraceInstanceHandle TraceHandle)
{
	CanHit = true;
	OnStarted.Broadcast();
}

void UAsync_WaitForMeleeTraceEvent::HandleTraceEnded(
	UMeleeTraceComponent* ThisComponent, int32 HitCount, FMeleeTraceInstanceHandle TraceHandle)
{
	
	OnEnded.Broadcast();
}
