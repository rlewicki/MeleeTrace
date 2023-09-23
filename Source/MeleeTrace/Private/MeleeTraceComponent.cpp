// Copyright 2023, Robert Lewicki, All rights reserved.

#include "MeleeTraceComponent.h"

#include "MeleeTraceSettings.h"

#if ENABLE_DRAW_DEBUG
#include "Engine/Private/KismetTraceUtils.h"


static TAutoConsoleVariable<bool> CVarMeleeTraceShouldDrawDebug(TEXT("MeleeTrace.ShouldDrawDebug"),
	false,
	TEXT("When set to true or 1 will draw debug drawings of melee traces. Set to false or 0 to disable."));
static TAutoConsoleVariable<float> CVarMeleeTraceDrawDebugDuration(TEXT("MeleeTrace.DrawDebugDuration"),
	5.0f,
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

#if ENABLE_DRAW_DEBUG
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
#if ENABLE_DRAW_DEBUG
			if (bShouldDrawDebug)
			{
				DrawDebugSphereTraceMulti(GetWorld(),
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
						ActiveMeleeTrace.Guid,
						HitResult.GetActor(),
						HitResult.ImpactPoint,
						HitResult.ImpactNormal);
				}
			}
		}

		ActiveMeleeTrace.PreviousFrameSampleLocations = MoveTemp(NewSamples);
	}
}

FGuid UMeleeTraceComponent::StartTrace(const FMeleeTraceInfo& MeleeTraceInfo)
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
			NewMeleeTraceInfo.Guid = FGuid::NewGuid();
			NewMeleeTraceInfo.SourceMeshComponent = TypedMeshComponent;
			GetTraceSamples(TypedMeshComponent, MeleeTraceInfo, NewMeleeTraceInfo.PreviousFrameSampleLocations);
			OnTraceStart.Broadcast(this, NewMeleeTraceInfo.Guid);
			return NewMeleeTraceInfo.Guid;
		}
	}

	ensureAlwaysMsgf(false, TEXT("None of the USkeletalMeshComponents contain sockets with names: ..."));
	return FGuid::NewGuid();
}

void UMeleeTraceComponent::EndTrace(const FGuid& TraceGuid)
{
	const int32 FoundIndex = ActiveMeleeTraces.IndexOfByPredicate(
		[TraceGuid](const FActiveMeleeTraceInfo& ActiveMeleeTraceInfo)
	{
		return TraceGuid == ActiveMeleeTraceInfo.Guid; 
	});

	if (ensureAlwaysMsgf(FoundIndex != INDEX_NONE,
		TEXT("Attemping to end trace with FGuid: %s but no trace with such FGuid exist"),
		*TraceGuid.ToString()))
	{
		OnTraceEnd.Broadcast(this, ActiveMeleeTraces[FoundIndex].Guid, ActiveMeleeTraces[FoundIndex].HitActors.Num());
		ActiveMeleeTraces.RemoveAtSwap(FoundIndex);
	}
}

void UMeleeTraceComponent::ForceEndAllTraces()
{
	for (const FActiveMeleeTraceInfo& ActiveMeleeTrace : ActiveMeleeTraces)
	{
		OnTraceEnd.Broadcast(this, ActiveMeleeTrace.Guid, ActiveMeleeTrace.HitActors.Num());
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
