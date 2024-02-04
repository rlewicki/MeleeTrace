// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MeleeTraceInfo.h"

#include "ActiveMeleeTraceInfo.generated.h"

class UMeshComponent;

USTRUCT()
struct MELEETRACE_API FActiveMeleeTraceInfo
{
	GENERATED_BODY()

	FMeleeTraceInfo MeleeTraceInfo;
	uint32 TraceHash;
	TWeakObjectPtr<UMeshComponent> SourceMeshComponent;
	TSet<TWeakObjectPtr<AActor>> HitActors;
	TArray<FVector> PreviousFrameSampleLocations;
};
