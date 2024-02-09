// Copyright 2023, Robert Lewicki, All rights reserved.

#include "AnimNotifyState_MeleeTrace.h"

#include "Components/SkeletalMeshComponent.h"

#include "MeleeTraceComponent.h"

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
			MeleeTraceComponent->StartTraceWithContext(MeleeTraceInfo, this);
		}
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
}