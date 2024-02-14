// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionShape.h"

#include "ActiveMeleeTraceInfo.generated.h"

class AActor;
class UMeshComponent;

USTRUCT()
struct MELEETRACE_API FActiveMeleeTraceInfo
{
	GENERATED_BODY()

	uint32 TraceHash = 0;
	int32 TraceDensity = 1;
	FQuat RotationOffset;
	FCollisionShape TraceCollisionShape;
	TWeakObjectPtr<UMeshComponent> SourceMeshComponent;
	TSet<TWeakObjectPtr<AActor>> HitActors;
	TArray<FVector> PreviousFrameSampleLocations;
	FName StartSocketName;
	FName EndSocketName;
};
