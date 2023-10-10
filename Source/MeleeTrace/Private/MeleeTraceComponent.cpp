// Copyright 2023, Robert Lewicki, All rights reserved.

#include "MeleeTraceComponent.h"

#include "Engine/World.h"

#include "MeleeTraceSettings.h"

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
	const ECollisionChannel TraceCollisionChannel = GetDefault<UMeleeTraceSettings>()->MeleeTraceCollisionChannel;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	TArray<FHitResult> HitResults;
	for (FActiveMeleeTraceInfo& ActiveMeleeTrace : ActiveMeleeTraces)
	{
		TArray<FVector> NewSamples;
		GetTraceSamples(ActiveMeleeTrace.SourceMeshComponent.Get(), ActiveMeleeTrace.MeleeTraceInfo, NewSamples);
		for (int32 Index = 0; Index < NewSamples.Num(); Index++)
		{
			HitResults.Reset();
			const bool bHit = GetWorld()->SweepMultiByChannel(HitResults,
				ActiveMeleeTrace.PreviousFrameSampleLocations[Index],
				NewSamples[Index],
				FQuat::Identity,
				TraceCollisionChannel,
				FCollisionShape::MakeSphere(ActiveMeleeTrace.MeleeTraceInfo.Radius),
				CollisionQueryParams);
#ifdef ENABLE_DRAW_DEBUG
			if (bShouldDrawDebug)
			{
				MeleeTrace::DrawDebugSphereTraceMulti(GetWorld(),
					ActiveMeleeTrace.PreviousFrameSampleLocations[Index],
					NewSamples[Index],
					ActiveMeleeTrace.MeleeTraceInfo.Radius,
					EDrawDebugTrace::ForDuration,
					bHit,
					HitResults,
					FLinearColor::Green,
					FLinearColor::Red,
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
						HitResult.ImpactNormal);
				}
			}
		}

		ActiveMeleeTrace.PreviousFrameSampleLocations = MoveTemp(NewSamples);
	}
}

void UMeleeTraceComponent::StartTrace(const FMeleeTraceInfo& MeleeTraceInfo, uint32 TraceHash)
{
	TArray<UActorComponent*> MeshComponents;
	GetOwner()->GetComponents(USkeletalMeshComponent::StaticClass(), MeshComponents);
	for (UActorComponent* MeshComponent : MeshComponents)
	{
		USkeletalMeshComponent* TypedMeshComponent = Cast<USkeletalMeshComponent>(MeshComponent);
		check(TypedMeshComponent);
		if (TypedMeshComponent->DoesSocketExist(MeleeTraceInfo.StartSocketName)
			&& TypedMeshComponent->DoesSocketExist(MeleeTraceInfo.EndSocketName))
		{
			FActiveMeleeTraceInfo& NewMeleeTraceInfo = ActiveMeleeTraces.AddDefaulted_GetRef();
			NewMeleeTraceInfo.MeleeTraceInfo = MeleeTraceInfo;
			NewMeleeTraceInfo.TraceHash = TraceHash;
			NewMeleeTraceInfo.SourceMeshComponent = TypedMeshComponent;
			GetTraceSamples(TypedMeshComponent, MeleeTraceInfo, NewMeleeTraceInfo.PreviousFrameSampleLocations);
			OnTraceStart.Broadcast(this);
			return;
		}
	}

	ensureAlwaysMsgf(false,
		TEXT("None of the USkeletalMeshComponents contain sockets with names: %s and %s"),
		*MeleeTraceInfo.StartSocketName.ToString(),
		*MeleeTraceInfo.EndSocketName.ToString());
}

void UMeleeTraceComponent::EndTrace(uint32 TraceHash)
{
	const int32 FoundIndex = ActiveMeleeTraces.IndexOfByPredicate(
		[TraceHash](const FActiveMeleeTraceInfo& ActiveMeleeTraceInfo)
	{
		return TraceHash == ActiveMeleeTraceInfo.TraceHash; 
	});

	if (ensureAlwaysMsgf(FoundIndex != INDEX_NONE,
		TEXT("Attemping to end trace with FGuid: %u but no trace with such FGuid exist"),
		TraceHash))
	{
		OnTraceEnd.Broadcast(this, ActiveMeleeTraces[FoundIndex].HitActors.Num());
		ActiveMeleeTraces.RemoveAtSwap(FoundIndex);
	}
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

void UMeleeTraceComponent::GetTraceSamples(const USkeletalMeshComponent* MeshComponent,
	const FMeleeTraceInfo& MeleeTraceInfo,
	TArray<FVector>& OutSamples)
{
	OutSamples.Reset(MeleeTraceInfo.TraceDensity + 1);
	const FVector StartSampleLocation = MeshComponent->GetSocketLocation(MeleeTraceInfo.StartSocketName);
	const FVector EndSampleLocation = MeshComponent->GetSocketLocation(MeleeTraceInfo.EndSocketName);
	for (int32 Index = 0; Index <= MeleeTraceInfo.TraceDensity; Index++)
	{
		const float Alpha = static_cast<float>(Index) / static_cast<float>(MeleeTraceInfo.TraceDensity);
		const FVector Sample = FMath::Lerp(StartSampleLocation, EndSampleLocation, Alpha);
		OutSamples.Add(Sample);
	}
}
