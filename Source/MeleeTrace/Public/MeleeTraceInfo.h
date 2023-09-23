// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MeleeTraceInfo.generated.h"

USTRUCT(BlueprintType)
struct MELEETRACE_API FMeleeTraceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName StartSocketName;

	UPROPERTY(EditDefaultsOnly)
	FName EndSocketName;
	
	UPROPERTY(EditDefaultsOnly)
	float Radius = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 TraceDensity = 5;
};
