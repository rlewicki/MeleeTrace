// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "ActiveMeleeTraceInfo.h"
#include "Async_WaitForMeleeTraceEvent.generated.h"

class UMeleeTraceComponent;

USTRUCT(Blueprintable)
struct MELEETRACE_API FAsyncMeleeHitInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Melee Trace Info")
	TObjectPtr<UMeleeTraceComponent> OwnerTraceComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Melee Trace Info")
	TObjectPtr<AActor> HitActor;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Melee Trace Info")
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Melee Trace Info")
	FVector HitNormal = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Melee Trace Info")
	FName HitBoneName;
};

UCLASS()
class MELEETRACE_API UAsync_WaitForMeleeTraceEvent : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UFUNCTION(
		BlueprintCallable,
		Category = "Melee Trace",
		meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "True"))
	static UAsync_WaitForMeleeTraceEvent* WaitForMeleeTraceEventHit(UObject* WorldContextObject, AActor* ActorToWatch,
		bool OncePerMultiTrace);

	virtual void Cancel() override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
		FAsyncWaitForMeleeHitDetected, FAsyncMeleeHitInfo, HitInfo, FMeleeTraceInstanceHandle, TraceHandle);
	UPROPERTY(BlueprintAssignable)
	FAsyncWaitForMeleeHitDetected OnHit;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncWaitForMeleeTraceStarted);
	UPROPERTY(BlueprintAssignable)
	FAsyncWaitForMeleeTraceStarted OnStarted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncWaitForMeleeTraceEnded);
	UPROPERTY(BlueprintAssignable)
	FAsyncWaitForMeleeTraceEnded OnEnded;
protected:
	virtual void Activate() override;

	bool CanHit;
	bool OncePerMultiTrace;
	TWeakObjectPtr<AActor> ActorToWatch;
	TWeakObjectPtr<UMeleeTraceComponent> MeleeTraceComponent;

	UFUNCTION()
	void HandleTraceHit(
		UMeleeTraceComponent* ThisComponent,
		AActor* HitActor,
		const FVector& HitLocation,
		const FVector& HitNormal,
		FName HitBoneName,
		FMeleeTraceInstanceHandle TraceHandle);

	UFUNCTION()
	void HandleTraceStarted(UMeleeTraceComponent* ThisComponent, FMeleeTraceInstanceHandle TraceHandle);

	UFUNCTION()
	void HandleTraceEnded(UMeleeTraceComponent* ThisComponent, int32 HitCount, FMeleeTraceInstanceHandle TraceHandle);
};
