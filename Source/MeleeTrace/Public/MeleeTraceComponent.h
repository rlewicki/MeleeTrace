// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ActiveMeleeTraceInfo.h"
#include "MeleeTraceInfo.h"

#include "MeleeTraceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMeleeTraceStart,
	UMeleeTraceComponent*,
	ThisComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMeleeTraceEnd,
	UMeleeTraceComponent*,
	ThisComponent,
	int32,
	HitCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FMeleeTraceHit,
	UMeleeTraceComponent*,
	ThisComponent,
	AActor*,
	HitActor,
	const FVector&,
	HitLocation,
	const FVector&,
	HitNormal,
	FName,
	HitBoneName);

USTRUCT(BlueprintType)
struct MELEETRACE_API FMeleeTraceInstanceHandle
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 TraceHash;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MELEETRACE_API UMeleeTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeTraceComponent();
	virtual void TickComponent(float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// Starts trace and uses Context object to generate and later retrieve unique handle for the trace that has been
	// started. Using this has a known limitation of only one trace being active per Context object. This is fine
	// in most cases as for example each anim notify state is a unique object and it can only control one trace.
	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void StartTraceWithContext(const FMeleeTraceInfo& MeleeTraceInfo, const UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void EndTraceWithContext(const UObject* Context);

	// Starts trace and returns a unique handle for this given trace. To stop it, EndTrace must be called with
	// the same handle structure. Owner is responsible for storing the handle.
	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	FMeleeTraceInstanceHandle StartTrace(const FMeleeTraceInfo& MeleeTraceInfo);

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void EndTrace(FMeleeTraceInstanceHandle MeleeTraceInstanceHandle);

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void ForceEndAllTraces();

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	bool IsAnyTraceActive() const;

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void SetTraceChannel(ECollisionChannel NewTraceChannel);

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	ECollisionChannel GetTraceChannel() const;

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	TArray<AActor*> GetActorsHitByTraceWithContext(const UObject* Context) const;

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	TArray<AActor*> GetActorsHitByTrace(const FMeleeTraceInstanceHandle& Handle) const;

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	static void InvalidateMeleeTraceHandle(UPARAM(Ref) FMeleeTraceInstanceHandle& Handle);

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	static bool IsMeleeTraceHandleValid(const FMeleeTraceInstanceHandle& Handle);

	static void GetTraceSamples(const UMeshComponent* MeshComponent,
		int32 TraceDensity,
		const FName& StartSocketName,
		const FName& EndSocketName,
		TArray<FVector>& OutSamples);

	const TArray<FActiveMeleeTraceInfo>& GetActiveMeleeTraces() const { return ActiveMeleeTraces; }

	UPROPERTY(BlueprintAssignable)
	FMeleeTraceStart OnTraceStart;

	UPROPERTY(BlueprintAssignable)
	FMeleeTraceEnd OnTraceEnd;

	UPROPERTY(BlueprintAssignable)
	FMeleeTraceHit OnTraceHit;
protected:
	void InternalStartTrace(const FMeleeTraceInfo& MeleeTraceInfo, uint32 TraceHash);
	void InternalEndTrace(uint32 TraceHash);
	TArray<AActor*> InternalGetActorsHitByTrace(uint32 TraceHash) const;
	uint32 GetContextHash(const UObject* Context) const;
	
	TArray<FActiveMeleeTraceInfo> ActiveMeleeTraces;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel;
};
