// Copyright 2024, Robert Lewicki, All rights reserved.

#include "MeleeTraceCommon.h"

DEFINE_LOG_CATEGORY(LogMeleeTrace);

uint32 GetTraceHash(const UObject* Context, const UObject* Component)
{
	const uint32 BaseHash = Context->GetUniqueID();
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	const uint32 TraceHash = HashCombine(BaseHash, Component->GetUniqueID());
#else
	const uint32 TraceHash = HashCombineFast(BaseHash, Component->GetUniqueID());
#endif
	return TraceHash;
}
