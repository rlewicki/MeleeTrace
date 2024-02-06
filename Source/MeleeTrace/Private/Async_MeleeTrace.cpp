// Copyright 2024, Robert Lewicki, All rights reserved.

#include "Async_MeleeTrace.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

#include "MeleeTraceCommon.h"
#include "MeleeTraceComponent.h"

UAsync_MeleeTrace* UAsync_MeleeTrace::MeleeTrace(
	UObject* WorldContextObject,
	AActor* TracingActor,
	FMeleeTraceInfo TraceInfo)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	if (!TracingActor)
	{
		UE_LOG(
			LogMeleeTrace,
			Error,
			TEXT("%s: Invalid reference for tracing actor"), StringCast<TCHAR>(__FUNCTION__).Get());
		return nullptr;
	}

	auto* MeleeTraceComponent = TracingActor->FindComponentByClass<UMeleeTraceComponent>();
	if (!MeleeTraceComponent)
	{
		UE_LOG(
			LogMeleeTrace,
			Error,
			TEXT("%s: %s to watch doesn't have MeleeTraceComponent attached"),
				StringCast<TCHAR>(__FUNCTION__).Get(),
				*GetNameSafe(TracingActor));
		return nullptr;
	}

	auto* NewAction = NewObject<UAsync_MeleeTrace>();
	NewAction->TracingActor = TracingActor;
	NewAction->MeleeTraceComponent = MeleeTraceComponent;
	NewAction->TraceInfo = TraceInfo;
	return NewAction;
}

void UAsync_MeleeTrace::Cancel()
{
	StopTrace();
}

void UAsync_MeleeTrace::StartTrace()
{
	if (ensure(MeleeTraceComponent.IsValid()))
	{
		TraceHash = GetTraceHash(MeleeTraceComponent.Get(), this);
		MeleeTraceComponent->StartTrace(TraceInfo, TraceHash);
	}
}

void UAsync_MeleeTrace::StopTrace()
{
	if (MeleeTraceComponent.IsValid())
	{
		MeleeTraceComponent->EndTrace(TraceHash);
	}

	TracingActor.Reset();
	MeleeTraceComponent.Reset();
	SetReadyToDestroy();
}
