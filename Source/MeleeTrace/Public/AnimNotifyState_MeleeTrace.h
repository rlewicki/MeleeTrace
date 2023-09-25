// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "MeleeTraceInfo.h"

#include "AnimNotifyState_MeleeTrace.generated.h"

UCLASS(DisplayName = "Melee Trace")
class MELEETRACE_API UAnimNotifyState_MeleeTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	FGuid TraceGuid;

	UPROPERTY(EditInstanceOnly)
	FMeleeTraceInfo MeleeTraceInfo;
};
