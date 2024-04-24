// Copyright 2024, Robert Lewicki, All rights reserved.

#include "MeleeTraceCommon.h"
#include "Misc/EngineVersionComparison.h"

DEFINE_LOG_CATEGORY(LogMeleeTrace);

TMap<uint32, uint32> ActiveHashes;

uint32 MeleeTrace::CalculateNewTraceHash()
{
	static uint32 Hash = INVALID_HASH;
	if (Hash == MAX_uint32)
	{
		Hash = INVALID_HASH;
	}

	++Hash;
	return Hash;
}

uint32 MeleeTrace::CalculateNewTraceHashWithContext(const uint32 ContextID)
{
	ensureMsgf(!ActiveHashes.Contains(ContextID),
			TEXT("Hash for ContextID=%u already exist in the map. Previous hash is going to be overwritten."), ContextID);
	const uint32 Hash = CalculateNewTraceHash();
	ActiveHashes.Add(ContextID, Hash);
	return Hash;
}

void MeleeTrace::ReleaseTraceHash(const uint32 ContextID)
{
	ensureMsgf(ActiveHashes.Contains(ContextID), TEXT("Attempt to release hash by ContextID=%u that is not tracked"), ContextID);
	ActiveHashes.Remove(ContextID);
}


uint32 MeleeTrace::GetTraceHash(const uint32 ContextID)
{
	if (!ActiveHashes.Contains(ContextID))
	{
		return INVALID_HASH;
	}
	
	return ActiveHashes[ContextID];
}

uint32 MeleeTrace::CombineHashes(const uint32 LhsHash, const uint32 RhsHash)
{
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	const uint32 Hash = HashCombine(MeshHash, GetUniqueID());
	return Hash;
#else
	const uint32 Hash = HashCombineFast(LhsHash, RhsHash);
	return Hash;
#endif
}
