// Copyright 2024, Robert Lewicki, All rights reserved.

#include "Async_WaitForMeleeTraceHit.h"

#include "MeleeTraceCommon.h"
#include "MeleeTraceComponent.h"

UAsync_WaitForMeleeTraceHit* UAsync_WaitForMeleeTraceHit::WaitForMeleeHit(UObject* WorldContextObject, AActor* ActorToWatch)
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

	auto* NewAction = NewObject<UAsync_WaitForMeleeTraceHit>();
	NewAction->ActorToWatch = ActorToWatch;
	NewAction->MeleeTraceComponent = MeleeTraceComponent;
	NewAction->RegisterWithGameInstance(World);
	return NewAction;
}

void UAsync_WaitForMeleeTraceHit::Cancel()
{
	if (MeleeTraceComponent.IsValid())
	{
		MeleeTraceComponent->OnTraceHit.RemoveDynamic(this, &ThisClass::HandleTraceHit);
	}

	ActorToWatch.Reset();
	MeleeTraceComponent.Reset();

	Super::Cancel();
}

void UAsync_WaitForMeleeTraceHit::Activate()
{
	MeleeTraceComponent->OnTraceHit.AddDynamic(this, &ThisClass::HandleTraceHit);
}

void UAsync_WaitForMeleeTraceHit::HandleTraceHit(
	UMeleeTraceComponent* ThisComponent,
	AActor* HitActor,
	const FVector& HitLocation,
	const FVector& HitNormal,
	FName HitBoneName)
{
	FAsyncMeleeHitInfo HitInfo;
	HitInfo.OwnerTraceComponent = ThisComponent;
	HitInfo.HitActor = HitActor;
	HitInfo.HitLocation = HitLocation;
	HitInfo.HitNormal = HitNormal;
	HitInfo.HitBoneName = HitBoneName;
	OnHit.Broadcast(HitInfo);
}
