// Copyright 2023, Robert Lewicki, All rights reserved.

#include "AnimNotifyState_MeleeTrace.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"

#include "MeleeTraceCommon.h"
#include "MeleeTraceComponent.h"
#include "MeleeTraceDebug.h"
#include "MeleeTraceShape.h"

#if UE_VERSION_OLDER_THAN(5, 0, 0)
void UAnimNotifyState_MeleeTrace::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
#else
void UAnimNotifyState_MeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
#endif
{
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
#else
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
#endif

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UMeleeTraceComponent* MeleeTraceComponent = OwnerActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			if (!ensure(MeleeTraceInfo.TraceShape->IsValidLowLevelFast()))
			{
				UE_LOG(LogMeleeTrace,
					Error,
					TEXT("Invalid trace shape defined in a MeleeTrace anim notify for animation %s"),
					*Animation->GetName());
			}

			MeleeTraceComponent->StartTraceWithContext(MeleeTraceInfo, this);
		}
#if WITH_EDITOR
		else if (bShouldDrawDebugInEditor)
		{
			if (MeshComp->DoesSocketExist(MeleeTraceInfo.StartSocketName)
				&& MeshComp->DoesSocketExist(MeleeTraceInfo.EndSocketName))
			{
				DebugMeshComponent = MeshComp;
			}
			else
			{
				TArray<USceneComponent*> ChildrenComponents;
				MeshComp->GetChildrenComponents(true, ChildrenComponents);
				for (USceneComponent* ChildComponent : ChildrenComponents)
				{
					if (auto* TypedMeshComponent = Cast<UMeshComponent>(ChildComponent))
					{
						if (TypedMeshComponent->DoesSocketExist(MeleeTraceInfo.StartSocketName)
							&& TypedMeshComponent->DoesSocketExist(MeleeTraceInfo.EndSocketName))
						{
							DebugMeshComponent = TypedMeshComponent;
						}
					}
				}
			}
		}
#endif
	}
}

#if UE_VERSION_OLDER_THAN(5, 0, 0)
void UAnimNotifyState_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
#else
void UAnimNotifyState_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
#endif
{
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	Super::NotifyEnd(MeshComp, Animation);
#else
	Super::NotifyEnd(MeshComp, Animation, EventReference);
#endif

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UMeleeTraceComponent* MeleeTraceComponent = OwnerActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			MeleeTraceComponent->EndTraceWithContext(this);
		}
	}

#if WITH_EDITOR
	DebugMeshComponent.Reset();
	PreviousFrameSamples.Reset();
#endif
}

void UAnimNotifyState_MeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

#if WITH_EDITOR
	if (DebugMeshComponent.IsValid() && bShouldDrawDebugInEditor)
	{
		const UWorld* World = MeshComp->GetWorld();
		FCollisionShape CollisionShape;
		if (MeleeTraceInfo.TraceShape)
		{
			 CollisionShape = MeleeTraceInfo.TraceShape->CreateCollisionShape();
		}
		
		TArray<FVector> Samples;
		const FQuat Rotation = DebugMeshComponent->GetSocketQuaternion(MeleeTraceInfo.StartSocketName);
		UMeleeTraceComponent::GetTraceSamples(DebugMeshComponent.Get(),
			MeleeTraceInfo.TraceDensity,
			MeleeTraceInfo.StartSocketName,
			MeleeTraceInfo.EndSocketName,
			Samples);
		if (PreviousFrameSamples.Num() == Samples.Num())
		{
			for (int32 Index = 0; Index < Samples.Num(); Index++)
			{
				MeleeTrace::DrawDebugTrace(World,
					CollisionShape,
					FTransform(Rotation, PreviousFrameSamples[Index]),
					FTransform(Rotation, Samples[Index]),
					EDrawDebugTrace::ForDuration,
					false,
					{},
					DebugDrawDuration);	
			}
		}

		PreviousFrameSamples = MoveTemp(Samples);
	}
#endif
}
