// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMeleeTrace, Log, All);

namespace MeleeTrace
{
	constexpr uint32 INVALID_HASH = 0;
	uint32 CalculateNewTraceHash();
	uint32 CalculateNewTraceHashWithContext(const uint32 ContextID);
	void ReleaseTraceHash(const uint32 ContextID);
	uint32 GetTraceHash(const uint32 ContextID);
	uint32 CombineHashes(const uint32 LhsHash, const uint32 RhsHash);
}