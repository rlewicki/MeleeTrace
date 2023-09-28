// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "MeleeTraceSettings.generated.h"

enum ECollisionChannel : int;

UCLASS(Config=Game, DefaultToInstanced)
class MELEETRACE_API UMeleeTraceSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "General")
	TEnumAsByte<ECollisionChannel> MeleeTraceCollisionChannel;
};
