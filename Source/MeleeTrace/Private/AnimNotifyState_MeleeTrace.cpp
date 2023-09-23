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
			TraceGuid = MeleeTraceComponent->StartTrace(MeleeTraceInfo);
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
			MeleeTraceComponent->EndTrace(TraceGuid);
		}
	}
}
