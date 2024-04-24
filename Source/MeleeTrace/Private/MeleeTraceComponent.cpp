// Copyright 2023, Robert Lewicki, All rights reserved.

#include "MeleeTraceComponent.h"

#include "Components/MeshComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"

#include "MeleeTraceCommon.h"
#include "MeleeTraceSettings.h"
#include "MeleeTraceShape.h"

#ifdef ENABLE_DRAW_DEBUG
#include "MeleeTraceDebug.h"

static TAutoConsoleVariable<bool> CVarMeleeTraceShouldDrawDebug(TEXT("MeleeTrace.ShouldDrawDebug"),
	false,
	TEXT("When set to true or 1 will draw debug drawings of melee traces. Set to false or 0 to disable."));
static TAutoConsoleVariable<float> CVarMeleeTraceDrawDebugDuration(TEXT("MeleeTrace.DrawDebugDuration"),
	1.0f,
	TEXT("Defines how long debug drawings will be visible in the viewport."));
#endif

UMeleeTraceComponent::UMeleeTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceChannel = GetDefault<UMeleeTraceSettings>()->MeleeTraceCollisionChannel;
}

void UMeleeTraceComponent::TickComponent(float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TRACE_CPUPROFILER_EVENT_SCOPE(UMeleeTraceComponent::TickComponent);

#ifdef ENABLE_DRAW_DEBUG
	const bool bShouldDrawDebug = CVarMeleeTraceShouldDrawDebug.GetValueOnGameThread();
	const float DrawDebugDuration = CVarMeleeTraceDrawDebugDuration.GetValueOnGameThread();
#endif
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	TArray<FHitResult> HitResults;
	for (FActiveMeleeTraceInfo& ActiveMeleeTrace : ActiveMeleeTraces)
	{
		TArray<FVector> NewSamples;
		GetTraceSamples(ActiveMeleeTrace.SourceMeshComponent.Get(),
			ActiveMeleeTrace.TraceDensity,
			ActiveMeleeTrace.StartSocketName,
			ActiveMeleeTrace.EndSocketName,
			NewSamples);
		const FQuat SamplesRotation = ActiveMeleeTrace.SourceMeshComponent->GetSocketRotation(
			ActiveMeleeTrace.StartSocketName).Quaternion();
		const FQuat OffsetRotation = ActiveMeleeTrace.RotationOffset * SamplesRotation;
		for (int32 Index = 0; Index < NewSamples.Num(); Index++)
		{
			HitResults.Reset();
			FVector PreviousSampleLocation = ActiveMeleeTrace.PreviousFrameSampleLocations[Index];
			if (PreviousSampleLocation.Equals(NewSamples[Index]))
			{
				PreviousSampleLocation += FVector::UpVector * UE_KINDA_SMALL_NUMBER;
			}
			const bool bHit = GetWorld()->SweepMultiByChannel(HitResults,
				PreviousSampleLocation,
				NewSamples[Index],
				OffsetRotation,
				TraceChannel,
				ActiveMeleeTrace.TraceCollisionShape,
				CollisionQueryParams);
#ifdef ENABLE_DRAW_DEBUG
			if (bShouldDrawDebug)
			{
				MeleeTrace::DrawDebugTrace(this,
					ActiveMeleeTrace.TraceCollisionShape,
					FTransform(OffsetRotation, ActiveMeleeTrace.PreviousFrameSampleLocations[Index]),
					FTransform(OffsetRotation, NewSamples[Index]),
					EDrawDebugTrace::Type::ForDuration,
					bHit,
					HitResults,
					DrawDebugDuration);
			}
#endif
			if (!bHit)
			{
				continue;
			}

			for (const FHitResult& HitResult : HitResults)
			{
				if (!ActiveMeleeTrace.HitActors.Contains(HitResult.GetActor()))
				{
					ActiveMeleeTrace.HitActors.Add(HitResult.GetActor());
					OnTraceHit.Broadcast(this,
						HitResult.GetActor(),
						HitResult.ImpactPoint,
						HitResult.ImpactNormal,
						HitResult.BoneName);
				}
			}
		}

		ActiveMeleeTrace.PreviousFrameSampleLocations = MoveTemp(NewSamples);
	}
}

void UMeleeTraceComponent::StartTraceWithContext(const FMeleeTraceInfo& MeleeTraceInfo, const UObject* Context)
{
	if (!ensureMsgf(Context, TEXT("Invalid Context object passed to %s"), StringCast<TCHAR>(__FUNCTION__).Get()))
	{
		return;
	}

	const uint32 ContextHash = GetContextHash(Context);
	const uint32 TraceHash = MeleeTrace::CalculateNewTraceHashWithContext(ContextHash);
	InternalStartTrace(MeleeTraceInfo, TraceHash);
}

