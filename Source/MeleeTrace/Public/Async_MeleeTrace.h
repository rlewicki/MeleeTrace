// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MeleeTraceInfo.h"
#include "Engine/CancellableAsyncAction.h"
#include "Async_MeleeTrace.generated.h"

class UMeleeTraceComponent;

UCLASS()
class MELEETRACE_API UAsync_MeleeTrace : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UFUNCTION(
		BlueprintCallable,
		Category = "Melee Trace",
		meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "True"))
	static UAsync_MeleeTrace* MeleeTrace(UObject* WorldContextObject, AActor* TracingActor, FMeleeTraceInfo TraceInfo);

	virtual void Cancel() override;
	
	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void StartTrace();

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void StopTrace();
protected:
	TWeakObjectPtr<AActor> TracingActor;
	TWeakObjectPtr<UMeleeTraceComponent> MeleeTraceComponent;
	FMeleeTraceInfo TraceInfo;
	uint32 TraceHash = MAX_uint32;
};
