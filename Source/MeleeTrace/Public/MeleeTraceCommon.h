// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMeleeTrace, Log, All);

uint32 GetTraceHash(const UObject* Context, const UObject* Component);