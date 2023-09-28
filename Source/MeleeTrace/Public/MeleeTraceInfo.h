// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MeleeTraceInfo.generated.h"

USTRUCT(BlueprintType)
struct MELEETRACE_API FMeleeTraceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "General")
	FName StartSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	FName EndSocketName;
	
	UPROPERTY(EditDefaultsOnly, Category = "General")
	float Radius = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	int32 TraceDensity = 5;
};
