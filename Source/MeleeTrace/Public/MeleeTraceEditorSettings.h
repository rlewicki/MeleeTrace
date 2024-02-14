// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "MeleeTraceEditorSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings, DefaultToInstanced)
class MELEETRACE_API UMeleeTraceEditorSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
public:
	UPROPERTY(Config,
		EditDefaultsOnly,
		Category = "Melee Trace Debug",
		meta = (ConsoleVariable="MeleeTrace.ShouldDrawDebug"))
	bool bShouldDebugDraw = false;

	UPROPERTY(Config,
		EditDefaultsOnly,
		Category = "Melee Trace Debug",
		meta = (ConsoleVariable="MeleeTrace.DrawDebugDuration"))
	float DrawDebugDuration = 1.0f;
};