void UMeleeTraceComponent::EndTraceWithContext(const UObject* Context)
{
	if (!ensureMsgf(Context, TEXT("Invalid Context object passed to %s"), StringCast<TCHAR>(__FUNCTION__).Get()))
	{
		return;
	}

	const uint32 ContextHash = GetContextHash(Context);
	const uint32 TraceHash = MeleeTrace::GetTraceHash(ContextHash);
	InternalEndTrace(TraceHash);
	MeleeTrace::ReleaseTraceHash(ContextHash);
}

FMeleeTraceInstanceHandle UMeleeTraceComponent::StartTrace(const FMeleeTraceInfo& MeleeTraceInfo)
{
	FMeleeTraceInstanceHandle InstanceHandle;
	InstanceHandle.TraceHash = MeleeTrace::CalculateNewTraceHash();
	InternalStartTrace(MeleeTraceInfo, InstanceHandle.TraceHash);
	return InstanceHandle;
}

void UMeleeTraceComponent::EndTrace(FMeleeTraceInstanceHandle MeleeTraceInstanceHandle)
{
	InternalEndTrace(MeleeTraceInstanceHandle.TraceHash);
}

void UMeleeTraceComponent::ForceEndAllTraces()
{
	for (const FActiveMeleeTraceInfo& ActiveMeleeTrace : ActiveMeleeTraces)
	{
		OnTraceEnd.Broadcast(this, ActiveMeleeTrace.HitActors.Num());
	}

	ActiveMeleeTraces.Reset();
}

bool UMeleeTraceComponent::IsAnyTraceActive() const
{
	return ActiveMeleeTraces.Num() > 0;
}

void UMeleeTraceComponent::SetTraceChannel(ECollisionChannel NewTraceChannel)
{
	TraceChannel = NewTraceChannel;
}

ECollisionChannel UMeleeTraceComponent::GetTraceChannel() const
{
	return TraceChannel;
}

TArray<AActor*> UMeleeTraceComponent::GetActorsHitByTraceWithContext(const UObject* Context) const
{
	const uint32 ContextHash = GetContextHash(Context);
	const uint32 TraceHash = MeleeTrace::GetTraceHash(ContextHash);
	TArray<AActor*> HitActors = InternalGetActorsHitByTrace(TraceHash); 
	return MoveTemp(HitActors);
}

TArray<AActor*> UMeleeTraceComponent::GetActorsHitByTrace(const FMeleeTraceInstanceHandle& Handle) const
{
	TArray<AActor*> HitActors = InternalGetActorsHitByTrace(Handle.TraceHash); 
	return MoveTemp(HitActors);
}

void UMeleeTraceComponent::InvalidateMeleeTraceHandle(FMeleeTraceInstanceHandle& Handle)
{
	Handle.TraceHash = MeleeTrace::INVALID_HASH;
}

bool UMeleeTraceComponent::IsMeleeTraceHandleValid(const FMeleeTraceInstanceHandle& Handle)
{
	return Handle.TraceHash != MeleeTrace::INVALID_HASH;
}

void UMeleeTraceComponent::GetTraceSamples(const UMeshComponent* MeshComponent,
	int32 TraceDensity,
	const FName& StartSocketName,
	const FName& EndSocketName,
	TArray<FVector>& OutSamples)
{
	OutSamples.Reset(TraceDensity + 1);
	const FVector StartSampleLocation = MeshComponent->GetSocketLocation(StartSocketName);
	const FVector EndSampleLocation = MeshComponent->GetSocketLocation(EndSocketName);
	for (int32 Index = 0; Index <= TraceDensity; Index++)
	{
		const float Alpha = static_cast<float>(Index) / static_cast<float>(TraceDensity);
		const FVector Sample = FMath::Lerp(StartSampleLocation, EndSampleLocation, Alpha);
		OutSamples.Add(Sample);
	}
}

