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

	FGuid StartTrace(const FMeleeTraceInfo& MeleeTraceInfo);
	void EndTrace(const FGuid& TraceGuid);

	UFUNCTION(BlueprintCallable)
	void ForceEndAllTraces();

	UFUNCTION(BlueprintPure)
	bool IsAnyTraceActive() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceStart, UMeleeTraceComponent*, Self, const FGuid&, TraceGuid);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTraceEnd,
		UMeleeTraceComponent*,
		Self,
		const FGuid&,
		TraceGuid,
		int32,
		HitCount);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTraceHit,
		UMeleeTraceComponent*,
		Self,
		const FGuid&,
		TraceGuid,
		AActor*,
		HitActor,
		const FVector&,
		HitLocation,
		const FVector&,
		HitNormal);

	UPROPERTY(BlueprintAssignable)
	FTraceStart OnTraceStart;

	UPROPERTY(BlueprintAssignable)
	FTraceEnd OnTraceEnd;

	UPROPERTY(BlueprintAssignable)
	FTraceHit OnTraceHit;

protected:
	static void GetTraceSamples(const USkeletalMeshComponent* MeshComponent,
		const FMeleeTraceInfo& MeleeTraceInfo,
		TArray<FVector>& OutSamples);
	TArray<FActiveMeleeTraceInfo> ActiveMeleeTraces;
};
