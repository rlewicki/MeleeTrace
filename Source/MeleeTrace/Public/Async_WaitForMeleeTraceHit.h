// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "Async_WaitForMeleeTraceHit.generated.h"

class UMeleeTraceComponent;

USTRUCT(Blueprintable)
struct MELEETRACE_API FAsyncMeleeHitInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<UMeleeTraceComponent> OwnerTraceComponent;

	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<AActor> HitActor;

	UPROPERTY(BlueprintReadOnly, Transient)
	FVector HitLocation;

	UPROPERTY(BlueprintReadOnly, Transient)
	FVector HitNormal;

	UPROPERTY(BlueprintReadOnly, Transient)
	FName HitBoneName;
};

UCLASS()
class MELEETRACE_API UAsync_WaitForMeleeTraceHit : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UFUNCTION(
		BlueprintCallable,
		Category = "Melee Trace",
		meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "True"))
	static UAsync_WaitForMeleeTraceHit* WaitForMeleeHit(UObject* WorldContextObject, AActor* ActorToWatch);

	virtual void Cancel() override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncWaitForMeleeHitDetected, FAsyncMeleeHitInfo, HitInfo);
	UPROPERTY(BlueprintAssignable)
	FAsyncWaitForMeleeHitDetected OnHit;
protected:
	virtual void Activate() override;

	TWeakObjectPtr<AActor> ActorToWatch;
	TWeakObjectPtr<UMeleeTraceComponent> MeleeTraceComponent;

	UFUNCTION()
	void HandleTraceHit(
		UMeleeTraceComponent* ThisComponent,
		AActor* HitActor,
		const FVector& HitLocation,
		const FVector& HitNormal,
		FName HitBoneName);
};