void UMeleeTraceComponent::InternalStartTrace(const FMeleeTraceInfo& MeleeTraceInfo, uint32 TraceHash)
{
	AActor* Owner = GetOwner();
	check(Owner);

	TArray<UActorComponent*> MeshComponents;
	TArray<AActor*> ActorsToCheck = { Owner };
	TArray<AActor*> AttachedActors;

	Owner->GetAttachedActors(AttachedActors);
	ActorsToCheck.Append(AttachedActors);

	for (const AActor* Actor : ActorsToCheck)
	{
		TArray<UActorComponent*> ActorMeshComponents;
		Actor->GetComponents(UMeshComponent::StaticClass(), ActorMeshComponents);
		MeshComponents.Append(ActorMeshComponents);
	}

	for (UActorComponent* MeshComponent : MeshComponents)
	{
		UMeshComponent* TypedMeshComponent = Cast<UMeshComponent>(MeshComponent);
		check(TypedMeshComponent);
		if (TypedMeshComponent->DoesSocketExist(MeleeTraceInfo.StartSocketName) 
			&& TypedMeshComponent->DoesSocketExist(MeleeTraceInfo.EndSocketName))
		{
			FActiveMeleeTraceInfo& NewMeleeTraceInfo = ActiveMeleeTraces.AddDefaulted_GetRef();
			NewMeleeTraceInfo.TraceHash = TraceHash;
			NewMeleeTraceInfo.TraceDensity = MeleeTraceInfo.TraceDensity;
			NewMeleeTraceInfo.StartSocketName = MeleeTraceInfo.StartSocketName;
			NewMeleeTraceInfo.EndSocketName = MeleeTraceInfo.EndSocketName;
			if (ensureMsgf(MeleeTraceInfo.TraceShape->IsValidLowLevelFast(),
				TEXT("%s: Invalid trace shape definition"),
				*GetNameSafe(GetOwner())))
			{
				NewMeleeTraceInfo.RotationOffset = MeleeTraceInfo.TraceShape->GetRotationOffset();
				NewMeleeTraceInfo.TraceCollisionShape = MeleeTraceInfo.TraceShape->CreateCollisionShape();
			}
			else
			{
				// This is a fallback in case of incorrect user settings
				NewMeleeTraceInfo.RotationOffset = FQuat::Identity;
				NewMeleeTraceInfo.TraceCollisionShape = FCollisionShape();
			}
			NewMeleeTraceInfo.SourceMeshComponent = TypedMeshComponent;
			GetTraceSamples(TypedMeshComponent,
				MeleeTraceInfo.TraceDensity,
				MeleeTraceInfo.StartSocketName,
				MeleeTraceInfo.EndSocketName,
				NewMeleeTraceInfo.PreviousFrameSampleLocations);
			OnTraceStart.Broadcast(this);
			return;
		}
	}

	ensureAlwaysMsgf(false,
		TEXT("None of the USkeletalMeshComponents contain sockets with names: %s and %s"),
		*MeleeTraceInfo.StartSocketName.ToString(),
		*MeleeTraceInfo.EndSocketName.ToString());
}

void UMeleeTraceComponent::InternalEndTrace(uint32 TraceHash)
{
	if (!ensureMsgf(TraceHash != MeleeTrace::INVALID_HASH, TEXT("Invalid TraceHash used to end trace")))
	{
		return;
	}

	const int32 FoundIndex = ActiveMeleeTraces.IndexOfByPredicate(
		[TraceHash](const FActiveMeleeTraceInfo& ActiveMeleeTraceInfo)
		{
			return TraceHash == ActiveMeleeTraceInfo.TraceHash;
		});

	if (ensureAlwaysMsgf(FoundIndex != INDEX_NONE,
		TEXT("Attemping to end trace with hash: %u but no trace with hash exist"),
		TraceHash))
	{
		OnTraceEnd.Broadcast(this, ActiveMeleeTraces[FoundIndex].HitActors.Num());
		ActiveMeleeTraces.RemoveAtSwap(FoundIndex);
	}
}

TArray<AActor*> UMeleeTraceComponent::InternalGetActorsHitByTrace(uint32 TraceHash) const
{
	const int32 TraceIndex = ActiveMeleeTraces.IndexOfByPredicate([TraceHash](const FActiveMeleeTraceInfo& TraceInfo)
	{
		return TraceInfo.TraceHash == TraceHash;
	});

	if (TraceIndex == INDEX_NONE)
	{
		return {};
	}

	TArray<AActor*> OutHitActors;
	for (TWeakObjectPtr<AActor> HitActor : ActiveMeleeTraces[TraceIndex].HitActors)
	{
		if (HitActor.IsValid())
		{
			OutHitActors.Add(HitActor.Get());
		}
	}

	return MoveTemp(OutHitActors);
}

uint32 UMeleeTraceComponent::GetContextHash(const UObject* Context) const
{
	return MeleeTrace::CombineHashes(Context->GetUniqueID(), GetUniqueID());
}
