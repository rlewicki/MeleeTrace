// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MeleeTraceInfo.generated.h"

class UMeleeTraceShape;

USTRUCT(BlueprintType)
struct MELEETRACE_API FMeleeTraceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "General")
	FName StartSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "General")
	FName EndSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Instanced, Category = "General")
	TObjectPtr<UMeleeTraceShape> TraceShape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "General")
	int32 TraceDensity = 5;
};
