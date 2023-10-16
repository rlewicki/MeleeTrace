// Copyright 2023, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ActiveMeleeTraceInfo.h"
#include "MeleeTraceInfo.h"

#include "MeleeTraceComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MELEETRACE_API UMeleeTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeTraceComponent();
	virtual void TickComponent(float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void StartTrace(const FMeleeTraceInfo& MeleeTraceInfo, uint32 TraceHash);
	void EndTrace(uint32 TraceHash);

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void ForceEndAllTraces();

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	bool IsAnyTraceActive() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceStart,
		UMeleeTraceComponent*,
		ThisComponent);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceEnd,
		UMeleeTraceComponent*,
		ThisComponent,
		int32,
		HitCount);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTraceHit,
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

	UPROPERTY(BlueprintAssignable)
	FTraceStart OnTraceStart;

	UPROPERTY(BlueprintAssignable)
	FTraceEnd OnTraceEnd;

	UPROPERTY(BlueprintAssignable)
	FTraceHit OnTraceHit;

protected:
	static void GetTraceSamples(const UMeshComponent* MeshComponent,
		const FMeleeTraceInfo& MeleeTraceInfo,
		TArray<FVector>& OutSamples);
	TArray<FActiveMeleeTraceInfo> ActiveMeleeTraces;
};
