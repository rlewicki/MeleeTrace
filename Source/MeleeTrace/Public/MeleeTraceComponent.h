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

	UFUNCTION(BlueprintCallable, Category = "Melee Trace")
	void SetTraceChannel(ECollisionChannel NewTraceChannel);

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	ECollisionChannel GetTraceChannel() const;

	UPROPERTY(BlueprintAssignable)
	FMeleeTraceStart OnTraceStart;

	UPROPERTY(BlueprintAssignable)
	FMeleeTraceEnd OnTraceEnd;

	UPROPERTY(BlueprintAssignable)
	FMeleeTraceHit OnTraceHit;
protected:
	static void GetTraceSamples(const UMeshComponent* MeshComponent,
		const FMeleeTraceInfo& MeleeTraceInfo,
		TArray<FVector>& OutSamples);
	TArray<FActiveMeleeTraceInfo> ActiveMeleeTraces;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel;
};
