// Copyright 2023, Robert Lewicki, All rights reserved.

#include "AnimNotifyState_MeleeTrace.h"

#include "MeleeTraceComponent.h"

void UAnimNotifyState_MeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UMeleeTraceComponent* MeleeTraceComponent = OwnerActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			const uint32 TraceHash = GetTraceHash(MeshComp);
			MeleeTraceComponent->StartTrace(MeleeTraceInfo, TraceHash);
		}
	}
}

void UAnimNotifyState_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UMeleeTraceComponent* MeleeTraceComponent = OwnerActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			const uint32 TraceHash = GetTraceHash(MeshComp);
			MeleeTraceComponent->EndTrace(TraceHash);
		}
	}
}

uint32 UAnimNotifyState_MeleeTrace::GetTraceHash(USkeletalMeshComponent* MeshComp) const
{
	const uint32 MeshHash = MeshComp->GetUniqueID();
	const uint32 TraceHash = HashCombineFast(MeshHash, GetUniqueID());
	return TraceHash;	
}
